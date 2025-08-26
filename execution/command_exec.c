/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_exec.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:17:02 by anassih           #+#    #+#             */
/*   Updated: 2025/08/26 11:30:52 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"


// Skip an initial empty command argument and shift args up.
static void skip_empty_command_args(t_ast *ast)
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

static void child_exec(t_ast *ast, t_context *ctx)
{
	t_builtin	type;
	int			status;
	char		*path;

	reset_default_signals();
	if (handle_redirection(ast) < 0)
	{
		ctx->exit_status = 1;
	   // Do not free AST in child; parent will free full AST list
		free_environment(ctx->env);
		exit(ctx->exit_status);
	}
	   type = get_builtin_type(ast->args[0]);
	   if (type != NOT_BUILTIN)
	   {
			   status = handle_builtin(ast->args, ctx);
			   ctx->exit_status = status;
			   // Do not free AST in child; parent will free full AST list
			   free_environment(ctx->env);
			   exit(ctx->exit_status);
	   }
	path = find_command_path(ast->command, ctx);
	if (!path)
	{
		ctx->exit_status = 127;
	   // Do not free AST in child; parent will free full AST list
		free_environment(ctx->env);
		exit(ctx->exit_status);
	}
	execve(path, ast->args, ctx->env);
	perror("minishell");
	ctx->exit_status = 126;
	free(path);
	   // Do not free AST in child; parent will free full AST list
	free_environment(ctx->env);
	exit(ctx->exit_status);
}

void	exec_command(t_ast *ast, t_context *ctx)
{
	if (!ast || !ctx)
		return;
	skip_empty_command_args(ast);
	if (!ast->command || ast->command[0] == '\0') {
		int has_heredoc = 0;
		t_redir *redir = ast->redirs;
		while (redir) {
			if (redir->type == REDIR_HEREDOC) {
				has_heredoc = 1;
				break;
			}
			redir = redir->next;
		}
		int is_arg_empty = 1;
		if (ast->args && ast->args[0]) {
			size_t i = 0;
			while (ast->args[0][i] && ft_isspace((unsigned char)ast->args[0][i]))
				i++;
			if (ast->args[0][i] != '\0')
				is_arg_empty = 0;
		}
		if (!(ctx->exit_status == 130 || g_signal == SIGINT) && !has_heredoc && !is_arg_empty) {
			handle_command_not_found(ast->command);
			ctx->exit_status = 127;
		}
		return;
	}
	t_builtin type = get_builtin_type(ast->command);
	if (type != NOT_BUILTIN && ast->next == NULL) {
		ctx->exit_status = handle_builtin(ast->args, ctx);
		return;
	}
	// If part of a pipeline or external command, fork
	pid_t pid = fork();
	int status;
	   if (pid == 0) {
			   reset_default_signals();
			   child_exec(ast, ctx);
			   exit(ctx->exit_status);
	   } else if (pid > 0) {
			   signal(SIGINT, SIG_IGN);
			   waitpid(pid, &status, 0);
			   setup_shell_signals();
			   if (WIFEXITED(status))
					   ctx->exit_status = WEXITSTATUS(status);
			   else if (WIFSIGNALED(status)) {
					   ctx->exit_status = 128 + WTERMSIG(status);
					   write(STDOUT_FILENO, "\n", 1);
			   }
	   } else {
			   ctx->exit_status = 1;
			   perror("minishell: fork");
	   }
}
