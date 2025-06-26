/*
 * rift/include/rift/core/stage-0/tokenizer.h
 * RIFT Stage 0: Tokenization Engine Header
 * OBINexus Computing Framework - AEGIS Methodology
 * Technical Lead: Nnamdi Michael Okpala
 */

#ifndef RIFT_CORE_STAGE_0_TOKENIZER_H
#define RIFT_CORE_STAGE_0_TOKENIZER_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// AEGIS Compliance Constants
#define RIFT_TOKENIZER_VERSION_MAJOR 1
#define RIFT_TOKENIZER_VERSION_MINOR 0
#define RIFT_TOKENIZER_VERSION_PATCH 0

#define RIFT_MAX_TOKEN_LENGTH 256
#define RIFT_MAX_TOKENS 4096
#define RIFT_MEMORY_ALIGNMENT 4096

// RIFT Error Codes
#define RIFT_SUCCESS 0
#define RIFT_ERROR_INVALID_ARGUMENT -1
#define RIFT_ERROR_MEMORY_ALLOCATION -2
#define RIFT_ERROR_INVALID_STATE -3
#define RIFT_ERROR_TOKEN_BUFFER_OVERFLOW -4
#define RIFT_ERROR_TOKENIZATION_FAILED -5
#define RIFT_ERROR_GOVERNANCE_VIOLATION -6
#define RIFT_ERROR_END_OF_INPUT -7

// Forward Declarations
typedef struct rift_tokenizer_state rift_tokenizer_state_t;
typedef struct rift_token rift_token_t;

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

// RIFT Token Structure - AEGIS Compliant Three-Field Design
typedef struct rift_token {
    rift_token_type_t type;           // Token classification
    char value[RIFT_MAX_TOKEN_LENGTH]; // Token lexical content
    size_t matched_state;             // AST minimization preservation field
    size_t line_number;               // Source location tracking
    size_t column_number;             // Column position tracking
    size_t complexity_cost;           // Computational complexity metric
} rift_token_t;

// Tokenizer State Management Structure
typedef struct rift_tokenizer_state {
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

/*
 * Core Tokenizer API Functions
 */

/**
 * rift_tokenizer_init - Initialize tokenizer with AEGIS compliance
 * @input: Source text to tokenize (must be null-terminated)
 * @state: Tokenizer state structure to initialize
 * 
 * Initializes the tokenizer state for processing input text according to
 * AEGIS methodology requirements. Allocates memory-aligned token buffer
 * and configures governance validation settings.
 * 
 * Returns: RIFT_SUCCESS on success, negative error code on failure
 */
int rift_tokenizer_init(const char* input, rift_tokenizer_state_t* state);

/**
 * rift_tokenizer_process - Main tokenization processing function
 * @state: Initialized tokenizer state
 * 
 * Processes the input text and generates tokens according to the RIFT
 * language specification. Maintains AEGIS compliance through systematic
 * validation and preserves matched_state for AST minimization.
 * 
 * Returns: Number of tokens generated, or negative error code on failure
 */
int rift_tokenizer_process(rift_tokenizer_state_t* state);

/**
 * rift_tokenizer_cleanup - Resource cleanup with AEGIS compliance
 * @state: Tokenizer state to cleanup
 * 
 * Safely deallocates all resources associated with the tokenizer state.
 * Ensures no memory leaks and resets state to safe initial condition.
 */
void rift_tokenizer_cleanup(rift_tokenizer_state_t* state);

/**
 * rift_tokenizer_get_tokens - Access token array
 * @state: Tokenizer state containing tokens
 * 
 * Returns: Pointer to token array, or NULL if state is invalid
 */
const rift_token_t* rift_tokenizer_get_tokens(const rift_tokenizer_state_t* state);

/**
 * rift_tokenizer_get_token_count - Get number of generated tokens
 * @state: Tokenizer state
 * 
 * Returns: Number of tokens in the token array
 */
size_t rift_tokenizer_get_token_count(const rift_tokenizer_state_t* state);

/*
 * Specialized Tokenization Functions
 */

/**
 * tokenize_identifier - Process identifier or keyword tokens
 * @state: Current tokenizer state
 * @token: Token structure to populate
 * 
 * Returns: RIFT_SUCCESS on success, error code on failure
 */
int tokenize_identifier(rift_tokenizer_state_t* state, rift_token_t* token);

/**
 * tokenize_number - Process numeric literal tokens
 * @state: Current tokenizer state
 * @token: Token structure to populate
 * 
 * Handles both integer and floating-point literals with proper
 * type classification and complexity cost calculation.
 * 
 * Returns: RIFT_SUCCESS on success, error code on failure
 */
int tokenize_number(rift_tokenizer_state_t* state, rift_token_t* token);

/**
 * tokenize_string - Process string literal tokens
 * @state: Current tokenizer state
 * @token: Token structure to populate
 * 
 * Handles string literals with escape sequence processing and
 * proper boundary detection.
 * 
 * Returns: RIFT_SUCCESS on success, error code on failure
 */
int tokenize_string(rift_tokenizer_state_t* state, rift_token_t* token);

/**
 * tokenize_operator - Process operator tokens
 * @state: Current tokenizer state
 * @token: Token structure to populate
 * 
 * Returns: RIFT_SUCCESS on success, error code on failure
 */
int tokenize_operator(rift_tokenizer_state_t* state, rift_token_t* token);

/**
 * tokenize_punctuation - Process punctuation tokens
 * @state: Current tokenizer state
 * @token: Token structure to populate
 * 
 * Returns: RIFT_SUCCESS on success, error code on failure
 */
int tokenize_punctuation(rift_tokenizer_state_t* state, rift_token_t* token);

/*
 * Utility and Validation Functions
 */

/**
 * rift_token_type_to_string - Convert token type to string representation
 * @type: Token type enumeration value
 * 
 * Returns: String representation of token type
 */
const char* rift_token_type_to_string(rift_token_type_t type);

/**
 * rift_tokenizer_validate_token - AEGIS governance token validation
 * @token: Token to validate
 * 
 * Validates token according to AEGIS governance policies and
 * security requirements.
 * 
 * Returns: RIFT_SUCCESS if valid, error code if validation fails
 */
int rift_tokenizer_validate_token(const rift_token_t* token);

/**
 * rift_tokenizer_print_token - Debug output for token
 * @token: Token to print
 * @output: File pointer for output (stdout, stderr, etc.)
 * 
 * Prints formatted token information for debugging purposes.
 */
void rift_tokenizer_print_token(const rift_token_t* token, FILE* output);

/**
 * rift_tokenizer_get_version - Get tokenizer version information
 * @major: Pointer to store major version
 * @minor: Pointer to store minor version
 * @patch: Pointer to store patch version
 */
void rift_tokenizer_get_version(int* major, int* minor, int* patch);

/*
 * AEGIS Governance Integration Points
 */

/**
 * rift_governance_validate_token - External governance validation
 * @token: Token to validate against governance policies
 * 
 * External function implemented in governance module that validates
 * tokens according to AEGIS methodology requirements.
 * 
 * Returns: RIFT_SUCCESS if compliant, error code if violation detected
 */
int rift_governance_validate_token(const rift_token_t* token);

/*
 * Memory Management Utilities
 */

/**
 * aligned_alloc - POSIX aligned memory allocation
 * @alignment: Memory alignment requirement
 * @size: Size of memory to allocate
 * 
 * Returns: Pointer to aligned memory, or NULL on failure
 */
#ifndef aligned_alloc
void* aligned_alloc(size_t alignment, size_t size);
#endif

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_STAGE_0_TOKENIZER_H */
