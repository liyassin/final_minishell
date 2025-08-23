#include "builtins.h"
#include "minishell.h"

// Validate identifier for export/unset: [A-Za-z_][A-Za-z0-9_]*.
int	is_valid_identifier(const char *name)
{
	const char	*p;

	if (!name || !*name)
		return (0);
	if (!ft_isalpha((unsigned char)name[0]) && name[0] != '_')
		return (0);
	p = name + 1;
	while (*p)
	{
		if (!ft_isalnum((unsigned char)*p) && *p != '_')
			return (0);
		p++;
	}
	return (1);
}
