// ================================
// include/rift1/core/lexer/lexer.h
// ================================
#ifndef RIFT1_LEXER_H
#define RIFT1_LEXER_H

#include "rift1/core/rift_core.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RIFT_TOKEN_UNKNOWN = 0,
    RIFT_TOKEN_IDENTIFIER,
    RIFT_TOKEN_KEYWORD,
    RIFT_TOKEN_OPERATOR,
    RIFT_TOKEN_LITERAL,
    RIFT_TOKEN_DELIMITER,
    RIFT_TOKEN_WHITESPACE,
    RIFT_TOKEN_COMMENT,
    RIFT_TOKEN_EOF
} RiftTokenType;

typedef struct {
    char *buffer;
    size_t position;
    size_t length;
    size_t line;
    size_t column;
} RiftLexerState;

// Lexer core functions
RiftResult rift_lexer_init(RiftLexerState *lexer, const char *input);
RiftResult rift_lexer_cleanup(RiftLexerState *lexer);
RiftTokenType rift_lexer_next_token(RiftLexerState *lexer);

#ifdef __cplusplus
}
#endif

#endif // RIFT1_LEXER_H

// ================================
// include/rift1/core/token_types/token_type.h
// ================================
#ifndef RIFT1_TOKEN_TYPE_H
#define RIFT1_TOKEN_TYPE_H

#include "rift1/core/lexer/lexer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    RiftTokenType type;
    char *literal;
    size_t position;
    size_t length;
    size_t line;
    size_t column;
} RiftToken;

typedef struct {
    RiftToken *tokens;
    size_t count;
    size_t capacity;
} RiftTokenArray;

// Token type management
RiftResult rift_token_create(RiftToken *token, RiftTokenType type, const char *literal);
RiftResult rift_token_destroy(RiftToken *token);
const char* rift_token_type_string(RiftTokenType type);

// Token array management
RiftResult rift_token_array_init(RiftTokenArray *array, size_t initial_capacity);
RiftResult rift_token_array_push(RiftTokenArray *array, const RiftToken *token);
RiftResult rift_token_array_cleanup(RiftTokenArray *array);

#ifdef __cplusplus
}
#endif

#endif // RIFT1_TOKEN_TYPE_H

// ================================
// include/rift1/core/token_values/token_value.h
// ================================
#ifndef RIFT1_TOKEN_VALUE_H
#define RIFT1_TOKEN_VALUE_H

#include "rift1/core/token_types/token_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RIFT_VALUE_STRING,
    RIFT_VALUE_INTEGER,
    RIFT_VALUE_FLOAT,
    RIFT_VALUE_BOOLEAN,
    RIFT_VALUE_NULL
} RiftValueType;

typedef union {
    char *string_value;
    long integer_value;
    double float_value;
    bool boolean_value;
} RiftValueData;

typedef struct {
    RiftValueType type;
    RiftValueData data;
    bool is_valid;
} RiftTokenValue;

// Token value operations
RiftResult rift_token_value_create(RiftTokenValue *value, RiftValueType type);
RiftResult rift_token_value_destroy(RiftTokenValue *value);
RiftResult rift_token_value_set_string(RiftTokenValue *value, const char *str);
RiftResult rift_token_value_set_integer(RiftTokenValue *value, long num);
RiftResult rift_token_value_set_float(RiftTokenValue *value, double num);
RiftResult rift_token_value_set_boolean(RiftTokenValue *value, bool val);

// Value extraction
RiftResult rift_token_extract_value(const RiftToken *token, RiftTokenValue *value);
const char* rift_value_type_string(RiftValueType type);

#ifdef __cplusplus
}
#endif

#endif // RIFT1_TOKEN_VALUE_H

// ================================
// include/rift1/core/token_memory/token_memory.h
// ================================
#ifndef RIFT1_TOKEN_MEMORY_H
#define RIFT1_TOKEN_MEMORY_H

#include "rift1/core/token_values/token_value.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    RiftToken *token_pool;
    RiftTokenValue *value_pool;
    bool *free_tokens;
    bool *free_values;
    size_t token_capacity;
    size_t value_capacity;
    size_t token_count;
    size_t value_count;
} RiftTokenMemoryPool;

typedef struct {
    RiftTokenMemoryPool pool;
    size_t peak_token_usage;
    size_t peak_value_usage;
    size_t allocation_count;
    size_t deallocation_count;
} RiftTokenMemoryManager;

// Memory management functions
RiftResult rift_token_memory_init(RiftTokenMemoryManager *mgr, size_t token_capacity, size_t value_capacity);
RiftResult rift_token_memory_cleanup(RiftTokenMemoryManager *mgr);

// Token allocation/deallocation
RiftToken* rift_token_memory_alloc_token(RiftTokenMemoryManager *mgr);
RiftResult rift_token_memory_free_token(RiftTokenMemoryManager *mgr, RiftToken *token);

// Value allocation/deallocation
RiftTokenValue* rift_token_memory_alloc_value(RiftTokenMemoryManager *mgr);
RiftResult rift_token_memory_free_value(RiftTokenMemoryManager *mgr, RiftTokenValue *value);

// Memory statistics
size_t rift_token_memory_tokens_used(const RiftTokenMemoryManager *mgr);
size_t rift_token_memory_values_used(const RiftTokenMemoryManager *mgr);
double rift_token_memory_utilization(const RiftTokenMemoryManager *mgr);

#ifdef __cplusplus
}
#endif

#endif // RIFT1_TOKEN_MEMORY_H

// ================================
// include/rift1/core/tokenizer/tokenizer.h
// ================================
#ifndef RIFT1_TOKENIZER_H
#define RIFT1_TOKENIZER_H

#include "rift1/core/token_memory/token_memory.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    RiftLexerState lexer;
    RiftTokenMemoryManager memory;
    RiftTokenArray tokens;
    const RiftAdvancedConfig *config;  // ← CRITICAL: This fixes your compilation error
    bool is_initialized;
} RiftTokenizer;

// Primary tokenizer interface
RiftResult rift_tokenizer_init(RiftTokenizer *tokenizer, const RiftAdvancedConfig *config);
RiftResult rift_tokenizer_cleanup(RiftTokenizer *tokenizer);

// Tokenization operations
RiftResult rift_tokenizer_process_input(RiftTokenizer *tokenizer, const char *input);
RiftResult rift_tokenizer_get_tokens(const RiftTokenizer *tokenizer, RiftTokenArray **tokens);

// Governance-aware tokenization (resolves your error)
static inline bool rift_tokenizer_governance_enabled(const RiftTokenizer *tokenizer) {
    return tokenizer && tokenizer->config && rift_governance_enabled(tokenizer->config);
}

// Token processing with governance validation
RiftResult rift_tokenizer_process_with_governance(RiftTokenizer *tokenizer, const char *input);

// Utility functions
RiftResult rift_tokenizer_reset(RiftTokenizer *tokenizer);
size_t rift_tokenizer_token_count(const RiftTokenizer *tokenizer);

#ifdef __cplusplus
}
#endif

#endif // RIFT1_TOKENIZER_H