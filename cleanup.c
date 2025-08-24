/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 17:27:55 by anassih           #+#    #+#             */
/*   Updated: 2025/08/23 21:47:54 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/tokenization.h"
#include "../includes/env_utils.h"

// Concatenate s1 + s2, free s1, return the new string.
char	*join_free(char *s1, char *s2)
{
	char	*result;

	result = ft_strjoin(s1, s2);
	if (s1)
		free(s1);
	return (result);
}

// Concatenate s1 + s2, free s1, return the new string.
// s2 is const so we don’t free it.
char	*join_free_const(char *s1, const char *s2)
{
	char	*result;

	result = ft_strjoin(s1, s2);
	if (s1)
		free(s1);
	return (result);
}

// Free a NULL-terminated array of strings and the array itself.
void	free_split(char **str)
{
	int	i;

	if (!str)
		return ;
	i = 0;
	while (str[i])
		free(str[i++]);
	free(str);
}

// Iteratively free an AST list and all redirection structs.
void	free_ast(t_ast *ast)
{
	t_ast	*current;
	t_ast	*next;
	t_redir	*redir;
	t_redir	*next_redir;
	int		i;

	current = ast;
	while (current)
	{
		next = current->next;
		if (current->command)
			free(current->command);
		if (current->args)
		{
			i = 0;
			while (current->args[i])
				free(current->args[i++]);
			free(current->args);
		}
		redir = current->redirs;
		while (redir)
		{
			next_redir = redir->next;
			if (redir->target)
				free(redir->target);
			if (redir->type == REDIR_HEREDOC)
			{
				if (redir->heredoc_fd[0] != -1)
					close(redir->heredoc_fd[0]);
				if (redir->heredoc_fd[1] != -1)
					close(redir->heredoc_fd[1]);
			}
			free(redir);
			redir = next_redir;
		}
		free(current);
		current = next;
	}
}
void cleanup_shell(t_ast *ast, t_context *ctx)
{
    if (ast)
        free_ast(ast);
    if (ctx && ctx->env)
        free_environment(ctx->env);
    clear_history();
}