/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:16:18 by anassih           #+#    #+#             */
/*   Updated: 2025/08/26 05:25:40 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"

int	main(int argc, char **argv, char **envp)
{
	t_context	ctx;
	t_ast		*head;
	char		*input;
	int			should_exit;

	(void)argc;
	(void)argv;
	ctx.env = copy_environment(envp);
	ctx.exit_status = 0;
	should_exit = 0;
	setup_shell_signals();
	while (!should_exit)
	{
		g_signal = 0;
		input = read_input(&should_exit, &ctx);
		if (!input)
			continue ;
		head = build_ast_pipeline(input, &ctx);
		if (!head)
			continue ;
		process_heredocs(head, &ctx);
		if (ctx.heredoc_interrupted)
		{
			// heredoc interrupted, skip command execution, preserve 130 for this prompt
			free_ast(head);
			ctx.heredoc_interrupted = 0;
			continue;
		}
		execute_if_needed(head, &ctx, &should_exit);
		free_ast(head);
	}
	clear_history();
	free_environment(ctx.env);
	return (ctx.exit_status);
}