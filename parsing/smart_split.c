/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   smart_split.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:12:38 by anassih           #+#    #+#             */
/*   Updated: 2025/08/26 10:20:13 by anassih          ###   ########.fr       */
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
	   /* Remove tokens that are only whitespace */
	   size_t	j;
	   size_t	k;
	   size_t	out_count;
	   char	*tok;

	   out_count = 0;
	   j = 0;
	   while (ctx.tokens && ctx.tokens[j])
	   {
			   tok = ctx.tokens[j];
			   k = 0;
			   while (tok[k] && ft_isspace((unsigned char)tok[k]))
					   k++;
			   if (tok[k] == '\0')
			   {
					   free(tok);
					   ctx.tokens[j] = NULL;
			   }
			   else
			   {
					   ctx.tokens[out_count] = tok;
					   out_count++;
			   }
			   j++;
	   }
	   if (ctx.tokens)
			   ctx.tokens[out_count] = NULL;
	   return (ctx.tokens);
}

char	**smart_split(const char *input)
{
	return (split_loop(input));
}
