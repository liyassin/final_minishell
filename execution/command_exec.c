#include "minishell.h"
#include "signals.h"

// Skip an initial empty command argument and shift args up.
static void	skip_empty_command_args(t_ast *ast)
{
	char	*empty;
	char	**new_args;
	int		count;
	int		i;

	if (!ast->command || ast->command[0] != '\0' || !ast->args
		|| !ast->args[0] || !ast->args[1])
		return ;
	empty = ast->args[0];
	count = 0;
	while (ast->args[count + 1])
		count++;
	new_args = ft_calloc(count + 1, sizeof(char *));
	if (!new_args)
	{
		ast->command = ft_strdup("");
		return ;
	}
	i = 0;
	while (i < count)
	{
		new_args[i] = ast->args[i + 1];
		i++;
	}
	free(ast->args);
	ast->args = new_args;
	free(ast->command);
	ast->command = ft_strdup(ast->args[0]);
	free(empty);
}

// In child: set signals, apply redirs, run builtin or external.
static void	child_exec(t_ast *ast, t_context *ctx)
{
	t_builtin	type;
	int			status;
	char		*path;

	reset_default_signals();
	if (handle_redirection(ast) < 0)
		exit(EXIT_FAILURE);
	type = get_builtin_type(ast->args[0]);
	if (type != NOT_BUILTIN)
	{
		status = handle_builtin(ast->args, ctx);
		exit(status);
	}
	path = find_command_path(ast->command, ctx);
	if (!path)
		handle_command_not_found(ast->command);
	execve(path, ast->args, ctx->env);
	perror("minishell");
	free(path);
	exit(126);
}

// Execute single command
void	exec_command(t_ast *ast, t_context *ctx)
{
	pid_t	pid;
	int		status;

	if (!ast || !ctx)
		return ;
	skip_empty_command_args(ast);
	if (!ast->command || ast->command[0] == '\0')
	{
		if (handle_redirection(ast) == 0)
			ctx->exit_status = 0;
		else
			ctx->exit_status = 1;
		return ;
	}
	pid = fork();
	if (pid == 0)
		child_exec(ast, ctx);
	else if (pid > 0)
	{
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			ctx->exit_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			ctx->exit_status = 128 + WTERMSIG(status);
	}
	else
		perror("minishell: fork");
}
