/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenization.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 10:12:29 by anassih           #+#    #+#             */
/*   Updated: 2025/05/02 00:40:54 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKENIZATION_H
# define TOKENIZATION_H

# include <stddef.h>
# include <stdlib.h>
# include <stdio.h>
# include "libft/inc/libft.h"
# include "env_utils.h"

/* AST structure definition */
typedef struct s_ast
{
	char			*command;  /* Command to execute (e.g., "echo") */
	char			**args;    /* Arguments for the command (e.g., ["hello", "world"]) */
	char			**input_file; /* Input redirection file (e.g., "< input.txt") */
	char			**output_file; /* Output redirection file (e.g., "> output.txt") */
	int				flag; /*1 = input | 2 = output | 3 = both */
	int				count_in; /*count for input in tokenizer for multiple redirections*/
	int				count_out; /*count for output in tokenizer for tmultiple redirections*/
	struct s_ast	*next; /* Pointer to the next command in a pipeline */
}					t_ast;

/* Functions for tokenization and token handling */
t_ast	*tokenize_input(const char *input, char **envp);
char	*expand_env_var(const char *str, char **envp);
char	*process_double_quotes(const char *token, char **envp);
char	*handle_quotes(char *token, char **envp);
char	**smart_split(const char *input);

/* Forward declaration of the AST freeing function 
 * (actual implementation is in cleanup) */
void	free_ast(t_ast *ast);

#endif