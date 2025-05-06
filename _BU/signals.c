/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 00:15:00 by anassih           #+#    #+#             */
/*   Updated: 2025/04/29 00:33:52 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Global variable to store the original terminal attributes
struct termios original_termios;

// Signal handler for SIGINT (Ctrl+C)
void sigint_handler(int sig)
{
    (void)sig;
    
    // Print a newline to create a new prompt
    write(STDOUT_FILENO, "\n", 1);
    
    // Clear the current line and display a new prompt
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}

// Save the original terminal attributes
void save_terminal_settings(void)
{
    tcgetattr(STDIN_FILENO, &original_termios);
}

// Restore the original terminal attributes
void restore_terminal_settings(void)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}

// Setup signal handlers
void setup_signals(void)
{
    // Save the original terminal attributes
    save_terminal_settings();
    
    // Install signal handlers
    signal(SIGINT, sigint_handler);  // Ctrl+C
    signal(SIGQUIT, SIG_IGN);        // Ctrl+\
}

// Restore default signal handlers for child processes
void reset_signals_for_child(void)
{
    signal(SIGINT, SIG_DFL);  // Default Ctrl+C behavior
    signal(SIGQUIT, SIG_DFL); // Default Ctrl+\ behavior
}