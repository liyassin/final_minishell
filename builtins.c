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

static int	validate_identifier(const char *name)
{
	int	i;

	if (!name || (!ft_isalpha(name[0]) && name[0] != '_'))
		return (0);
	i = 0;
	while (name[++i])
		if (!ft_isalnum(name[i]) && name[i] != '_')
			return (0);
	return (1);
}

int builtin_exit(char **args)
{
    (void)args;
    printf("exit\n");
    exit(0);
}

/*
** Path resolution with tilde expansion
** Returns: resolved path or NULL on error
*/
static char *resolve_cd_path(const char *path, char **env)
{
    char    *expanded;
    char    *home;

    home = get_env_value("HOME", env);
    if (!home)
        return (ft_putstr_fd("minishell: cd: HOME not set\n", 2), NULL); // this too NULL

    if (path[1] == '/')
    {
        expanded = ft_strjoin(home, path + 1);
        free(home); // Free home after joining
    }
    else
    {
        expanded = ft_strdup(home);
        free(home); // Free home after duplication
    }

    if (!expanded)
        return (perror("minishell: cd"), NULL); // must be NULL next time cuz i changed
    return (expanded);
}

/*
** Updates PWD and OLDPWD environment variables
** Uses getcwd() for accuracy, falls back to given path if needed
*/
static void update_pwd_env(char ***shell_env, const char *new_path)
{
    char    *old_pwd;
    char    *cwd;

    // Save old PWD value
    old_pwd = get_env_value("PWD", *shell_env);
    if (old_pwd)
        update_env(shell_env, "OLDPWD", old_pwd);

    // Get current directory (dynamic allocation)
    cwd = getcwd(NULL, 0);
    if (!cwd)  // Fallback to given path if getcwd fails
        cwd = ft_strdup(new_path);
    
    if (cwd)
    {
        update_env(shell_env, "PWD", cwd);
        free(cwd);
    }
    else
        perror("minishell: cd");
}

/*
** Builtin cd command implementation
** Returns: 0 on success, 1 on error (following shell convention)
** Handles: ~ expansion, -, environment updates
*/
int     builtin_cd(char **args, char ***shell_env)
{
    char    *path;
    char    *resolved_path;

    // Case 1: cd without arguments or "cd ~"
    if (!args[1] || ft_strcmp(args[1], "~") == 0)
    {
        if (!(path = get_env_value("HOME", *shell_env)))
            return (ft_putstr_fd("minishell: cd: HOME not set\n", 2), 1);
    }
    // Case 2: cd -
    else if (ft_strcmp(args[1], "-") == 0)
    {
        if (!(path = get_env_value("OLDPWD", *shell_env)))
            return (ft_putstr_fd("minishell: cd: OLDPWD not set\n", 2), 1);
        ft_putendl_fd(path, STDOUT_FILENO); // Print directory after cd -
    }
    // Case 3: Regular path argument
    else
        path = args[1];
    // Resolve path (tilde expansion and error checking)
    if (!(resolved_path = resolve_cd_path(path, *shell_env)))
        return (1); // Error message already handled in resolve_cd_path
    // Attempt directory change
    if (chdir(resolved_path) != 0)
    {
        free(resolved_path);
        return (perror("minishell: cd"), 1);
    }

    // Update environment variables after successful directory change
    update_pwd_env(shell_env, resolved_path);
    free(resolved_path);
    return (0); // Success
}

int builtin_env(char **envp)
{
    int i;

    i = 0;
    if (!envp)
    {
        write (STDERR_FILENO, "env: NULL environment\n", 21);
        return (1);
    }
    while (envp[i])
    {
        if (envp[i])
            printf("%s\n", envp[i]);
        else
            write  (STDERR_FILENO, "(null)\n", 7);
        i++;
    }
    return (1);
}

int builtin_pwd(void)
{
    char    *cwd;

    cwd = getcwd(NULL, 0); // allocating buffer via malloc() // getcwd => place an absolute pathname of the current working directory in the array pointed to by buf, and return buf.
    if (cwd)
    {
        printf("%s\n", cwd);
        free(cwd); // must free whenever cwd !NULL
    }
    else
        perror("pwd");
    return (1);
}

int builtin_echo(char **args)
{
    int i, j;
    int newline;

    newline = 1;
    i = 1;
    // Check for valid "-n" flags
    while (args[i] && ft_strncmp(args[i], "-n", 2) == 0)
    {
        j = 2;
        while (args[i][j] == 'n') // Ensure all characters after "-n" are 'n'
            j++;
        if (args[i][j] != '\0') // If there's any other character, stop treating it as a flag
            break ;
        newline = 0;
        i++;
    }
    // Print remaining arguments
    while (args[i])
    {
        ft_putstr_fd(args[i], STDOUT_FILENO);
        if (args[++i])
            ft_putchar_fd(' ', STDOUT_FILENO);
    }
    // Print newline if not suppressed
    if (newline)
        ft_putchar_fd('\n', STDOUT_FILENO);
    return (1);
}

int		builtin_export(char **args, char ***shell_env)
{
	char	*eq_pos;
	char	*name;
	char	*value;
	char	*expanded_value;
	int		i;

	i = 0;
	if (!args[1])
		return (builtin_env(*shell_env));
	while (args[++i])
	{
		if ((eq_pos = ft_strchr(args[i], '=')))
		{
			name = ft_strndup(args[i], eq_pos - args[i]);
			value = strip_quotes(eq_pos + 1);
			expanded_value = expand_env_vars(value, *shell_env);
			free(value);
			value = expanded_value;
			if (!validate_identifier(name))
			{
                ft_putstr_fd("minishell: export: `", STDERR_FILENO);
        		ft_putstr_fd(args[i], STDERR_FILENO);
        		ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
				free(name);
				free(value);
				return (1);
			}
			if (!update_env(shell_env, name, value))
			{
				free(name);
				free(value);
				return (1);
			}
			free(name);
			free(value);
		}
	}
	return (1);
}


int builtin_unset(char **args, char ***shell_env)
{
    int i;
    int j;
    int k;

    if (!shell_env || !*shell_env) {
        perror("Error: Invalid environment pointer\n");
        return (1);
    }
    i = 1;
    if (!args[i])
    {
        write (2, "unset: Too few arguments\n", 24);
        return (1);
    }
    while (args[i])
    {
        if (!ft_isalpha(args[i][0]) && args[i][0] != '_')
        {
            write (2, "unset: not a valid identifier\n", 30);
            i++;
            continue ;
        }
        // Find and remove variable from shell_env
        j = 0;
        while ((*shell_env)[j])
        {
            if (ft_strncmp((*shell_env)[j], args[i], ft_strlen(args[i])) == 0 
                && (*shell_env)[j][ft_strlen(args[i])] == '=')
            {
                free((*shell_env)[j]);
                k = j;
                while ((*shell_env)[k])
                {
                    (*shell_env)[k] = (*shell_env)[k + 1];
                    k++;
                }
                break ;
            }
            j++;
        }
        i++;
    }
    return (1);
}