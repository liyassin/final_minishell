/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_context_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 05:30:00 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 06:54:03 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/tokenization.h"
#include "../includes/env_utils.h"

static int	is_whitespace_only(char *tok)
{
	size_t	i;

	if (!tok)
		return (1);
	i = 0;
	while (tok[i])
	{
		if (!ft_isspace((unsigned char)tok[i]))
			return (0);
		i++;
	}
	return (1);
}

void	remove_whitespace_only_tokens(t_split_ctx *ctx)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	while (i < ctx->count)
	{
		if (!is_whitespace_only(ctx->tokens[i]))
		{
			if (i != j)
				ctx->tokens[j] = ctx->tokens[i];
			j++;
		}
		else
			free(ctx->tokens[i]);
		i++;
	}
	ctx->count = j;
}
