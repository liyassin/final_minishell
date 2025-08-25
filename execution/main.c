/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:16:18 by anassih           #+#    #+#             */
/*   Updated: 2025/08/25 03:23:27 by anassih          ###   ########.fr       */
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
		input = read_input(&should_exit, &ctx);
		if (!input)
			continue ;
		head = build_ast_pipeline(input, &ctx);
		if (!head)
			continue ;
		process_heredocs(head, &ctx);
		execute_if_needed(head, &ctx, &should_exit);
		free_ast(head);
	}
	clear_history();
	free_environment(ctx.env);
	return (ctx.exit_status);
}
