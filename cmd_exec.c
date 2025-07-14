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
#include "signals.h"

// Finds the full path of a command using PATH
char *find_command_path(char *cmd, t_context *ctx)
{
    if (!cmd) {
        return NULL;
    }
    
    // Try absolute path first
    if (ft_strchr(cmd, '/')) {
        if (access(cmd, X_OK) == 0) {
            return ft_strdup(cmd);
        }
        return NULL; // Not found or not executable
    }
    
    // Get PATH from environment
    char *path_env = get_env_value("PATH", ctx->env);
    if (!path_env) {
        return NULL;
    }
    
    char **paths = ft_split(path_env, ':');
    if (!paths) {
        return NULL;
    }
    
    char *full_path = NULL;
    for (int i = 0; paths[i]; i++) {
        char *test_path = ft_strjoin(paths[i], "/");
        if (!test_path) {
            continue;
        }
        
        char *test_cmd = join_free_const(test_path, cmd);
        if (!test_cmd) {
            continue;
        }
        
        if (access(test_cmd, X_OK) == 0) {
            full_path = test_cmd;
            break;
        }
        free(test_cmd);
    }
    
    free_split(paths);
    return full_path;
}

// Redirection helpers: return 0 on success, -1 on error
static int redirect_input(const char *file) {
    int fd = open(file, O_RDONLY);
    if (fd < 0) {
        //perror("minishell");
        /* Print “minishell: <file>: <system error>” */
        ft_putstr_fd("minishell: ", STDERR_FILENO);
        ft_putstr_fd((char *)file, STDERR_FILENO);
        ft_putstr_fd(": ", STDERR_FILENO);
        perror(NULL);
        return -1;
    }
    if (dup2(fd, STDIN_FILENO) < 0) {
        perror("minishell");
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

static int redirect_output(const char *file, int flags) {
    int fd = open(file, flags, 0644);
    if (fd < 0) {
        //perror("minishell");
        ft_putstr_fd("minishell: ", STDERR_FILENO);
        ft_putstr_fd((char *)file, STDERR_FILENO);
        ft_putstr_fd(": ", STDERR_FILENO);
        perror(NULL);
        return -1;
    }
    if (dup2(fd, STDOUT_FILENO) < 0) {
        perror("minishell");
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

// Unified redirection handling: returns 0 on success, -1 on failure
int handle_redirection(t_ast *ast)
{
    if (!ast) {
        return -1;
    }
    
    for (t_redir *r = ast->redirs; r; r = r->next) {
        switch (r->type) {
            case REDIR_IN:
                if (redirect_input(r->target) < 0) {
                    return -1;
                }
                break;
                
            case REDIR_OUT:
                if (redirect_output(r->target, O_WRONLY | O_CREAT | O_TRUNC) < 0) {
                    return -1;
                }
                break;
                
            case REDIR_APPEND:
                if (redirect_output(r->target, O_WRONLY | O_CREAT | O_APPEND) < 0) {
                    return -1;
                }
                break;
                
            case REDIR_HEREDOC:
                if (r->heredoc_fd[0] == -1) {
                    ft_putstr_fd("minishell: heredoc not initialized\n", STDERR_FILENO);
                    return -1;
                }
                if (dup2(r->heredoc_fd[0], STDIN_FILENO) < 0) {
                    perror("minishell");
                    return -1;
                }
                close(r->heredoc_fd[0]);
                break;

            case REDIR_NONE:
                break;
        }
    }
    return 0;
}

static void handle_command_not_found(char *cmd)
{
    if (!cmd || !*cmd) {
        ft_putstr_fd("minishell: : command not found\n", STDERR_FILENO);
        exit(127);
    }
    if (ft_strchr(cmd, '/')) {
        struct stat st;
        if (stat(cmd, &st) == 0 && S_ISDIR(st.st_mode)) {
            ft_putstr_fd("minishell: ", STDERR_FILENO);
            ft_putstr_fd(cmd, STDERR_FILENO);
            ft_putstr_fd(": is a directory\n", STDERR_FILENO);
            exit(126);
        }
        else if (access(cmd, F_OK) == 0) {
            ft_putstr_fd("minishell: ", STDERR_FILENO);
            ft_putstr_fd(cmd, STDERR_FILENO);
            ft_putstr_fd(": Permission denied\n", STDERR_FILENO);
            exit(126);
        }
        else {
            ft_putstr_fd("minishell: ", STDERR_FILENO);
            ft_putstr_fd(cmd, STDERR_FILENO);
            ft_putstr_fd(": No such file or directory\n", STDERR_FILENO);
            exit(127);
        }
    }
    
    ft_putstr_fd("minishell: ", STDERR_FILENO);
    ft_putstr_fd(cmd, STDERR_FILENO);
    ft_putstr_fd(": command not found\n", STDERR_FILENO);
    exit(127);
}

// Execute single command
void exec_command(t_ast *ast, t_context *ctx)
{
    if (!ast || !ctx) {
        return;
    }
    
    // Handle empty command with arguments
    if ((!ast->command || !*ast->command) && ast->args && ast->args[0] && ast->args[1]) {
        // Save the empty string to free later
        char *empty_command = ast->args[0];
        
        // Count number of arguments after the empty command
        int new_count = 0;
        while (ast->args[new_count + 1]) new_count++;
        
        // Create new arguments array
        char **new_args = ft_calloc(new_count + 1, sizeof(char *));
        if (!new_args) {
            // Memory error, proceed with original args
            ast->command = ft_strdup("");
            return;
        }
        
        // Copy arguments skipping the empty command
        for (int i = 0; i < new_count; i++) {
            new_args[i] = ast->args[i + 1];
        }
        
        // Replace args array
        free(ast->args);
        ast->args = new_args;
        
        // Update command
        free(ast->command);
        ast->command = ft_strdup(ast->args[0]);
        
        // Free the empty command string
        free(empty_command);
    }

    // Handle empty command without arguments
    if (!ast->command || !*ast->command) {
        // Apply redirections but don't execute anything
        if (handle_redirection(ast) == 0) {
            ctx->exit_status = 0;
        } else {
            ctx->exit_status = 1;
        }
        return;
    }

    pid_t pid = fork();
    if (pid == 0) { // Child process
        reset_default_signals();
        
        // Apply redirections
        if (handle_redirection(ast) < 0) {
            exit(EXIT_FAILURE);
        }
        
        // Handle builtins
        t_builtin type = get_builtin_type(ast->args[0]);
        if (type != NOT_BUILTIN) {
            int status = handle_builtin(ast->args, ctx);
            exit(status);
        }
        
        // Handle external commands
        char *path = find_command_path(ast->command, ctx);
        if (!path) {
            handle_command_not_found(ast->command);
        }
        execve(path, ast->args, ctx->env);
        perror("minishell");
        free(path);
        exit(126);
    }
    else if (pid > 0) { // Parent process
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            ctx->exit_status = WEXITSTATUS(status);
        }
        else if (WIFSIGNALED(status)) {
            ctx->exit_status = 128 + WTERMSIG(status);
        }
    }
    else {
        perror("minishell: fork");
    }
}

// Execute pipeline
void execute_pipeline(t_ast *ast, t_context *ctx)
{
    if (!ast || !ctx) {
        return;
    }
    
    // Count pipeline stages
    int n = 0;
    for (t_ast *it = ast; it; it = it->next) n++;
    if (n == 0) return;
    
    // Create pipes
    int (*pipes)[2] = ft_calloc(n-1, sizeof(int[2]));
    if (!pipes) {
        perror("minishell: malloc");
        return;
    }
    
    for (int i = 0; i < n-1; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("minishell: pipe");
            free(pipes);
            return;
        }
    }
    
    // Fork children
    pid_t *pids = ft_calloc(n, sizeof(pid_t));
    if (!pids) {
        perror("minishell: malloc");
        free(pipes);
        return;
    }
    
    t_ast *node = ast;
    for (int i = 0; i < n; i++, node = node->next) {
        pids[i] = fork();
        
        if (pids[i] == 0) { // Child
            reset_default_signals();
            
            // Connect pipes
            if (i > 0) {
                dup2(pipes[i-1][0], STDIN_FILENO);
            }
            if (i < n-1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            
            // Close all pipe FDs
            for (int j = 0; j < n-1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            
            // Apply redirections
            if (handle_redirection(node) < 0) {
                exit(EXIT_FAILURE);
            }
            
            // Handle empty command
            if (!node->command || !*node->command) {
                exit(0);
            }

            // Handle builtins
            if (get_builtin_type(node->args[0]) != NOT_BUILTIN) {
                int status = handle_builtin(node->args, ctx);
                exit(status);
            }
            
            // Execute external command
            char *path = find_command_path(node->command, ctx);
            if (!path) {
                handle_command_not_found(node->command);
            }
            execve(path, node->args, ctx->env);
            perror("minishell");
            free(path);
            exit(126);
        }
        else if (pids[i] < 0) {
            perror("minishell: fork");
            break;
        }
    }
    
    // Parent: close all pipes
    for (int i = 0; i < n-1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    free(pipes);
    
    // Wait for children
    for (int i = 0; i < n; i++) {
        if (pids[i] > 0) {
            int status;
            waitpid(pids[i], &status, 0);
            
            // Only care about last command's status
            if (i == n-1) {
                if (WIFEXITED(status)) {
                    ctx->exit_status = WEXITSTATUS(status);
                }
                else if (WIFSIGNALED(status)) {
                    ctx->exit_status = 128 + WTERMSIG(status);
                }
            }
        }
    }
    
    free(pids);
}