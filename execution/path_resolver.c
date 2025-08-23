#include "minishell.h"
#include "signals.h"

// Return cmd itself if it contains '/' and is executable.
static char	*get_abs_path(char *cmd)
{
	if (ft_strchr(cmd, '/'))
		if (access(cmd, X_OK) == 0)
			return (ft_strdup(cmd));
	return (NULL);
}

// Search for cmd in each directory of paths[]. Return first match.
static char	*search_in_paths(char *cmd, char **paths)
{
	char	*full;
	char	*path;
	char	*cand;
	int		i;

	full = NULL;
	i = 0;
	while (paths[i])
	{
		path = ft_strjoin(paths[i], "/");
		if (path)
		{
			cand = join_free_const(path, cmd);
			if (cand)
			{
				if (access(cand, X_OK) == 0)
				{
					full = cand;
					break ;
				}
				free(cand);
			}
		}
		i++;
	}
	return (full);
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
