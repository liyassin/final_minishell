/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 16:37:57 by anassih           #+#    #+#             */
/*   Updated: 2025/08/28 02:35:52 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/builtins.h"
#include "../includes/minishell.h"

// exit builtin: print exit, validate args, set ctx->exit_status,
// return 255 to signal shell should terminate.
static int	exit_too_many_args(t_context *ctx)
{
	ft_putstr_fd("exit: too many arguments\n", STDERR_FILENO);
	ctx->exit_status = 1;
	return (1);
}

static int	exit_numeric_error(char *arg, t_context *ctx)
{
	ft_putstr_fd("exit: ", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
	ctx->exit_status = 2;
	return (2);
}

static int	exit_is_numeric(char *arg)
{
	int	i;

	if (arg[0] == '+' || arg[0] == '-')
		i = 1;
	else
		i = 0;
	if (!arg[i])
		return (0);
	while (arg[i])
	{
		if (!ft_isdigit((unsigned char)arg[i]))
			return (0);
		i++;
	}
	return (1);
}

int	builtin_exit(char **args, t_context *ctx)
{
	long	code;

	if (!ctx)
		return (1);
	code = (long)ctx->exit_status;
	if (!ctx->in_child_process)
		ft_putendl_fd("exit", STDOUT_FILENO);
	if (args[1])
	{
		if (args[2])
			return (exit_too_many_args(ctx));
		if (!exit_is_numeric(args[1]))
			return (exit_numeric_error(args[1], ctx));
		code = ft_atoi(args[1]);
		ctx->exit_status = (int)code;
	}
	return (255);
}
