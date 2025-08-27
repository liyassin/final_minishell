/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:16:18 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 03:19:27 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"

static void	init_context(t_context *ctx, char **envp)
{
	ctx->env = copy_environment(envp);
	ctx->exported = copy_environment(envp);
	ctx->exit_status = 0;
	ctx->ast_head = NULL;
	ctx->heredoc_interrupted = 0;
	setup_shell_signals();
}

static void	handle_signal_status(t_context *ctx)
{
	if (g_signal == SIGINT)
	{
		ctx->exit_status = 130;
		g_signal = 0;
	}
}

static int	handle_ast_execution(t_context *ctx, int *should_exit,
		int *skip_prompt)
{
	process_heredocs(ctx->ast_head, ctx);
	if (ctx->heredoc_interrupted)
	{
		free_ast(ctx->ast_head);
		ctx->ast_head = NULL;
		ctx->heredoc_interrupted = 0;
		*skip_prompt = 1;
		return (1);
	}
	execute_if_needed(ctx->ast_head, ctx, should_exit);
	free_ast(ctx->ast_head);
	ctx->ast_head = NULL;
	if (g_signal == SIGINT)
	{
		g_signal = 0;
		return (1);
	}
	return (0);
}

static int	process_command(t_context *ctx, int *should_exit, int *skip_prompt)
{
	char	*input;

	if (*skip_prompt)
	{
		*skip_prompt = 0;
		return (1);
	}
	input = read_input(should_exit, ctx);
	handle_signal_status(ctx);
	if (!input)
		return (1);
	ctx->ast_head = build_ast_pipeline(input, ctx);
	if (!ctx->ast_head)
		return (1);
	return (handle_ast_execution(ctx, should_exit, skip_prompt));
}

static void	cleanup_context(t_context *ctx)
{
	clear_history();
	free_environment(ctx->env);
	if (ctx->exported)
		free_environment(ctx->exported);
}

int	main(int argc, char **argv, char **envp)
{
	t_context	ctx;
	int			should_exit;
	int			skip_prompt;

	(void)argc;
	(void)argv;
	init_context(&ctx, envp);
	should_exit = 0;
	skip_prompt = 0;
	while (!should_exit)
	{
		g_signal = 0;
		if (process_command(&ctx, &should_exit, &skip_prompt))
			continue ;
	}
	cleanup_context(&ctx);
	return (ctx.exit_status);
}
