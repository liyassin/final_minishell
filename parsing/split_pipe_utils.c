/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_pipe_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 06:00:00 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 07:05:36 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/tokenization.h"
#include "../includes/env_utils.h"

void	update_quote_state(char c, char *quote)
{
	if ((c == '"' || c == '\'') && !(*quote))
		*quote = c;
	else if (c == *quote)
		*quote = 0;
}

char	**process_token_loop(const char *line, t_pipe_context *ctx)
{
	size_t	i;
	size_t	start;
	char	quote;
	size_t	len;

	i = 0;
	start = 0;
	quote = 0;
	len = ft_strlen(line);
	while (i <= len)
	{
		update_quote_state(line[i], &quote);
		if (!quote && (line[i] == '|' || line[i] == '\0'))
		{
			ctx->parts = handle_pipe_segment(line, ctx, start, i);
			if (!ctx->parts)
				return (NULL);
			start = i + 1;
		}
		++i;
	}
	return (ctx->parts);
}
