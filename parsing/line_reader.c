/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   line_reader.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:13:13 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 09:07:00 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"

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
	{
		ft_putendl_fd("exit", STDOUT_FILENO);
		*should_exit = 1;
		return (NULL);
	}
	trimmed = ft_strtrim(input, " ");
	if (!trimmed || !*trimmed)
	{
		free(input);
		free(trimmed);
		ctx->exit_status = 0;
		return (NULL);
	}
	free(trimmed);
	input = handle_line_continuation(input, ctx);
	if (!input)
		return (NULL);
	if (*input)
		add_history(input);
	return (input);
}
