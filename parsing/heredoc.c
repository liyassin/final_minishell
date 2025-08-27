/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:13:43 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 06:54:03 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"

extern volatile sig_atomic_t	g_signal;

static void	heredoc_sigint_handler(int sig)
{
	g_signal = sig;
	write(2, "\n", 1);
	exit(1);
}

static void	parent_sigint_handler(int sig)
{
	g_signal = sig;
}

static void	heredoc_child_process(t_redir *r, t_context *ctx)
{
	struct sigaction	sa;
	char				*line;

	sa.sa_handler = heredoc_sigint_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
	while (1)
	{
		line = readline("> ");
		if (!line || g_signal == SIGINT)
			exit(0);
		if (ft_strcmp(line, r->target) == 0)
		{
			free(line);
			break ;
		}
		write_heredoc_line(r, line, ctx);
		free(line);
	}
	close(r->heredoc_fd[1]);
	exit(0);
}

static void	handle_heredoc_parent(t_redir *r, t_context *ctx, pid_t pid)
{
	int	status;

	signal(SIGINT, parent_sigint_handler);
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
	{
		handle_interrupted_heredoc(r, ctx);
		g_signal = SIGINT;
	}
	else if (g_signal == SIGINT)
	{
		handle_interrupted_heredoc(r, ctx);
	}
	else
	{
		handle_successful_heredoc(r);
	}
}

void	read_heredoc(t_redir *r, t_context *ctx)
{
	pid_t	pid;

	if (pipe(r->heredoc_fd) == -1)
	{
		perror("pipe");
		ctx->exit_status = 1;
		return ;
	}
	pid = fork();
	if (pid == 0)
		heredoc_child_process(r, ctx);
	else if (pid > 0)
		handle_heredoc_parent(r, ctx, pid);
	else
	{
		perror("fork");
		close(r->heredoc_fd[0]);
		close(r->heredoc_fd[1]);
		ctx->exit_status = 1;
	}
}
