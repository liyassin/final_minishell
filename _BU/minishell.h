/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 09:53:01 by anassih           #+#    #+#             */
/*   Updated: 2025/04/28 23:59:30 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

#include <stdio.h> // printf, perror
#include <stdlib.h> // malloc, free, exit
#include <string.h> // write, fork, execve, chdir
#include <unistd.h> // waitpid
#include <sys/wait.h> // readline
#include <sys/types.h> // add_history
#include <sys/stat.h> // pid_t
#include <fcntl.h> // signal
#include <signal.h> // errno
#include <errno.h> // open, close
#include <dirent.h> // opendir, readdir, closedir
#include <termios.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "libft/inc/libft.h"
#include "tokenization.h"

// Built-in commands
int		handle_builtin(char **tokens, char ***env_ptr);

// Memory management
char	*join_free(char *s1, char *s2);
void	free_split(char **str);
void	free_ast(t_ast *ast);

// Command execution
char	*find_command_path(char *cmd, char **envp);
void	exec_command(t_ast *ast, char **envp);
void	handle_redirection(t_ast *node);

// Pipeline execution
int     count_pipeline_commands(t_ast *ast);
void    execute_single_command(t_ast *ast, char **envp);
void    execute_pipeline(t_ast *ast, char **envp);

// Signal handling
void    setup_signals(void);
void    reset_signals_for_child(void);

// Environment variables
char    **copy_env(char **envp);
char    *get_env_value(char **env, const char *name);
void    set_env_var(char ***env, const char *name, const char *value);
void    unset_env_var(char ***env, const char *name);
void    print_env(char **env);
void    free_env(char **env);

#endif