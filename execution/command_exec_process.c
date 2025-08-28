/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_exec_process.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 05:30:00 by anassih           #+#    #+#             */
/*   Updated: 2025/08/28 02:05:14 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"
#include "../includes/cleanup.h"

static void	handle_child_exit(char **args, t_context *ctx)
{
	long	code;

	code = (long)ctx->exit_status;
	ft_putendl_fd("exit", STDOUT_FILENO);
	if (args[1])
	{
		if (args[2])
		{
			ft_putstr_fd("exit: too many arguments\n", STDERR_FILENO);
			ctx->exit_status = 1;
			exit(1);
		}
		if (!is_numeric_arg(args[1]))
		{
			ft_putstr_fd("exit: ", STDERR_FILENO);
			ft_putstr_fd(args[1], STDERR_FILENO);
			ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
			ctx->exit_status = 2;
			exit(2);
		}
		code = ft_atoi(args[1]);
		ctx->exit_status = (int)code;
	}
	exit(ctx->exit_status);
}

static void	handle_child_builtin(t_ast *ast, t_context *ctx)
{
	t_builtin	type;
	int			status;

	type = get_builtin_type(ast->args[0]);
	if (type != NOT_BUILTIN)
	{
		if (!ft_strcmp(ast->args[0], "exit"))
			handle_child_exit(ast->args, ctx);
		status = handle_builtin(ast->args, ctx);
		ctx->exit_status = status;
		exit(ctx->exit_status);
	}
}

void	child_exec(t_ast *ast, t_context *ctx)
{
	char	*path;

	reset_default_signals();
	if (handle_redirection(ast) < 0)
	{
		ctx->exit_status = 1;
		exit(ctx->exit_status);
	}
	handle_child_builtin(ast, ctx);
	path = find_command_path(ast->command, ctx);
	if (!path)
		handle_command_not_found(ast->command);
	if (path == (char *)-1)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(ast->command, STDERR_FILENO);
		ft_putstr_fd(": Permission denied\n", STDERR_FILENO);
		exit(126);
	}
	execve(path, ast->args, ctx->env);
	handle_command_not_found(path);
	free(path);
	exit(ctx->exit_status);
}

void	handle_parent_process(pid_t pid, t_context *ctx)
{
	int	status;

	signal(SIGINT, SIG_IGN);
	waitpid(pid, &status, 0);
	setup_shell_signals();
	if (WIFEXITED(status))
		ctx->exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
	{
		ctx->exit_status = 128 + WTERMSIG(status);
		write(STDOUT_FILENO, "\n", 1);
	}
}
