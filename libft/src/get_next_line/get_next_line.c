/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 17:50:55 by anassih           #+#    #+#             */
/*   Updated: 2025/01/31 17:53:22 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/get_next_line.h"

static char	*append_buffer(char *basin_buffer, char *buffer)
{
	char	*tmp;

	if (!basin_buffer)
	{
		tmp = ft_substr(buffer, 0, ft_strlen(buffer));
		return (tmp);
	}
	tmp = ft_strjoin(basin_buffer, buffer);
	free(basin_buffer);
	return (tmp);
}

static char	*read_file(char *basin_buffer, int fd)
{
	ssize_t	lineread;
	char	*buffer;

	buffer = ft_calloc(BUFFER_SIZE + 1, sizeof(char));
	if (!buffer)
		return (NULL);
	lineread = 1;
	while (lineread > 0)
	{
		lineread = read(fd, buffer, BUFFER_SIZE);
		if (lineread == -1)
		{
			free(buffer);
			free(basin_buffer);
			return (NULL);
		}
		buffer[lineread] = '\0';
		basin_buffer = append_buffer(basin_buffer, buffer);
		if (!basin_buffer)
			return (free(buffer), NULL);
		if (ft_strchr(basin_buffer, '\n'))
			break ;
	}
	free(buffer);
	return (basin_buffer);
}

static char	*extract_line(char *basin_buffer)
{
	char	*line;
	size_t	len;

	if (!basin_buffer || !*basin_buffer)
		return (NULL);
	len = 0;
	while (basin_buffer[len] && basin_buffer[len] != '\n')
		len++;
	if (basin_buffer[len] == '\n')
		len++;
	line = ft_substr(basin_buffer, 0, len);
	return (line);
}

static char	*extract_remaining(char *basin_buffer)
{
	char	*remaining;
	char	*newline;

	newline = ft_strchr(basin_buffer, '\n');
	if (!newline)
		return (free(basin_buffer), NULL);
	remaining = ft_substr(newline + 1, 0, ft_strlen(newline + 1));
	free(basin_buffer);
	return (remaining);
}

char	*get_next_line(int fd)
{
	static char	*basin_buffer;
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0)
	{
		if (basin_buffer)
			free(basin_buffer);
		basin_buffer = NULL;
		return (NULL);
	}
	if (!basin_buffer)
		basin_buffer = ft_calloc(1, sizeof(char));
	if (!basin_buffer)
		return (NULL);
	basin_buffer = read_file(basin_buffer, fd);
	if (!basin_buffer)
		return (NULL);
	line = extract_line(basin_buffer);
	if (!line)
		return (free(basin_buffer), basin_buffer = NULL, NULL);
	basin_buffer = extract_remaining(basin_buffer);
	return (line);
}
