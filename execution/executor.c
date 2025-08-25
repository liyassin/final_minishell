/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:16:45 by anassih           #+#    #+#             */
/*   Updated: 2025/08/25 04:14:34 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"

void	execute_if_needed(t_ast *head, t_context *ctx, int *should_exit)
{
	t_builtin	type;
	int		ret;


	if (!head)
		return ;
	if (!head->command)
	{
		if (head->redirs)
		{
			t_redir *r;
			struct stat st;

			r = head->redirs;
			// Only check for directory error for file redirections, not heredoc
			if ((r->type == REDIR_IN || r->type == REDIR_OUT || r->type == REDIR_APPEND)
				&& r->target && stat(r->target, &st) == 0 && S_ISDIR(st.st_mode))
			{
				ft_putstr_fd("minishell: ", STDERR_FILENO);
				ft_putstr_fd(r->target, STDERR_FILENO);
				ft_putstr_fd(": Is a directory\n", STDERR_FILENO);
			}
			ctx->exit_status = 1;
		}
		else
		{
			ctx->exit_status = 0;
		}
		return ;
	}
	type = get_builtin_type(head->command);
	if (type == PARENT_BUILTIN && !head->next)
	{
		ret = handle_builtin(head->args, ctx);
		if (ret == 255)
			*should_exit = 1;
	}
	else
	{
		if (head->next)
			execute_pipeline(head, ctx);
		else
			exec_command(head, ctx);
	}
}
