/*
 * rift/src/core/stage-0/tokenizer.c
 * RIFT Stage 0: Tokenization Engine
 * OBINexus Computing Framework - AEGIS Methodology
 * Technical Lead: Nnamdi Michael Okpala
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>

// Include RIFT core headers
#include "rift/core/stage-0/tokenizer.h"
#include "rift/core/common.h"
#include "rift/governance/policy.h"

// AEGIS Compliance Constants
#define RIFT_TOKENIZER_VERSION "1.0.0"
#define RIFT_MAX_TOKEN_LENGTH 256
#define RIFT_MAX_TOKENS 4096
#define RIFT_MEMORY_ALIGNMENT 4096

// Token Type Classification - AEGIS Three-Field Schema
typedef enum {
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_LITERAL_INTEGER,
    TOKEN_LITERAL_FLOAT,
    TOKEN_LITERAL_STRING,
    TOKEN_OPERATOR,
    TOKEN_PUNCTUATION,
    TOKEN_WHITESPACE,
    TOKEN_NEWLINE,
    TOKEN_EOF,
    TOKEN_ERROR,
    TOKEN_UNKNOWN
} rift_token_type_t;

// RIFT Token Structure - Three-Field AEGIS Compliance
typedef struct {
    rift_token_type_t type;           // Token classification
    char value[RIFT_MAX_TOKEN_LENGTH]; // Token lexical content
    size_t matched_state;             // AST minimization preservation field
    size_t line_number;               // Source location tracking
    size_t column_number;             // Column position tracking
    size_t complexity_cost;           // Computational complexity metric
} rift_token_t;

// Tokenizer State Management
typedef struct {
    const char* input;                // Source text input
    size_t position;                  // Current position in input
    size_t length;                    // Total input length
    size_t line;                      // Current line number
    size_t column;                    // Current column number
    rift_token_t* tokens;             // Token array buffer
    size_t token_count;               // Number of tokens generated
    size_t token_capacity;            // Maximum token capacity
    bool aegis_validation_enabled;    // AEGIS governance flag
} rift_tokenizer_state_t;

// AEGIS Keywords Registry
static const char* rift_keywords[] = {
    "let", "const", "var", "fn", "return", "if", "else", "while",
    "for", "break", "continue", "true", "false", "null", "undefined",
    "struct", "enum", "type", "interface", "impl", "mod", "pub",
    "async", "await", "yield", "match", "case", "default"
};

static const size_t rift_keywords_count = sizeof(rift_keywords) / sizeof(rift_keywords[0]);

// Forward Declarations
static rift_token_type_t classify_token(const char* lexeme);
static bool is_keyword(const char* lexeme);
static bool is_operator_char(char c);
static bool is_punctuation_char(char c);
static size_t calculate_complexity_cost(rift_token_type_t type, const char* value);
static int advance_tokenizer(rift_tokenizer_state_t* state);
static char peek_current(const rift_tokenizer_state_t* state);
static char peek_next(const rift_tokenizer_state_t* state);
static int tokenize_operator(rift_tokenizer_state_t* state, rift_token_t* token);
static int tokenize_punctuation(rift_tokenizer_state_t* state, rift_token_t* token);

/*
 * rift_tokenizer_init - Initialize tokenizer with AEGIS compliance
 * @input: Source text to tokenize
 * @state: Tokenizer state structure to initialize
 * Returns: 0 on success, negative error code on failure
 */
int rift_tokenizer_init(const char* input, rift_tokenizer_state_t* state) {
    if (!input || !state) {
        return -RIFT_ERROR_INVALID_ARGUMENT;
    }

    // Initialize state with AEGIS compliance
    state->input = input;
    state->position = 0;
    state->length = strlen(input);
    state->line = 1;
    state->column = 1;
    state->token_count = 0;
    state->token_capacity = RIFT_MAX_TOKENS;
    state->aegis_validation_enabled = true;

    // Allocate token buffer with memory alignment
    state->tokens = aligned_alloc(RIFT_MEMORY_ALIGNMENT, 
                                  sizeof(rift_token_t) * RIFT_MAX_TOKENS);
    if (!state->tokens) {
        return -RIFT_ERROR_MEMORY_ALLOCATION;
    }

    // Zero-initialize token buffer for security
    memset(state->tokens, 0, sizeof(rift_token_t) * RIFT_MAX_TOKENS);

    return RIFT_SUCCESS;
}

/*
 * rift_tokenizer_process - Main tokenization processing function
 * @state: Initialized tokenizer state
 * Returns: Number of tokens generated, or negative error code
 */
