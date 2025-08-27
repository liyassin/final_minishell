/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_exec_process_utils.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 07:39:44 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 07:42:13 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	handle_fork_error(t_context *ctx)
{
	ctx->exit_status = 1;
	perror("minishell: fork");
}

void	execute_fork_process(t_ast *ast, t_context *ctx)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
	{
		reset_default_signals();
		child_exec(ast, ctx);
		exit(ctx->exit_status);
	}
	else if (pid > 0)
		handle_parent_process(pid, ctx);
	else
		handle_fork_error(ctx);
}
