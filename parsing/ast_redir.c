/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_redir.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 01:00:00 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 00:37:48 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/tokenization.h"

static void	add_redir_to_list(t_redir ***tail, t_redir_type type, char *target,
	t_context *ctx)
{
	t_redir	*new_redir;

	new_redir = malloc(sizeof(t_redir));
	if (!new_redir)
		return ;
	new_redir->type = type;
	new_redir->target = target;
	new_redir->next = NULL;
	**tail = new_redir;
	*tail = &new_redir->next;
	(void)ctx;
}

static int	process_redirection_token(t_redir ***tail, char **tokens, int i,
	t_context *ctx)
{
	char			*target;
	t_redir_type	type;

	if (!ft_strcmp(tokens[i], ">"))
		type = REDIR_OUT;
	else if (!ft_strcmp(tokens[i], ">>"))
		type = REDIR_APPEND;
	else if (!ft_strcmp(tokens[i], "<"))
		type = REDIR_IN;
	else if (!ft_strcmp(tokens[i], "<<"))
		type = REDIR_HEREDOC;
	else
		return (i);
	if (!tokens[i + 1])
		return (i);
	target = ft_strdup(tokens[i + 1]);
	if (!target)
		return (i);
	add_redir_to_list(tail, type, target, ctx);
	return (i + 1);
}

void	fill_redirections(t_ast *ast, char **tokens, t_context *ctx)
{
	t_redir	**tail;
	int		i;

	tail = &ast->redirections;
	i = 0;
	while (tokens[i])
	{
		i = process_redirection_token(&tail, tokens, i, ctx);
		i++;
	}
}

static int	skip_redirection_tokens(char **tokens, int i)
{
	if (!ft_strcmp(tokens[i], ">") || !ft_strcmp(tokens[i], ">>")
		|| !ft_strcmp(tokens[i], "<") || !ft_strcmp(tokens[i], "<<"))
	{
		if (tokens[i + 1])
			return (i + 2);
		return (i + 1);
	}
	return (i + 1);
}
