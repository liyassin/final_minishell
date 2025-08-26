/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 16:38:10 by anassih           #+#    #+#             */
/*   Updated: 2025/08/26 23:27:24 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/builtins.h"
#include "../includes/minishell.h"

static void	add_new_export(char ***exported, const char *arg, int count);
static int	find_existing_export(char **exported, const char *name,
		size_t name_len, const char *arg);

static void	print_export_with_value(const char *exported_var)
{
	char	*eq;
	size_t	name_len;
	char	*name;

	eq = ft_strchr(exported_var, '=');
	name_len = eq - exported_var;
	name = ft_strndup(exported_var, name_len);
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

static void	print_all_exports(char **exported)
{
	int		i;
	char	*eq;

	i = 0;
	while (exported && exported[i])
	{
		eq = ft_strchr(exported[i], '=');
		if (eq)
			print_export_with_value(exported[i]);
		else
		{
			ft_putstr_fd("declare -x ", STDOUT_FILENO);
			ft_putstr_fd(exported[i], STDOUT_FILENO);
			ft_putstr_fd("\n", STDOUT_FILENO);
		}
		i++;
	}
}

static int	find_existing_export(char **exported, const char *name,
		size_t name_len, const char *arg)
{
	int	i;

	i = 0;
	while (exported[i])
	{
		if (!ft_strncmp(exported[i], name, name_len)
			&& (exported[i][name_len] == '=' || exported[i][name_len] == '\0'))
		{
			free(exported[i]);
			exported[i] = ft_strdup(arg);
			return (1);
		}
		i++;
	}
	return (0);
}

static void	add_exported(char ***exported, const char *arg)
{
	int		i;
	char	*eq;
	size_t	name_len;
	char	*name;

	i = 0;
	eq = ft_strchr(arg, '=');
	if (eq)
		name_len = eq - arg;
	else
		name_len = ft_strlen(arg);
	name = ft_strndup(arg, name_len);
	if (*exported)
	{
		if (find_existing_export(*exported, name, name_len, arg))
		{
			free(name);
			return ;
		}
		while ((*exported)[i])
			i++;
	}
	add_new_export(exported, arg, i);
	free(name);
}

static void	add_new_export(char ***exported, const char *arg, int count)
{
	char	**new_list;
	int		i;

	new_list = malloc(sizeof(char *) * (count + 2));
	i = 0;
	while (i < count)
	{
		new_list[i] = (*exported)[i];
		i++;
	}
	new_list[count] = ft_strdup(arg);
	new_list[count + 1] = NULL;
	if (*exported)
		free(*exported);
	*exported = new_list;
}

static int	validate_and_get_name(char *arg, char **name, size_t *name_len)
{
	char	*eq;

	eq = ft_strchr(arg, '=');
	if (eq)
		*name_len = eq - arg;
	else
		*name_len = ft_strlen(arg);
	*name = ft_strndup(arg, *name_len);
	if (!is_valid_identifier(*name))
	{
		ft_putstr_fd("minishell: export: `", STDERR_FILENO);
		ft_putstr_fd(arg, STDERR_FILENO);
		ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
		free(*name);
		return (1);
	}
	return (0);
}

static int	process_export_arg(char *arg, t_context *ctx)
{
	char	*eq;
	size_t	name_len;
	char	*name;

	if (validate_and_get_name(arg, &name, &name_len) != 0)
		return (1);
	eq = ft_strchr(arg, '=');
	if (eq)
		add_exported(&ctx->exported, arg);
	else
		add_exported(&ctx->exported, name);
	if (eq)
		update_env(&ctx->env, name, eq + 1);
	free(name);
	return (0);
}

int	builtin_export(char **args, t_context *ctx)
{
	int	i;
	int	status;

	if (!args[1])
	{
		print_all_exports(ctx->exported);
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
