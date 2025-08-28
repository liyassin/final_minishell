/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_executor.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:15:30 by anassih           #+#    #+#             */
/*   Updated: 2025/08/28 02:42:37 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"
#include "../includes/cleanup.h"
#include <signal.h>

// Get the nth node from the AST linked list
static t_ast	*get_nth_node(t_ast *ast, int n)
{
	int		i;
	t_ast	*cur;

	cur = ast;
	i = 0;
	while (cur && i < n)
	{
		cur = cur->next;
		i++;
	}
	return (cur);
}

static int	fork_single_child(t_ast *ast, t_context *ctx,
		t_pipeline_data *data, int i)
{
	t_ast	*cur;

	cur = get_nth_node(ast, i);
	if (!cur)
		return (-1);
	data->pids[i] = fork();
	if (data->pids[i] == 0)
	{
		reset_default_signals();
		setup_pipes(i, data->n, data->pipes);
		close_pipes(data->n, data->pipes);
		exec_node(cur, ctx);
	}
	else if (data->pids[i] < 0)
	{
		ctx->exit_status = 1;
		perror("minishell: fork");
		return (-1);
	}
	return (0);
}

int	fork_children(t_ast *ast, t_context *ctx, t_pipeline_data *data)
{
	int	i;

	i = data->n - 1;
	while (i >= 0)
	{
		if (fork_single_child(ast, ctx, data, i) == -1)
			return (-1);
		i--;
	}
	return (0);
}

// Execute an AST pipeline by forking and wiring pipes.
void	execute_pipeline(t_ast *ast, t_context *ctx)
{
	int				n;
	int				(*pipes)[2];
	pid_t			*pids;
	t_pipeline_data	data;

	if (!ast || !ctx)
		return ;
	ctx->in_pipeline = 1;
	n = count_pipeline(ast);
	if (n <= 0)
	{
		ctx->in_pipeline = 0;
		return ;
	}
	if (setup_pipeline_resources(n, &pipes, &pids) < 0)
		return ;
	data.n = n;
	data.pipes = pipes;
	data.pids = pids;
	execute_pipeline_main(ast, ctx, &data);
	ctx->in_pipeline = 0;
}
