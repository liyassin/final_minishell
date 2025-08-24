/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:12:53 by anassih           #+#    #+#             */
/*   Updated: 2025/08/23 18:12:57 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/tokenization.h"
#include "../includes/env_utils.h"

// Helper function to remove all quotes from a string
char	*remove_all_quotes(const char *str)
{
	size_t	len;
	size_t	i;
	size_t	j;
	char	*result;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	result = malloc(len + 1);
	if (!result)
		return (NULL);
	i = 0;
	j = 0;
	while (str[i])
	{
		if (str[i] != '"' && str[i] != '\'')
			result[j++] = str[i];
		i++;
	}
	result[j] = '\0';
	return (result);
}

// Helper to create redirection nodes
t_redir	*create_redir_node(t_redir_type type, char *raw_target,
		t_context *ctx)
{
	t_redir	*new;
	char	*stripped;
	char	*expanded;
	int		quoted;
	size_t	raw_len;

	new = ft_calloc(1, sizeof(t_redir));
	if (!new)
		return (NULL);
	new->type = type;
	new->next = NULL;
	raw_len = ft_strlen(raw_target);
	quoted = (raw_target[0] == '"' && raw_target[raw_len - 1] == '"')
		|| (raw_target[0] == '\'' && raw_target[raw_len - 1] == '\'');
	stripped = remove_all_quotes(raw_target);
	if (!stripped)
		return (free(new), (t_redir *) NULL);
	if (raw_target[0] != '\'')
	{
		expanded = expand_env_vars(stripped, ctx->env, ctx->exit_status);
		free(stripped);
		new->target = expanded;
	}
	else
		new->target = stripped;
	if (type == REDIR_HEREDOC)
	{
		new->quoted = quoted;
		if (pipe(new->heredoc_fd) < 0)
			return (perror("minishell: pipe"), free(new->target), free(new),
				(t_redir *) NULL);
	}
	return (new);
}