int rift_tokenizer_process(rift_tokenizer_state_t* state) {
    if (!state || !state->input || !state->tokens) {
        return -RIFT_ERROR_INVALID_STATE;
    }

    while (state->position < state->length) {
        char current = peek_current(state);
        
        // Skip whitespace with position tracking
        if (isspace(current)) {
            if (current == '\n') {
                state->line++;
                state->column = 1;
            } else {
                state->column++;
            }
            advance_tokenizer(state);
            continue;
        }

        // Check token capacity
        if (state->token_count >= state->token_capacity) {
            return -RIFT_ERROR_TOKEN_BUFFER_OVERFLOW;
        }

        rift_token_t* token = &state->tokens[state->token_count];
        token->line_number = state->line;
        token->column_number = state->column;
        
        // Tokenize based on character classification
        if (isalpha(current) || current == '_') {
            // Identifier or keyword
            if (tokenize_identifier(state, token) != RIFT_SUCCESS) {
                return -RIFT_ERROR_TOKENIZATION_FAILED;
            }
        } else if (isdigit(current)) {
            // Numeric literal
            if (tokenize_number(state, token) != RIFT_SUCCESS) {
                return -RIFT_ERROR_TOKENIZATION_FAILED;
            }
        } else if (current == '"' || current == '\'') {
            // String literal
            if (tokenize_string(state, token) != RIFT_SUCCESS) {
                return -RIFT_ERROR_TOKENIZATION_FAILED;
            }
        } else if (is_operator_char(current)) {
            // Operator
            if (tokenize_operator(state, token) != RIFT_SUCCESS) {
                return -RIFT_ERROR_TOKENIZATION_FAILED;
            }
        } else if (is_punctuation_char(current)) {
            // Punctuation
            if (tokenize_punctuation(state, token) != RIFT_SUCCESS) {
                return -RIFT_ERROR_TOKENIZATION_FAILED;
            }
        } else {
            // Unknown character - error token
            token->type = TOKEN_ERROR;
            token->value[0] = current;
            token->value[1] = '\0';
            token->matched_state = 0;
            token->complexity_cost = 1;
            advance_tokenizer(state);
        }

        // AEGIS governance validation
        if (state->aegis_validation_enabled) {
            if (rift_governance_validate_token(token) != RIFT_SUCCESS) {
                return -RIFT_ERROR_GOVERNANCE_VIOLATION;
            }
        }

        state->token_count++;
    }

    // Add EOF token
    if (state->token_count < state->token_capacity) {
        rift_token_t* eof_token = &state->tokens[state->token_count];
        eof_token->type = TOKEN_EOF;
        eof_token->value[0] = '\0';
        eof_token->matched_state = 0;
        eof_token->line_number = state->line;
        eof_token->column_number = state->column;
        eof_token->complexity_cost = 0;
        state->token_count++;
    }

    return (int)state->token_count;
}

/*
 * tokenize_identifier - Process identifier or keyword tokens
 */
static int tokenize_identifier(rift_tokenizer_state_t* state, rift_token_t* token) {
    size_t start_pos = state->position;
    size_t value_index = 0;

    while (state->position < state->length && value_index < RIFT_MAX_TOKEN_LENGTH - 1) {
        char current = peek_current(state);
        if (isalnum(current) || current == '_') {
            token->value[value_index++] = current;
            advance_tokenizer(state);
        } else {
            break;
        }
    }

    token->value[value_index] = '\0';
    token->type = is_keyword(token->value) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;
    token->matched_state = start_pos; // AST minimization preservation
    token->complexity_cost = calculate_complexity_cost(token->type, token->value);

    return RIFT_SUCCESS;
}

/*
 * tokenize_number - Process numeric literal tokens
 */
static int tokenize_number(rift_tokenizer_state_t* state, rift_token_t* token) {
    size_t start_pos = state->position;
    size_t value_index = 0;
    bool has_decimal = false;

    while (state->position < state->length && value_index < RIFT_MAX_TOKEN_LENGTH - 1) {
        char current = peek_current(state);
        if (isdigit(current)) {
            token->value[value_index++] = current;
            advance_tokenizer(state);
        } else if (current == '.' && !has_decimal) {
            has_decimal = true;
            token->value[value_index++] = current;
            advance_tokenizer(state);
        } else {
            break;
        }
    }

    token->value[value_index] = '\0';
    token->type = has_decimal ? TOKEN_LITERAL_FLOAT : TOKEN_LITERAL_INTEGER;
    token->matched_state = start_pos;
    token->complexity_cost = calculate_complexity_cost(token->type, token->value);

    return RIFT_SUCCESS;
}

/*
 * tokenize_string - Process string literal tokens
 */
