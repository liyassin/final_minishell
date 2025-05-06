/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenization.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 10:12:29 by anassih           #+#    #+#             */
/*   Updated: 2025/04/28 23:50:58 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKENIZATION_H
# define TOKENIZATION_H

# include <stddef.h> // size_t
# include <string.h> // malloc, free
# include <unistd.h> 
# include <stdlib.h>
# include <stdio.h> // perror
# include "libft/inc/libft.h" // <<-- khassni nzid ft_strtok fiha o n7eyed stdlib o unistd

typedef struct s_ast
{
	char			*command;  // Command to execute (e.g., "echo")
	char			**args;    // Arguments for the command (e.g., ["hello", "world"])
	char			*input_file; // Input redirection file (e.g., "< input.txt")
	char			*output_file; // Output redirection file (e.g., "> output.txt")
	struct	s_ast	*next; // Pointer to the next command in a pipeline
}					t_ast;

// Helper functions for tokenization
int     is_whitespace(char c);
int     is_operator(char c);
int     count_tokens(char *input);
char    *extract_token(char **input_ptr);
t_ast   *process_tokens(char **tokens, int token_count);

// Main tokenizer function
t_ast	*tokenize_input(char *input);

#endif