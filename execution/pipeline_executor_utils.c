/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_executor_utils.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 07:00:00 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 07:05:36 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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
