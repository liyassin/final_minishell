/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 21:35:01 by anassih           #+#    #+#             */
/*   Updated: 2025/08/28 02:42:37 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <ctype.h>
# include <dirent.h>
# include <errno.h>
# include <fcntl.h>
# include <libft.h>
# include <limits.h>
# include <stdio.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>
# include "builtins.h"
# include "env_utils.h"
# include "signals.h"
# include "tokenization.h"

typedef struct s_ast			t_ast;
typedef struct s_redir			t_redir;
typedef struct s_context		t_context;
extern volatile sig_atomic_t	g_signal;

typedef enum e_builtin
{
	NOT_BUILTIN,
	PARENT_BUILTIN,
	CHILD_BUILTIN
}	t_builtin;

typedef struct s_context
{
	char	**env;
	char	**exported;
	int		exit_status;
	pid_t	main_process_pid;
	int		heredoc_interrupted;
	int		in_child_process;
	int		in_pipeline;
	t_ast	*ast_head;
}	t_context;

t_builtin	get_builtin_type(const char *cmd);
int			handle_builtin(char **args, t_context *ctx);

char		*find_command_path(char *cmd, t_context *ctx);
int			handle_redirection(t_ast *ast);

void		exec_command(t_ast *ast, t_context *ctx);
void		execute_pipeline(t_ast *ast, t_context *ctx);
t_ast		*build_ast_pipeline(char *input, t_context *ctx);

char		**split_by_pipes(const char *line);
char		*join_free(char *s1, char *s2);
char		*join_free_const(char *s1, const char *s2);
void		free_ast(t_ast *ast);
char		*read_input(int *should_exit, t_context *ctx);
char		*handle_eof(int *should_exit);
char		*handle_empty_input(char *input, char *trimmed, t_context *ctx);
char		*handle_quote_error(char *input, t_context *ctx);
void		handle_command_not_found(char *cmd);
void		execute_if_needed(t_ast *head, t_context *ctx, int *should_exit);

// heredoc functions
void		read_heredoc(t_redir *r, t_context *ctx);
void		process_heredocs(t_ast *head, t_context *ctx);
void		write_heredoc_line(t_redir *r, char *line, t_context *ctx);
void		close_remaining_heredocs(t_ast *head);
void		handle_interrupted_heredoc(t_redir *r, t_context *ctx);
void		handle_successful_heredoc(t_redir *r);

typedef struct s_pipeline_data
{
	int		n;
	int		(*pipes)[2];
	pid_t	*pids;
}	t_pipeline_data;

// pipeline executor utils
void		exec_node(t_ast *node, t_context *ctx);
void		wait_pipeline(int n, pid_t *pids, t_context *ctx);
void		cleanup_pipeline_fail(int (*pipes)[2], pid_t *pids);
void		execute_pipeline_main(t_ast *ast, t_context *ctx,
				t_pipeline_data *data);
int			fork_children(t_ast *ast, t_context *ctx, t_pipeline_data *data);

// substitution utility functions
int			setup_substitution_pipe(int pipefd[2]);
char		*read_subcmd_output(int pipefd[2]);
char		*assemble_substitution(const char *input, const char *buf,
				char *open, char *end);

int			count_pipeline(t_ast *ast);
int			open_pipes(int n, int (**pipes)[2]);
pid_t		*alloc_pids(int n);
void		setup_pipes(int i, int n, int pipes[][2]);
void		close_pipes(int n, int pipes[][2]);
void		cleanup_shell(t_ast *ast, t_context *ctx);

// executor_utils.c
void		handle_directory_redirection_error(t_redir *r, t_context *ctx);
void		consume_stdin_and_discard(void);
void		handle_empty_head(t_ast *head, t_context *ctx);
void		handle_redirection_only(t_ast *head, t_context *ctx);
int			setup_pipeline_resources(int n, int (**pipes)[2], pid_t **pids);

// command_exec_utils.c
void		setup_child_signals(void);
void		handle_exec_failure(char *command, char *full_path, t_context *ctx);
int			should_search_path(char *command);
int			has_heredoc_redir(t_ast *ast);
int			is_arg_empty_str(char **args);
int			is_numeric_arg(char *arg);

// command_exec_process.c
void		child_exec(t_ast *ast, t_context *ctx);
void		handle_parent_process(pid_t pid, t_context *ctx);
void		handle_fork_error(t_context *ctx);
void		execute_fork_process(t_ast *ast, t_context *ctx);

// main_utils.c
void		init_context(t_context *ctx, char **envp);

// heredoc_utils.c
void		write_heredoc_line(t_redir *r, char *line, t_context *ctx);
void		close_remaining_heredocs(t_ast *head);

#endif
