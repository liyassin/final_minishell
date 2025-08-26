/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:16:45 by anassih           #+#    #+#             */
/*   Updated: 2025/08/26 23:27:21 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"

static void	handle_directory_redirection_error(t_redir *r, t_context *ctx)
{
	struct stat	st;

	if ((r->type == REDIR_IN || r->type == REDIR_OUT
			|| r->type == REDIR_APPEND) && r->target
		&& stat(r->target, &st) == 0 && S_ISDIR(st.st_mode))
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(r->target, STDERR_FILENO);
		ft_putstr_fd(": Is a directory\n", STDERR_FILENO);
	}
	ctx->exit_status = 1;
}

static void	handle_empty_head(t_ast *head, t_context *ctx)
{
	if (head->redirs)
		handle_directory_redirection_error(head->redirs, ctx);
	else
		ctx->exit_status = 0;
}

void	execute_if_needed(t_ast *head, t_context *ctx, int *should_exit)
{
	t_builtin	type;
	int			ret;

	if (!head)
	{
		handle_empty_head(head, ctx);
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
