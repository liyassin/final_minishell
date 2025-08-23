#include "builtins.h"
#include "minishell.h"

// exit builtin: print exit, validate args, set ctx->exit_status,
// return 255 to signal shell should terminate.
int	builtin_exit(char **args, t_context *ctx)
{
	long	code;
	int		idx;
	int		i;

	if (!ctx)
		return (1);
	code = (long)ctx->exit_status;
	ft_putendl_fd("exit", STDOUT_FILENO);
	if (args[1])
	{
		if (args[2])
		{
			ft_putstr_fd("exit: too many arguments\n", STDERR_FILENO);
			ctx->exit_status = 1;
			return (1);
		}
		idx = 0;
		if (args[1][0] == '+' || args[1][0] == '-')
			idx++;
		i = idx;
		while (args[1][i])
		{
			if (!ft_isdigit((unsigned char)args[1][i]))
			{
				ft_putstr_fd("exit: ", STDERR_FILENO);
				ft_putstr_fd(args[1], STDERR_FILENO);
				ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
				ctx->exit_status = EXIT_BUILTIN_SIGNAL;
				return (EXIT_BUILTIN_SIGNAL);
			}
			i++;
		}
		code = ft_atoi(args[1]);
	}
	ctx->exit_status = (int)code;
	return (255);
}
