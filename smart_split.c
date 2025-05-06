/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   smart_split.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 15:16:55 by anassih           #+#    #+#             */
/*   Updated: 2025/05/02 00:58:51 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tokenization.h"

char    **smart_split(const char *input)
{
    char **result = malloc(sizeof(char *) * 1024); // adjust size if needed
    int i = 0, j = 0, k = 0;
    char quote = 0;
    char buffer[1024]; // temp buffer for current word

    if (!result)
        return (NULL);
    while (input[i])
    {
        if ((input[i] == '"' || input[i] == '\'') && quote == 0)
            quote = input[i]; // enter quote
        else if (input[i] == quote)
            quote = 0; // exit quote
        if (input[i] == ' ' && quote == 0)
        {
            if (k > 0)
            {
                buffer[k] = '\0';
                result[j++] = ft_strdup(buffer);
                k = 0;
            }
        }
        else
            buffer[k++] = input[i];
        i++;
    }
    if (k > 0)
    {
        buffer[k] = '\0';
        result[j++] = ft_strdup(buffer);
    }
    result[j] = NULL;
    return (result);
}