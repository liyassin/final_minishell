/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_exec.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 17:49:49 by anassih           #+#    #+#             */
/*   Updated: 2025/04/29 00:00:12 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char    *find_command_path(char *cmd, char **envp)
{
    char    *path_env;
    char    **paths;
    char    *full_path;
    int     i;

    // If command has a slash, try using it directly
    if (ft_strchr(cmd, '/'))
    {
        if (access(cmd, X_OK) == 0)
            return (ft_strdup(cmd));
        return (NULL);
    }

    // find PATH in envp
    path_env = get_env_value(envp, "PATH");
    if (!path_env)
        return (NULL);
        
    paths = ft_split(path_env, ':');
    if (!paths)
        return (NULL);
    i = 0;
    while (paths[i])
    {
        full_path = ft_strjoin(paths[i], "/");
        full_path = join_free(full_path, cmd);
        if (access(full_path, X_OK) == 0)
        {
            // Found executable command
            free_split(paths);
            return (full_path);
        }
        free(full_path);
        i++;
    }
    free_split(paths);
    return (NULL);
}

// Helper function to remove redirection operators from args
static char	**remove_redirection_operator(char **args, const char *operator)
{
    int i = 0;
    int j = 0;
    
    while (args[i])
    {
        if (ft_strcmp(args[i], operator) == 0)
        {
            // Skip over the operator and the next argument (file path)
            i += 2;  // Skip the operator and the file
        }
        else
        {
            args[j++] = args[i++];
        }
    }
    args[j] = NULL;  // Terminate the array
    return args;
}

void	handle_redirection(t_ast *ast)
{
    int fd;

    // Handle output redirection ">"
    if (ast->output_file)
    {
        fd = open(ast->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1)
        {
            perror("Error opening file for writing");
            return;
        }
        // Replace standard output (stdout) with the file
        if (dup2(fd, STDOUT_FILENO) == -1)
        {
            perror("Error redirecting output");
            close(fd);
            return;
        }
        close(fd);
    }
    
    // Handle input redirection "<"
    if (ast->input_file)
    {
        fd = open(ast->input_file, O_RDONLY);
        if (fd == -1)
        {
            perror("Error opening file for reading");
            return;
        }
        // Replace standard input (stdin) with the file
        if (dup2(fd, STDIN_FILENO) == -1)
        {
            perror("Error redirecting input");
            close(fd);
            return;
        }
        close(fd);
    }
}

void exec_command(t_ast *ast, char **envp)
{
    pid_t	pid;
    int		status;
    char	*full_path;

    pid = fork();
    if (pid == 0)
    {
        // Reset signal handlers in child process
        reset_signals_for_child();
        
        // Handle redirections
        handle_redirection(ast);
        
        // In child process, execute the command
        full_path = find_command_path(ast->command, envp);
        if (full_path)
        {
            if (execve(full_path, ast->args, envp) == -1)
            {
                perror("execve failed");
                free(full_path);
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            ft_putstr_fd(ast->command, 2);
            ft_putstr_fd(": command not found\n", 2);
            exit(127); // Command not found exit status
        }
    }
    else if (pid < 0)
    {
        // Fork failed
        perror("fork failed");
    }
    else
    {
        // Parent process, wait for child to finish
        waitpid(pid, &status, 0);
    }
}