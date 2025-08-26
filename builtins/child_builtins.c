/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child_builtins.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:23:05 by anassih           #+#    #+#             */
/*   Updated: 2025/08/26 09:36:39 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/builtins.h"
#include "../includes/minishell.h"

// env builtin: print all VAR=VALUE entries.
int	builtin_env(t_context *ctx)
{
	int i;
	char *eq;

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

int	builtin_echo(char **args, t_context *ctx)
{
	int i;
	int newline;
	int escape;
	int j;
	(void)ctx;

	i = 1;
	newline = 1;
	escape = 0;
	while (args[i] && args[i][0] == '-')
	{
		j = 1;
		while (args[i][j])
		{
			if (args[i][j] == 'n')
				newline = 0;
			else if (args[i][j] == 'e')
				escape = 1;
			else
				break ;
			j++;
		}
		if (args[i][j] != '\0')
			break ;
		i++;
	}
	while (args[i])
	{
		j = 0;
		while (args[i][j])
		{
			if (escape && args[i][j] == '\\' && args[i][j + 1])
			{
				j++;
				if (args[i][j] == 'n')
					write(1, "\n", 1);
				else if (args[i][j] == 't')
					write(1, "\t", 1);
				else if (args[i][j] == 'r')
					write(1, "\r", 1);
				else if (args[i][j] == 'b')
					write(1, "\b", 1);
				else if (args[i][j] == 'v')
					write(1, "\v", 1);
				else if (args[i][j] == 'a')
					write(1, "\a", 1);
				else
					write(1, "\\", 1);
			}
			else
				write(1, &args[i][j], 1);
			j++;
		}
		if (args[i + 1])
			write(1, " ", 1);
		i++;
	}
	if (newline)
		write(1, "\n", 1);
	// Do not reset exit_status on success; retain previous value
	return (0);
}
