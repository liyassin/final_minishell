/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 17:49:55 by anassih           #+#    #+#             */
/*   Updated: 2025/01/31 17:49:56 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/libft.h"

void	*ft_calloc(size_t nbe, size_t size)
{
	char	*zero;
	size_t	i;

	i = 0;
	if (nbe && (nbe * size) / nbe != size)
		return (NULL);
	zero = (char *)malloc(nbe * size);
	if (!zero)
		return (NULL);
	while (i < size * nbe)
		zero[i++] = 0;
	return (zero);
}
