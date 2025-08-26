/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child_builtins.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:23:05 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 00:16:20 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/builtins.h"
#include "../includes/minishell.h"

// env builtin: print all VAR=VALUE entries.
int	builtin_env(t_context *ctx)
{
	int		i;
	char	*eq;

	if (!ctx->env)
	{
		ft_putstr_fd("env: environment not initialized\n", STDERR_FILENO);
		ctx->exit_status = 1;
		return (1);
	}
	i = 0;
	while (ctx->env[i])
	{
		eq = ft_strchr(ctx->env[i], '=');
		if (eq)
			ft_putendl_fd(ctx->env[i], STDOUT_FILENO);
		i++;
	}
	ctx->exit_status = 0;
	return (0);
}

// pwd builtin: print current working directory.
int	builtin_pwd(t_context *ctx)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (cwd)
	{
		ft_putendl_fd(cwd, STDOUT_FILENO);
		free(cwd);
		ctx->exit_status = 0;
		return (0);
	}
	ft_putstr_fd("pwd: ", STDERR_FILENO);
	perror("minishell: pwd");
	ctx->exit_status = 1;
	return (1);
}

static int	parse_echo_flags(char **args, int *i, int *newline, int *escape)
{
	int	j;
	int	valid;

	while (args[*i] && args[*i][0] == '-')
	{
		j = 1;
		valid = 1;
		while (args[*i][j] && valid)
			if (args[*i][j++] != 'n' && args[*i][j - 1] != 'e')
				valid = 0;
		if (!valid)
			break ;
		j = 1;
		while (args[*i][j])
		{
			if (args[*i][j] == 'n')
				*newline = 0;
			else if (args[*i][j] == 'e')
				*escape = 1;
			j++;
		}
		(*i)++;
	}
	return (*i);
}

static void	print_echo_arg(char *arg, int escape)
{
	int		j;
	char	*pos;

	j = 0;
	while (arg[j])
	{
		if (escape && arg[j] == '\\' && arg[j + 1])
		{
			j++;
			pos = ft_strchr("ntrb", arg[j]);
			if (pos)
				write(1, &"\n\t\r\b"[pos - "ntrb"], 1);
			else if (arg[j] == 'v')
				write(1, "\v", 1);
			else if (arg[j] == 'a')
				write(1, "\a", 1);
			else
				write(1, "\\", 1);
		}
		else
			write(1, &arg[j], 1);
		j++;
	}
}

int	builtin_echo(char **args, t_context *ctx)
{
	int	i;
	int	newline;
	int	escape;

	(void)ctx;
	i = 1;
	newline = 1;
	escape = 0;
	parse_echo_flags(args, &i, &newline, &escape);
	while (args[i])
	{
		print_echo_arg(args[i], escape);
		if (args[i + 1])
			write(1, " ", 1);
		i++;
	}
	if (newline)
		write(1, "\n", 1);
	return (0);
}
