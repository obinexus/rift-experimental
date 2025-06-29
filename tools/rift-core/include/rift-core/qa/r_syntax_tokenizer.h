/*
 * RIFT R-Syntax Regular Expression Tokenization Framework
 * Supports R"" (bottom-up) and R'' (top-down) matching patterns
 */

#ifndef RIFT_R_SYNTAX_TOKENIZER_H
#define RIFT_R_SYNTAX_TOKENIZER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// R-Syntax Token Types
typedef enum {
    R_REGEX_BOTTOM_UP,          // R"pattern" - bottom-up matching
    R_REGEX_TOP_DOWN,           // R'pattern' - top-down matching
    R_REGEX_INVALID
} r_regex_type_t;

// Token Flags
typedef enum {
    BOTTOM_MATCHING = 0x01,
    TOP_DOWN_MATCHING = 0x02,
    SCOPE_DEFAULT = 0x04,
    SCOPE_USER_OUTPUT = 0x08
} token_flags_t;

// Scope Channels
typedef struct {
    bool default_channel;
    bool user_output_channel;
    uint32_t scope_region_id;
} scope_channels_t;

// Token Memory Model
typedef struct {
    void* type_data;
    void* value_data;
    size_t memory_size;
    scope_channels_t scope;
} token_memory_model_t;

// R-Syntax Regular Expression Token
typedef struct {
    r_regex_type_t type;
    uint32_t flags;
    scope_channels_t scope;
    token_memory_model_t memory;
    char* pattern;
    size_t pattern_length;
} rift_r_regex_token_t;

// Tokenization functions
rift_r_regex_token_t* r_syntax_tokenize(const char* input, size_t input_length);
bool r_syntax_validate_token(const rift_r_regex_token_t* token);
void r_syntax_token_destroy(rift_r_regex_token_t* token);

// Pattern matching functions
bool r_syntax_match_bottom_up(const char* pattern, const char* input);
bool r_syntax_match_top_down(const char* pattern, const char* input);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_R_SYNTAX_TOKENIZER_H */
