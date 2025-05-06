/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 09:46:40 by anassih           #+#    #+#             */
/*   Updated: 2025/05/05 16:37:17 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Detect and execute builtin commands */
int handle_builtin(char **args, char ***shell_env)
{
    if (!args[0])
        return (0);
    if (ft_strcmp(args[0], "exit") == 0)
        return (builtin_exit(args));
    else if (ft_strcmp(args[0], "cd") == 0)
        return (builtin_cd(args, shell_env));
    else if (ft_strcmp(args[0], "env") == 0)
        return (builtin_env(*shell_env));
    else if (ft_strcmp(args[0], "pwd") == 0)
        return (builtin_pwd());
    else if (ft_strcmp(args[0], "echo") == 0)
        return (builtin_echo(args));
    else if (ft_strcmp(args[0], "export") == 0)
        return (builtin_export(args, shell_env));
    else if (ft_strcmp(args[0], "unset") == 0)
        return (builtin_unset(args, shell_env));
    return (0);
}

/* Main entry point of the minishell */
int main(int argc, char **argv, char **envp)
{
    char    *input;
    t_ast   *ast;
    char    **shell_env; // shell's environment

    (void)argc;
    (void)argv;
    shell_env = copy_environment(envp);
    while (1)
    {
        /* Display prompt and read user input */
        input = readline("$ [minishell] > ");
        if (!input)
        {
            printf("exit\n");
            break ;
        }
        if (*input)
            add_history(input); 
            // Place string at the end of the history list. The associated data field (if any) is set to NULL. 
            // If the maximum number of history entries has been set using stifle_history(), 
            // and the new number of history entries would exceed that maximum, the oldest history entry is removed.
            // https://tiswww.case.edu/php/chet/readline/history.html
        /* Tokenize input into AST */
        ast = tokenize_input(input, shell_env);
        if (ast && ast->command)
        {
            char *term;
            int tmp_fd;
            int i = 0;

            term = ttyname(1); // callable service obtains the pathname of the terminal that is associated with the file descriptor.
            /* If not a builtin, execute as external command */
            handle_redirection(ast);
            if (!handle_builtin(ast->args, &shell_env))
                exec_command(ast, shell_env);
            while (i < 3)
            {
                tmp_fd = open(term, O_RDWR);
                if (tmp_fd < 0)
                    return (perror("Error in reseting fd"), errno);
                tmp_fd = dup2(tmp_fd, i);
                if (tmp_fd < 0)
                    return (perror("Error in reseting fd"), errno);
                i++;
            }
        }
        /* Free allocated memory */
        free_ast(ast);
        free(input);
    }
    free_environment(shell_env);
    return (0);
}
