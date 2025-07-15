/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   smart_split.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 15:16:55 by anassih           #+#    #+#             */
/*   Updated: 2025/07/15 07:20:13 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Improved smart_split: merges quoted fragments into single tokens, strips quotes */
/* smart_split.c */
#include "tokenization.h"
#include <stdlib.h>
#include <stdbool.h>
#include "libft/inc/libft.h"

typedef enum {
    STATE_NORMAL,
    STATE_IN_SQUOTE,
    STATE_IN_DQUOTE
} t_split_state;

// Custom realloc implementation for buffer management
static void *ft_realloc(void *ptr, size_t old_size, size_t new_size) {
    if (new_size == 0) {
        free(ptr);
        return NULL;
    }
    
    void *new_ptr = malloc(new_size);
    if (!new_ptr) return NULL;
    
    if (ptr) {
        size_t copy_size = old_size < new_size ? old_size : new_size;
        ft_memcpy(new_ptr, ptr, copy_size);
        free(ptr);
    }
    return new_ptr;
}

// Efficient buffer-based character appending
static char *append_char(char *str, char c, size_t *len, size_t *cap) {
    if (*len + 1 >= *cap) {
        *cap = (*cap == 0) ? 16 : *cap * 2;
        str = ft_realloc(str, *len + 1, *cap); // +1 for null terminator
        if (!str) return NULL;
    }
    str[(*len)++] = c;
    str[*len] = '\0';
    return str;
}

static void add_token(char ***tokens, size_t *count, size_t *cap, char *token) {
    if (*count + 1 >= *cap) {
        *cap = (*cap == 0) ? 16 : *cap * 2;
        char **new_tokens = ft_realloc(*tokens, 
                                      *count * sizeof(char *), 
                                      *cap * sizeof(char *));
        if (!new_tokens) {
            free(token);
            return;
        }
        *tokens = new_tokens;
    }
    (*tokens)[(*count)++] = token;
    (*tokens)[*count] = NULL;
}

// Handle only mandatory operators (no && or ||)
static void handle_operator(const char *input, size_t *i, char ***tokens, 
                           size_t *count, size_t *cap) {
    char op[3] = {input[*i], '\0', '\0'};
    
    // Check for two-character operators: >> and <<
    if ((input[*i] == '>' && input[*i+1] == '>') || 
        (input[*i] == '<' && input[*i+1] == '<')) {
        op[1] = input[*i]; // Same as first character
        op[2] = '\0';
        (*i)++;
    }
    
    add_token(tokens, count, cap, ft_strdup(op));
}

char **smart_split(const char *input) {
    t_split_state state = STATE_NORMAL;
    char *current = NULL;
    size_t current_len = 0;
    size_t current_cap = 0;
    char **tokens = NULL;
    size_t count = 0;
    size_t cap = 0;

    for (size_t i = 0; input[i]; i++) {
        char c = input[i];
        
        switch (state) {
            case STATE_NORMAL:
                if (ft_isspace(c)) {
                    if (current) {
                        add_token(&tokens, &count, &cap, current);
                        current = NULL;
                        current_len = current_cap = 0;
                    }
                    continue;
                }
                else if (c == '\'') {
                    state = STATE_IN_SQUOTE;
                    current = append_char(current, c, &current_len, &current_cap);
                }
                else if (c == '"') {
                    state = STATE_IN_DQUOTE;
                    current = append_char(current, c, &current_len, &current_cap);
                }
                else if (c == '>' || c == '<' || c == '|') {
                    if (current) {
                        add_token(&tokens, &count, &cap, current);
                        current = NULL;
                        current_len = current_cap = 0;
                    }
                    handle_operator(input, &i, &tokens, &count, &cap);
                }
                else {
                    current = append_char(current, c, &current_len, &current_cap);
                }
                break;
                
            case STATE_IN_SQUOTE:
                current = append_char(current, c, &current_len, &current_cap);
                if (c == '\'') 
                    state = STATE_NORMAL;
                break;
                
            case STATE_IN_DQUOTE:
                current = append_char(current, c, &current_len, &current_cap);
                if (c == '"') 
                    state = STATE_NORMAL;
                break;
        }
    }

    // Handle unterminated quotes
    if (state != STATE_NORMAL) {
        ft_putstr_fd("minishell: error: unmatched quote\n", STDERR_FILENO);
        if (current) free(current);
        free_split(tokens);
        return NULL;
    }

    if (current) {
        add_token(&tokens, &count, &cap, current);
    }

    return tokens;
}
