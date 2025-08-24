/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:13:43 by anassih           #+#    #+#             */
/*   Updated: 2025/08/24 17:45:33 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"

static void	read_heredoc(t_redir *r, t_context *ctx)
{
	char	*line;
	char	*expanded;

	while (!g_signal)
	{
		line = readline("> ");
		if (!line)
			break ;
		if (ft_strcmp(line, r->target) == 0)
		{
			free(line);
			break ;
		}
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
		free(line);
	}
	close(r->heredoc_fd[1]);
	r->heredoc_fd[1] = -1;
}

// Process heredocs for all commands in p~/.config/nvim/lua/pluginsipeline
void	process_heredocs(t_ast *head, t_context *ctx)
{
	t_ast	*node;
	t_redir	*r;

	setup_heredoc_signals();
	g_signal = 0;
	node = head;
	while (node && !g_signal)
	{
		r = node->redirs;
		while (r && !g_signal)
		{
			if (r->type == REDIR_HEREDOC)
				read_heredoc(r, ctx);
			r = r->next;
		}
		node = node->next;
	}
	setup_shell_signals();
}
