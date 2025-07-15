# ────────────────────────────────────────────────────────────────────────────
# ROOT Makefile for MINISHELL
# ────────────────────────────────────────────────────────────────────────────

# Name of the final executable
NAME        = minishell

# Compiler and flags
CC          = cc
CFLAGS      = -Wall -Wextra -Werror -g -fsanitize=address

# Directories
LIBFT_DIR   = libft
LIBFT_INC   = $(LIBFT_DIR)/inc
LIBFT_LIB   = $(LIBFT_DIR)/libft.a

# Minishell sources (all .c in root of MINISHELL/)
SRCS        = \
	builtins.c \
	cleanup.c \
	cmd_exec.c \
	env_utils.c \
	minishell.c \
	smart_split.c \
	token_utils.c \
	signals.c

# Corresponding object files
OBJS        = $(SRCS:.c=.o)

# Include paths for Minishell and libft headers
INCLUDES    = -I. -I$(LIBFT_INC)

# Program-wide linker flags (link against readline and libft)
LDLIBS      =  -lreadline -lcurses $(LIBFT_LIB) #-ledit for linux

.PHONY: all clean fclean re

# ────────────────────────────────────────────────────────────────────────────
#  Default target: build libft first, then minishell
# ────────────────────────────────────────────────────────────────────────────
all: $(LIBFT_LIB) $(NAME)

# ────────────────────────────────────────────────────────────────────────────
#  Link rule for minishell
# ────────────────────────────────────────────────────────────────────────────
$(NAME): $(OBJS) $(LIBFT_LIB)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) $(LDLIBS) -o $(NAME)

# ────────────────────────────────────────────────────────────────────────────
#  Compile each .c → .o (Minishell sources)
# ────────────────────────────────────────────────────────────────────────────
#  Note: if you add more .c files under MINISHELL/, just update SRCS.
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# ────────────────────────────────────────────────────────────────────────────
#  Build the libft library by delegating to its own Makefile
# ────────────────────────────────────────────────────────────────────────────
$(LIBFT_LIB):
	@$(MAKE) -C $(LIBFT_DIR)

# ────────────────────────────────────────────────────────────────────────────
#  Remove object files and invoke libft's clean
# ────────────────────────────────────────────────────────────────────────────
clean:
	@$(RM) $(OBJS)
	@$(MAKE) -C $(LIBFT_DIR) clean

# ────────────────────────────────────────────────────────────────────────────
#  Full cleanup: remove minishell binary and libft artifacts
# ────────────────────────────────────────────────────────────────────────────
fclean: clean
	@$(RM) $(NAME)
	@$(MAKE) -C $(LIBFT_DIR) fclean

# ────────────────────────────────────────────────────────────────────────────
#  Rebuild everything from scratch
# ────────────────────────────────────────────────────────────────────────────
re: fclean all

.PHONY: all bonus clean fclean re
