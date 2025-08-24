/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:23:25 by anassih           #+#    #+#             */
/*   Updated: 2025/08/23 18:49:36 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/builtins.h"
#include "../includes/minishell.h"

static char	*expand_home(const char *path, t_context *ctx)
{
	char	*home;

	home = get_env_value("HOME", ctx->env);
	if (!home)
	{
		ft_putstr_fd("minishell: cd: HOME not set\n", STDERR_FILENO);
		return (NULL);
	}
	if (path[1] == '/')
		return (ft_strjoin(home, path + 1));
	if (path[1] == '\0')
		return (ft_strdup(home));
	ft_putstr_fd("minishell: cd: invalid tilde usage\n", STDERR_FILENO);
	return (NULL);
}

static char	*resolve_cd_path(const char *path, t_context *ctx)
{
	char	*cwd;
	char	*tmp;
	char	*expanded;

	if (path[0] == '~')
		return (expand_home(path, ctx));
	if (path[0] == '/')
		return (ft_strdup(path));
	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		perror("minishell: cd");
		return (NULL);
	}
	tmp = ft_strjoin(cwd, "/");
	free(cwd);
	if (!tmp)
		return (NULL);
	expanded = ft_strjoin(tmp, path);
	free(tmp);
	return (expanded);
}

static void	update_pwd_env(t_context *ctx)
{
	char	*old_pwd;
	char	*cwd;

	old_pwd = get_env_value("PWD", ctx->env);
	if (old_pwd)
		update_env(&ctx->env, "OLDPWD", old_pwd);
	cwd = getcwd(NULL, 0);
	if (cwd)
	{
		update_env(&ctx->env, "PWD", cwd);
		free(cwd);
	}
}


static int	cd_perform(const char *path, t_context *ctx)
{
	char	*resolved;
	int		status;

	resolved = resolve_cd_path(path, ctx);
	status = 1;
	if (resolved)
	{
		if (chdir(resolved) == 0)
		{
			update_pwd_env(ctx);
			status = 0;
		}
		else
		{
			ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
			perror(resolved);
		}
		free(resolved);
	}
	ctx->exit_status = status;
	return (status);
}

int	builtin_cd(char **args, t_context *ctx)
{
	char	*path;

	path = NULL;
	if (!args[1] || ft_strcmp(args[1], "~") == 0)
		path = get_env_value("HOME", ctx->env);
	else if (ft_strcmp(args[1], "-") == 0)
	{
		path = get_env_value("OLDPWD", ctx->env);
		if (path)
			ft_putendl_fd(path, STDOUT_FILENO);
	}
	else if (args[2])
	{
		ft_putstr_fd("cd: too many arguments\n", STDERR_FILENO);
		return (ctx->exit_status = 1, 1);
	}
	else
		path = args[1];
	if (!path)
	{
		ft_putstr_fd("minishell: cd: HOME or OLDPWD not set\n", STDERR_FILENO);
		ctx->exit_status = 1;
		return (1);
	}
	return (cd_perform(path, ctx));
}
