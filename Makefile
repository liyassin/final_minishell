# ────────────────────────────────────────────────────────────────────────────
# MINISHELL Makefile
# ────────────────────────────────────────────────────────────────────────────

NAME        := minishell

CC          := cc
CFLAGS      := -Wall -Wextra -Werror -g #-fsanitize=address

# Libft
LIBFT_DIR   := libft
LIBFT_INC   := $(LIBFT_DIR)/inc
LIBFT_LIB   := $(LIBFT_DIR)/libft.a

INCLUDES    := -Iincludes -I$(LIBFT_INC)
LDLIBS      := -lreadline -lcurses $(LIBFT_LIB)

# Sources grouped by directory
PARSING_SRCS := \
	parsing/ast_builder.c \
	parsing/line_reader.c \
	parsing/smart_split.c \
	parsing/split_pipe.c \
	parsing/split_pipe_utils.c \
	parsing/ast_utils.c \
	parsing/ast_utils_helpers.c \
	parsing/ast_utils_syntax.c \
	parsing/ast_utils_processing.c \
	parsing/quotes.c \
	parsing/split_context.c \
	parsing/split_context_utils.c \
	parsing/substitution.c \
	parsing/substitution_utils.c \
	parsing/heredoc.c \
	parsing/heredoc_utils.c \
	parsing/heredoc_utils2.c \
	parsing/redir_utils.c \
	parsing/split_helpers.c

EXEC_SRCS := \
	execution/builtin_type.c \
	execution/builtin_dispatch.c \
	execution/command_exec.c \
	execution/command_exec_utils.c \
	execution/command_exec_process.c \
	execution/command_errors.c \
	execution/executor.c \
	execution/executor_utils.c \
	execution/main.c \
	execution/main_utils.c \
	execution/path_resolver.c \
	execution/pipeline_setup.c \
	execution/pipeline_executor.c \
	execution/pipeline_executor_utils.c \
	execution/redirections.c

BUILTIN_SRCS := \
	builtins/cd.c \
	builtins/exit.c \
	builtins/child_builtins.c \
	builtins/export.c \
	builtins/export_utils.c \
	builtins/unset.c \
	builtins/utils.c

ENV_SRCS := \
	env_utils/env_array.c \
	env_utils/env_expand_build.c \
	env_utils/env_expand_len.c \
	env_utils/env_utils.c

ROOT_SRCS := \
	signals.c \
	cleanup.c

# All sources and objects
SRCS := $(PARSING_SRCS) $(EXEC_SRCS) $(BUILTIN_SRCS) $(ENV_SRCS) $(ROOT_SRCS)
OBJDIR := obj
OBJS := $(addprefix $(OBJDIR)/, $(notdir $(SRCS:.c=.o)))


.PHONY: all clean fclean re libft

all: libft $(NAME)

libft:
	$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(OBJS) $(LIBFT_LIB)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) $(LDLIBS) -o $@



# Compile rules for sources in subdirectories
$(OBJDIR)/%.o: parsing/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
$(OBJDIR)/%.o: execution/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
$(OBJDIR)/%.o: builtins/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
$(OBJDIR)/%.o: env_utils/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR)
	$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	rm -f $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all
