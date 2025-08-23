#include "minishell.h"
#include "signals.h"

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
			ctx->exit_status = 1;
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