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
# include <stdbool.h>
# include <ctype.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <dirent.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "libft/inc/libft.h"

// Add forward declarations before including other headers
typedef struct s_ast t_ast;
typedef struct s_redir t_redir;
typedef struct s_context t_context;
# include "tokenization.h"
# include "builtins.h"
# include "env_utils.h"
# include "signals.h"

// Define builtin types
typedef enum e_builtin {
    NOT_BUILTIN,
    PARENT_BUILTIN,
    CHILD_BUILTIN
} t_builtin;

// Changed to single-purpose signal variable
extern volatile sig_atomic_t g_signal;

// Context structure for environment and exit status
typedef struct s_context {
    char    **env;
    int     exit_status;
} t_context;

// Builtins
t_builtin get_builtin_type(const char *cmd);
int     handle_builtin(char **args, t_context *ctx);
/*
**  Command lookup & redirection
*/
char    *find_command_path(char *cmd, t_context *ctx);
int    handle_redirection(t_ast *ast);
/*
**  Pipeline execution
*/
void    exec_command(t_ast *ast, t_context *ctx);
void    execute_pipeline(t_ast *ast, t_context *ctx);

/*
**  Helpers
*/
char    **split_on_pipe(const char *line);
char    *join_free(char *s1, char *s2);
char    *join_free_const(char *s1, const char *s2);
void    free_ast(t_ast *ast);

#endif