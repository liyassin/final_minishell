#ifndef BUILTINS_H
# define BUILTINS_H

// Special return code to signal shell exit
# define EXIT_BUILTIN_SIGNAL 255

# include "minishell.h"

// Built-in function prototypes with context
int	builtin_exit(char **args, t_context *ctx);
int	builtin_cd(char **args, t_context *ctx);
int	builtin_env(t_context *ctx);
int	builtin_pwd(t_context *ctx);
int	builtin_echo(char **args, t_context *ctx);
int	builtin_export(char **args, t_context *ctx);
int	builtin_unset(char **args, t_context *ctx);
int	is_valid_identifier(const char *name);

#endif
