/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_executor.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:15:30 by anassih           #+#    #+#             */
/*   Updated: 2025/08/24 18:00:28 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"

// Execute one AST node in child: redirs, builtins, or external.
static void	exec_node(t_ast *node, t_context *ctx)
{
	t_builtin	type;
	char		*path;

	if (handle_redirection(node) < 0)
		exit(EXIT_FAILURE);
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

// Fork each stage: child sets up pipes and exec_node(), parent records pid.
static int	fork_children(t_ast *ast, t_context *ctx,
	int n, int pipes[][2], pid_t *pids)
{

		// Collect nodes in array for right-to-left forking
		t_ast *nodes[n];
		t_ast *cur = ast;
		int idx = 0;
		while (cur && idx < n) {
			nodes[idx++] = cur;
			cur = cur->next;
		}

		for (int i = n - 1; i >= 0; i--) {
			pids[i] = fork();
			if (pids[i] == 0) {
				reset_default_signals();
				setup_pipes(i, n, pipes);
				close_pipes(n, pipes);
				exec_node(nodes[i], ctx);
			} else if (pids[i] < 0) {
				ctx->exit_status = 1;
				perror("minishell: fork");
				return (-1);
			}
		}
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
