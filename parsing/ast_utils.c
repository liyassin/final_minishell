/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:14:09 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 05:09:10 by anassih          ###   ########.fr       */
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

void	fill_redirections(t_ast *ast, char **tokens, t_context *ctx)
{
	t_redir	**tail;
	int		i;

	tail = &ast->redirs;
	i = 0;
	while (tokens[i])
		i = process_redirection_token(&tail, tokens, i, ctx);
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
