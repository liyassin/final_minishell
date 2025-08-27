/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   line_reader.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:13:13 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 03:43:55 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"

char	*read_input(int *should_exit, t_context *ctx)
{
	char    *input;
	char    *trimmed;
	char    *more;
	size_t  len;

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
	// PS2 prompt: keep reading if line ends with |, ||, or &&
	len = ft_strlen(input);
	while (len > 0 && ( (len >= 2 && (!ft_strncmp(input + len - 2, "||", 2) || !ft_strncmp(input + len - 2, "&&", 2))) || input[len - 1] == '|'))
	{
		more = readline("> ");
		if (g_signal == SIGINT)
		{
			free(input);
			if (more)
				free(more);
			ctx->exit_status = 130;
			return NULL;
		}
		if (!more)
			break;
		char *tmp = ft_strjoin(input, more);
		free(input);
		free(more);
		input = tmp;
		len = ft_strlen(input);
	}
	if (*input)
		add_history(input);
	return (input);
}
