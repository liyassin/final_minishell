/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 10:13:32 by anassih           #+#    #+#             */
/*   Updated: 2025/04/28 23:54:19 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tokenization.h"

// Function to check if a character is a whitespace
int is_whitespace(char c)
{
    return (c == ' ' || c == '\t' || c == '\n');
}

// Function to check if a character is a special shell operator
int is_operator(char c)
{
    return (c == '|' || c == '<' || c == '>');
}

// Count the number of tokens in a command
int count_tokens(char *input)
{
    int count = 0;
    int i = 0;
    int in_token = 0;
    char quote = 0;

    while (input[i])
    {
        // Handle quotes
        if ((input[i] == '\'' || input[i] == '\"') && (!quote || quote == input[i]))
        {
            if (quote)
                quote = 0;
            else
                quote = input[i];
            i++;
            continue;
        }

        // Skip if inside quotes
        if (quote)
        {
            i++;
            continue;
        }

        // Handle operators (|, <, >)
        if (is_operator(input[i]))
        {
            if (in_token)
                in_token = 0;
            
            count++;
            
            // Handle >> as a single token
            if (input[i] == '>' && input[i + 1] == '>')
                i++;
                
            i++;
            continue;
        }

        // Handle whitespace
        if (is_whitespace(input[i]))
        {
            if (in_token)
                in_token = 0;
            i++;
            continue;
        }

        // Handle normal characters
        if (!in_token)
        {
            in_token = 1;
            count++;
        }
        i++;
    }
    
    return count;
}

// Extract the next token from the input string
char *extract_token(char **input_ptr)
{
    char *input = *input_ptr;
    char *start;
    char *token;
    int len = 0;
    char quote = 0;

    // Skip leading whitespace
    while (*input && is_whitespace(*input))
        input++;

    // Check for empty string
    if (!*input)
    {
        *input_ptr = input;
        return NULL;
    }

    start = input;

    // Handle operators (|, <, >)
    if (is_operator(*input))
    {
        len = 1;
        // Handle >> as a single token
        if (*input == '>' && *(input + 1) == '>')
            len = 2;
            
        token = ft_substr(input, 0, len);
        *input_ptr = input + len;
        return token;
    }

    // Extract a normal token (word)
    while (*input)
    {
        // Handle quotes
        if (*input == '\'' || *input == '\"')
        {
            if (!quote)
                quote = *input;
            else if (quote == *input)
                quote = 0;
            input++;
            len++;
            continue;
        }

        // Skip if inside quotes
        if (quote)
        {
            input++;
            len++;
            continue;
        }

        // Stop at whitespace or operators if not in quotes
        if (is_whitespace(*input) || is_operator(*input))
            break;

        input++;
        len++;
    }

    token = ft_substr(start, 0, len);
    *input_ptr = input;
    return token;
}

// Process tokens into an AST node
t_ast *process_tokens(char **tokens, int token_count)
{
    t_ast *ast = malloc(sizeof(t_ast));
    int i = 0;
    int j = 0;
    int arg_count = 0;

    if (!ast)
        return NULL;

    // Initialize AST node
    ast->command = NULL;
    ast->args = NULL;
    ast->input_file = NULL;
    ast->output_file = NULL;
    ast->next = NULL;

    // Count arguments (excluding redirections)
    for (i = 0; i < token_count; i++)
    {
        if (ft_strcmp(tokens[i], "<") == 0 || ft_strcmp(tokens[i], ">") == 0 || 
            ft_strcmp(tokens[i], ">>") == 0)
        {
            i++; // Skip the redirection operator and its argument
        }
        else if (ft_strcmp(tokens[i], "|") == 0)
        {
            break; // Stop at pipe
        }
        else
        {
            arg_count++;
        }
    }

    // Allocate args array (including NULL terminator)
    ast->args = malloc(sizeof(char*) * (arg_count + 1));
    if (!ast->args)
    {
        free(ast);
        return NULL;
    }

    // Process tokens
    i = 0;
    j = 0;
    while (i < token_count)
    {
        if (ft_strcmp(tokens[i], "<") == 0)
        {
            // Input redirection
            if (i + 1 < token_count)
            {
                ast->input_file = ft_strdup(tokens[i + 1]);
                i += 2;
            }
            else
                i++;
        }
        else if (ft_strcmp(tokens[i], ">") == 0)
        {
            // Output redirection
            if (i + 1 < token_count)
            {
                ast->output_file = ft_strdup(tokens[i + 1]);
                i += 2;
            }
            else
                i++;
        }
        else if (ft_strcmp(tokens[i], "|") == 0)
        {
            // Pipeline
            i++;
            if (i < token_count)
            {
                // Process the rest of the tokens as a new command
                ast->next = process_tokens(&tokens[i], token_count - i);
            }
            break;
        }
        else
        {
            // Command and arguments
            if (j == 0)
                ast->command = ft_strdup(tokens[i]); // First token is the command
            
            ast->args[j++] = ft_strdup(tokens[i]);
            i++;
        }
    }

    // NULL-terminate the args array
    ast->args[j] = NULL;

    return ast;
}

// Tokenize the input and create an AST
t_ast *tokenize_input(char *input)
{
    char **tokens;
    t_ast *ast;
    int token_count;
    int i;
    char *input_ptr;
    
    // Count how many tokens we need
    token_count = count_tokens(input);
    if (token_count == 0)
        return NULL;
        
    // Allocate memory for tokens
    tokens = malloc(sizeof(char*) * (token_count + 1));
    if (!tokens)
        return NULL;
        
    // Extract tokens
    input_ptr = input;
    for (i = 0; i < token_count; i++)
    {
        tokens[i] = extract_token(&input_ptr);
        if (!tokens[i])
            break;
    }
    tokens[i] = NULL;
    
    // Process tokens into an AST
    ast = process_tokens(tokens, i);
    
    // Free tokens
    for (i = 0; tokens[i]; i++)
        free(tokens[i]);
    free(tokens);
    
    return ast;
}