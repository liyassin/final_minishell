#include "env_utils.h"

char	**copy_environment(char **envp)
{
	int		count;
	char	**env_copy;
	int		i;

	if (!envp)
		return (NULL);
	count = 0;
	while (envp[count])
		count++;
	env_copy = ft_calloc(count + 1, sizeof(char *));
	if (!env_copy)
		return (NULL);
	i = 0;
	while (i < count)
	{
		env_copy[i] = ft_strdup(envp[i]);
		if (!env_copy[i])
		{
			free_split(env_copy);
			return (NULL);
		}
		i++;
	}
	return (env_copy);
}

// Update or add KEY=VALUE in *env. Return 1 on success, 0 on failure.
int	update_env(char ***env, const char *key, const char *value)
{
	char	*new_entry;

	if (!key || !env || !*env)
		return (0);
	new_entry = make_new_entry(key, value);
	if (!new_entry)
		return (0);
	if (try_replace_env(env, key, new_entry))
		return (1);
	if (add_new_env(env, new_entry))
		return (1);
	free(new_entry);
	return (0);
}

// Build a new "KEY=VALUE" entry or return NULL on failure.
char	*make_new_entry(const char *key, const char *value)
{
	char	*prefix;
	char	*entry;

	prefix = ft_strjoin(key, "=");
	if (!prefix)
		return (NULL);
	entry = join_free_const(prefix, value);
	return (entry);
}

// Try to replace an existing KEY=... entry. Returns 1 if done, 0 otherwise.
int	try_replace_env(char ***env, const char *key, char *new_entry)
{
	char	**envp;
	size_t	key_len;
	int		i;

	envp = *env;
	key_len = ft_strlen(key);
	i = 0;
	while (envp[i])
	{
		if (!ft_strncmp(envp[i], key, key_len) && envp[i][key_len] == '=')
		{
			free(envp[i]);
			envp[i] = new_entry;
			return (1);
		}
		i++;
	}
	return (0);
}

// Append a new entry to the environment array. Returns 1 on success.
int	add_new_env(char ***env, char *new_entry)
{
	char	**old;
	char	**new_env;
	int		i;

	old = *env;
	i = 0;
	while (old[i])
		i++;
	new_env = ft_calloc(i + 2, sizeof(char *));
	if (!new_env)
		return (0);
	ft_memcpy(new_env, old, i * sizeof(char *));
	new_env[i] = new_entry;
	new_env[i + 1] = NULL;
	free(old);
	*env = new_env;
	return (1);
}
