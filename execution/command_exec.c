/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_exec.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:17:02 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 05:38:25 by anassih          ###   ########.fr       */
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
