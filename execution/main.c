/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:16:18 by anassih           #+#    #+#             */
/*   Updated: 2025/08/25 04:46:04 by anassih          ###   ########.fr       */
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
	should_exit = 0;
	setup_shell_signals();
	while (!should_exit)
	{
		g_signal = 0;
		input = read_input(&should_exit, &ctx);
		if (!input)
			continue ;
		// Only reset exit_status to 0 if a real command is entered and previous status is not 130
		if (input[0] != '\0' && ctx.exit_status != 130)
			ctx.exit_status = 0;
		head = build_ast_pipeline(input, &ctx);
		if (!head)
			continue ;
		process_heredocs(head, &ctx);
	// ...existing code...
		if (g_signal == SIGINT)
		{
			ctx.exit_status = 130;
			// ...existing code...
			free_ast(head);
			continue;
		}
		execute_if_needed(head, &ctx, &should_exit);
	// ...existing code...
		free_ast(head);
	}
	clear_history();
	free_environment(ctx.env);
	return (ctx.exit_status);
}
