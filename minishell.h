#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <limits.h>
# include <string.h>
# include <errno.h>
# include <fcntl.h>
# include <signal.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <dirent.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "libft/inc/libft.h"
# include "tokenization.h"
# include "builtins.h"
# include "env_utils.h"

// Builtins
int		handle_builtin(char **args, char ***shell_env);
char	*find_command_path(char *cmd, char **envp);
void	handle_redirection(t_ast *ast);

// Execution
void    exec_command(t_ast *ast, char **env);
void    execute_pipeline(t_ast *ast, char **env);

// Utils
void	free_split(char **str);
char	*join_free(char *s1, char *s2);
char	*join_free_const(char *s1, const char *s2);
void	free_ast(t_ast *ast);

#endif
