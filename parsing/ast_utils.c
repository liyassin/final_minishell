#include "env_utils.h"
#include "minishell.h"
#include "tokenization.h"

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
	int	i;
	int	arg_i;

	i = 0;
	arg_i = 0;
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
			ast->args[arg_i] = handle_quotes(tokens[i], ctx);
			if (!ast->args[arg_i])
				return (0);
			if (arg_i == 0)
				ast->command = ft_strdup(ast->args[0]);
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
		return (NULL);
	ast = alloc_ast(tokens);
	if (!ast)
		return (free_split(tokens), (t_ast *) NULL);
	fill_redirections(ast, tokens, ctx);
	ok = fill_args(ast, tokens, ctx);
	free_split(tokens);
	if (!ok || !ast->command)
		return (free_ast(ast), (t_ast *) NULL);
	return (ast);
}
