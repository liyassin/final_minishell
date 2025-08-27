/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_pipe.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:12:16 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 07:05:36 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/tokenization.h"
#include "../includes/env_utils.h"

char	*trim_segment(const char *line, size_t start, size_t end)
{
	size_t	a;
	size_t	seglen;

	a = start;
	seglen = end - start;
	while (seglen && ft_isspace((unsigned char)line[a]))
	{
		++a;
		--seglen;
	}
	while (seglen && ft_isspace((unsigned char)line[a + seglen - 1]))
		--seglen;
	return (ft_substr(line, a, seglen));
}

static char	**expand_parts_array(char **parts, size_t *cap)
{
	char	**new_parts;

	new_parts = ft_realloc(parts, sizeof(*parts) * (*cap),
			sizeof(*parts) * (*cap) * 2);
	if (new_parts)
		*cap *= 2;
	return (new_parts);
}

char	**handle_pipe_segment(const char *line, t_pipe_context *ctx,
		size_t start, size_t i)
{
	ctx->parts[(*(ctx->count))++] = trim_segment(line, start, i);
	if (*(ctx->count) + 1 >= *(ctx->cap))
	{
		ctx->parts = expand_parts_array(ctx->parts, ctx->cap);
		if (!ctx->parts)
			return (NULL);
	}
	return (ctx->parts);
}

static char	**process_pipe_tokens(const char *line, char **parts,
		size_t *count, size_t *cap)
{
	t_pipe_context	ctx;

	ctx.parts = parts;
	ctx.count = count;
	ctx.cap = cap;
	return (process_token_loop(line, &ctx));
}

char	**split_by_pipes(const char *line)
{
	char	**parts;
	size_t	count;
	size_t	cap;

	cap = 8;
	count = 0;
	parts = malloc(sizeof(*parts) * cap);
	if (!parts)
		return (NULL);
	parts = process_pipe_tokens(line, parts, &count, &cap);
	if (!parts)
		return (NULL);
	parts[count] = NULL;
	return (parts);
}
