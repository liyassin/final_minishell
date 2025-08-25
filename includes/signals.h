#ifndef SIGNALS_H
# define SIGNALS_H

# include <signal.h>

// removed g_signal declaration

void	setup_shell_signals(void);
void	setup_heredoc_signals(void);
void	reset_default_signals(void);

#endif
