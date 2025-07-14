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
#include "signals.h"

// Returns builtin type
t_builtin get_builtin_type(const char *cmd)
{
    if (!cmd) return NOT_BUILTIN;
    if (!ft_strcmp(cmd, "cd") || !ft_strcmp(cmd, "export") ||
        !ft_strcmp(cmd, "unset") || !ft_strcmp(cmd, "exit"))
        return PARENT_BUILTIN;
    if (!ft_strcmp(cmd, "echo") || !ft_strcmp(cmd, "pwd") || 
        !ft_strcmp(cmd, "env"))
        return CHILD_BUILTIN;
    return NOT_BUILTIN;
}

/*
** Split `line` on unquoted '|' into a NULL-terminated array,
** trimming whitespace around each segment.
*/
char    **split_on_pipe(const char *line)
{
    size_t  len = ft_strlen(line);
    char    **parts;
    size_t  count;
    size_t  cap;
    size_t  start;
    char    quote;
    size_t  i;

    cap   = 8;
    count = 0;
    parts = malloc(sizeof(*parts) * cap);
    if (!parts)
        return (NULL);
    start = 0;
    quote = 0;
    for (i = 0; i <= len; ++i)
    {
        if ((line[i] == '"' || line[i] == '\'') && !quote)
            quote = line[i];
        else if (line[i] == quote)
            quote = 0;
        if ((!quote && (line[i] == '|' || line[i] == '\0')))
        {
            size_t  a = start;
            size_t  seglen = i - start;
            while (seglen && ft_isspace((unsigned char)line[a]))
            {
                ++a;
                --seglen;
            }
            while (seglen && ft_isspace((unsigned char)line[a + seglen - 1]))
                --seglen;
            parts[count++] = ft_substr(line, a, seglen);
            if (count + 1 >= cap) {
                char **new_parts = realloc(parts, sizeof(*parts) * cap * 2);
                if (!new_parts) {
                    free_split(parts);
                    return NULL;
                }
                parts = new_parts;
                cap *= 2;
            }
            start = i + 1;
        }
    }
    parts[count] = NULL;
    return (parts);
}

/* Process heredocs for all commands in pipeline */
static void process_heredocs(t_ast *head, t_context *ctx)
{
    setup_heredoc_signals();
    g_signal = 0;  // Reset signal flag before heredoc
    
    for (t_ast *node = head; node && !g_signal; node = node->next) {
        for (t_redir *r = node->redirs; r; r = r->next) {
            if (r->type != REDIR_HEREDOC) continue;
            
            char *line;
            while (!g_signal) {
                line = readline("> ");
                if (!line) break;  // EOF
                
                if (ft_strcmp(line, r->target) == 0) {
                    free(line);
                    break;
                }
                
                if (!r->quoted) {
                    char *expanded = expand_env_vars(line, ctx->env, ctx->exit_status);
                    write(r->heredoc_fd[1], expanded, strlen(expanded));
                    write(r->heredoc_fd[1], "\n", 1);
                    free(expanded);
                } else {
                    write(r->heredoc_fd[1], line, strlen(line));
                    write(r->heredoc_fd[1], "\n", 1);
                }
                free(line);
            }
            close(r->heredoc_fd[1]);
            r->heredoc_fd[1] = -1;
            
            if (g_signal) break;
        }
    }
    setup_shell_signals();
}

int handle_builtin(char **args, t_context *ctx)
{
    if (!args[0] || !*args[0]) {  // Handle empty command
        return 0;
    }
    
    if (ft_strcmp(args[0], "exit") == 0)
        return builtin_exit(args, ctx);
    else if (ft_strcmp(args[0], "cd") == 0)
        return builtin_cd(args, ctx);
    else if (ft_strcmp(args[0], "env") == 0)
        return builtin_env(ctx);  // FIXED: pass context
    else if (!ft_strcmp(args[0], "pwd"))
    {
        /* If an extra argument is passed, error out like Bash */
        if (args[1])
        {
            ft_putstr_fd("minishell: pwd: too many arguments\n", STDERR_FILENO);
            ctx->exit_status = 1;
            return 1;
        }
        return builtin_pwd(ctx);
    }        
    else if (ft_strcmp(args[0], "echo") == 0)
        return builtin_echo(args, ctx);  // FIXED: pass context
    else if (ft_strcmp(args[0], "export") == 0)
        return builtin_export(args, ctx);
    else if (ft_strcmp(args[0], "unset") == 0)
        return builtin_unset(args, ctx);
    
    return 1;
}

int main(int argc, char **argv, char **envp)
{
    t_context ctx = {
        .env = copy_environment(envp),
        .exit_status = 0
    };
    char *input;
    t_ast *head = NULL;
    int should_exit = 0;

    (void)argc;
    (void)argv;
    setup_shell_signals();

    while (!should_exit)
    {
        g_signal = 0;
        input = readline("$ [minishell] > ");
        if (!input) {
            ft_putendl_fd("exit", STDOUT_FILENO);
            break;
        }
        
        // Handle empty command
        char *trimmed = ft_strtrim(input, " ");
        if (!trimmed || !*trimmed) {
            free(input);
            free(trimmed);
            ctx.exit_status = 0;
            continue;
        }
        free(trimmed);

        if (*input) add_history(input);
        char **segments = split_on_pipe(input);
        free(input);
        if (!segments) continue;
        
        // Build AST pipeline
        head = NULL;
        t_ast *tail = NULL;
        int parse_error = 0;
        
        for (int i = 0; segments[i]; i++) {
            t_ast *node = tokenize_input(segments[i], &ctx);
            if (!node) {
                parse_error = 1;
                break;
            }
            
            if (!head) head = tail = node;
            else tail->next = node, tail = node;
        }
        free_split(segments);
        
        if (parse_error) {
            free_ast(head);
            ctx.exit_status = 1;
            continue;
        }

        // Process heredocs
        if (head) {
            process_heredocs(head, &ctx);
            if (g_signal) {
                ctx.exit_status = 130;
                free_ast(head);
                head = NULL;
                continue;
            }
        }

        // Execute commands
        if (head && head->command) {
            t_builtin type = get_builtin_type(head->command);
            
            if (type == PARENT_BUILTIN && !head->next) {
                int ret = handle_builtin(head->args, &ctx);
                if (ret == 255) should_exit = 1;  // Exit builtin
            } 
            else {
                if (head->next) execute_pipeline(head, &ctx);
                else exec_command(head, &ctx);
            }
        }
        
        if (head) free_ast(head);
    }
    
    clear_history();
    free_environment(ctx.env);
    return ctx.exit_status;
}