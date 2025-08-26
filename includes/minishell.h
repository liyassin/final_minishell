#ifndef MINISHELL_H
# define MINISHELL_H

# include <ctype.h>
# include <dirent.h>
# include <errno.h>
# include <fcntl.h>
# include <../libft/inc/libft.h>
# include <limits.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdbool.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

// Add forward declarations before including other headers
typedef struct	s_ast t_ast;
typedef struct	s_redir t_redir;
typedef struct	s_context t_context;
# include "builtins.h"
# include "env_utils.h"
# include "signals.h"
# include "tokenization.h"

// Define builtin types
typedef enum e_builtin
{
	NOT_BUILTIN,
	PARENT_BUILTIN,
	CHILD_BUILTIN
}	t_builtin;

// Changed to single-purpose signal variable
extern volatile sig_atomic_t	g_signal;

// Context structure for environment and exit status
typedef struct s_context
{
	char	**env;
	char	**exported;
	int		exit_status;
	pid_t	main_process_pid;
	int		heredoc_interrupted;
	t_ast	*ast_head;
}	t_context;

// Builtins
t_builtin	get_builtin_type(const char *cmd);
int			handle_builtin(char **args, t_context *ctx);

//  Command lookup & redirection
char		*find_command_path(char *cmd, t_context *ctx);
int			handle_redirection(t_ast *ast);

//  Pipeline execution
void		exec_command(t_ast *ast, t_context *ctx);
void		execute_pipeline(t_ast *ast, t_context *ctx);
t_ast		*build_ast_pipeline(char *input, t_context *ctx);

//  Helpers
char		**split_on_pipe(const char *line);
char		*join_free(char *s1, char *s2);
char		*join_free_const(char *s1, const char *s2);
void		free_ast(t_ast *ast);
char		*read_input(int *should_exit, t_context *ctx);
void		handle_command_not_found(char *cmd);
void		execute_if_needed(t_ast *head, t_context *ctx, int *should_exit);
int			count_pipeline(t_ast *ast);
int			open_pipes(int n, int (**pipes)[2]);
pid_t		*alloc_pids(int n);
void		setup_pipes(int i, int n, int pipes[][2]);
void		close_pipes(int n, int pipes[][2]);
void		cleanup_shell(t_ast *ast, t_context *ctx);

#endif
