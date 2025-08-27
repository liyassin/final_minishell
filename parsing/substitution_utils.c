/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   substitution_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 12:00:00 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 06:54:03 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	setup_substitution_pipe(int pipefd[2])
{
	if (pipe(pipefd) < 0)
	{
		perror("minishell: pipe");
		return (-1);
	}
	return (0);
}

char	*read_subcmd_output(int pipefd[2])
{
	char	buf[4096];
	ssize_t	n;

	close(pipefd[1]);
	n = read(pipefd[0], buf, sizeof(buf) - 1);
	close(pipefd[0]);
	if (n > 0)
		buf[n] = '\0';
	else
		buf[0] = '\0';
	return (ft_strdup(buf));
}

char	*assemble_substitution(const char *input, const char *buf,
		char *open, char *end)
{
	size_t	prefix_len;
	char	*prefix;
	char	*trimmed;
	char	*result;

	prefix_len = open - input - 2;
	prefix = ft_substr(input, 0, prefix_len);
	trimmed = ft_strtrim(buf, "\n");
	result = ft_strjoin(prefix, trimmed);
	free(prefix);
	free(trimmed);
	prefix = ft_strdup(end + 1);
	trimmed = ft_strjoin(result, prefix);
	free(result);
	free(prefix);
	return (trimmed);
}
