/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 16:48:32 by anassih           #+#    #+#             */
/*   Updated: 2025/05/06 16:11:25 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env_utils.h"

char    *strip_quotes(const char *str)
{
    size_t len;

    if (!str)
        return (NULL);

    len = ft_strlen(str);
    if ((str[0] == '"' && str[len - 1] == '"')
        || (str[0] == '\'' && str[len - 1] == '\''))
        return (ft_substr(str, 1, len - 2)); // Remove surrounding quotes
    return (ft_strdup(str)); // Return as-is
}

char    **copy_environment(char **envp)
{
    int i = 0;
    int j = 0;
    char **env_copy;

    // Count the number of environment variables
    while (envp[i])
        i++;
    // Allocate memory for the copy
    env_copy = malloc(sizeof(char *) * (i + 1));
    if (!env_copy)
    {
        perror("malloc");
        return (NULL);
    }
    // Copy each environment variable
    while (j < i)
    {
        env_copy[j] = strdup(envp[j]);
        if (!env_copy[j])
        {
            perror("strdup");
            while (j > 0)
                free(env_copy[--j]);
            free(env_copy);
            return (NULL);
        }
        j++;
    }
    env_copy[i] = NULL; // NULL-terminate the array
    return (env_copy);
}

void free_environment(char **env)
{
    int i = 0;

    if (!env)
        return;
    while (env[i])
    {
        free(env[i]);
        i++;
    }
    free(env);
}

// Function to update the environment with a new variable (name=value)
int update_env(char ***env, const char *key, const char *value)
{
    int     i;
    int     count;
    char    *value_dup;
    char    *new_var;
    char    **new_env;

    /* 1. Create new var string */
    new_var = ft_strjoin(key, "=");
    if (!new_var)
        return (0);
    value_dup = ft_strdup(value);
    if (!value_dup)
    {
        free(new_var); // Cleanup previous allocation
        return (0);
    }
    new_var = join_free(new_var, value_dup);
    if (!new_var)
        return (0);
    /* 2. Check if var exists */
    i = -1;
    while ((*env)[++i])
        if (ft_strncmp((*env)[i], key, ft_strlen(key)) == 0 && 
            (*env)[i][ft_strlen(key)] == '=')
        {
            free((*env)[i]);
            (*env)[i] = new_var;
            return (1);
        }
    /* 3. Create new environment */
    count = i;
    new_env = malloc(sizeof(char *) * (count + 2));
    if (!new_env)
    {
        free(new_var);
        return (0);
    }
    /* 4. Copy existing vars */
    i = -1;
    while (++i < count)
        new_env[i] = (*env)[i];
    /* 5. Add new var */
    new_env[count] = new_var;
    new_env[count + 1] = NULL;
    /* 6. Replace environment */
    free(*env);
    *env = new_env;
    return (1);
}

char	*get_env_value(const char *name, char **envp)
{
	int		i;
	size_t	name_len;

	if (!name || !envp)
		return (NULL);
	name_len = ft_strlen(name);
	i = -1;
	while (envp[++i])
	{
		if (ft_strncmp(envp[i], name, name_len) == 0 
			&& envp[i][name_len] == '=')
			return (envp[i] + name_len + 1);
	}
	return (NULL);
}

static int	expand_var(char **buf, size_t *j, const char *input, char **envp)
{
	size_t	var_len;
	char	*var_value;
	char	var_name[256];

	var_len = 0;
	while (ft_isalnum(input[var_len]) || input[var_len] == '_')
		var_len++;
	ft_strlcpy(var_name, input, var_len + 1);
	var_value = get_env_value(var_name, envp);
	if (var_value)
	{
		while (*var_value)
			(*buf)[(*j)++] = *var_value++;
	}
	return (var_len);
}

char		*expand_env_vars(const char *input, char **envp)
{
	char	*result;
	char	*buffer;
	size_t	i;
	size_t	j;

	if (!input || !envp)
		return (NULL);
	buffer = ft_calloc(ft_strlen(input) * 2 + 1, sizeof(char));
	if (!buffer)
		return (NULL);
	i = 0;
	j = 0;
	while (input[i])
	{
		if (input[i] == '$' && (ft_isalpha(input[i + 1]) || input[i + 1] == '_'))
			i += expand_var(&buffer, &j, input + i + 1, envp) + 1;
		else
			buffer[j++] = input[i++];
	}
	result = ft_strdup(buffer);
	free(buffer);
	return (result);
}
