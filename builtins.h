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

# include "minishell.h"

/* Built-in function */
int     builtin_exit(char **args);
int     builtin_cd(char **args, char ***shell_env);
int     builtin_env(char **envp);
int     builtin_pwd(void);
int     builtin_echo(char **args);
int     builtin_export(char **args, char ***shell_env);
int     builtin_unset(char **args, char ***shell_env);

#endif
