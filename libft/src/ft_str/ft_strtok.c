/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtok.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 18:20:29 by anassih           #+#    #+#             */
/*   Updated: 2025/04/29 18:21:46 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/libft.h"

char    *ft_strtok(char *str, const char *delim)
{
	static char *backup;
	char        *token;

	if (str != NULL)
		backup = str;
	else if (backup == NULL)
		return (NULL);
	while (*backup && strchr(delim, *backup))
		backup++;
	if (*backup == '\0')
		return (NULL);
	token = backup;
	while (*backup && !strchr(delim, *backup))
		backup++;
	if (*backup)
	{
		*backup = '\0';
		backup++;
	}
	else
		backup = NULL;
	return (token);
}