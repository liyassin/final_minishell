/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_exec.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:17:02 by anassih           #+#    #+#             */
/*   Updated: 2025/08/26 23:27:24 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"

static char	**create_shifted_args(char **args, int count)
{
	char	**new_args;
	int		i;

	new_args = ft_calloc(count + 1, sizeof(char *));
	if (!new_args)
		return (NULL);
	i = 0;
	while (i < count)
	{
		new_args[i] = args[i + 1];
		i++;
	}
	return (new_args);
}

static void	skip_empty_command_args(t_ast *ast)
{
	char	*empty;
	char	**new_args;
	int		count;

	if (!ast->command || ast->command[0] != '\0' || !ast->args
		|| !ast->args[0] || !ast->args[1])
		return ;
	empty = ast->args[0];
	count = 0;
	while (ast->args[count + 1])
		count++;
	new_args = create_shifted_args(ast->args, count);
	if (!new_args)
	{
		ast->command = ft_strdup("");
		return ;
	}
	free(ast->args);
	ast->args = new_args;
	free(ast->command);
	ast->command = ft_strdup(ast->args[0]);
	free(empty);
}

static int	has_heredoc_redir(t_ast *ast)
{
	t_redir	*redir;

	redir = ast->redirs;
	while (redir)
	{
		if (redir->type == REDIR_HEREDOC)
			return (1);
		redir = redir->next;
	}
	return (0);
}

static int	is_arg_empty_str(char **args)
{
	size_t	i;

	if (!args || !args[0])
		return (1);
	i = 0;
	while (args[0][i] && ft_isspace((unsigned char)args[0][i]))
		i++;
	if (args[0][i] != '\0')
		return (0);
	return (1);
}

static void	handle_empty_command(t_ast *ast, t_context *ctx)
{
	int	has_heredoc;
	int	is_arg_empty;

	has_heredoc = has_heredoc_redir(ast);
	is_arg_empty = is_arg_empty_str(ast->args);
	if (!(ctx->exit_status == 130 || g_signal == SIGINT) && !has_heredoc
		&& !is_arg_empty)
	{
		handle_command_not_found(ast->command);
		ctx->exit_status = 127;
	}
}

static void	child_exec(t_ast *ast, t_context *ctx)
{
	t_builtin	type;
	int			status;
	char		*path;

	reset_default_signals();
	if (handle_redirection(ast) < 0)
	{
		ctx->exit_status = 1;
		exit(ctx->exit_status);
	}
	type = get_builtin_type(ast->args[0]);
	if (type != NOT_BUILTIN)
	{
		status = handle_builtin(ast->args, ctx);
		ctx->exit_status = status;
		exit(ctx->exit_status);
	}
	path = find_command_path(ast->command, ctx);
	if (!path)
		handle_command_not_found(ast->command);
	execve(path, ast->args, ctx->env);
	handle_command_not_found(path);
	free(path);
	exit(ctx->exit_status);
}

static void	handle_parent_process(pid_t pid, t_context *ctx)
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

static void	handle_fork_error(t_context *ctx)
{
	ctx->exit_status = 1;
	perror("minishell: fork");
}

static void	execute_fork_process(t_ast *ast, t_context *ctx)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
	{
		reset_default_signals();
		child_exec(ast, ctx);
		exit(ctx->exit_status);
	}
	else if (pid > 0)
		handle_parent_process(pid, ctx);
	else
		handle_fork_error(ctx);
}

void	exec_command(t_ast *ast, t_context *ctx)
{
	t_builtin	type;

	if (!ast || !ctx)
		return ;
	skip_empty_command_args(ast);
	if (!ast->command || ast->command[0] == '\0')
	{
		handle_empty_command(ast, ctx);
		return ;
	}
	type = get_builtin_type(ast->command);
	if (type != NOT_BUILTIN && ast->next == NULL && !ast->redirs)
	{
		ctx->exit_status = handle_builtin(ast->args, ctx);
		return ;
	}
	execute_fork_process(ast, ctx);
}
