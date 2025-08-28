/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 17:27:55 by anassih           #+#    #+#             */
/*   Updated: 2025/08/28 02:05:14 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/tokenization.h"
#include "../includes/env_utils.h"

// Concatenate s1 + s2, free s1, return the new string.
char	*join_free(char *s1, char *s2)
{
	char	*result;

	result = ft_strjoin(s1, s2);
	if (s1)
		free(s1);
	return (result);
}

// Concatenate s1 + s2, free s1, return the new string.
// s2 is const so we don’t free it.
char	*join_free_const(char *s1, const char *s2)
{
	char	*result;

	result = ft_strjoin(s1, s2);
	if (s1)
		free(s1);
	return (result);
}

// Free a NULL-terminated array of strings and the array itself.
void	free_split(char **str)
{
	int	i;

	if (!str)
		return ;
	i = 0;
	while (str[i])
		free(str[i++]);
	free(str);
}

// Declaration only, implementation is in ast_utils.c
void	free_ast(t_ast *ast);

// Cleanup function for child processes before execve
void	cleanup_child_before_exec(t_context *ctx)
{
	if (ctx->ast_head)
		free_ast(ctx->ast_head);
	ctx->ast_head = NULL;
}
