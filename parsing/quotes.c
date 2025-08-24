/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:13:02 by anassih           #+#    #+#             */
/*   Updated: 2025/08/23 18:13:07 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/tokenization.h"
#include "../includes/env_utils.h"

static char	*strip_and_validate_quotes(char *token, int *error)
{
	char	*res;
	size_t	i;
	size_t	j;
	int		in_dq;
	int		in_sq;

	res = ft_calloc(ft_strlen(token) + 1, 1);
	if (!res)
		return (NULL);
	i = 0;
	j = 0;
	in_dq = 0;
	in_sq = 0;
	while (token[i])
	{
		if (token[i] == '"' && !in_sq)
			in_dq = !in_dq;
		else if (token[i] == '\'' && !in_dq)
			in_sq = !in_sq;
		else if (in_dq && token[i] == '\\' && token[i + 1]
			&& (token[i + 1] == '"' || token[i + 1] == '\\'
				||token[i + 1] == '$'))
			res[j++] = token[++i];
		else
			res[j++] = token[i];
		i++;
	}
	res[j] = '\0';
	if (in_dq || in_sq)
		*error = 1;
	return (res);
}

// Updated quote handling with context and error detection
char	*handle_quotes(char *token, t_context *ctx)
{
	char	*clean;
	char	*sub;
	char	*env;
	int		error;
	int		single;

	if (!token)
		return (NULL);
	single = (token[0] == '\'' && token[ft_strlen(token) - 1] == '\'');
	error = 0;
	clean = strip_and_validate_quotes(token, &error);
	if (error || !clean)
	{
		ft_putstr_fd("minishell: error: unmatched quote\n", STDERR_FILENO);
		ctx->exit_status = 2;
		free(clean);
		return (NULL);
	}
	if (single)
		return (clean);
	sub = handle_command_substitution(clean, ctx);
	env = expand_env_vars(sub, ctx->env, ctx->exit_status);
	free(clean);
	free(sub);
	return (env);
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
