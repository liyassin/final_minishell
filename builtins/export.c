#include "builtins.h"
#include "minishell.h"

// Print all exported vars in the form:
// declare -x NAME="value"
static void	print_all_exports(char **env)
{
	int		i;
	char	*eq;
	size_t	name_len;
	char	*name;

	i = 0;
	while (env[i])
	{
		eq = ft_strchr(env[i], '=');
		if (eq)
		{
			name_len = eq - env[i];
			name = ft_strndup(env[i], name_len);
			if (name)
			{
				ft_putstr_fd("declare -x ", STDOUT_FILENO);
				ft_putstr_fd(name, STDOUT_FILENO);
				ft_putstr_fd("=\"", STDOUT_FILENO);
				ft_putstr_fd(eq + 1, STDOUT_FILENO);
				ft_putstr_fd("\"\n", STDOUT_FILENO);
				free(name);
			}
		}
		i++;
	}
}

// Process one export argument "NAME[=VALUE]".
// Returns 0 on success, 1 on invalid identifier.
static int	process_export_arg(char *arg, t_context *ctx)
{
	char	*eq;
	size_t	name_len;
	char	*name;

	eq = ft_strchr(arg, '=');
	if (eq)
		name_len = eq - arg;
	else
		name_len = ft_strlen(arg);
	name = ft_strndup(arg, name_len);
	if (!is_valid_identifier(name))
	{
		ft_putstr_fd("minishell: export: `", STDERR_FILENO);
		ft_putstr_fd(arg, STDERR_FILENO);
		ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
		free(name);
		return (1);
	}
	if (eq)
		update_env(&ctx->env, name, eq + 1);
	else
		update_env(&ctx->env, name, "");
	free(name);
	return (0);
}

// export builtin: with no args, print all;
// otherwise process each NAME[=VALUE].
int	builtin_export(char **args, t_context *ctx)
{
	int	i;
	int	status;

	if (!args[1])
	{
		print_all_exports(ctx->env);
		ctx->exit_status = 0;
		return (0);
	}
	i = 1;
	status = 0;
	while (args[i])
	{
		if (process_export_arg(args[i], ctx) != 0)
			status = 1;
		i++;
	}
	ctx->exit_status = status;
	return (status);
}
