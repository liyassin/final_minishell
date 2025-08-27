/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_utils_processing.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 05:15:00 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 05:38:25 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/tokenization.h"

char	*extract_first_command_word(char *str)
{
	char	*trimmed;
	char	*first_word;
	int		j;
	int		len;

	trimmed = ft_strtrim(str, " \t");
	if (!trimmed)
		return (NULL);
	j = 0;
	while (trimmed[j] && !ft_isspace((unsigned char)trimmed[j]))
		j++;
	len = j;
	first_word = ft_substr(trimmed, 0, len);
	free(trimmed);
	return (first_word);
}

int	set_command_if_first_arg(t_ast *ast, char *tok, char *expanded,
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

int	process_argument_token(t_ast *ast, char *tok, int arg_i, t_context *ctx)
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

int	validate_syntax(char **tokens, t_context *ctx)
{
	int	i;

	if (!tokens || !tokens[0])
		return (1);
	if (!check_pipe_start(tokens, ctx))
		return (0);
	i = 0;
	while (tokens[i])
	{
		if (is_triple_pipe_error(tokens, i, ctx))
			return (0);
		if (is_invalid_redir_token(tokens, i, ctx))
			return (0);
		i++;
	}
	return (1);
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
