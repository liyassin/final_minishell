/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_executor.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:15:30 by anassih           #+#    #+#             */
/*   Updated: 2025/08/26 11:45:16 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"
#include <signal.h>

// Execute one AST node in child: redirs, builtins, or external.
static void	exec_node(t_ast *node, t_context *ctx)
{
	t_builtin	type;
	char		*path;

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
	execve(path, node->args, ctx->env);
	perror("minishell");
	free(path);
	exit(126);
}

static int	fork_children(t_ast *ast, t_context *ctx,
	int n, int pipes[][2], pid_t *pids)
{
	int		idx;
	t_ast	*cur;
	t_ast	**nodes;
	int		i;

	nodes = (t_ast **)malloc(sizeof(t_ast *) * n);
	if (!nodes)
		return (-1);
	cur = ast;
	idx = 0;
	while (cur && idx < n)
	{
		nodes[idx] = cur;
		idx++;
		cur = cur->next;
	}
	i = n - 1;
	while (i >= 0)
	{
		pids[i] = fork();
		if (pids[i] == 0)
		{
			reset_default_signals();
			setup_pipes(i, n, pipes);
			close_pipes(n, pipes);
			exec_node(nodes[i], ctx);
		}
		else if (pids[i] < 0)
		{
			ctx->exit_status = 1;
			perror("minishell: fork");
			free(nodes);
			return (-1);
		}
		i--;
	}
	free(nodes);
	return (0);
}

// Wait for all children, store only last exit status.
static void	wait_pipeline(int n, pid_t *pids, t_context *ctx)
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

// Execute an AST pipeline by forking and wiring pipes.
void	execute_pipeline(t_ast *ast, t_context *ctx)
{
	int		n;
	int		(*pipes)[2];
	pid_t	*pids;

	if (!ast || !ctx)
		return ;
	n = count_pipeline(ast);
	if (n <= 0)
		return ;
	// Print debug after confirming pipeline will run
	// ...existing code...
	if (open_pipes(n, &pipes) < 0)
		return ;
	pids = alloc_pids(n);
	if (!pids)
	{
		free(pipes);
		return ;
	}
	if (fork_children(ast, ctx, n, pipes, pids) < 0)
	{
		free(pipes);
		free(pids);
		return ;
	}
	close_pipes(n, pipes);
	free(pipes);
	wait_pipeline(n, pids, ctx);
	free(pids);
}
