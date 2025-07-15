/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenization.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 10:12:29 by anassih           #+#    #+#             */
/*   Updated: 2025/07/15 07:15:34 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKENIZATION_H
# define TOKENIZATION_H

# include <stddef.h>
# include <stdlib.h>
# include <stdio.h>
# include <ctype.h>
# include <readline/readline.h>
# include <string.h>
# include <unistd.h>
# include "libft/inc/libft.h"
//# include "minishell.h"
# include "env_utils.h"

typedef struct s_context t_context;  // Add forward declaration

/* Redirection types */
typedef enum e_redir_type {
    REDIR_NONE = 0,
    REDIR_IN,       // <
    REDIR_OUT,      // >
    REDIR_APPEND,   // >>
    REDIR_HEREDOC   // <<
} t_redir_type;

/* Unified redirection structure */
typedef struct s_redir {
    t_redir_type   type;
    char           *target;       // File or delimiter
    int            heredoc_fd[2]; // Pipe FDs for heredoc
    int            quoted;        // 1 if target was quoted
    struct s_redir *next;         // Next redirection
} t_redir;

/* Simplified AST structure */
typedef struct s_ast {
    char         *command;    // Command name (e.g., "echo")
    char         **args;      // Argument array
    t_redir      *redirs;     // Linked list of redirections
    struct s_ast *next;       // Next command in pipeline
} t_ast;


/* Functions for tokenization and token handling */
t_ast   *tokenize_input(const char *input, t_context *ctx);
char    *process_double_quotes(const char *token, t_context *ctx);
char    *handle_quotes(char *token, t_context *ctx);
char    **smart_split(const char *input);
char *handle_command_substitution(char *input, t_context *ctx);

/* Forward declaration of the AST freeing function 
 * (actual implementation is in cleanup) */
void	free_ast(t_ast *ast);

#endif