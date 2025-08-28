/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 21:20:00 by anassih           #+#    #+#             */
/*   Updated: 2025/08/28 02:03:02 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLEANUP_H
# define CLEANUP_H

# include "minishell.h"

char	*join_free(char *s1, char *s2);
char	*join_free_const(char *s1, const char *s2);
void	free_split(char **str);
void	free_ast(t_ast *ast);
void	cleanup_child_before_exec(t_context *ctx);

#endif