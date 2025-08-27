/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:20:01 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 03:45:06 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/builtins.h"
#include "../includes/minishell.h"

//Remove one variable NAME from env array if present.
static void	remove_env_var(char **env, const char *name)
{
	int		j;
	char	*eq;
	size_t	name_len;

	j = 0;
	while (env[j])
	{
		eq = ft_strchr(env[j], '=');
		if (eq)
			name_len = eq - env[j];
		else
			name_len = ft_strlen(env[j]);
		if (!ft_strncmp(env[j], name, name_len)
			&& name_len == ft_strlen(name))
		{
			free(env[j]);
			while (env[j])
			{
				env[j] = env[j + 1];
				j++;
			}
			break ;
		}
		j++;
	}
}

// unset builtin: remove each valid NAME from env.
int	builtin_unset(char **args, t_context *ctx)
{
	int	i;
	int	status;

	i = 1;
	status = 0;
	while (args[i])
	{
		if (!is_valid_identifier(args[i]))
		{
			ft_putstr_fd("minishell: unset: `", STDERR_FILENO);
			ft_putstr_fd(args[i], STDERR_FILENO);
			ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
			status = 1;
		}
		else
		{
			remove_env_var(ctx->env, args[i]);
			remove_env_var(ctx->exported, args[i]);
		}
		i++;
	}
	ctx->exit_status = status;
	return (status);
}
