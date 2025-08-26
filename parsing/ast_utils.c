/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:14:09 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 00:37:48 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/tokenization.h"
#include "../includes/env_utils.h"

void	free_redirs(t_redir *redir)
{
	t_redir	*tmp;

	while (redir)
	{
		tmp = redir->next;
		if (redir->target)
			free(redir->target);
		free(redir);
		redir = tmp;
	}
}

void	free_ast(t_ast *ast)
{
	int		i;
	t_ast	*tmp;

	while (ast)
	{
		tmp = ast->next;
		if (ast->args)
		{
			i = 0;
			while (ast->args[i])
			{
				free(ast->args[i]);
				i++;
			}
			free(ast->args);
		}
		if (ast->command)
			free(ast->command);
		if (ast->redirs)
			free_redirs(ast->redirs);
		free(ast);
		ast = tmp;
	}
}

static char	*extract_first_command_word(char *str)
{
	char	*trimmed;
	char	*first_word;
	size_t	j;
	size_t	len;

	j = 0;
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

static void	add_redir_to_list(t_redir ***tail, t_redir_type type, char *target,
		t_context *ctx)
{
	**tail = create_redir_node(type, target, ctx);
	if (**tail)
		*tail = &(**tail)->next;
}

static int	process_redirection_token(t_redir ***tail, char **tokens, int i,
		t_context *ctx)
{
	if (!ft_strcmp(tokens[i], "<<") && tokens[i + 1])
	{
		add_redir_to_list(tail, REDIR_HEREDOC, tokens[i + 1], ctx);
		return (i + 2);
	}
	else if (!ft_strcmp(tokens[i], ">>") && tokens[i + 1])
	{
		add_redir_to_list(tail, REDIR_APPEND, tokens[i + 1], ctx);
		return (i + 2);
	}
	else if (!ft_strcmp(tokens[i], ">") && tokens[i + 1])
	{
		add_redir_to_list(tail, REDIR_OUT, tokens[i + 1], ctx);
		return (i + 2);
	}
	else if (!ft_strcmp(tokens[i], "<") && tokens[i + 1])
	{
		add_redir_to_list(tail, REDIR_IN, tokens[i + 1], ctx);
		return (i + 2);
	}
	return (i + 1);
}

void	fill_redirections(t_ast *ast, char **tokens, t_context *ctx)
{
	t_redir	**tail;
	int		i;

	tail = &ast->redirs;
	i = 0;
	while (tokens[i])
		i = process_redirection_token(&tail, tokens, i, ctx);
}

static int	skip_redirection_tokens(char **tokens, int i)
{
	if (!ft_strcmp(tokens[i], "<<") && tokens[i + 1])
		return (i + 2);
	else if (!ft_strcmp(tokens[i], ">>") && tokens[i + 1])
		return (i + 2);
	else if (!ft_strcmp(tokens[i], ">") && tokens[i + 1])
		return (i + 2);
	else if (!ft_strcmp(tokens[i], "<") && tokens[i + 1])
		return (i + 2);
	else if (!ft_strcmp(tokens[i], "|"))
		return (i + 1);
	return (-1);
}

static int	set_command_if_first_arg(t_ast *ast, char *tok, char *expanded,
		int quoted)
{
	char	*cmd_tmp;

	cmd_tmp = NULL;
	if (quoted)
		ast->command = ft_strdup(tok);
	else
	{
		cmd_tmp = extract_first_command_word(expanded);
		ast->command = cmd_tmp;
	}
	if (!ast->command)
	{
		free(expanded);
		return (0);
	}
	return (1);
}

static int	process_argument_token(t_ast *ast, char *tok, int arg_i,
		t_context *ctx)
{
	int		quoted;
	int		tmp;
	char	*expanded;

	quoted = 0;
	if ((tok[0] == '"' && tok[ft_strlen(tok) - 1] == '"'))
		quoted = 1;
	else if ((tok[0] == '\'' && tok[ft_strlen(tok) - 1] == '\''))
		quoted = 1;
	tmp = ctx->exit_status;
	expanded = handle_quotes(tok, ctx);
	ctx->exit_status = tmp;
	if (!expanded)
		return (0);
	ast->args[arg_i] = expanded;
	if (arg_i == 0)
	{
		if (!set_command_if_first_arg(ast, tok, expanded, quoted))
			return (0);
	}
	return (1);
}

int	fill_args(t_ast *ast, char **tokens, t_context *ctx)
{
	int		i;
	int		arg_i;
	int		skip_result;
	int		prev_exit_status;

	i = 0;
	arg_i = 0;
	prev_exit_status = ctx->exit_status;
	while (tokens[i])
	{
		skip_result = skip_redirection_tokens(tokens, i);
		if (skip_result != -1)
			i = skip_result;
		else
		{
			ctx->exit_status = prev_exit_status;
			if (!process_argument_token(ast, tokens[i], arg_i, ctx))
				return (0);
			arg_i++;
			i++;
		}
	}
	ast->args[arg_i] = NULL;
	return (1);
}

// Validate syntax and print appropriate error messages
static int	check_pipe_start(char **tokens, t_context *ctx)
{
	if (!ft_strcmp(tokens[0], "|"))
	{
		ft_putstr_fd("bash: syntax error near unexpected token `|'\n",
			STDERR_FILENO);
		ctx->exit_status = 2;
		return (0);
	}
	return (1);
}

static int	validate_syntax(char **tokens, t_context *ctx)
{
	int	i;

	if (!tokens || !tokens[0])
		return (1);
	if (!check_pipe_start(tokens, ctx))
		return (0);
	i = 0;
	while (tokens[i])
	{
		if (!ft_strcmp(tokens[i], "|") && tokens[i + 1]
			&& !ft_strcmp(tokens[i + 1], "|"))
		{
			ft_putstr_fd("bash: syntax error near unexpected token `|'\n",
				STDERR_FILENO);
			ctx->exit_status = 2;
			return (0);
		}
		i++;
	}
	return (1);
}

t_ast	*tokenize_input(const char *input, t_context *ctx)
{
	char	**tokens;
	t_ast	*ast;
	int		ok;

	tokens = smart_split(input);
	if (!tokens)
		return (ctx->exit_status = 2, (t_ast *) NULL);
	if (!validate_syntax(tokens, ctx))
		return (free_split(tokens), (t_ast *) NULL);
	ast = alloc_ast(tokens);
	if (!ast)
		return (free_split(tokens), (t_ast *) NULL);
	fill_redirections(ast, tokens, ctx);
	ok = fill_args(ast, tokens, ctx);
	if (!ok && ast->redirs && ast->redirs->type == REDIR_HEREDOC)
		ok = 1;
	free_split(tokens);
	if (!ok || (!ast->command && !ast->redirs))
		return (free_ast(ast), (t_ast *) NULL);
	return (ast);
}
