/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_errors.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:17:15 by anassih           #+#    #+#             */
/*   Updated: 2025/08/24 18:11:07 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/signals.h"

static void	print_and_exit(char *cmd, char *msg, int code, int should_free)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	if (cmd)
		ft_putstr_fd(cmd, STDERR_FILENO);
	ft_putstr_fd(msg, STDERR_FILENO);
	if (should_free)
		free(cmd);
	exit(code);
}

// Handles errors for commands with '/' in their name
static void	handle_slash_error(char *cmd)
{
	struct stat	st;

	if (stat(cmd, &st) == 0)
	{
		if (S_ISDIR(st.st_mode))
			print_and_exit(cmd, ": Is a directory\n", 126, 0);
		if (access(cmd, F_OK) == 0)
			print_and_exit(cmd, ": Permission denied\n", 126, 0);
	}
	print_and_exit(cmd, ": No such file or directory\n", 127, 0);
}

void	handle_command_not_found(char *cmd)
{
	if (!cmd || !*cmd)
		print_and_exit(NULL, ": command not found\n", 127, 0);
	if (ft_strchr(cmd, '/'))
		handle_slash_error(cmd);
	print_and_exit(cmd, ": command not found\n", 127, 1);
}
