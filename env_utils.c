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

char *strip_quotes(const char *str)
{
    size_t len;

    if (!str) return NULL;
    len = ft_strlen(str);
    
    if ((len >= 2) && 
        ((str[0] == '"' && str[len-1] == '"') ||
         (str[0] == '\'' && str[len-1] == '\''))) 
    {
        return ft_substr(str, 1, len - 2);
    }
    return ft_strdup(str);
}

char **copy_environment(char **envp)
{
    int count = 0;
    char **env_copy;

    if (!envp) return NULL;
    while (envp[count]) count++;
    
    env_copy = ft_calloc(count + 1, sizeof(char *));
    if (!env_copy) return NULL;

    for (int i = 0; i < count; i++) {
        env_copy[i] = ft_strdup(envp[i]);
        if (!env_copy[i]) {
            free_split(env_copy);
            return NULL;
        }
    }
    return env_copy;
}

void free_environment(char **env)
{
    if (!env) return;
    for (int i = 0; env[i]; i++) free(env[i]);
    free(env);
}

int update_env(char ***env, const char *key, const char *value)
{
    if (!key || !env || !*env) return 0;

    char *new_entry = join_free_const(ft_strjoin(key, "="), value);
    if (!new_entry) return 0;

    int i = 0;
    size_t key_len = ft_strlen(key);
    
    while ((*env)[i]) {
        if (ft_strncmp((*env)[i], key, key_len) == 0 && 
            (*env)[i][key_len] == '=') 
        {
            free((*env)[i]);
            (*env)[i] = new_entry;
            return 1;
        }
        i++;
    }
    
    // Add new entry
    char **new_env = ft_calloc(i + 2, sizeof(char *));
    if (!new_env) {
        free(new_entry);
        return 0;
    }
    
    for (int j = 0; j < i; j++) new_env[j] = (*env)[j];
    new_env[i] = new_entry;
    free(*env);
    *env = new_env;
    return 1;
}

char *get_env_value(const char *name, char **envp)
{
    if (!name || !envp) return NULL;
    size_t name_len = ft_strlen(name);

    // Handle empty environment case
    if (!*envp) return NULL;
    
    for (int i = 0; envp[i]; i++) {
        if (ft_strncmp(envp[i], name, name_len) == 0 && 
            envp[i][name_len] == '=') 
        {
            return envp[i] + name_len + 1;
        }
    }
    return NULL;
}

static size_t extract_var_name(const char *input, char *buf, size_t buf_size)
{
    size_t len = 0;
    
    if (input[0] == '?') {  // Special $? case
        buf[0] = '?';
        buf[1] = '\0';
        return 1;
    }
    
    // Handle invalid variable names
    if (!ft_isalpha(input[0]) && input[0] != '_') {
        buf[0] = '\0';
        return 0;
    }

    while (len < buf_size - 1 && 
          (ft_isalnum(input[len]) || input[len] == '_')) 
    {
        buf[len] = input[len];
        len++;
    }
    buf[len] = '\0';
    return len;
}

char *expand_env_vars(const char *input, char **envp, int exit_status)
{
    if (!input) return NULL;
    
    // First pass: calculate required length
    size_t len = 0;
    for (size_t i = 0; input[i]; ) {
        if (input[i] == '$' && input[i+1]) {
            // Handle $? special case
            if (input[i+1] == '?') {
                // Calculate length of exit status
                char status_str[12];
                int num_len = snprintf(status_str, sizeof(status_str), "%d", exit_status);
                len += num_len;
                i += 2;  // Skip $?
            } 
            // Handle valid variables
            else if (ft_isalpha(input[i+1]) || input[i+1] == '_') {
                char var_name[128];
                size_t var_len = extract_var_name(input + i + 1, var_name, sizeof(var_name));
                char *value = get_env_value(var_name, envp);
                if (value) len += ft_strlen(value);
                i += 1 + var_len;
            }
            // Handle invalid variables - preserve the $
            else {
                len += 1; // Count the $ character
                i++;      // Move to next character
            }
        } else {
            len++;
            i++;
        }
    }
    
    // Allocate output buffer
    char *output = ft_calloc(len + 1, 1);
    if (!output) return NULL;
    
    // Second pass: build output
    size_t out_pos = 0;
    for (size_t i = 0; input[i]; ) {
        if (input[i] == '$' && input[i+1]) {
            // Handle $? special case
            if (input[i+1] == '?') {
                char status_str[12];
                int num_len = snprintf(status_str, sizeof(status_str), "%d", exit_status);
                ft_strlcpy(output + out_pos, status_str, len - out_pos + 1);
                out_pos += num_len;
                i += 2;
            } 
            // Handle valid variables
            else if (ft_isalpha(input[i+1]) || input[i+1] == '_') {
                char var_name[128];
                size_t var_len = extract_var_name(input + i + 1, var_name, sizeof(var_name));
                char *value = get_env_value(var_name, envp);
                if (value) {
                    size_t val_len = ft_strlen(value);
                    ft_strlcpy(output + out_pos, value, len - out_pos + 1);
                    out_pos += val_len;
                }
                i += 1 + var_len;
            }
            // Handle invalid variables - preserve the $
            else {
                output[out_pos++] = input[i++];
            }
        } else {
            output[out_pos++] = input[i++];
        }
    }
    return output;
}
/*
char *join_free(char *s1, char *s2)
{
    char *result = ft_strjoin(s1, s2);
    if (s1) free(s1);
    return result;
}

char *join_free_const(char *s1, const char *s2)
{
    char *result = ft_strjoin(s1, s2);
    if (s1) free(s1);
    return result;
}

void free_split(char **str)
{
    if (!str) return;
    for (int i = 0; str[i]; i++) free(str[i]);
    free(str);
}*/