#include "minishell.h"
#include "signals.h"

char	*read_input(int *should_exit, t_context *ctx)
{
	char	*input;
	char	*trimmed;

	g_signal = 0;
	input = readline("$ [minishell] > ");
	if (!input)
	{
		ft_putendl_fd("exit", STDOUT_FILENO);
		*should_exit = 1;
		return (NULL);
	}
	trimmed = ft_strtrim(input, " ");
	if (!trimmed || !*trimmed)
	{
		free(input);
		free(trimmed);
		ctx->exit_status = 0;
		return (NULL);
	}
	free(trimmed);
	if (*input)
		add_history(input);
	return (input);
}
