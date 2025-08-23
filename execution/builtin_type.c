#include "minishell.h"
#include "signals.h"

// Returns builtin type
t_builtin	get_builtin_type(const char *cmd)
{
	if (!cmd)
		return (NOT_BUILTIN);
	if (!ft_strcmp(cmd, "cd") || !ft_strcmp(cmd, "export")
		|| !ft_strcmp(cmd, "unset") || !ft_strcmp(cmd, "exit"))
		return (PARENT_BUILTIN);
	if (!ft_strcmp(cmd, "echo") || !ft_strcmp(cmd, "pwd")
		|| !ft_strcmp(cmd, "env"))
		return (CHILD_BUILTIN);
	return (NOT_BUILTIN);
}
