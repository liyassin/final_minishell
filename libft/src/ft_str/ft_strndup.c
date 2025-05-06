/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strndup.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 17:17:17 by anassih           #+#    #+#             */
/*   Updated: 2025/05/01 18:32:05 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/libft.h"

char	*ft_strndup(const char *s, size_t n)
{
    char	*dup;
	
	dup = malloc(n + 1);
    if (dup)
	{
        ft_memcpy(dup, s, n);
        dup[n] = '\0';
    }
    return (dup);
}
