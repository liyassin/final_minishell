/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_utils_syntax.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 15:30:00 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 05:38:25 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/tokenization.h"

int	syntax_error_near_token(char *token)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	if (token)
		ft_putstr_fd(token, 2);
	else
		ft_putstr_fd("newline", 2);
	ft_putstr_fd("'\n", 2);
	return (0);
}

int	validate_pipe_syntax(char **tokens, int i)
{
	if (i == 0)
		return (syntax_error_near_token(tokens[i]));
	if (!tokens[i + 1] || is_operator(tokens[i + 1]))
		return (syntax_error_near_token(tokens[i + 1]));
	return (1);
}

int	validate_redirection_syntax(char **tokens, int i)
{
	if (!tokens[i + 1])
		return (syntax_error_near_token(NULL));
	if (is_operator(tokens[i + 1]))
		return (syntax_error_near_token(tokens[i + 1]));
	return (1);
}

int	check_pipe_start(char **tokens, t_context *ctx)
{
	if (!ft_strcmp(tokens[0], "|"))
	{
		ft_putstr_fd("bash: syntax error near unexpected token `|'\n",
			STDERR_FILENO);
		ctx->exit_status = 2;
		return (0);
	}
	return (1);
}

int	is_triple_pipe_error(char **tokens, int i, t_context *ctx)
{
	if (!ft_strcmp(tokens[i], "|||"))
	{
		ft_putstr_fd("bash: syntax error near unexpected token `|'\n",
			STDERR_FILENO);
		ctx->exit_status = 2;
		return (1);
	}
	if (!ft_strcmp(tokens[i], "|") && tokens[i + 1]
		&& !ft_strcmp(tokens[i + 1], "|") && tokens[i + 2]
		&& !ft_strcmp(tokens[i + 2], "|"))
	{
		ft_putstr_fd("bash: syntax error near unexpected token `|'\n",
			STDERR_FILENO);
		ctx->exit_status = 2;
		return (1);
	}
	return (0);
}
