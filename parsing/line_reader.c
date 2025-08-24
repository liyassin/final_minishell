/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   line_reader.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:13:13 by anassih           #+#    #+#             */
/*   Updated: 2025/08/23 18:13:32 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"

char	*read_input(int *should_exit, t_context *ctx)
{
	char	*input;
	char	*trimmed;

	g_signal = 0;
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
	if (*input)
		add_history(input);
	return (input);
}
