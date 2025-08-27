/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_expand_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 07:42:09 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 07:42:13 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/env_utils.h"

size_t	handle_exit_status(t_expand_ctx *ctx, char *out, size_t *skip)
{
	char	*s;
	size_t	written;

	s = ft_itoa(ctx->exit_status);
	if (!s)
		return (0);
	written = ft_strlen(s);
	ft_strlcpy(out, s, written + 1);
	free(s);
	*skip = 2;
	return (written);
}

size_t	handle_var_expansion(const char *in, t_expand_ctx *ctx,
	char *out, size_t *skip)
{
	size_t	vlen;
	char	name[128];
	char	*value;
	size_t	written;

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
