#include "tokenization.h"
#include <libft.h>
#include <stdbool.h>
#include <stdlib.h>

static char	**split_loop(const char *input)
{
	t_split_ctx	ctx;
	size_t		i;
	char		c;

	init_ctx(&ctx);
	i = 0;
	while (input[i])
	{
		c = input[i];
		if (!process_ws_op(c, &ctx, input, &i))
			update_state_and_append(c, &ctx);
		i++;
	}
	if (ctx.state != STATE_NORMAL)
	{
		ft_putstr_fd("minishell: error: unmatched quote\n", STDERR_FILENO);
		free(ctx.cur);
		free_split(ctx.tokens);
		return (NULL);
	}
	if (ctx.cur)
		flush_token(&ctx);
	return (ctx.tokens);
}

char	**smart_split(const char *input)
{
	return (split_loop(input));
}
