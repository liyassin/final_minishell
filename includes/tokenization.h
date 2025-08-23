#ifndef TOKENIZATION_H
#define TOKENIZATION_H

#include <ctype.h>
#include <libft.h>
#include <readline/readline.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// # include "minishell.h"
#include "env_utils.h"

typedef struct s_context t_context; // Add forward declaration

// Redirection types
typedef enum e_redir_type {
    REDIR_NONE = 0,
    REDIR_IN,     // <
    REDIR_OUT,    // >
    REDIR_APPEND, // >>
    REDIR_HEREDOC // <<
} t_redir_type;

// Unified redirection structure
typedef struct s_redir {
    t_redir_type type;
    char *target;         // File or delimiter
    int heredoc_fd[2];    // Pipe FDs for heredoc
    int quoted;           // 1 if target was quoted
    struct s_redir *next; // Next redirection
} t_redir;

// Simplified AST structure
typedef struct s_ast {
    char *command;      // Command name (e.g., "echo")
    char **args;        // Argument array
    t_redir *redirs;    // Linked list of redirections
    struct s_ast *next; // Next command in pipeline
} t_ast;

// for smart_split

typedef enum e_split_state {
    STATE_NORMAL,
    STATE_IN_SQUOTE,
    STATE_IN_DQUOTE
} t_split_state;

typedef struct s_split_ctx {
    char *cur;
    size_t cur_len;
    size_t cur_cap;
    char **tokens;
    size_t count;
    size_t cap;
    t_split_state state;
} t_split_ctx;

// Functions for tokenization and token handling
t_ast *tokenize_input(const char *input, t_context *ctx);
char *process_double_quotes(const char *token, t_context *ctx);
char *handle_quotes(char *token, t_context *ctx);
char **smart_split(const char *input);
char *handle_command_substitution(char *input, t_context *ctx);

/* Forward declaration of the AST freeing function
 * (actual implementation is in cleanup) */
void free_ast(t_ast *ast);

// split functions
void init_ctx(t_split_ctx *ctx);
void flush_token(t_split_ctx *ctx);
int process_ws_op(char c, t_split_ctx *ctx, const char *input, size_t *i);
void update_state_and_append(char c, t_split_ctx *ctx);
void *ft_realloc(void *ptr, size_t old_size, size_t new_size);
char *append_char(char *str, char c, size_t *len, size_t *cap);
void add_token(char ***tokens, size_t *count, size_t *cap, char *token);
void handle_operator(const char *input, size_t *i, char ***tokens,
                     size_t *count, size_t *cap);
char *trim_segment(const char *line, size_t start, size_t end);

// AST functions
t_ast *alloc_ast(char **tokens);
void fill_redirections(t_ast *ast, char **tokens, t_context *ctx);
int fill_args(t_ast *ast, char **tokens, t_context *ctx);
t_ast *tokenize_input(const char *input, t_context *ctx);
t_redir *create_redir_node(t_redir_type type, char *raw_target, t_context *ctx);
void process_heredocs(t_ast *head, t_context *ctx);

#endif