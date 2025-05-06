/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_exec.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 17:49:49 by anassih           #+#    #+#             */
/*   Updated: 2025/05/05 16:49:19 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Finds the full path of a command using the PATH environment variable
char *find_command_path(char *cmd, char **envp)
{
    char *path_env;
    char **paths;
    char *full_path;
    int i;

    // Find PATH in envp since its already an env variable
    i = 0;
    while (envp[i] && ft_strncmp(envp[i], "PATH=", 5) != 0)
        i++;
    if (!envp[i])
        return (NULL);
    path_env = envp[i] + 5; // Skip "PATH="
    paths = ft_split(path_env, ':');
    if (!paths)
        return (NULL);
    i = 0;
    while (paths[i])
    {
        full_path = ft_strjoin(paths[i], "/");
        full_path = join_free(full_path, cmd); // Free + strjoin
        if (access(full_path, X_OK) == 0)
        {
            free_split(paths);
            return (full_path);
        }
        free(full_path);
        i++;
    }
    free_split(paths);
    return (NULL);
}

//// Helper function to remove redirection operators from args
//static char **remove_redirection_operator(char **args, const char *operator)
//{
//    int i;
//    int j;
//
//    i = 0;
//    j = 0;
//    while (args[i])
//    {
//        if (ft_strcmp(args[i], operator) == 0)
//        {
//            // Skip over the operator and the next argument (file path)
//            i += 2; // Skip the operator and the file
//        }
//        else
//        {
//            args[j++] = args[i++];
//        }
//    }
//    args[j] = NULL; // Terminate the array
//    return (args);
//}

// Handles input and output redirection
void handle_redirection(t_ast *ast)
{
    int fd;
    int i = 0;

    // Handle output redirection "">
    if (ast->flag == 2 || ast->flag == 3)
    {
        while (ast->output_file[i])
        {
            if (ast->output_file[i])
            {
                fd = open(ast->output_file[i], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd == -1)
                {
                    perror("Error opening file for writing");
                    exit(EXIT_FAILURE);
                }
                if (dup2(fd, STDOUT_FILENO) == -1)
                {
                    perror("dup2 failed for output redirection");
                    close(fd);
                    exit(EXIT_FAILURE);
                }
            }
            i++;
        }
        close(fd);
    }
    // Handle input redirection "<"
    if (ast->flag == 1 || ast->flag == 3)
    {
        i = 0;
        while (ast->input_file[i])
        {
            if (ast->input_file[i])
            {
                fd = open(ast->input_file[i], O_RDONLY);
                if (fd == -1)
                {
                    perror("Error opening file for reading");
                    exit(EXIT_FAILURE);
                }
                if (dup2(fd, STDIN_FILENO) == -1)
                {
                    perror("dup2 failed for input redirection");
                    close(fd);
                    exit(EXIT_FAILURE);
                }
            }
            i++;
        }
        close (fd);
    }
}

// Executes a single command from the AST
void exec_command(t_ast *ast, char **envp)
{
    pid_t pid;
    int status;
    char *full_path;

    pid = fork();
    if (pid == 0)
    {
        // Child process
        // handle_redirection(ast); // Ensure redirection is handled
        full_path = find_command_path(ast->command, envp);
        if (full_path)
        {
            execve(full_path, ast->args, envp);
            perror("execve failed");
            free(full_path);
            exit(EXIT_FAILURE);
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
        perror("fork failed");
    }
    else
    {
        // Parent process
        waitpid(pid, &status, 0);
    }
}
