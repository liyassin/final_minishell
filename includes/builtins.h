/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 21:35:01 by anassih           #+#    #+#             */
/*   Updated: 2025/08/26 23:27:24 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILTINS_H
# define BUILTINS_H

# define EXIT_BUILTIN_SIGNAL 255

typedef struct s_context	t_context;

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
