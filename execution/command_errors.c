#include "minishell.h"
#include "signals.h"

void	handle_command_not_found(char *cmd)
{
	struct stat	st;

	if (!cmd || !*cmd)
	{
		ft_putstr_fd("minishell: : command not found\n", STDERR_FILENO);
		exit(127);
	}
	if (ft_strchr(cmd, '/'))
	{
		if (stat(cmd, &st) == 0 && S_ISDIR(st.st_mode))
		{
			ft_putstr_fd("minishell: ", STDERR_FILENO);
			ft_putstr_fd(cmd, STDERR_FILENO);
			ft_putstr_fd(": is a directory\n", STDERR_FILENO);
			exit(126);
		}
		if (access(cmd, F_OK) == 0)
		{
			ft_putstr_fd("minishell: ", STDERR_FILENO);
			ft_putstr_fd(cmd, STDERR_FILENO);
			ft_putstr_fd(": Permission denied\n", STDERR_FILENO);
			exit(126);
		}
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(cmd, STDERR_FILENO);
		ft_putstr_fd(": No such file or directory\n", STDERR_FILENO);
		exit(127);
	}
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd(cmd, STDERR_FILENO);
	ft_putstr_fd(": command not found\n", STDERR_FILENO);
	exit(127);
}
