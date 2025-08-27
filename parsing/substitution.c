/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   substitution.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:12:09 by anassih           #+#    #+#             */
/*   Updated: 2025/08/27 06:54:03 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../includes/tokenization.h"
#include "../includes/env_utils.h"

// Find the next "$(" ... ")" range. Returns 1 if found, 0 otherwise.
static int	find_substitution_range(const char *input, char **open, char **end)
{
	char	*start;

	start = ft_strnstr(input, "$(", ft_strlen(input));
	if (!start)
		return (0);
	*open = start + 2;
	*end = ft_strchr(*open, ')');
	if (!*end)
		return (0);
	return (1);
}

// Extract the command between open ("$(") and end (")").
static char	*extract_subcmd(const char *open, const char *end)
{
	size_t	len;

	len = end - open;
	return (ft_substr(open, 0, len));
}

// In child: run the subcommand and write stdout to pipefd[1].
static void	exec_subcmd(const char *subcmd, t_context *ctx, int pipefd[2])
{
	t_ast	*ast;

	close(pipefd[0]);
	dup2(pipefd[1], STDOUT_FILENO);
	close(pipefd[1]);
	ast = tokenize_input((char *)subcmd, ctx);
	if (ast && ast->command)
		exec_command(ast, ctx);
	free_ast(ast);
	exit(0);
}

// Run the subcommand in a forked child, read its stdout, return it.
static char	*run_subcmd_and_collect(const char *subcmd, t_context *ctx)
{
	int		pipefd[2];
	pid_t	pid;
	char	*result;

	if (setup_substitution_pipe(pipefd) < 0)
		return (ft_strdup(""));
	pid = fork();
	if (pid < 0)
	{
		perror("minishell: fork");
		close(pipefd[0]);
		close(pipefd[1]);
		return (ft_strdup(""));
	}
	if (pid == 0)
		exec_subcmd(subcmd, ctx, pipefd);
	result = read_subcmd_output(pipefd);
	waitpid(pid, NULL, 0);
	return (result);
}

// Public API: replace first "$(…)" in input with its command output.
char	*handle_command_substitution(char *input, t_context *ctx)
{
	char	*open;
	char	*end;
	char	*subcmd;
	char	*output;
	char	*result;

	if (!find_substitution_range(input, &open, &end))
		return (ft_strdup(input));
	subcmd = extract_subcmd(open, end);
	if (!subcmd)
		return (ft_strdup(input));
	output = run_subcmd_and_collect(subcmd, ctx);
	free(subcmd);
	result = assemble_substitution(input, output, open, end);
	free(output);
	return (result);
}
