/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 18:29:11 by anassih           #+#    #+#             */
/*   Updated: 2025/05/05 17:26:31 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tokenization.h"
#include "minishell.h"
#include "env_utils.h"

// Helper function to remove all quotes from a string
static char *remove_all_quotes(const char *str) {
    if (!str) return NULL;
    size_t len = ft_strlen(str);
    char *result = malloc(len + 1);
    if (!result) return NULL;
    
    size_t j = 0;
    for (size_t i = 0; str[i]; i++) {
        if (str[i] != '"' && str[i] != '\'') {
            result[j++] = str[i];
        }
    }
    result[j] = '\0';
    return result;
}

/* Helper to create redirection nodes */
static t_redir *create_redir_node(t_redir_type type, char *raw_target, t_context *ctx) {
    t_redir *new = ft_calloc(1, sizeof(t_redir));
    if (!new) return NULL;
    
    new->type = type;
    new->next = NULL;
    
	int heredoc_is_quoted = 0;
    size_t raw_len = ft_strlen(raw_target);
    if ((raw_target[0] == '"' && raw_target[raw_len - 1] == '"') ||
        (raw_target[0] == '\'' && raw_target[raw_len - 1] == '\'')) {
        heredoc_is_quoted = 1;
    }

    // Remove ALL quotes from redirection targets
    char *stripped = remove_all_quotes(raw_target);
    if (!stripped) {
        free(new);
        return NULL;
    }
    
    // Expand variables except in single quotes
    if (raw_target[0] != '\'') {
        char *expanded = expand_env_vars(stripped, ctx->env, ctx->exit_status);
        free(stripped);
        new->target = expanded;
    } else {
        new->target = stripped;
    }
    
    // Create pipe for heredocs
    if (type == REDIR_HEREDOC) {
		new->quoted = heredoc_is_quoted; 
        if (pipe(new->heredoc_fd) < 0) {
            perror("minishell: pipe");
            free(new->target);
            free(new);
            return NULL;
        }
    }
    return new;
}

/* Core tokenizer → AST builder */
t_ast *tokenize_input(const char *input, t_context *ctx) {
    t_ast *ast = ft_calloc(1, sizeof(t_ast));
    if (!ast) return NULL;
    
    char **tokens = smart_split(input);
    if (!tokens) {
        free(ast);
        return NULL;
    }

    // Allocate args array (size = token count + 1)
    int token_count = 0;
    while (tokens[token_count]) token_count++;
    ast->args = ft_calloc(token_count + 1, sizeof(char *));
    if (!ast->args) {
        free_split(tokens);
        free(ast);
        return NULL;
    }

    // Process tokens
    int arg_i = 0;
    t_redir **redir_tail = &ast->redirs;  // Pointer to last redir pointer
    
    for (int i = 0; tokens[i]; i++) {
        if (ft_strcmp(tokens[i], "<<") == 0 && tokens[i+1]) {
            t_redir *new = create_redir_node(REDIR_HEREDOC, tokens[++i], ctx);
            if (new) {
                *redir_tail = new;
                redir_tail = &new->next;
            }
        }
        else if (ft_strcmp(tokens[i], ">>") == 0 && tokens[i+1]) {
            t_redir *new = create_redir_node(REDIR_APPEND, tokens[++i], ctx);
            if (new) {
                *redir_tail = new;
                redir_tail = &new->next;
            }
        }
        else if (ft_strcmp(tokens[i], ">") == 0 && tokens[i+1]) {
            t_redir *new = create_redir_node(REDIR_OUT, tokens[++i], ctx);
            if (new) {
                *redir_tail = new;
                redir_tail = &new->next;
            }
        }
        else if (ft_strcmp(tokens[i], "<") == 0 && tokens[i+1]) {
            t_redir *new = create_redir_node(REDIR_IN, tokens[++i], ctx);
            if (new) {
                *redir_tail = new;
                redir_tail = &new->next;
            }
        }
        else if (ft_strcmp(tokens[i], "|") == 0) {
            // Skip pipe tokens
            continue;
        }
        else {
            // Handle quotes and expansion for command arguments
            ast->args[arg_i] = handle_quotes(tokens[i], ctx);
            if (!ast->args[arg_i]) {
                // Quote error occurred, clean up and return
                free_split(tokens);
                free_ast(ast);
                return NULL;
            }
            arg_i++;
        }
    }
    
    // Finalize command
    ast->args[arg_i] = NULL;
    if (arg_i > 0 && ast->args[0])
        ast->command = ft_strdup(ast->args[0]);
    
    free_split(tokens);
    
    // Validate AST creation
    if (!ast->command) {
        free_ast(ast);
        return NULL;
    }
    return ast;
}

