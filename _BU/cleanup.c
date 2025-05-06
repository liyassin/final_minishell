/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 17:27:55 by anassih           #+#    #+#             */
/*   Updated: 2025/04/28 23:53:37 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*join_free(char *s1, char *s2)
{
	char	*result;

	if (!s1 || !s2)
		return (NULL);
	result = ft_strjoin(s1, s2);
	free(s1);
	return (result);
}

void	free_split(char **str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		free(str[i]);
		i++;
	}
	free(str);
}

void free_ast(t_ast *ast)
{
	if (ast == NULL)
		return;

	// Free command and arguments
	if (ast->command)
		free(ast->command);
	if (ast->args)
	{
		for (int i = 0; ast->args[i]; i++)
			free(ast->args[i]);
		free(ast->args);
	}
	// Free input and output files (if they exist)
	if (ast->input_file)
		free(ast->input_file);
	if (ast->output_file)
		free(ast->output_file);
	// Recursively free the next node if it exists
	if (ast->next)
		free_ast(ast->next);
	// Finally, free the AST node itself
	free(ast);
}
