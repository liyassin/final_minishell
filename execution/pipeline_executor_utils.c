/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_executor_utils.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 07:00:00 by anassih           #+#    #+#             */
/*   Updated: 2025/08/28 02:42:37 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	exec_node(t_ast *node, t_context *ctx)
{
	t_builtin	type;
	char		*path;

	ctx->in_child_process = 1;
	if (handle_redirection(node) < 0)
		exit(1);
	if (!node->command || !*node->command)
		exit(0);
	type = get_builtin_type(node->args[0]);
	if (type != NOT_BUILTIN)
		exit(handle_builtin(node->args, ctx));
	path = find_command_path(node->command, ctx);
	if (!path)
		handle_command_not_found(node->command);
	if (path == (char *)-1)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(node->command, STDERR_FILENO);
		ft_putstr_fd(": Permission denied\n", STDERR_FILENO);
		exit(126);
	}
	execve(path, node->args, ctx->env);
	perror("minishell");
	free(path);
	exit(126);
}

void	wait_pipeline(int n, pid_t *pids, t_context *ctx)
{
	int	i;
	int	status;

	i = 0;
	while (i < n)
	{
		if (pids[i] > 0)
		{
			waitpid(pids[i], &status, 0);
			if (i == n - 1)
			{
				if (WIFEXITED(status))
					ctx->exit_status = WEXITSTATUS(status);
				else if (WIFSIGNALED(status))
					ctx->exit_status = 128 + WTERMSIG(status);
			}
		}
		i++;
	}
}

void	cleanup_pipeline_fail(int (*pipes)[2], pid_t *pids)
{
	free(pipes);
	free(pids);
}

void	execute_pipeline_main(t_ast *ast, t_context *ctx,
	t_pipeline_data *data)
{
	if (fork_children(ast, ctx, data) < 0)
	{
		cleanup_pipeline_fail(data->pipes, data->pids);
		return ;
	}
	close_pipes(data->n, data->pipes);
	free(data->pipes);
	wait_pipeline(data->n, data->pids, ctx);
	free(data->pids);
}
