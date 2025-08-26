/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:14:09 by anassih           #+#    #+#             */
/*   Updated: 2025/08/26 09:36:39 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/tokenization.h"
#include "../includes/env_utils.h"

// Extracts the first word after trimming leading spaces/tabs
static char *extract_first_command_word(char *str)
{
	char    *trimmed;
	char    *first_word;
	size_t  j = 0;
	size_t  len;

	trimmed = ft_strtrim(str, " \t");
	if (!trimmed)
		return (NULL);
	while (trimmed[j] && !ft_isspace(trimmed[j]))
		j++;
	len = j;
	first_word = ft_substr(trimmed, 0, len);
	free(trimmed);
	return (first_word);
}

t_ast	*alloc_ast(char **tokens)
{
	int		count;
	t_ast	*ast;

	count = 0;
	while (tokens[count])
		count++;
	ast = ft_calloc(1, sizeof(t_ast));
	if (!ast)
		return (NULL);
	ast->args = ft_calloc(count + 1, sizeof(char *));
	if (!ast->args)
		return (free(ast), (t_ast *) NULL);
	return (ast);
}

void	fill_redirections(t_ast *ast, char **tokens, t_context *ctx)
{
	t_redir	**tail;
	int		i;

	tail = &ast->redirs;
	i = 0;
	while (tokens[i])
	{
		if (!ft_strcmp(tokens[i], "<<") && tokens[i + 1])
		{
			*tail = create_redir_node(REDIR_HEREDOC, tokens[i + 1], ctx);
			if (*tail)
				tail = &(*tail)->next;
			i += 2;
		}
		else if (!ft_strcmp(tokens[i], ">>") && tokens[i + 1])
		{
			*tail = create_redir_node(REDIR_APPEND, tokens[i + 1], ctx);
			if (*tail)
				tail = &(*tail)->next;
			i += 2;
		}
		else if (!ft_strcmp(tokens[i], ">") && tokens[i + 1])
		{
			*tail = create_redir_node(REDIR_OUT, tokens[i + 1], ctx);
			if (*tail)
				tail = &(*tail)->next;
			i += 2;
		}
		else if (!ft_strcmp(tokens[i], "<") && tokens[i + 1])
		{
			*tail = create_redir_node(REDIR_IN, tokens[i + 1], ctx);
			if (*tail)
				tail = &(*tail)->next;
			i += 2;
		}
		else
			i++;
	}
}

int	fill_args(t_ast *ast, char **tokens, t_context *ctx)
{
	int		i;
	int		arg_i;
	int		quoted;
	char	*tok;

	i = 0;
	arg_i = 0;
	int prev_exit_status = ctx->exit_status;
	while (tokens[i])
	{
		if (!ft_strcmp(tokens[i], "<<") && tokens[i + 1])
			i += 2;
		else if (!ft_strcmp(tokens[i], ">>") && tokens[i + 1])
			i += 2;
		else if (!ft_strcmp(tokens[i], ">") && tokens[i + 1])
			i += 2;
		else if (!ft_strcmp(tokens[i], "<") && tokens[i + 1])
			i += 2;
		else if (!ft_strcmp(tokens[i], "|"))
			i++;
		else
		{
			quoted = 0;
			tok = tokens[i];
			if ((tok[0] == '"' && tok[ft_strlen(tok) - 1] == '"'))
				quoted = 1;
			else if ((tok[0] == '\'' && tok[ft_strlen(tok) - 1] == '\''))
				quoted = 1;
			int tmp = ctx->exit_status;
			ctx->exit_status = prev_exit_status;
			ast->args[arg_i] = handle_quotes(tok, ctx);
			ctx->exit_status = tmp;
			if (!ast->args[arg_i])
				return (0);
			if (arg_i == 0)
			{
				if (quoted)
					ast->command = ft_strdup(tok);
				else
					ast->command = extract_first_command_word(ast->args[0]);
				if (!ast->command)
					return (0);
			}
			arg_i++;
			i++;
		}
	}
	ast->args[arg_i] = NULL;
	return (1);
}

// Core tokenizer → AST builder
t_ast	*tokenize_input(const char *input, t_context *ctx)
{
	char	**tokens;
	t_ast	*ast;
	int		ok;

	tokens = smart_split(input);
	if (!tokens)
	{
		ctx->exit_status = 2;
		return (NULL);
	}
	ast = alloc_ast(tokens);
	if (!ast)
		return (free_split(tokens), (t_ast *) NULL);
	fill_redirections(ast, tokens, ctx);
	ok = fill_args(ast, tokens, ctx);
	// If no args/command but heredoc redir exists, keep AST node
	if (!ok && ast->redirs && ast->redirs->type == REDIR_HEREDOC)
		ok = 1;
	free_split(tokens);
	if (!ok || (!ast->command && !ast->redirs))
		return (free_ast(ast), (t_ast *) NULL);
	return (ast);
}
