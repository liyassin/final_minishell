/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_exec_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 15:30:00 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 06:54:03 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	setup_child_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

void	handle_exec_failure(char *command, char *full_path, t_context *ctx)
{
	if (full_path)
	{
		free(full_path);
		perror(command);
		ctx->exit_status = 126;
	}
	else
		handle_command_not_found(command);
}

int	should_search_path(char *command)
{
	return (!ft_strchr(command, '/'));
}

int	has_heredoc_redir(t_ast *ast)
{
	t_redir	*redir;

	redir = ast->redirs;
	while (redir)
	{
		if (redir->type == REDIR_HEREDOC)
			return (1);
		redir = redir->next;
	}
	return (0);
}

int	is_arg_empty_str(char **args)
{
	size_t	i;

	if (!args || !args[0])
		return (1);
	i = 0;
	while (args[0][i] && ft_isspace((unsigned char)args[0][i]))
		i++;
	if (args[0][i] != '\0')
		return (0);
	return (1);
}

int	is_numeric_arg(char *arg)
{
	int	i;

	if (arg[0] == '+' || arg[0] == '-')
		i = 1;
	else
		i = 0;
	if (!arg[i])
		return (0);
	while (arg[i])
	{
		if (!ft_isdigit(arg[i]))
			return (0);
		i++;
	}
	return (1);
}
