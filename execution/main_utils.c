/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 05:35:00 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 05:38:25 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	init_context(t_context *ctx, char **envp)
{
	ctx->env = copy_environment(envp);
	ctx->exported = copy_environment(envp);
	ctx->exit_status = 0;
	ctx->ast_head = NULL;
	ctx->heredoc_interrupted = 0;
	setup_shell_signals();
}
