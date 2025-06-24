#ifndef RIFT_CORE_H
#define RIFT_CORE_H

/**
 * RIFT1 Core Header - AEGIS Framework
 * OBINexus Computing - TokenMemory → Parser Pipeline
 *
 * Structure IS the syntax - AEGIS methodology compliance
 * Maintains token type/value separation and matched_state preservation
 */

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations (resolved in implementation)
typedef struct RiftToken RiftToken;
typedef struct RiftPattern RiftPattern;
typedef struct RiftASTNode RiftASTNode;
typedef struct RiftParseContext RiftParseContext;
typedef struct RiftTokenMemory RiftTokenMemory;

// Enums with complete definitions (fixes forward reference issues)
typedef enum {
  RIFT_SUCCESS = 0,
  RIFT_ERROR = 1,
  RIFT_ERROR_INVALID_INPUT = 2,
  RIFT_ERROR_MEMORY = 3,
  RIFT_ERROR_PARSE = 4,
  RIFT_ERROR_IO = 5,
  RIFT_ERROR_NOT_FOUND = 6
} RiftResult;

typedef enum {
  RIFT_TOKEN_UNKNOWN = 0,
  RIFT_TOKEN_IDENTIFIER = 1,
  RIFT_TOKEN_KEYWORD = 2,
  RIFT_TOKEN_OPERATOR = 3,
  RIFT_TOKEN_LITERAL = 4,
  RIFT_TOKEN_DELIMITER = 5,
  RIFT_TOKEN_PATTERN = 6,
  RIFT_TOKEN_EOF = 7
} RiftTokenType;

typedef enum {
  RIFT_AST_UNKNOWN = 0,
  RIFT_AST_ROOT = 1,
  RIFT_AST_EXPRESSION = 2,
  RIFT_AST_STATEMENT = 3,
  RIFT_AST_DECLARATION = 4,
  RIFT_AST_PATTERN = 5,
  RIFT_AST_BLOCK = 6
} RiftASTNodeType;

// Complete structure definitions (fixes sizeof() issues)
struct RiftToken {
  RiftTokenType type; // AEGIS: Type/value separation enforced
  char *value;        // AEGIS: Separate value field
  size_t line;
  size_t column;
  size_t matched_state; // AEGIS: Preserved for AST minimization
  struct RiftToken *next;
};

struct RiftASTNode {
  RiftASTNodeType type; // Complete type definition
  char *value;          // Node value
  struct RiftASTNode *parent;
  struct RiftASTNode *children;
  struct RiftASTNode *next;
  size_t child_count;
  size_t line;
  size_t column;
};

struct RiftPattern {
  char *pattern;
  char *flags;
  char *mode;
  bool compiled;
  void *regex_context;
};

struct RiftTokenMemory {
  RiftToken *tokens;
  size_t count;
  size_t capacity;
  pthread_mutex_t mutex; // Thread-safe access
};

struct RiftParseContext {
  RiftTokenMemory *token_memory;
  RiftASTNode *root;
  size_t current_token;
  RiftResult last_error;
  char *error_message;
};

// Core API Functions
RiftResult rift_init(void);
void rift_cleanup(void);

// Token Management (AEGIS: Type/value separation)
RiftToken *rift_token_create(RiftTokenType type, const char *value);
void rift_token_destroy(RiftToken *token);
RiftResult rift_token_set_matched_state(RiftToken *token, size_t state);

// TokenMemory Bridge (Stage 0 → Stage 1)
RiftTokenMemory *rift_tokenmemory_create(void);
void rift_tokenmemory_destroy(RiftTokenMemory *tm);
RiftResult rift_tokenmemory_add_token(RiftTokenMemory *tm, RiftToken *token);
RiftToken *rift_tokenmemory_get_token(RiftTokenMemory *tm, size_t index);

// AST Management (Complete definitions)
RiftASTNode *rift_ast_node_create(RiftASTNodeType type, const char *value);
void rift_ast_node_destroy(RiftASTNode *node);
RiftResult rift_ast_add_child(RiftASTNode *parent, RiftASTNode *child);

// Parsing Engine (Stage 1 Core)
RiftParseContext *rift_parse_context_create(RiftTokenMemory *tm);
void rift_parse_context_destroy(RiftParseContext *ctx);
RiftResult rift_parse_tokens(RiftParseContext *ctx);

// Pattern Management
RiftPattern *rift_pattern_create(const char *pattern, const char *flags);
void rift_pattern_destroy(RiftPattern *pattern);
RiftResult rift_pattern_compile(RiftPattern *pattern);

// I/O Functions
RiftResult rift_read_rift0_file(const char *filename, RiftTokenMemory **tm);
RiftResult rift_write_rift1_file(const char *filename, RiftParseContext *ctx);

// Configuration Management
typedef struct {
  char *output_dir;
  bool debug_mode;
  bool verbose;
  char *log_level;
  int stage;
} RiftConfig;

RiftConfig *rift_config_create(void);
void rift_config_destroy(RiftConfig *config);
RiftResult rift_config_load(RiftConfig *config, const char *filename);

// Error Handling
const char *rift_result_to_string(RiftResult result);
void rift_set_error(RiftParseContext *ctx, RiftResult error,
                    const char *message);

// AEGIS Architecture Validation
RiftResult rift_validate_architecture(void);
bool rift_check_type_value_separation(RiftToken *token);
bool rift_check_matched_state_preservation(RiftToken *token);

#endif // RIFT_CORE_H
