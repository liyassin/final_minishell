/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_vars.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 01:15:00 by anassih           #+#    #+#             */
/*   Updated: 2025/04/28 23:58:32 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Make a copy of the environment variables
char **copy_env(char **envp)
{
    char **env_copy;
    int count = 0;
    int i;
    
    // Count the number of environment variables
    while (envp[count])
        count++;
        
    // Allocate memory for the copy
    env_copy = (char **)malloc(sizeof(char *) * (count + 1));
    if (!env_copy)
        return (NULL);
        
    // Copy each environment variable
    i = 0;
    while (i < count)
    {
        env_copy[i] = ft_strdup(envp[i]);
        if (!env_copy[i])
        {
            // Free previously allocated memory
            while (--i >= 0)
                free(env_copy[i]);
            free(env_copy);
            return (NULL);
        }
        i++;
    }
    
    env_copy[count] = NULL;
    return (env_copy);
}

// Find an environment variable by name
char *get_env_value(char **env, const char *name)
{
    int i;
    int name_len;
    
    if (!name || !env)
        return (NULL);
        
    name_len = ft_strlen(name);
    
    i = 0;
    while (env[i])
    {
        if (ft_strncmp(env[i], name, name_len) == 0 && env[i][name_len] == '=')
            return (env[i] + name_len + 1);
        i++;
    }
    
    return (NULL);
}

// Set or add an environment variable
void set_env_var(char ***env, const char *name, const char *value)
{
    char *new_var;
    char **new_env;
    int env_size = 0;
    int i;
    int name_len;
    
    if (!name || !env || !*env)
        return;
        
    name_len = ft_strlen(name);
    
    // Check if the variable already exists
    i = 0;
    while ((*env)[i])
    {
        if (ft_strncmp((*env)[i], name, name_len) == 0 && (*env)[i][name_len] == '=')
        {
            // Variable exists, update its value
            free((*env)[i]);
            new_var = ft_strjoin(name, "=");
            new_var = join_free(new_var, (char *)value);
            (*env)[i] = new_var;
            return;
        }
        i++;
        env_size++;
    }
    
    // Variable doesn't exist, add it
    new_var = ft_strjoin(name, "=");
    new_var = join_free(new_var, (char *)value);
    
    // Create a new environment array with one more element
    new_env = (char **)malloc(sizeof(char *) * (env_size + 2));
    if (!new_env)
    {
        free(new_var);
        return;
    }
    
    // Copy existing variables
    i = 0;
    while (i < env_size)
    {
        new_env[i] = (*env)[i];
        i++;
    }
    
    // Add the new variable and NULL terminator
    new_env[env_size] = new_var;
    new_env[env_size + 1] = NULL;
    
    // Free the old array but not its contents
    free(*env);
    
    // Update the environment pointer
    *env = new_env;
}

// Remove an environment variable
void unset_env_var(char ***env, const char *name)
{
    char **new_env;
    int env_size = 0;
    int new_index = 0;
    int i;
    int name_len;
    int found = 0;
    
    if (!name || !env || !*env)
        return;
    
    name_len = ft_strlen(name);
    
    // Count environment variables and check if the variable exists
    i = 0;
    while ((*env)[i])
    {
        if (ft_strncmp((*env)[i], name, name_len) == 0 && (*env)[i][name_len] == '=')
            found = 1;
        env_size++;
        i++;
    }
    
    // If the variable doesn't exist, do nothing
    if (!found)
        return;
    
    // Create a new environment array with one fewer element
    new_env = (char **)malloc(sizeof(char *) * env_size);
    if (!new_env)
        return;
    
    // Copy all variables except the one to be removed
    i = 0;
    while ((*env)[i])
    {
        if (ft_strncmp((*env)[i], name, name_len) != 0 || (*env)[i][name_len] != '=')
        {
            new_env[new_index] = (*env)[i];
            new_index++;
        }
        else
        {
            // Free the variable to be removed
            free((*env)[i]);
        }
        i++;
    }
    
    // Add NULL terminator
    new_env[new_index] = NULL;
    
    // Free the old array but not its contents (except the removed one)
    free(*env);
    
    // Update the environment pointer
    *env = new_env;
}

// Print all environment variables
void print_env(char **env)
{
    int i;
    
    if (!env)
        return;
    
    i = 0;
    while (env[i])
    {
        ft_putendl_fd(env[i], 1);
        i++;
    }
}

// Free the environment array and all its contents
void free_env(char **env)
{
    int i;
    
    if (!env)
        return;
    
    i = 0;
    while (env[i])
    {
        free(env[i]);
        i++;
    }
    
    free(env);
}