/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:16:45 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 05:09:10 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"

static void	execute_command_or_pipeline(t_ast *head, t_context *ctx)
{
	if (head->next)
		execute_pipeline(head, ctx);
	else
		exec_command(head, ctx);
}

static void	handle_parent_builtin(t_ast *head, t_context *ctx, int *should_exit)
{
	t_builtin	type;
	int			ret;

	type = get_builtin_type(head->command);
	if (type == PARENT_BUILTIN && !head->next)
	{
		ret = handle_builtin(head->args, ctx);
		if (ret == 255)
			*should_exit = 1;
	}
	else
		execute_command_or_pipeline(head, ctx);
}

void	execute_if_needed(t_ast *head, t_context *ctx, int *should_exit)
{
	if (!head)
	{
		handle_empty_head(head, ctx);
		return ;
	}
	if (!head->command && head->redirs)
	{
		handle_redirection_only(head, ctx);
		return ;
	}
	handle_parent_builtin(head, ctx, should_exit);
}
