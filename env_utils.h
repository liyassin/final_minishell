/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 15:21:42 by anassih           #+#    #+#             */
/*   Updated: 2025/05/05 17:02:02 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENV_UTILS_H
# define ENV_UTILS_H

# include <stdlib.h>      // For malloc, free
# include <string.h>
# include <stdio.h>
# include "libft/inc/libft.h"

char	**copy_environment(char **envp);
void	free_environment(char **env);
int		update_env(char ***env, const char *name, const char *value);
char	*join_free(char *s1, char *s2);
char	*strip_quotes(const char *str);
void	free_split(char **str);
char	*join_free_const(char *s1, const char *s2);
char	*expand_env_vars(const char *input, char **envp);
char	*get_env_value(const char *name, char **envp);



#endif