/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_setup.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:15:04 by anassih           #+#    #+#             */
/*   Updated: 2025/08/23 18:15:20 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"

// Count how many commands in the pipeline.
int	count_pipeline(t_ast *ast)
{
	t_ast	*it;
	int		n;

	it = ast;
	n = 0;
	while (it)
	{
		n++;
		it = it->next;
	}
	return (n);
}

// Allocate and open (n-1) pipes. On error, free and return -1.
int	open_pipes(int n, int (**pipes)[2])
{
	int	(*p)[2];
	int	i;

	p = ft_calloc(n - 1, sizeof(int[2]));
	if (!p)
		return (perror("minishell: malloc"), -1);
	i = 0;
	while (i < n - 1)
	{
		if (pipe(p[i]) < 0)
			return (perror("minishell: pipe"), free(p), -1);
		i++;
	}
	*pipes = p;
	return (0);
}

// Allocate pid array or perror on fail.
pid_t	*alloc_pids(int n)
{
	pid_t	*pids;

	pids = ft_calloc(n, sizeof(pid_t));
	if (!pids)
		perror("minishell: malloc");
	return (pids);
}

// Set up dup2 for child i in a pipeline of n.
void	setup_pipes(int i, int n, int pipes[][2])
{
	if (i > 0)
		dup2(pipes[i - 1][0], STDIN_FILENO);
	if (i < n - 1)
		dup2(pipes[i][1], STDOUT_FILENO);
}

// Close all pipe file descriptors.
void	close_pipes(int n, int pipes[][2])
{
	int	j;

	j = 0;
	while (j < n - 1)
	{
		close(pipes[j][0]);
		close(pipes[j][1]);
		j++;
	}
}
