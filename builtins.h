/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 17:48:21 by anassih           #+#    #+#             */
/*   Updated: 2025/05/05 16:36:33 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILTINS_H
# define BUILTINS_H

/* Special return code to signal shell exit */
#define EXIT_BUILTIN_SIGNAL 255

# include "minishell.h"

/* Built-in function prototypes with context */
int     builtin_exit(char **args, t_context *ctx);
int     builtin_cd(char **args, t_context *ctx);
int     builtin_env(t_context *ctx);
int     builtin_pwd(t_context *ctx);
int     builtin_echo(char **args, t_context *ctx);
int     builtin_export(char **args, t_context *ctx);
int     builtin_unset(char **args, t_context *ctx);

#endif