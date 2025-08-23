#include "env_utils.h"
#include <libft.h>

size_t	extract_var_name(const char *input, char *buf, size_t buf_size)
{
	size_t	len;

	len = 0;
	if (input[0] == '?')
	{
		buf[0] = '?';
		buf[1] = '\0';
		return (1);
	}
	if (!ft_isalpha(input[0]) && input[0] != '_')
	{
		buf[0] = '\0';
		return (0);
	}
	while (len < buf_size - 1
		&& (ft_isalnum(input[len]) || input[len] == '_'))
	{
		buf[len] = input[len];
		len++;
	}
	buf[len] = '\0';
	return (len);
}

size_t	get_exit_len(int code)
{
	char	*s;
	size_t	len;

	s = ft_itoa(code);
	if (!s)
		return (0);
	len = ft_strlen(s);
	free(s);
	return (len);
}

size_t	get_dollar_seq_len(const char *in, char **envp, int exit_status, size_t *skip)
{
	size_t	vlen;
	char	name[128];
	char	*value;

	if (in[1] == '?')
	{
		*skip = 2;
		return (get_exit_len(exit_status));
	}
	if (ft_isalpha(in[1]) || in[1] == '_')
	{
		vlen = extract_var_name(in + 1, name, sizeof(name));
		value = get_env_value(name, envp);
		*skip = 1 + vlen;
		if (value)
			return (ft_strlen(value));
		return (0);
	}
	*skip = 1;
	return (1);
}

size_t	calc_len(const char *in, char **envp, int exit_status)
{
	size_t	i;
	size_t	outlen;
	size_t	skip;

	i = 0;
	outlen = 0;
	while (in[i])
	{
		if (in[i] == '$')
		{
			outlen += get_dollar_seq_len(in + i, envp, exit_status, &skip);
			i += skip;
		}
		else
		{
			outlen++;
			i++;
		}
	}
	return (outlen);
}