static int tokenize_string(rift_tokenizer_state_t* state, rift_token_t* token) {
    size_t start_pos = state->position;
    char quote_char = peek_current(state);
    size_t value_index = 0;
    bool terminated = false;

    advance_tokenizer(state); // Skip opening quote

    while (state->position < state->length && value_index < RIFT_MAX_TOKEN_LENGTH - 1) {
        char current = peek_current(state);
        if (current == quote_char) {
            advance_tokenizer(state); // Skip closing quote
            terminated = true;
            break;
        } else if (current == '\\') {
            // Handle escape sequences
            advance_tokenizer(state);
            if (state->position < state->length) {
                char escaped = peek_current(state);
                switch (escaped) {
                    case 'n': token->value[value_index++] = '\n'; break;
                    case 't': token->value[value_index++] = '\t'; break;
                    case 'r': token->value[value_index++] = '\r'; break;
                    case '\\': token->value[value_index++] = '\\'; break;
                    case '"': token->value[value_index++] = '"'; break;
                    case '\'': token->value[value_index++] = '\''; break;
                    default: token->value[value_index++] = escaped; break;
                }
                advance_tokenizer(state);
            }
        } else {
            token->value[value_index++] = current;
            advance_tokenizer(state);
        }
    }

    token->value[value_index] = '\0';
    token->matched_state = start_pos;

    if (!terminated) {
        token->type = TOKEN_ERROR;
        token->complexity_cost = calculate_complexity_cost(token->type, token->value);
        return -RIFT_ERROR_UNTERMINATED_STRING;
    }

    token->type = TOKEN_LITERAL_STRING;
    token->complexity_cost = calculate_complexity_cost(token->type, token->value);

    return RIFT_SUCCESS;
}

/*
 * tokenize_operator - Process operator tokens
 */
static int tokenize_operator(rift_tokenizer_state_t* state, rift_token_t* token) {
    size_t start_pos = state->position;
    size_t value_index = 0;

    char current = peek_current(state);
    token->value[value_index++] = current;
    advance_tokenizer(state);

    char next = peek_current(state);
    if ((current == '=' && next == '=') ||
        (current == '!' && next == '=') ||
        (current == '<' && (next == '=' || next == '<')) ||
        (current == '>' && (next == '=' || next == '>')) ||
        (current == '+' && (next == '+' || next == '=')) ||
        (current == '-' && (next == '-' || next == '=')) ||
        ((current == '*' || current == '/' || current == '%' ||
          current == '&' || current == '|' || current == '^') && next == '=') ||
        (current == '&' && next == '&') ||
        (current == '|' && next == '|')) {
        token->value[value_index++] = next;
        advance_tokenizer(state);
    }

    token->value[value_index] = '\0';
    token->type = TOKEN_OPERATOR;
    token->matched_state = start_pos;
    token->complexity_cost = calculate_complexity_cost(token->type, token->value);

    return RIFT_SUCCESS;
}

/*
 * tokenize_punctuation - Process punctuation tokens
 */
static int tokenize_punctuation(rift_tokenizer_state_t* state, rift_token_t* token) {
    size_t start_pos = state->position;

    char current = peek_current(state);
    token->value[0] = current;
    token->value[1] = '\0';
    token->type = TOKEN_PUNCTUATION;
    token->matched_state = start_pos;
    token->complexity_cost = calculate_complexity_cost(token->type, token->value);

    advance_tokenizer(state);

    return RIFT_SUCCESS;
}

/*
 * Helper Functions
 */
static bool is_keyword(const char* lexeme) {
    for (size_t i = 0; i < rift_keywords_count; i++) {
        if (strcmp(lexeme, rift_keywords[i]) == 0) {
            return true;
        }
    }
    return false;
}

static bool is_operator_char(char c) {
    return strchr("+-*/=<>!&|^~%", c) != NULL;
}

static bool is_punctuation_char(char c) {
    return strchr("();,{}.[]:", c) != NULL;
}

static size_t calculate_complexity_cost(rift_token_type_t type, const char* value) {
    size_t base_cost = 1;
    size_t length_factor = strlen(value);
    
    switch (type) {
        case TOKEN_KEYWORD: return base_cost + length_factor / 2;
        case TOKEN_IDENTIFIER: return base_cost + length_factor / 3;
        case TOKEN_LITERAL_STRING: return base_cost + length_factor;
        case TOKEN_OPERATOR: return base_cost;
        default: return base_cost;
    }
}

static int advance_tokenizer(rift_tokenizer_state_t* state) {
    if (state->position < state->length) {
        state->position++;
        state->column++;
        return RIFT_SUCCESS;
    }
    return -RIFT_ERROR_END_OF_INPUT;
}

static char peek_current(const rift_tokenizer_state_t* state) {
    if (state->position < state->length) {
        return state->input[state->position];
    }
    return '\0';
}

static char peek_next(const rift_tokenizer_state_t* state) {
    if (state->position + 1 < state->length) {
        return state->input[state->position + 1];
    }
    return '\0';
}

/*
 * rift_tokenizer_cleanup - Resource cleanup with AEGIS compliance
 */
void rift_tokenizer_cleanup(rift_tokenizer_state_t* state) {
    if (state && state->tokens) {
        free(state->tokens);
        state->tokens = NULL;
        state->token_count = 0;
        state->token_capacity = 0;
    }
}

/*
 * rift_tokenizer_get_tokens - Access token array
 */
const rift_token_t* rift_tokenizer_get_tokens(const rift_tokenizer_state_t* state) {
    return state ? state->tokens : NULL;
}

/*
 * rift_tokenizer_get_token_count - Get number of generated tokens
 */
size_t rift_tokenizer_get_token_count(const rift_tokenizer_state_t* state) {
    return state ? state->token_count : 0;
}