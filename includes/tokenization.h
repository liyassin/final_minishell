/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenization.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 21:35:01 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 07:07:05 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKENIZATION_H
# define TOKENIZATION_H

# include <ctype.h>
# include <libft.h>
# include <readline/readline.h>
# include <stddef.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include "env_utils.h"

typedef struct s_context	t_context;

typedef enum e_redir_type
{
	REDIR_NONE = 0,
	REDIR_IN,
	REDIR_OUT,
	REDIR_APPEND,
	REDIR_HEREDOC
}	t_redir_type;

typedef struct s_redir
{
	t_redir_type	type;
	char			*target;
	int				heredoc_fd[2];
	int				quoted;
	struct s_redir	*next;
}	t_redir;

typedef struct s_ast
{
	char			*command;
	char			**args;
	t_redir			*redirs;
	struct s_ast	*next;
}	t_ast;

typedef enum e_split_state
{
	STATE_NORMAL,
	STATE_IN_SQUOTE,
	STATE_IN_DQUOTE
}	t_split_state;

typedef struct s_split_ctx
{
	char			*cur;
	size_t			cur_len;
	size_t			cur_cap;
	char			**tokens;
	size_t			count;
	size_t			cap;
	t_split_state	state;
}	t_split_ctx;

typedef struct s_token_ctx
{
	char			***tokens;
	size_t			*count;
	size_t			*cap;
}	t_token_ctx;

t_ast	*tokenize_input(const char *input, t_context *ctx);
char	*process_double_quotes(const char *token, t_context *ctx);
char	*handle_quotes(char *token, t_context *ctx);
char	**smart_split(const char *input);
char	**split_by_pipes(const char *line);
void	update_quote_state(char c, char *quote);
char	*handle_command_substitution(char *input, t_context *ctx);

typedef struct s_pipe_context
{
	char	**parts;
	size_t	*count;
	size_t	*cap;
}	t_pipe_context;

char	**process_token_loop(const char *line, t_pipe_context *ctx);
char	**handle_pipe_segment(const char *line, t_pipe_context *ctx,
			size_t start, size_t i);

void	free_ast(t_ast *ast);

void	init_ctx(t_split_ctx *ctx);
void	flush_token(t_split_ctx *ctx);
void	remove_whitespace_only_tokens(t_split_ctx *ctx);
int		process_ws_op(char c, t_split_ctx *ctx, const char *input, size_t *i);
void	update_state_and_append(char c, t_split_ctx *ctx);
void	*ft_realloc(void *ptr, size_t old_size, size_t new_size);
char	*append_char(char *str, char c, size_t *len, size_t *cap);
void	add_token(char ***tokens, size_t *count, size_t *cap, char *token);
void	handle_operator(const char *input, size_t *i, t_token_ctx *ctx);
char	*trim_segment(const char *line, size_t start, size_t end);
t_ast	*alloc_ast(char **tokens);
void	fill_redirections(t_ast *ast, char **tokens, t_context *ctx);
int		fill_args(t_ast *ast, char **tokens, t_context *ctx);
t_ast	*tokenize_input(const char *input, t_context *ctx);
t_redir	*create_redir_node(t_redir_type type, char *raw_target, t_context *ctx);
void	process_heredocs(t_ast *head, t_context *ctx);

int		syntax_error_near_token(char *token);
int		is_operator(char *token);
int		validate_pipe_syntax(char **tokens, int i);
int		validate_redirection_syntax(char **tokens, int i);
int		check_pipe_start(char **tokens, t_context *ctx);
int		is_triple_pipe_error(char **tokens, int i, t_context *ctx);
int		is_invalid_redir_token(char **tokens, int i, t_context *ctx);
void	add_redir_to_list(t_redir ***tail, t_redir_type type, char *target,
			t_context *ctx);
int		process_redirection_token(t_redir ***tail, char **tokens, int i,
			t_context *ctx);
int		skip_redirection_tokens(char **tokens, int i);
char	*extract_first_command_word(char *str);
int		set_command_if_first_arg(t_ast *ast, char *tok, char *expanded,
			int quoted);
int		process_argument_token(t_ast *ast, char *tok, int arg_i,
			t_context *ctx);
int		validate_syntax(char **tokens, t_context *ctx);

#endif