/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 16:47:18 by anassih           #+#    #+#             */
/*   Updated: 2025/05/06 16:14:51 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"

int builtin_exit(char **args, t_context *ctx)
{
    long code = ctx->exit_status;
    int idx;

    /* Always print `exit` */
    printf("exit\n");

    if (args[1]) {
        /* Too many arguments */
        if (args[2]) {
            ft_putstr_fd("exit: too many arguments\n", STDERR_FILENO);
            ctx->exit_status = 1;  // Set error status but don't exit shell
            return 1;
        }
        
        /* Validate numeric argument */
        idx = 0;
        if (args[1][0] == '+' || args[1][0] == '-')
            idx++;
        
        /* Check for valid numeric format */
        for (int i = idx; args[1][i]; i++) {
            if (!ft_isdigit((unsigned char)args[1][i])) {
                ft_putstr_fd("exit: ", STDERR_FILENO);
                ft_putstr_fd(args[1], STDERR_FILENO);
                ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
                ctx->exit_status = 255;
                return 255;  // Special code to indicate exit
            }
        }
        
        /* Convert to long */
        code = ft_atoi(args[1]);
    }

    ctx->exit_status = (int)code;
    return 255;  // Special code to indicate exit
}

static char *resolve_cd_path(const char *path, t_context *ctx)
{
    char *expanded = NULL;
    char *home;

    /* Tilde expansion */
    if (path[0] == '~') {
        home = get_env_value("HOME", ctx->env);
        if (!home) {
            ft_putstr_fd("minishell: cd: HOME not set\n", STDERR_FILENO);
            return NULL;
        }
        
        /* Handle ~/path */
        if (path[1] == '/')
            return ft_strjoin(home, path + 1);
        
        /* Handle bare ~ */
        if (path[1] == '\0')
            return ft_strdup(home);
        
        /* Invalid tilde usage */
        ft_putstr_fd("minishell: cd: invalid tilde usage\n", STDERR_FILENO);
        return NULL;
    }
    
    /* Absolute path */
    if (path[0] == '/')
        return ft_strdup(path);
    
    /* Relative path */
    char *cwd = getcwd(NULL, 0);
    if (!cwd) {
        perror("minishell: cd");
        return NULL;
    }
    
    char *tmp = ft_strjoin(cwd, "/");
    expanded = ft_strjoin(tmp, path);
    free(tmp);
    free(cwd);
    return expanded;
}

static void update_pwd_env(t_context *ctx)
{
    char *old_pwd = get_env_value("PWD", ctx->env);
    if (old_pwd)
        update_env(&ctx->env, "OLDPWD", old_pwd);

    char *cwd = getcwd(NULL, 0);
    if (cwd) {
        update_env(&ctx->env, "PWD", cwd);
        free(cwd);
    }
}

int builtin_cd(char **args, t_context *ctx)
{
    char *path = NULL;
    char *resolved_path = NULL;
    int status = 1; // Default to error status

    if (!args[1] || ft_strcmp(args[1], "~") == 0) {
        path = get_env_value("HOME", ctx->env);
        if (!path) {
            ft_putstr_fd("minishell: cd: HOME not set\n", STDERR_FILENO);
            ctx->exit_status = 1;
            return 1;
        }
    }
    else if (ft_strcmp(args[1], "-") == 0) {
        path = get_env_value("OLDPWD", ctx->env);
        if (!path) {
            ft_putstr_fd("minishell: cd: OLDPWD not set\n", STDERR_FILENO);
            ctx->exit_status = 1;
            return 1;
        }
        ft_putendl_fd(path, STDOUT_FILENO);
    }
    else {
        path = args[1];
    }

    resolved_path = resolve_cd_path(path, ctx);
    if (resolved_path) {
        if (chdir(resolved_path) != 0) {
            ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
            perror(resolved_path);
            status = 1;
        }
        else {
            update_pwd_env(ctx);
            status = 0;
        }
        free(resolved_path);
    }

    ctx->exit_status = status;
    return status;
}

int builtin_env(t_context *ctx)
{
    if (!ctx->env) {
        ft_putstr_fd("env: environment not initialized\n", STDERR_FILENO);
        return (ctx->exit_status = 1, 1);
    }
    
    for (int i = 0; ctx->env[i]; i++) {
        /* Only show variables with values (VAR=value) */
        if (ft_strchr(ctx->env[i], '=')) {
            printf("%s\n", ctx->env[i]);
        }
    }
    return (ctx->exit_status = 0, 0);
}

