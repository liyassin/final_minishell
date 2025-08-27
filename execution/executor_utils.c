/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 15:30:00 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 06:15:16 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	handle_directory_redirection_error(t_redir *r, t_context *ctx)
{
	struct stat	st;

	if ((r->type == REDIR_IN || r->type == REDIR_OUT
			|| r->type == REDIR_APPEND) && r->target
		&& stat(r->target, &st) == 0 && S_ISDIR(st.st_mode))
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(r->target, STDERR_FILENO);
		ft_putstr_fd(": Is a directory\n", STDERR_FILENO);
	}
	ctx->exit_status = 1;
}

void	consume_stdin_and_discard(void)
{
	char	buffer[4096];
	ssize_t	bytes_read;

	bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));
	while (bytes_read > 0)
		bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));
}

void	handle_empty_head(t_ast *head, t_context *ctx)
{
	if (head->redirs)
		handle_directory_redirection_error(head->redirs, ctx);
	else
		ctx->exit_status = 0;
}

void	handle_redirection_only(t_ast *head, t_context *ctx)
{
	int	saved_stdin;

	saved_stdin = dup(STDIN_FILENO);
	if (handle_redirection(head) < 0)
	{
		close(saved_stdin);
		ctx->exit_status = 1;
	}
	else
	{
		consume_stdin_and_discard();
		dup2(saved_stdin, STDIN_FILENO);
		close(saved_stdin);
		ctx->exit_status = 0;
	}
}

int	setup_pipeline_resources(int n, int (**pipes)[2], pid_t **pids)
{
	if (open_pipes(n, pipes) < 0)
		return (-1);
	*pids = alloc_pids(n);
	if (!*pids)
	{
		free(*pipes);
		return (-1);
	}
	return (0);
}
