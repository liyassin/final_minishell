/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:13:43 by anassih           #+#    #+#             */
/*   Updated: 2025/08/25 03:23:56 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"

/*
** Handles SIGINT in heredoc child process.
** Prints a newline and exits with status 1.
*/
static void	heredoc_sigint_handler(int sig)
{
	(void)sig;
	write(2, "\n", 1);
	exit(1);
}

/*
** Writes a line to the heredoc file descriptor.
** Expands environment variables if not quoted.
*/
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

/*
** Child process for heredoc input.
** Sets SIGINT handler and reads lines until delimiter is found.
*/
static void	heredoc_child_process(t_redir *r, t_context *ctx)
{
	struct sigaction	sa;
	char				*line;

	sa.sa_handler = heredoc_sigint_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	while (1)
	{
		line = readline("> ");
		if (!line)
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

/*
** Forks a child to read heredoc input.
** Parent ignores SIGINT and sets exit status on interruption.
*/
void	read_heredoc(t_redir *r, t_context *ctx)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == 0)
		heredoc_child_process(r, ctx);
	else if (pid > 0)
	{
		signal(SIGINT, SIG_IGN);
		waitpid(pid, &status, 0);
		setup_shell_signals();
		if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
		{
			close(r->heredoc_fd[1]);
			close(r->heredoc_fd[0]);
			r->heredoc_fd[0] = -1;
			r->heredoc_fd[1] = -1;
			ctx->exit_status = 130;
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
		ctx->exit_status = 1;
	}
}

/*
** Processes all heredoc redirections in the AST.
** Stops on SIGINT interruption.
*/
void	process_heredocs(t_ast *head, t_context *ctx)
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
				read_heredoc(r, ctx);
				if (ctx->exit_status == 130)
					return ;
			}
			r = r->next;
		}
		node = node->next;
	}
}