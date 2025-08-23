#include "minishell.h"
#include "signals.h"

void	execute_if_needed(t_ast *head, t_context *ctx, int *should_exit)
{
	t_builtin	type;
	int			ret;

	if (!head || !head->command)
		return ;
	type = get_builtin_type(head->command);
	if (type == PARENT_BUILTIN && !head->next)
	{
		ret = handle_builtin(head->args, ctx);
		if (ret == 255)
			*should_exit = 1;
	}
	else
	{
		if (head->next)
			execute_pipeline(head, ctx);
		else
			exec_command(head, ctx);
	}
}
