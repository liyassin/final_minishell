/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:13:43 by anassih           #+#    #+#             */
/*   Updated: 2025/08/26 11:45:16 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"

extern volatile sig_atomic_t g_signal;

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

static void	write_heredoc_line(t_redir *r, char *line, t_context *ctx)
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

static void	close_remaining_heredocs(t_ast *head)
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

void	read_heredoc(t_redir *r, t_context *ctx)
{
	pid_t	pid;
	int		status;

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
	{
		signal(SIGINT, parent_sigint_handler);
		waitpid(pid, &status, 0);
		if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
		{
			close(r->heredoc_fd[1]);
			close(r->heredoc_fd[0]);
			r->heredoc_fd[0] = -1;
			r->heredoc_fd[1] = -1;
			ctx->exit_status = 130;
			ctx->heredoc_interrupted = 1;
			g_signal = SIGINT;
		}
		else if (g_signal == SIGINT)
		{
			close(r->heredoc_fd[1]);
			close(r->heredoc_fd[0]);
			r->heredoc_fd[0] = -1;
			r->heredoc_fd[1] = -1;
			ctx->exit_status = 130;
			ctx->heredoc_interrupted = 1;
		}
		else
		{
			close(r->heredoc_fd[1]);
			r->heredoc_fd[1] = -1;
		}
	}
	else
	{
		perror("fork");
		close(r->heredoc_fd[0]);
		close(r->heredoc_fd[1]);
		ctx->exit_status = 1;
	}
}

void	process_heredocs(t_ast *head, t_context *ctx)
{
	t_ast	*node;
	t_redir	*r;

g_signal = 0;
ctx->heredoc_interrupted = 0; // Reset flag at start
	node = head;
	   while (node)
	{
		r = node->redirs;
			   while (r)
		{
					   if (r->type == REDIR_HEREDOC)
					   {
							   read_heredoc(r, ctx);
							   if (ctx->exit_status == 130 || g_signal == SIGINT)
							   {
									   // Heredoc interrupted or finished: just return to prompt, do not exit shell
									   close_remaining_heredocs(head);
									   setup_shell_signals();
									   return ;
							   }
					   }
			r = r->next;
		}
		node = node->next;
	}
	setup_shell_signals();
}
