/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_utils_helpers.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 15:30:00 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 05:38:25 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/tokenization.h"

int	is_operator(char *token)
{
	if (!token)
		return (0);
	if (!ft_strcmp(token, "|") || !ft_strcmp(token, ">>")
		|| !ft_strcmp(token, "<<") || !ft_strcmp(token, ">")
		|| !ft_strcmp(token, "<"))
		return (1);
	return (0);
}

int	is_invalid_redir_token(char **tokens, int i, t_context *ctx)
{
	if ((tokens[i][0] == '>' || tokens[i][0] == '<'))
	{
		if (ft_strcmp(tokens[i], ">") && ft_strcmp(tokens[i], ">>")
			&& ft_strcmp(tokens[i], "<") && ft_strcmp(tokens[i], "<<"))
		{
			ft_putstr_fd("bash: syntax error near unexpected token `>'\n",
				STDERR_FILENO);
			ctx->exit_status = 2;
			return (1);
		}
	}
	return (0);
}

void	add_redir_to_list(t_redir ***tail, t_redir_type type, char *target,
		t_context *ctx)
{
	**tail = create_redir_node(type, target, ctx);
	if (**tail)
		*tail = &(**tail)->next;
}

int	process_redirection_token(t_redir ***tail, char **tokens, int i,
		t_context *ctx)
{
	if (!ft_strcmp(tokens[i], "<<") && tokens[i + 1])
	{
		add_redir_to_list(tail, REDIR_HEREDOC, tokens[i + 1], ctx);
		return (i + 2);
	}
	else if (!ft_strcmp(tokens[i], ">>") && tokens[i + 1])
	{
		add_redir_to_list(tail, REDIR_APPEND, tokens[i + 1], ctx);
		return (i + 2);
	}
	else if (!ft_strcmp(tokens[i], ">") && tokens[i + 1])
	{
		add_redir_to_list(tail, REDIR_OUT, tokens[i + 1], ctx);
		return (i + 2);
	}
	else if (!ft_strcmp(tokens[i], "<") && tokens[i + 1])
	{
		add_redir_to_list(tail, REDIR_IN, tokens[i + 1], ctx);
		return (i + 2);
	}
	return (i + 1);
}

int	skip_redirection_tokens(char **tokens, int i)
{
	if (!ft_strcmp(tokens[i], "<<") && tokens[i + 1])
		return (i + 2);
	else if (!ft_strcmp(tokens[i], ">>") && tokens[i + 1])
		return (i + 2);
	else if (!ft_strcmp(tokens[i], ">") && tokens[i + 1])
		return (i + 2);
	else if (!ft_strcmp(tokens[i], "<") && tokens[i + 1])
		return (i + 2);
	else if (!ft_strcmp(tokens[i], "|"))
		return (i + 1);
	return (-1);
}
