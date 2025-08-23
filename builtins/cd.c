#include "builtins.h"
#include "minishell.h"

//Resolve a cd path: handle ~, absolute, or relative paths.
static char	*resolve_cd_path(const char *path, t_context *ctx)
{
	char	*expanded;
	char	*home;
	char	*cwd;
	char	*tmp;

	expanded = NULL;
	if (path[0] == '~')
	{
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
	if (path[0] == '/')
		return (ft_strdup(path));
	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		perror("minishell: cd");
		return (NULL);
	}
	tmp = ft_strjoin(cwd, "/");
	if (tmp)
		expanded = ft_strjoin(tmp, path);
	free(tmp);
	free(cwd);
	return (expanded);
}

//Update PWD and OLDPWD in the environment.
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

//cd builtin: change directory and update PWD/OLDPWD.
int	builtin_cd(char **args, t_context *ctx)
{
	char	*path;
	char	*resolved;
	int		status;

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
		ctx->exit_status = 1;
		return (1);
	}
	else
		path = args[1];
	if (!path)
	{
		ft_putstr_fd("minishell: cd: HOME or OLDPWD not set\n", STDERR_FILENO);
		ctx->exit_status = 1;
		return (1);
	}
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
