/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:16:18 by anassih           #+#    #+#             */
/*   Updated: 2025/08/26 11:45:16 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../includes/minishell.h"
#include "../includes/signals.h"

int	main(int argc, char **argv, char **envp)
{
	t_context	ctx;
// t_ast *head; // replaced by ctx.ast_head
	char		*input;
	int			should_exit;

	(void)argc;
	(void)argv;
	   ctx.env = copy_environment(envp);
	   ctx.exported = NULL;
	   ctx.exit_status = 0;
	should_exit = 0;
	setup_shell_signals();
	ctx.ast_head = NULL;
	int         sigint_received = 0;
	int         skip_prompt = 0;
	while (!should_exit)
	{
		g_signal = 0;
		sigint_received = 0;
		if (skip_prompt) {
			skip_prompt = 0;
			continue;
		}
		input = read_input(&should_exit, &ctx);
		if (g_signal == SIGINT)
			ctx.exit_status = 130;
		if (!input)
			continue ;
		ctx.ast_head = build_ast_pipeline(input, &ctx);
		if (!ctx.ast_head)
			continue ;
		process_heredocs(ctx.ast_head, &ctx);
		if (ctx.heredoc_interrupted)
		{
			// heredoc interrupted, skip command execution, preserve 130 for this prompt
			free_ast(ctx.ast_head);
			ctx.ast_head = NULL;
			ctx.heredoc_interrupted = 0;
			skip_prompt = 1;
			continue;
		}
		execute_if_needed(ctx.ast_head, &ctx, &should_exit);
		free_ast(ctx.ast_head);
		ctx.ast_head = NULL;
		if (g_signal == SIGINT) {
			sigint_received = 1;
			g_signal = 0;
			continue;
		}
	}
	   clear_history();
	   free_environment(ctx.env);
	   if (ctx.exported)
			   free_environment(ctx.exported);
	   return (ctx.exit_status);
}