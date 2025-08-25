/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:14:39 by anassih           #+#    #+#             */
/*   Updated: 2025/08/25 04:14:39 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"

// Redirection helpers: return 0 on success, -1 on error
static int	redirect_input(const char *file)
{
	int	fd;

	fd = open(file, O_RDONLY);
	if (fd < 0)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd((char *)file, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		perror(NULL);
		return (-1);
	}
	if (dup2(fd, STDIN_FILENO) < 0)
	{
		perror("minishell");
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}

static int	redirect_output(const char *file, int flags)
{
	int	fd;

	fd = open(file, flags, 0644);
	if (fd < 0)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd((char *)file, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		perror(NULL);
		return (-1);
	}
	if (dup2(fd, STDOUT_FILENO) < 0)
	{
		perror("minishell");
		close(fd);
		return (-1);
	}
	close(fd);
	return (0);
}

// Unified redirection handling: returns 0 on success, -1 on failure
int	handle_redirection(t_ast *ast)
{
	t_redir	*r;

	if (!ast)
		return (-1);
	r = ast->redirs;
	while (r)
	{
		if (r->type == REDIR_IN)
		{
			if (redirect_input(r->target) < 0)
				return (-1);
		}
		else if (r->type == REDIR_OUT)
		{
			if (redirect_output(r->target, O_WRONLY | O_CREAT | O_TRUNC) < 0)
				return (-1);
		}
		else if (r->type == REDIR_APPEND)
		{
			if (redirect_output(r->target, O_WRONLY | O_CREAT | O_APPEND) < 0)
				return (-1);
		}
		else if (r->type == REDIR_HEREDOC)
		{
			if (r->heredoc_fd[0] == -1)
				return (-1);
			if (dup2(r->heredoc_fd[0], STDIN_FILENO) < 0)
				return (perror("minishell"), -1);
			close(r->heredoc_fd[0]);
		}
		r = r->next;
	}
	return (0);
}
