/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 16:41:57 by anassih           #+#    #+#             */
/*   Updated: 2025/08/26 11:45:16 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../includes/minishell.h"
#include "../includes/signals.h"
#include <signal.h>

volatile sig_atomic_t g_signal = 0;
#include <readline/readline.h>
#include <unistd.h>
#include <signal.h>

void	handle_sigint(int sig)
{
g_signal = SIGINT;
(void)sig;
write(STDOUT_FILENO, "\n", 1);
// Do not call rl_replace_line, rl_on_new_line, or rl_redisplay here; let main loop handle prompt
}

void	setup_shell_signals(void)
{
	struct sigaction	sa;
	sa.sa_handler = handle_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
}

void	setup_heredoc_signals(void)
{
	struct sigaction	sa;
	sa.sa_handler = handle_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
}

void	reset_default_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}
