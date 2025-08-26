/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   smart_split.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:12:38 by anassih           #+#    #+#             */
/*   Updated: 2025/08/26 04:27:38 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/tokenization.h"
#include "../includes/env_utils.h"

static char	**split_loop(const char *input)
{
	t_split_ctx	ctx;
	size_t		i;
	char		c;

	init_ctx(&ctx);
	i = 0;
	while (input[i])
	{
		c = input[i];
		if (!process_ws_op(c, &ctx, input, &i))
			update_state_and_append(c, &ctx);
		i++;
	}
	if (ctx.state != STATE_NORMAL)
	{
		ft_putstr_fd("minishell: error: unmatched quote\n", STDERR_FILENO);
		free(ctx.cur);
		free_split(ctx.tokens);
		return (NULL);
	}
	if (ctx.cur)
		flush_token(&ctx);
	return (ctx.tokens);
}

char	**smart_split(const char *input)
{
	return (split_loop(input));
}
