/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 00:00:00 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 06:15:16 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	write_heredoc_line(t_redir *r, char *line, t_context *ctx)
{
	char	*expanded;

	if (!r->quoted)
	{
		expanded = expand_env_vars(line, ctx->env, ctx->exit_status);
		write(r->heredoc_fd[1], expanded, ft_strlen(expanded));
		write(r->heredoc_fd[1], "\n", 1);
		free(expanded);
	}
	else
	{
		write(r->heredoc_fd[1], line, ft_strlen(line));
		write(r->heredoc_fd[1], "\n", 1);
	}
}

void	close_remaining_heredocs(t_ast *head)
{
	t_ast	*node;
	t_redir	*r;

	node = head;
	while (node)
	{
		r = node->redirs;
		while (r)
		{
			if (r->type == REDIR_HEREDOC)
			{
				if (r->heredoc_fd[0] != -1)
				{
					close(r->heredoc_fd[0]);
					r->heredoc_fd[0] = -1;
				}
				if (r->heredoc_fd[1] != -1)
				{
					close(r->heredoc_fd[1]);
					r->heredoc_fd[1] = -1;
				}
			}
			r = r->next;
		}
		node = node->next;
	}
}