/* Replaces $(...) with the result of executing the embedded command (optional) */
char *handle_command_substitution(char *input, t_context *ctx)
{
    char *start = ft_strnstr(input, "$(", ft_strlen(input));
    if (!start)
        return ft_strdup(input);  // No substitution

    char *open = start + 2;
    char *end = ft_strchr(open, ')');  // avoids shadowing
    if (!end)
        return ft_strdup(input);  // Malformed, treat literally

    // Extract subcommand
    size_t cmd_len = end - open;
    char *subcmd = ft_substr(open, 0, cmd_len);
    if (!subcmd)
        return ft_strdup(input);

    // Pipe setup
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("minishell: pipe");
        free(subcmd);
        return ft_strdup(input);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("minishell: fork");
        free(subcmd);
        close(pipefd[0]);
        close(pipefd[1]);
        return ft_strdup(input);
    }

    if (pid == 0) {
        // Child: redirect stdout to pipe
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        t_ast *ast = tokenize_input(subcmd, ctx);
        if (ast && ast->command)
			exec_command(ast, ctx);
        free_ast(ast);
        exit(0);
    }

    // Parent: read from pipe
    close(pipefd[1]);
    char buffer[4096];
    ssize_t bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1);
    close(pipefd[0]);
    waitpid(pid, NULL, 0);

    buffer[(bytes_read > 0) ? bytes_read : 0] = '\0';
    free(subcmd);

    // Assemble final result string
    size_t prefix_len = start - input;
    //size_t suffix_len = ft_strlen(end + 1);

    char *prefix = ft_substr(input, 0, prefix_len);
    char *suffix = ft_strdup(end + 1);
    char *mid = ft_strtrim(buffer, "\n");

    char *partial = ft_strjoin(prefix, mid);
    char *final = ft_strjoin(partial, suffix);

    free(prefix);
    free(suffix);
    free(mid);
    free(partial);

    return final;
}


/* Updated quote handling with context and error detection */
char *handle_quotes(char *token, t_context *ctx)
{
    if (!token)
        return NULL;

    size_t len = ft_strlen(token);
    if (len == 0)
        return ft_strdup("");
	
	int is_fully_single_quoted = (token[0] == '\'' && token[len - 1] == '\'');
    // Allocate buffer for cleaned result
    char *result = ft_calloc(len + 1, sizeof(char));
    if (!result)
        return NULL;

    size_t i = 0;
    size_t j = 0;
    int in_double_quotes = 0;
    int in_single_quotes = 0;

    while (i < len)
    {
        // Inside single quotes: treat all characters literally
        if (in_single_quotes)
        {
            if (token[i] == '\'')
            {
                in_single_quotes = 0;
                i++;
                continue;
            }
            result[j++] = token[i++];
            continue;
        }

        // Toggle quote states
        if (token[i] == '"' && !in_single_quotes)
        {
            in_double_quotes = !in_double_quotes;
            i++;
            continue;
        }
        if (token[i] == '\'' && !in_double_quotes)
        {
            in_single_quotes = !in_single_quotes;
            i++;
            continue;
        }

        // Handle escape sequences inside double quotes
        if (in_double_quotes && token[i] == '\\' && token[i + 1])
        {
            if (token[i + 1] == '"' || token[i + 1] == '\\' || token[i + 1] == '$')
            {
                result[j++] = token[i + 1]; // Push escaped char
                i += 2;
                continue;
            }
            else
            {
                result[j++] = token[i]; // Preserve backslash
                i++;
                continue;
            }
        }

        // Default character handling
        result[j++] = token[i++];
    }

    result[j] = '\0';

    // Unmatched quote detection
    if (in_double_quotes || in_single_quotes)
    {
        ft_putstr_fd("minishell: error: unmatched quote\n", STDERR_FILENO);
        ctx->exit_status = 2;
        free(result);
        return NULL;
    }

    // Expand environment variables unless inside single quotes
    if (!is_fully_single_quoted) {
		char *substituted = handle_command_substitution(result, ctx);
		char *expanded = expand_env_vars(substituted, ctx->env, ctx->exit_status);
		free(result);
		free(substituted);
		return expanded;
	}
	return result;
	
}

/* Double quote processing now uses context */
char *process_double_quotes(const char *token, t_context *ctx) {
    size_t len = ft_strlen(token);
    if (len < 2) return ft_strdup("");
    
    char *inner = ft_substr(token, 1, len - 2);
    if (!inner) return NULL;
    
    char *expanded = expand_env_vars(inner, ctx->env, ctx->exit_status);
    free(inner);
    return expanded;
}