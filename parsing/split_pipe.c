#include "minishell.h"
#include "signals.h"

char	*trim_segment(const char *line, size_t start, size_t end)
{
	size_t	a;
	size_t	seglen;

	a = start;
	seglen = end - start;
	while (seglen && ft_isspace((unsigned char)line[a]))
	{
		++a;
		--seglen;
	}
	while (seglen && ft_isspace((unsigned char)line[a + seglen - 1]))
		--seglen;
	return (ft_substr(line, a, seglen));
}

char	**split_on_pipe(const char *line)
{
	size_t	i;
	size_t	start;
	char	quote;
	char	**parts;
	size_t	count;
	size_t	cap;
	size_t	len;

	len = ft_strlen(line);
	cap = 8;
	count = 0;
	i = 0;
	start = 0;
	quote = 0;
	parts = malloc(sizeof(*parts) * cap);
	if (!parts)
		return (NULL);
	while (i <= len)
	{
		if ((line[i] == '"' || line[i] == '\'') && !quote)
			quote = line[i];
		else if (line[i] == quote)
			quote = 0;
		if (!quote && (line[i] == '|' || line[i] == '\0'))
		{
			parts[count++] = trim_segment(line, start, i);
			if (count + 1 >= cap)
			{
				char **new_parts;
				new_parts = realloc(parts, sizeof(*parts) * cap * 2);
				if (!new_parts)
				{
					free_split(parts);
					return (NULL);
				}
				parts = new_parts;
				cap *= 2;
			}
			start = i + 1;
		}
		++i;
	}
	parts[count] = NULL;
	return (parts);
}
