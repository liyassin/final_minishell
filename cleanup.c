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

char	*join_free(char *s1, char *s2)
{
	char	*result;

	if (!s1 || !s2)
		return (NULL);
	result = ft_strjoin(s1, s2);
	if (!result)
		return (NULL);
	free(s1);
	return (result);
}

//for const strings
char	*join_free_const(char *s1, const char *s2)
{
    char	*result;

    if (!s1 || !s2)
        return (NULL);
    result = ft_strjoin(s1, s2);
    free(s1);
	if (!result)
		return (NULL);
    return (result);
}

void	free_split(char **str)
{
	int	i;

	i = 0;
	if (!str)
		return ;
	while (str[i])
	{
		free(str[i]);
		i++;
	}
	free(str);
}

void	free_ast(t_ast *ast)
{
	int i;

	if (ast == NULL)
		return ;
	if (ast->command)
	{
		free(ast->command);
		ast->command = NULL;
	}
	if (ast->args)
	{
		i = 0;
		while (ast->args[i])
		{
			free(ast->args[i]);
			i++;
		}
		free(ast->args);
		ast->args = NULL;
	}
	if (ast->input_file) // to test with memo leak / I just applied same split logic
	{
		i = 0;
		while (ast->input_file[i])
		{
			free(ast->input_file[i]);
			i++;
		}
		free(ast->input_file);
		ast->input_file = NULL;
	}
	if (ast->output_file) // to test with memo leak / I just applied same split logic
	{
		i = 0;
		while (ast->output_file[i])
		{
			free(ast->output_file[i]);
			i++;
		}
		free(ast->output_file);
		ast->output_file = NULL;
	}
	if (ast->next)
	{
		free(ast->next);
		ast->next = NULL;
	}
	free(ast);
}
