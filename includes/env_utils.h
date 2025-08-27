/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 21:35:01 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 07:42:13 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENV_UTILS_H
# define ENV_UTILS_H

# include <libft.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

// Environment Management
char	**copy_environment(char **envp);
void	free_environment(char **env);
int		update_env(char ***env, const char *name, const char *value);
char	*get_env_value(const char *name, char **envp);

// String Utilities
char	*strip_quotes(const char *str);
char	*join_free(char *s1, char *s2);
char	*join_free_const(char *s1, const char *s2);
void	free_split(char **str);

// Variable Expansion
char	*expand_env_vars(const char *input, char **envp, int exit_status);
int		update_env(char ***env, const char *key, const char *value);
char	*make_new_entry(const char *key, const char *value);
int		try_replace_env(char ***env, const char *key, char *new_entry);
int		add_new_env(char ***env, char *new_entry);
size_t	extract_var_name(const char *input, char *buf, size_t buf_size);
size_t	get_exit_len(int code);
size_t	get_dollar_seq_len(const char *in, char **envp, int exit_status,
			size_t *skip);
size_t	calc_len(const char *in, char **envp, int exit_status);

typedef struct s_expand_ctx
{
	char	**envp;
	int		exit_status;
}	t_expand_ctx;

size_t	copy_dollar_seq(const char *in, t_expand_ctx *ctx, char *out,
			size_t *skip);
size_t	handle_exit_status(t_expand_ctx *ctx, char *out, size_t *skip);
size_t	handle_var_expansion(const char *in, t_expand_ctx *ctx,
			char *out, size_t *skip);
void	build_str(const char *in, char **envp, int exit_status, char *out);

#endif
