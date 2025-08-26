/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:13:02 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 00:16:20 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/tokenization.h"
#include "../includes/env_utils.h"

static char	*process_quoted_part(char *token, size_t *i, t_context *ctx)
{
	char	quote_char;
	size_t	start;
	char	*part;
	char	*temp;

	quote_char = token[*i];
	start = (*i)++;
	while (token[*i] && token[*i] != quote_char)
		(*i)++;
	if (!token[*i])
		return (NULL);
	(*i)++;
	part = ft_substr(token, start, *i - start);
	if (quote_char == '\'')
		temp = ft_substr(part, 1, ft_strlen(part) - 2);
	else
		temp = process_double_quotes(part, ctx);
	free(part);
	return (temp);
}

static char	*process_unquoted_part(char *token, size_t *i, t_context *ctx)
{
	size_t	start;
	char	*part;
	char	*temp;

	start = *i;
	while (token[*i] && token[*i] != '\'' && token[*i] != '"')
		(*i)++;
	part = ft_substr(token, start, *i - start);
	temp = expand_env_vars(part, ctx->env, ctx->exit_status);
	free(part);
	return (temp);
}

static char	*process_mixed_token(char *token, t_context *ctx)
{
	char	*result;
	char	*temp;
	char	*joined;
	size_t	i;

	result = ft_strdup("");
	i = 0;
	while (result && token[i])
	{
		if (token[i] == '\'' || token[i] == '"')
			temp = process_quoted_part(token, &i, ctx);
		else
			temp = process_unquoted_part(token, &i, ctx);
		if (!temp)
		{
			free(result);
			return (NULL);
		}
		joined = ft_strjoin(result, temp);
		free(result);
		free(temp);
		result = joined;
	}
	return (result);
}

// Updated quote handling with context and error detection
char	*handle_quotes(char *token, t_context *ctx)
{
	if (!token)
		return (NULL);
	return (process_mixed_token(token, ctx));
}

// Double quote processing now uses context
char	*process_double_quotes(const char *token, t_context *ctx)
{
	size_t	len;
	char	*inner;
	char	*expanded;

	len = ft_strlen(token);
	if (len < 2)
		return (ft_strdup(""));
	inner = ft_substr(token, 1, len - 2);
	if (!inner)
		return (NULL);
	expanded = expand_env_vars(inner, ctx->env, ctx->exit_status);
	free(inner);
	return (expanded);
}
