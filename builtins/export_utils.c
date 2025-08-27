/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 04:00:00 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 05:09:10 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/builtins.h"
#include "../includes/minishell.h"

void	print_export_with_value(const char *exported_var)
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

void	print_all_exports(char **exported)
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

int	find_existing_export(char **exported, const char *name,
		size_t name_len, const char *arg)
{
	int	i;

	i = 0;
	while (exported[i])
	{
		if (!ft_strncmp(exported[i], name, name_len)
			&& (exported[i][name_len] == '=' || exported[i][name_len] == '\0'))
		{
			if (ft_strchr(arg, '='))
			{
				free(exported[i]);
				exported[i] = ft_strdup(arg);
			}
			return (1);
		}
		i++;
	}
	return (0);
}

void	add_new_export(char ***exported, const char *arg, int count)
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
