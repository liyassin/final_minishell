/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_helpers.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:12:24 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 03:35:41 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/tokenization.h"
#include "../includes/env_utils.h"

// Custom realloc implementation for buffer management
void	*ft_realloc(void *ptr, size_t old_size, size_t new_size)
{
	void	*new_ptr;

	if (new_size == 0)
	{
		free(ptr);
		return (NULL);
	}
	new_ptr = malloc(new_size);
	if (!new_ptr)
		return (NULL);
	if (ptr)
	{
		if (old_size < new_size)
			ft_memcpy(new_ptr, ptr, old_size);
		else
			ft_memcpy(new_ptr, ptr, new_size);
		free(ptr);
	}
	return (new_ptr);
}

// Efficient buffer-based character appending
char	*append_char(char *str, char c, size_t *len, size_t *cap)
{
	size_t	new_cap;

	if (*len + 1 >= *cap)
	{
		if (*cap == 0)
			new_cap = 16;
		else
			new_cap = *cap * 2;
		str = ft_realloc(str, *len + 1, new_cap);
		if (!str)
			return (NULL);
		*cap = new_cap;
	}
	str[*len] = c;
	(*len)++;
	str[*len] = '\0';
	return (str);
}

void	add_token(char ***tokens, size_t *count, size_t *cap, char *token)
{
	char	**new_tokens;
	size_t	new_cap;

	if (*count + 1 >= *cap)
	{
		if (*cap == 0)
			new_cap = 16;
		else
			new_cap = *cap * 2;
		new_tokens = ft_realloc(*tokens, (*count) * sizeof(char *),
				new_cap * sizeof(char *));
		if (!new_tokens)
		{
			free(token);
			return ;
		}
		*tokens = new_tokens;
		*cap = new_cap;
	}
	(*tokens)[*count] = token;
	(*count)++;
	(*tokens)[*count] = NULL;
}

// Extract operator string and advance index
static char	*extract_operator(const char *input, size_t *i)
{
	char    op[16];
	size_t  start;
	size_t  len;

	start = *i;
	len = 0;
	if (input[*i] == '>' || input[*i] == '<')
	{
		char c = input[*i];
		while (input[*i + len] == c && len < 15)
			len++;
		ft_strlcpy(op, input + start, len + 1);
		*i += len - 1;
		return (ft_strdup(op));
	}
	op[0] = input[*i];
	op[1] = '\0';
	return (ft_strdup(op));
}

void	handle_operator(const char *input, size_t *i, t_token_ctx *ctx)
{
	char	*op_str;

	op_str = extract_operator(input, i);
	add_token(ctx->tokens, ctx->count, ctx->cap, op_str);
}
