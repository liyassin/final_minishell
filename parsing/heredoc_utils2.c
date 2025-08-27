/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 05:45:00 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 06:54:03 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"

void	handle_interrupted_heredoc(t_redir *r, t_context *ctx)
{
	close(r->heredoc_fd[1]);
	r->heredoc_fd[1] = -1;
	ctx->heredoc_interrupted = 1;
	ctx->exit_status = 130;
}

void	handle_successful_heredoc(t_redir *r)
{
	close(r->heredoc_fd[1]);
	r->heredoc_fd[1] = -1;
}

static int	process_node_heredocs(t_ast *node, t_context *ctx, t_ast *head)
{
	t_redir	*r;

	r = node->redirs;
	while (r)
	{
		if (r->type == REDIR_HEREDOC)
		{
			read_heredoc(r, ctx);
			if (ctx->exit_status == 130 || g_signal == SIGINT)
			{
				close_remaining_heredocs(head);
				setup_shell_signals();
				return (1);
			}
		}
		r = r->next;
	}
	return (0);
}

void	process_heredocs(t_ast *head, t_context *ctx)
{
	t_ast	*node;

	g_signal = 0;
	ctx->heredoc_interrupted = 0;
	node = head;
	while (node && !ctx->heredoc_interrupted)
	{
		if (process_node_heredocs(node, ctx, head))
			break ;
		node = node->next;
	}
}
