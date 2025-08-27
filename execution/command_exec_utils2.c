/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_exec_utils2.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 07:38:08 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 07:42:13 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	is_arg_empty_str(char **args)
{
	size_t	i;

	if (!args || !args[0])
		return (1);
	i = 0;
	while (args[0][i] && ft_isspace((unsigned char)args[0][i]))
		i++;
	if (args[0][i] != '\0')
		return (0);
	return (1);
}

int	is_numeric_arg(char *arg)
{
	int	i;

	if (arg[0] == '+' || arg[0] == '-')
		i = 1;
	else
		i = 0;
	if (!arg[i])
		return (0);
	while (arg[i])
	{
		if (!ft_isdigit(arg[i]))
			return (0);
		i++;
	}
	return (1);
}
