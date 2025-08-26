/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_resolver.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:16:00 by anassih           #+#    #+#             */
/*   Updated: 2025/08/26 23:27:24 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"

// Return cmd itself if it contains '/' and exists.
static char	*get_abs_path(char *cmd)
{
	if (ft_strchr(cmd, '/'))
	{
		if (access(cmd, F_OK) == 0)
			return (ft_strdup(cmd));
	}
	return (NULL);
}

// Search for cmd in each directory of paths[]. Return first match.
static char	*try_path_candidate(char *dir, char *cmd)
{
	char	*path;
	char	*cand;

	path = ft_strjoin(dir, "/");
	if (!path)
		return (NULL);
	cand = join_free_const(path, cmd);
	if (!cand)
		return (NULL);
	if (access(cand, X_OK) == 0)
		return (cand);
	free(cand);
	return (NULL);
}

static char	*search_in_paths(char *cmd, char **paths)
{
	char	*result;
	int		i;

	i = 0;
	while (paths[i])
	{
		result = try_path_candidate(paths[i], cmd);
		if (result)
			return (result);
		i++;
	}
	return (NULL);
}

// Finds full path of cmd using PATH in ctx->env.
char	*find_command_path(char *cmd, t_context *ctx)
{
	char	*env_path;
	char	**paths;
	char	*full;

	if (!cmd)
		return (NULL);
	full = get_abs_path(cmd);
	if (full)
		return (full);
	env_path = get_env_value("PATH", ctx->env);
	if (!env_path)
		return (NULL);
	paths = ft_split(env_path, ':');
	if (!paths)
		return (NULL);
	full = search_in_paths(cmd, paths);
	free_split(paths);
	return (full);
}
