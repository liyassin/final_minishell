/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 09:46:40 by anassih           #+#    #+#             */
/*   Updated: 2025/04/29 00:42:41 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int handle_builtin(char **args, char ***env_ptr)
{
    char    *home;

    if (args[0] == NULL)
        return (0);
    if (ft_strcmp(args[0], "exit") == 0)
    {
        // Built-IN: exit the shell
        exit(0);
    }
    else if (ft_strcmp(args[0], "cd") == 0)
    {
        if (args[1])
        {
            if (chdir(args[1]) != 0)
                perror("cd");
        }
        else
        {
            home = get_env_value(*env_ptr, "HOME");
            if (home)
            {
                if (chdir(home) != 0)
                    perror("cd");
            }
            else
                write(2, "cd: HOME not set\n", 17);
        }
        return (1);
    }
    else if (ft_strcmp(args[0], "echo") == 0)
    {
        int i = 1;
        int newline = 1;
        
        // Check for -n option
        if (args[1] && ft_strcmp(args[1], "-n") == 0)
        {
            newline = 0;
            i = 2;
        }
        
        // Print arguments
        while (args[i])
        {
            ft_putstr_fd(args[i], 1);
            if (args[i + 1])
                ft_putchar_fd(' ', 1);
            i++;
        }
        
        if (newline)
            ft_putchar_fd('\n', 1);
            
        return (1);
    }
    else if (ft_strcmp(args[0], "pwd") == 0)
    {
        char cwd[1024];
        
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            ft_putendl_fd(cwd, 1);
        }
        else
        {
            perror("pwd");
        }
        return (1);
    }
    else if (ft_strcmp(args[0], "env") == 0)
    {
        // Print all environment variables
        print_env(*env_ptr);
        return (1);
    }
    else if (ft_strcmp(args[0], "export") == 0)
    {
        // If no arguments are provided, print all environment variables
        if (!args[1])
        {
            print_env(*env_ptr);
            return (1);
        }
        
        // Process each argument
        int i = 1;
        while (args[i])
        {
            // Check if the argument has the format "name=value"
            char *equals_sign = ft_strchr(args[i], '=');
            if (equals_sign)
            {
                // Split the argument into name and value
                int name_len = equals_sign - args[i];
                char *name = ft_substr(args[i], 0, name_len);
                char *value = equals_sign + 1;
                
                // Set the environment variable
                set_env_var(env_ptr, name, value);
                
                free(name);
            }
            i++;
        }
        return (1);
    }
    else if (ft_strcmp(args[0], "unset") == 0)
    {
        // Process each argument
        int i = 1;
        while (args[i])
        {
            // Remove the environment variable
            unset_env_var(env_ptr, args[i]);
            i++;
        }
        return (1);
    }
    return (0);
}

// Signal handling moved to signals.c

int main(int argc, char **argv, char **envp)
{
    char *input;
    t_ast *ast;
    char **env;

    (void)argc;  // Not used, but included for standard signature
    (void)argv;  // Not used, but included for standard signature
    // Make a copy of the environment variables
    env = copy_env(envp);
    if (!env)
    {
        perror("Failed to copy environment");
        return (1);
    }
    
    // Setup signal handling
    setup_signals();
    
    while (1)
    {
        input = readline("$[[ minishell ]] > ");
        // If user presses Ctrl + D (EOF)
        if (!input)
        {
            printf("exit\n");
            break ;
        }
        // If the user actually typed something (not just enter)
        if (*input)
            add_history(input);
            
        ast = tokenize_input(input); // Tokenize and create the AST
        if (ast && ast->command) // Ensure there is a valid command
        {
            // Check if we have a pipeline or a single command
            if (ast->next)
            {
                // Pipeline of commands
                execute_pipeline(ast, env);
            }
            else
            {
                // Single command - check if it's a builtin first
                if (!handle_builtin(ast->args, &env))
                {
                    // If not a built-in, handle redirection and exec
                    exec_command(ast, env);  // Execute the command
                }
            }
        }
        free_ast(ast); // Free the AST after execution
        free(input); // Always free the line allocated by readline
    }
    
    // Free environment variables
    free_env(env);
    
    return (0);
}