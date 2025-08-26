/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_builder.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:14:19 by anassih           #+#    #+#             */
/*   Updated: 2025/08/26 04:30:14 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"

static t_ast	*build_pipeline_list(char **segments, t_context *ctx)
{
	t_ast	*head;
	t_ast	*tail;
	t_ast	*node;
	int		i;

	head = NULL;
	tail = NULL;
	i = 0;
	while (segments[i])
	{
		node = tokenize_input(segments[i], ctx);
		if (!node)
		{
			free_ast(head);
			ctx->exit_status = 2; // Syntax error exit code
			return (NULL);
		}
		if (!head)
		{
			head = node;
			tail = node;
		}
		else
		{
			tail->next = node;
			tail = node;
		}
		i++;
	}
	return (head);
}

t_ast	*build_ast_pipeline(char *input, t_context *ctx)
{
	char	**segments;
	t_ast	*head;

	segments = split_on_pipe(input);
	free(input);
	if (!segments)
		return (NULL);
	head = build_pipeline_list(segments, ctx);
	free_split(segments);
	return (head);
}