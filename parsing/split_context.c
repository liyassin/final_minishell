/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_context.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:12:30 by anassih           #+#    #+#             */
/*   Updated: 2025/08/23 18:12:31 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/tokenization.h"
#include "../includes/env_utils.h"

void	init_ctx(t_split_ctx *ctx)
{
	ctx->cur = NULL;
	ctx->cur_len = 0;
	ctx->cur_cap = 0;
	ctx->tokens = NULL;
	ctx->count = 0;
	ctx->cap = 0;
	ctx->state = STATE_NORMAL;
}

void	flush_token(t_split_ctx *ctx)
{
	add_token(&ctx->tokens, &ctx->count, &ctx->cap, ctx->cur);
	ctx->cur = NULL;
	ctx->cur_len = 0;
	ctx->cur_cap = 0;
}

int	process_ws_op(char c, t_split_ctx *ctx, const char *input, size_t *i)
{
	if (ctx->state == STATE_NORMAL && ft_isspace((unsigned char)c))
	{
		if (ctx->cur)
			flush_token(ctx);
		return (1);
	}
	if (ctx->state == STATE_NORMAL && (c == '>' || c == '<' || c == '|'))
	{
		if (ctx->cur)
			flush_token(ctx);
		handle_operator(input, i, &ctx->tokens, &ctx->count, &ctx->cap);
		return (1);
	}
	return (0);
}

void	update_state_and_append(char c, t_split_ctx *ctx)
{
	if (ctx->state == STATE_NORMAL && c == '\'')
		ctx->state = STATE_IN_SQUOTE;
	else if (ctx->state == STATE_IN_SQUOTE && c == '\'')
		ctx->state = STATE_NORMAL;
	else if (ctx->state == STATE_NORMAL && c == '"')
		ctx->state = STATE_IN_DQUOTE;
	else if (ctx->state == STATE_IN_DQUOTE && c == '"')
		ctx->state = STATE_NORMAL;
	ctx->cur = append_char(ctx->cur, c, &ctx->cur_len, &ctx->cur_cap);
}
