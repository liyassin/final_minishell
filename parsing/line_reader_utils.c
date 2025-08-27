/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   line_reader_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 00:25:00 by anassih           #+#    #+#             */
/*   Updated: 2025/08/28 00:22:49 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"

char	*handle_eof(int *should_exit)
{
	ft_putendl_fd("exit", STDOUT_FILENO);
	*should_exit = 1;
	return (NULL);
}

char	*handle_empty_input(char *input, char *trimmed, t_context *ctx)
{
	free(input);
	free(trimmed);
	ctx->exit_status = 0;
	return (NULL);
}

char	*handle_quote_error(char *input, t_context *ctx)
{
	ft_putstr_fd("minishell: syntax error: unmatched quote\n",
		STDERR_FILENO);
	ctx->exit_status = 2;
	free(input);
	return (NULL);
}
