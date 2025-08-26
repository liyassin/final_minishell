/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_dispatch.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:17:47 by anassih           #+#    #+#             */
/*   Updated: 2025/08/26 23:27:24 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"

static int	handle_pwd_builtin(char **args, t_context *ctx)
{
	char	*cwd;

	if (args[1])
	{
		cwd = getcwd(NULL, 0);
		if (cwd)
		{
			ft_putendl_fd(cwd, STDOUT_FILENO);
			free(cwd);
		}
		ft_putstr_fd("minishell: pwd:\n", STDERR_FILENO);
		ctx->exit_status = 1;
		return (0);
	}
	return (builtin_pwd(ctx));
}

int	handle_builtin(char **args, t_context *ctx)
{
	if (!args[0] || !*args[0])
		return (0);
	if (ft_strcmp(args[0], "exit") == 0)
		return (builtin_exit(args, ctx));
	else if (ft_strcmp(args[0], "cd") == 0)
		return (builtin_cd(args, ctx));
	else if (ft_strcmp(args[0], "env") == 0)
		return (builtin_env(ctx));
	else if (ft_strcmp(args[0], "pwd") == 0)
		return (handle_pwd_builtin(args, ctx));
	else if (ft_strcmp(args[0], "echo") == 0)
		return (builtin_echo(args, ctx));
	else if (ft_strcmp(args[0], "export") == 0)
		return (builtin_export(args, ctx));
	else if (ft_strcmp(args[0], "unset") == 0)
		return (builtin_unset(args, ctx));
	return (1);
}
