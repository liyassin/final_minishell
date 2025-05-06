/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 23:15:00 by anassih           #+#    #+#             */
/*   Updated: 2025/04/28 23:54:36 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Count the number of commands in a pipeline
int count_pipeline_commands(t_ast *ast)
{
    int count = 0;
    t_ast *current = ast;
    
    while (current)
    {
        count++;
        current = current->next;
    }
    
    return count;
}

// Execute a single command with redirections
void execute_single_command(t_ast *ast, char **envp)
{
    char *full_path;
    
    // Handle redirections
    handle_redirection(ast);
    
    // Check for built-in commands
    if (handle_builtin(ast->args))
        return;
        
    // Execute external command
    full_path = find_command_path(ast->command, envp);
    if (full_path)
    {
        if (execve(full_path, ast->args, envp) == -1)
        {
            perror("execve failed");
            free(full_path);
            exit(EXIT_FAILURE);
        }
        free(full_path);
    }
    else
    {
        ft_putstr_fd(ast->command, 2);
        ft_putstr_fd(": command not found\n", 2);
        exit(127); // Command not found exit status
    }
}

// Execute a pipeline of commands
void execute_pipeline(t_ast *ast, char **envp)
{
    int cmd_count;
    int **pipes;
    pid_t *pids;
    int i, j, status;
    t_ast *current;
    
    cmd_count = count_pipeline_commands(ast);
    
    // If only one command, no need for pipes
    if (cmd_count == 1)
    {
        exec_command(ast, envp);
        return;
    }
    
    // Allocate memory for pipes and pids
    pipes = (int **)malloc(sizeof(int *) * (cmd_count - 1));
    if (!pipes)
        return;
        
    i = 0;
    while (i < cmd_count - 1)
    {
        pipes[i] = (int *)malloc(sizeof(int) * 2);
        if (!pipes[i])
        {
            // Free already allocated memory
            j = 0;
            while (j < i)
            {
                free(pipes[j]);
                j++;
            }
            free(pipes);
            return;
        }
        i++;
    }
    
    pids = (pid_t *)malloc(sizeof(pid_t) * cmd_count);
    if (!pids)
    {
        // Free already allocated memory
        i = 0;
        while (i < cmd_count - 1)
        {
            free(pipes[i]);
            i++;
        }
        free(pipes);
        return;
    }
    
    // Create all needed pipes
    i = 0;
    while (i < cmd_count - 1)
    {
        if (pipe(pipes[i]) == -1)
        {
            perror("pipe");
            
            // Free allocated memory
            j = 0;
            while (j < cmd_count - 1)
            {
                free(pipes[j]);
                j++;
            }
            free(pipes);
            free(pids);
            return;
        }
        i++;
    }
    
    // Execute each command in the pipeline
    current = ast;
    i = 0;
    while (i < cmd_count)
    {
        pids[i] = fork();
        if (pids[i] == -1)
        {
            perror("fork");
            
            // Free allocated memory
            j = 0;
            while (j < cmd_count - 1)
            {
                free(pipes[j]);
                j++;
            }
            free(pipes);
            free(pids);
            return;
        }
        
        if (pids[i] == 0) // Child process
        {
            // Reset signal handlers in child process
            reset_signals_for_child();
            
            // Set up input pipe (except for first command)
            if (i > 0)
            {
                if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1)
                {
                    perror("dup2 input");
                    exit(EXIT_FAILURE);
                }
            }
            
            // Set up output pipe (except for last command)
            if (i < cmd_count - 1)
            {
                if (dup2(pipes[i][1], STDOUT_FILENO) == -1)
                {
                    perror("dup2 output");
                    exit(EXIT_FAILURE);
                }
            }
            
            // Close all pipe fds in the child
            j = 0;
            while (j < cmd_count - 1)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
                j++;
            }
            
            // Execute the command
            execute_single_command(current, envp);
            exit(EXIT_FAILURE); // Should not reach here
        }
        
        current = current->next;
        i++;
    }
    
    // Parent closes all pipe fds
    i = 0;
    while (i < cmd_count - 1)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
        i++;
    }
    
    // Wait for all child processes
    i = 0;
    while (i < cmd_count)
    {
        waitpid(pids[i], &status, 0);
        i++;
    }
    
    // Free allocated memory
    i = 0;
    while (i < cmd_count - 1)
    {
        free(pipes[i]);
        i++;
    }
    free(pipes);
    free(pids);
}