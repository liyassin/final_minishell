#include "env_utils.h"

char	*strip_quotes(const char *str)
{
	size_t	len;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	if (len >= 2 && ((str[0] == '"' && str[len - 1] == '"')
			|| (str[0] == '\'' && str[len - 1] == '\'')))
		return (ft_substr(str, 1, len - 2));
	return (ft_strdup(str));
}

void	free_environment(char **env)
{
	int	i;

	if (!env)
		return ;
	i = 0;
	while (env[i])
	{
		free(env[i]);
		i++;
	}
	free(env);
}

char	*get_env_value(const char *name, char **envp)
{
	int		i;
	size_t	name_len;

	if (!name || !envp)
		return (NULL);
	name_len = ft_strlen(name);
	if (!envp[0])
		return (NULL);
	i = 0;
	while (envp[i])
	{
		if (!ft_strncmp(envp[i], name, name_len) && envp[i][name_len] == '=')
			return (&envp[i][name_len + 1]);
		i++;
	}
	return (NULL);
}
