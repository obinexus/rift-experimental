// include/rift1/token_types/token_type.h - RIFT1 Token Type Engine
// OBINexus AEGIS Framework - Pattern-based token classification
#ifndef RIFT1_TOKEN_TYPE_H
#define RIFT1_TOKEN_TYPE_H

#include "../lexer/lexer.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// AEGIS Token Type Enumeration
typedef enum {
  RIFT_TOKEN_UNKNOWN = 0,

  // Literals
  RIFT_TOKEN_INTEGER,
  RIFT_TOKEN_FLOAT,
  RIFT_TOKEN_STRING,
  RIFT_TOKEN_CHAR,
  RIFT_TOKEN_BOOLEAN,

  // Identifiers
  RIFT_TOKEN_IDENTIFIER,
  RIFT_TOKEN_KEYWORD,

  // Operators
  RIFT_TOKEN_OPERATOR_ASSIGN,
  RIFT_TOKEN_OPERATOR_PLUS,
  RIFT_TOKEN_OPERATOR_MINUS,
  RIFT_TOKEN_OPERATOR_MULTIPLY,
  RIFT_TOKEN_OPERATOR_DIVIDE,
  RIFT_TOKEN_OPERATOR_MODULO,

  // Comparison operators
  RIFT_TOKEN_OPERATOR_EQUAL,
  RIFT_TOKEN_OPERATOR_NOT_EQUAL,
  RIFT_TOKEN_OPERATOR_LESS,
  RIFT_TOKEN_OPERATOR_GREATER,
  RIFT_TOKEN_OPERATOR_LESS_EQUAL,
  RIFT_TOKEN_OPERATOR_GREATER_EQUAL,

  // Logical operators
  RIFT_TOKEN_OPERATOR_AND,
  RIFT_TOKEN_OPERATOR_OR,
  RIFT_TOKEN_OPERATOR_NOT,

  // Delimiters
  RIFT_TOKEN_DELIMITER_SEMICOLON,
  RIFT_TOKEN_DELIMITER_COMMA,
  RIFT_TOKEN_DELIMITER_DOT,
  RIFT_TOKEN_DELIMITER_COLON,

  // Brackets
  RIFT_TOKEN_BRACKET_OPEN_PAREN,
  RIFT_TOKEN_BRACKET_CLOSE_PAREN,
  RIFT_TOKEN_BRACKET_OPEN_BRACE,
  RIFT_TOKEN_BRACKET_CLOSE_BRACE,
  RIFT_TOKEN_BRACKET_OPEN_SQUARE,
  RIFT_TOKEN_BRACKET_CLOSE_SQUARE,

  // Comments
  RIFT_TOKEN_COMMENT_LINE,
  RIFT_TOKEN_COMMENT_BLOCK,

  // Special tokens
  RIFT_TOKEN_EOF,
  RIFT_TOKEN_NEWLINE,
  RIFT_TOKEN_WHITESPACE,

  // RIFT-specific tokens
  RIFT_TOKEN_RIFT_DIRECTIVE,
  RIFT_TOKEN_QUANTUM_OPERATOR,
  RIFT_TOKEN_CLASSICAL_MODE,
  RIFT_TOKEN_QUANTUM_MODE,

  RIFT_TOKEN_TYPE_COUNT
} RiftTokenType;

// Token pattern definition
typedef struct {
  RiftTokenType type;
  const char *pattern;
  const char *description;
  bool is_regex;
  int priority; // Higher priority patterns matched first
} RiftTokenPattern;

// Token type matcher state
typedef struct {
  RiftTokenPattern *patterns;
  size_t pattern_count;
  size_t pattern_capacity;
  bool case_sensitive;
  bool quantum_mode_enabled;
} RiftTokenTypeMatcher;

// Core token type operations
RiftTokenTypeMatcher *rift_token_type_matcher_create(void);
void rift_token_type_matcher_destroy(RiftTokenTypeMatcher *matcher);

// Pattern management
bool rift_token_type_add_pattern(RiftTokenTypeMatcher *matcher,
                                 const RiftTokenPattern *pattern);
bool rift_token_type_remove_pattern(RiftTokenTypeMatcher *matcher,
                                    RiftTokenType type);

// Token classification
RiftTokenType rift_token_type_match(const RiftTokenTypeMatcher *matcher,
                                    const RiftLexeme *lexeme);
bool rift_token_type_is_valid(RiftTokenType type);

// Pattern utilities
const char *rift_token_type_name(RiftTokenType type);
const char *rift_token_type_description(RiftTokenType type);

// Regex pattern matching (simplified interface)
bool rift_pattern_match(const char *pattern, const char *text,
                        size_t text_length);

// RIFT-specific pattern recognition
bool rift_token_type_is_quantum_operator(const RiftLexeme *lexeme);
bool rift_token_type_is_classical_construct(const RiftLexeme *lexeme);

#ifdef __cplusplus
}
#endif

#endif // RIFT1_TOKEN_TYPE_H
