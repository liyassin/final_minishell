/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_builder.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:14:19 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 00:37:48 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"

static void	append_to_pipeline(t_ast **head, t_ast **tail, t_ast *node)
{
	if (!*head)
	{
		*head = node;
		*tail = node;
	}
	else
	{
		(*tail)->next = node;
		*tail = node;
	}
}

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
			ctx->exit_status = 2;
			return (NULL);
		}
		append_to_pipeline(&head, &tail, node);
		i++;
	}
	return (head);
}

static int	validate_pipeline_start(char *trimmed, t_context *ctx, char *input)
{
	if (trimmed[0] == '|')
	{
		ft_putstr_fd("bash: syntax error near unexpected token `|'\n",
			STDERR_FILENO);
		ctx->exit_status = 2;
		free(trimmed);
		free(input);
		return (0);
	}
	return (1);
}

t_ast	*build_ast_pipeline(char *input, t_context *ctx)
{
	char	**segments;
	t_ast	*head;
	char	*trimmed;

	trimmed = ft_strtrim(input, " \t");
	if (!trimmed)
	{
		free(input);
		return (NULL);
	}
	if (!validate_pipeline_start(trimmed, ctx, input))
		return (NULL);
	free(trimmed);
	segments = split_on_pipe(input);
	free(input);
	if (!segments)
		return (NULL);
	head = build_pipeline_list(segments, ctx);
	free_split(segments);
	return (head);
}
