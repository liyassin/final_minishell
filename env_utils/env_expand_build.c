/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_expand_build.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:19:33 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 00:37:48 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/env_utils.h"
#include <libft.h>

size_t	copy_dollar_seq(const char *in, t_expand_ctx *ctx, char *out,
	size_t *skip)
{
	size_t	vlen;
	char	name[128];
	char	*value;
	char	*s;
	size_t	written;

	if (in[1] == '?')
	{
		s = ft_itoa(ctx->exit_status);
		if (!s)
			return (0);
		written = ft_strlen(s);
		ft_strlcpy(out, s, written + 1);
		free(s);
		*skip = 2;
		return (written);
	}
	if (ft_isalpha(in[1]) || in[1] == '_')
	{
		vlen = extract_var_name(in + 1, name, sizeof(name));
		value = get_env_value(name, ctx->envp);
		*skip = 1 + vlen;
		if (value)
		{
			written = ft_strlen(value);
			ft_strlcpy(out, value, written + 1);
			return (written);
		}
		return (0);
	}
	out[0] = '$';
	*skip = 1;
	return (1);
}

// Second pass: build the expanded string.
void	build_str(const char *in, char **envp, int exit_status, char *out)
{
	size_t			i;
	size_t			pos;
	size_t			skip;
	size_t			written;
	t_expand_ctx	ctx;

	ctx.envp = envp;
	ctx.exit_status = exit_status;
	i = 0;
	pos = 0;
	while (in[i])
	{
		if (in[i] == '$')
		{
			written = copy_dollar_seq(in + i, &ctx, out + pos, &skip);
			pos += written;
			i += skip;
		}
		else
		{
			out[pos++] = in[i++];
		}
	}
	out[pos] = '\0';
}

char	*expand_env_vars(const char *input, char **envp, int exit_status)
{
	char	*out;
	size_t	total;

	if (!input)
		return (NULL);
	total = calc_len(input, envp, exit_status);
	out = ft_calloc(total + 1, 1);
	if (!out)
		return (NULL);
	build_str(input, envp, exit_status, out);
	return (out);
}
