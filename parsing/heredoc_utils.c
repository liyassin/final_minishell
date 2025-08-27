/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 00:00:00 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 06:54:03 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"

extern volatile sig_atomic_t	g_signal;

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

static void	close_node_heredocs(t_ast *node)
{
	t_redir	*r;

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
}

void	close_remaining_heredocs(t_ast *head)
{
	t_ast	*node;

	node = head;
	while (node)
	{
		close_node_heredocs(node);
		node = node->next;
	}
}