int builtin_pwd(t_context *ctx)
{
    char *cwd = getcwd(NULL, 0);
    if (cwd) {
        printf("%s\n", cwd);
        free(cwd);
        return (ctx->exit_status = 0, 0);
    }
    ft_putstr_fd("pwd: ", STDERR_FILENO);
    perror("minishell: pwd");
    return (ctx->exit_status = 1, 1);
}

int builtin_echo(char **args, t_context *ctx)
{
    int i = 1;
    int newline = 1;
    int escape = 0;

    /* Parse options like -n and -e */
    while (args[i] && args[i][0] == '-') {
        int valid = 1;
        for (int j = 1; args[i][j]; j++) {
            if (args[i][j] == 'n')
                newline = 0;
            else if (args[i][j] == 'e')
                escape = 1;
            else {
                valid = 0;
                break;
            }
        }
        if (!valid) break;
        i++;
    }

    /* Print arguments with optional escape parsing */
    for (; args[i]; i++) {
        for (int j = 0; args[i][j]; j++) {
            if (escape && args[i][j] == '\\') {
                char next = args[i][j + 1];
                if (next == 'n') { write(1, "\n", 1); j++; continue; }
                if (next == 't') { write(1, "\t", 1); j++; continue; }
                if (next == 'r') { write(1, "\r", 1); j++; continue; }
                if (next == 'b') { write(1, "\b", 1); j++; continue; }
                if (next == 'v') { write(1, "\v", 1); j++; continue; }
                if (next == 'a') { write(1, "\a", 1); j++; continue; }
                if (next == '\\'){ write(1, "\\", 1); j++; continue; }
                // Unsupported escape → treat literally
            }
            write(1, &args[i][j], 1);
        }
        if (args[i + 1])
            write(1, " ", 1);
    }

    if (newline)
        write(1, "\n", 1);

    return (ctx->exit_status = 0, 0);
}

static int is_valid_identifier(const char *name)
{
    if (!name || !*name)
        return 0;
    
    /* First character must be letter or underscore */
    if (!ft_isalpha(*name) && *name != '_')
        return 0;
    
    /* Remaining characters must be alnum or underscore */
    for (const char *p = name + 1; *p; p++) {
        if (!ft_isalnum(*p) && *p != '_')
            return 0;
    }
    return 1;
}

int builtin_export(char **args, t_context *ctx)
{
    int status = 0;
    
    /* Print environment when no arguments */
    if (!args[1]) {
        for (int i = 0; ctx->env[i]; i++) {
            if (ft_strchr(ctx->env[i], '=')) {
                /* Format: declare -x VAR="value" */
                char *eq = ft_strchr(ctx->env[i], '=');
                size_t name_len = eq - ctx->env[i];
                char *name = ft_strndup(ctx->env[i], name_len);
                char *value = eq + 1;
                
                printf("declare -x %s=\"%s\"\n", name, value);
                free(name);
            }
        }
        return (ctx->exit_status = 0, 0);
    }
    
    /* Process each argument */
    for (int i = 1; args[i]; i++) {
        char *eq = ft_strchr(args[i], '=');
        size_t name_len = eq ? (eq - args[i]) : ft_strlen(args[i]);
        char *name = ft_strndup(args[i], name_len);
        
        /* Validate identifier */
        if (!is_valid_identifier(name)) {
            ft_putstr_fd("minishell: export: `", STDERR_FILENO);
            ft_putstr_fd(args[i], STDERR_FILENO);
            ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
            free(name);
            status = 1;
            continue;
        }
        
        /* Update environment */
        if (eq) {
            /* Use literal value without expansion */
            update_env(&ctx->env, name, eq + 1);
        } else {
            /* Create variable without value */
            update_env(&ctx->env, name, "");
        }
        free(name);
    }
    
    return (ctx->exit_status = status, status);
}

int builtin_unset(char **args, t_context *ctx)
{
    int status = 0;
    
    for (int i = 1; args[i]; i++) {
        if (!is_valid_identifier(args[i])) {
            ft_putstr_fd("minishell: unset: `", STDERR_FILENO);
            ft_putstr_fd(args[i], STDERR_FILENO);
            ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
            status = 1;
            continue;
        }
        
        /* Find and remove variable */
        for (int j = 0; ctx->env[j]; j++) {
            char *eq = ft_strchr(ctx->env[j], '=');
            size_t name_len = eq ? (eq - ctx->env[j]) : ft_strlen(ctx->env[j]);
            
            if (ft_strncmp(ctx->env[j], args[i], name_len) == 0 && 
                name_len == ft_strlen(args[i])) 
            {
                free(ctx->env[j]);
                /* Shift remaining variables */
                for (int k = j; ctx->env[k]; k++) {
                    ctx->env[k] = ctx->env[k+1];
                }
                break;
            }
        }
    }
    
    return (ctx->exit_status = status, status);
}