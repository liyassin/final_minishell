/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 17:27:55 by anassih           #+#    #+#             */
/*   Updated: 2025/05/02 00:56:15 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "tokenization.h"
#include "env_utils.h"

char *join_free(char *s1, char *s2)
{
    char *result = ft_strjoin(s1, s2);
    if (s1) free(s1);
    return result;
}

char *join_free_const(char *s1, const char *s2)
{
    char *result = ft_strjoin(s1, s2);
    if (s1) free(s1);
    return result;
}

void free_split(char **str)
{
    if (!str) return;
    
    for (int i = 0; str[i]; i++) {
        free(str[i]);
    }
    free(str);
}

// Iterative AST freeing to prevent stack overflow
void free_ast(t_ast *ast)
{
    t_ast *current = ast;
    
    while (current) {
        t_ast *next = current->next;
        
        // Free command string
        if (current->command) {
            free(current->command);
            current->command = NULL;
        }
        
        // Free arguments array
        if (current->args) {
            for (int i = 0; current->args[i]; i++) {
                free(current->args[i]);
            }
            free(current->args);
            current->args = NULL;
        }
        
        // Free redirection list
        t_redir *redir = current->redirs;
        while (redir) {
            t_redir *next_redir = redir->next;
            
            // Free target string
            if (redir->target) {
                free(redir->target);
                redir->target = NULL;
            }
            
            // Close pipe FDs for heredocs
            if (redir->type == REDIR_HEREDOC) {
                if (redir->heredoc_fd[0] != -1) {
                    close(redir->heredoc_fd[0]);
                    redir->heredoc_fd[0] = -1;
                }
                if (redir->heredoc_fd[1] != -1) {
                    close(redir->heredoc_fd[1]);
                    redir->heredoc_fd[1] = -1;
                }
            }
            
            free(redir);
            redir = next_redir;
        }
        current->redirs = NULL;
        
        free(current);
        current = next;
    }
}