/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   line_reader.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:13:13 by anassih           #+#    #+#             */
/*   Updated: 2025/08/28 00:22:49 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"

static int	has_unclosed_quotes(char *input)
{
	char	quote;
	int		i;

	quote = 0;
	i = 0;
	while (input[i])
	{
		if ((input[i] == '"' || input[i] == '\'') && !quote)
			quote = input[i];
		else if (input[i] == quote)
			quote = 0;
		i++;
	}
	return (quote != 0);
}

static int	line_needs_continuation(char *input, size_t len)
{
	if (len < 1)
		return (0);
	if (input[len - 1] == '|')
		return (1);
	if (len >= 2 && (!ft_strncmp(input + len - 2, "||", 2)
			|| !ft_strncmp(input + len - 2, "&&", 2)))
		return (1);
	return (0);
}

static char	*handle_line_continuation(char *input, t_context *ctx)
{
	char	*more;
	char	*tmp;
	size_t	len;

	len = ft_strlen(input);
	while (line_needs_continuation(input, len))
	{
		more = readline("> ");
		if (g_signal == SIGINT)
		{
			free(input);
			if (more)
				free(more);
			ctx->exit_status = 130;
			return (NULL);
		}
		if (!more)
			break ;
		tmp = ft_strjoin(input, more);
		free(input);
		free(more);
		input = tmp;
		len = ft_strlen(input);
	}
	return (input);
}

char	*read_input(int *should_exit, t_context *ctx)
{
	char	*input;
	char	*trimmed;

	input = readline("$ [minishell] > ");
	if (!input)
		return (handle_eof(should_exit));
	trimmed = ft_strtrim(input, " ");
	if (!trimmed || !*trimmed)
		return (handle_empty_input(input, trimmed, ctx));
	free(trimmed);
	if (has_unclosed_quotes(input))
		return (handle_quote_error(input, ctx));
	input = handle_line_continuation(input, ctx);
	if (!input)
		return (NULL);
	if (*input)
		add_history(input);
	return (input);
}
