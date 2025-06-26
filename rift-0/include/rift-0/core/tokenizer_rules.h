/**
 * =================================================================
 * tokenizer_rules.h - RIFT-0 Complete Tokenizer Rules Interface
 * RIFT: RIFT Is a Flexible Translator
 * Component: DFA-based tokenization with R"" pattern support and R-macros
 * OBINexus Computing Framework - Stage 0 Implementation
 * 
 * Toolchain: riftlang.exe → .so.a → rift.exe → gosilang
 * Build Orchestration: nlink → polybuild (AEGIS Framework)
 * =================================================================
 */

#ifndef RIFT_0_CORE_TOKENIZER_RULES_H
#define RIFT_0_CORE_TOKENIZER_RULES_H

/* Include type definitions first */
#include "rift-0/core/tokenizer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DFAState DFAState;
typedef struct RegexComposition RegexComposition;

typedef struct {
    TokenTriplet token;
    int match_length;
    bool success;
    const char* error_msg;
} PatternMatchResult;

/* =================================================================
 * DFA STATE MANAGEMENT - CORE OPERATIONS
 * =================================================================
 */

/**
 * Create a new DFA state with specified ID and final state flag
 * @param state_id Unique identifier for the state
 * @param is_final Whether this is an accepting state
 * @return Pointer to created DFAState or NULL on failure
 */
DFAState* rift_dfa_create_state(uint32_t state_id, bool is_final);

/**
 * Destroy DFA state tree recursively from root
 * @param root Root state to begin destruction
 */
void rift_dfa_destroy_states(DFAState* root);

/**
 * Add transition between two DFA states
 * @param from Source state
 * @param to Destination state
 * @param transition_char Character triggering transition
 * @return true on success, false on failure
 */
bool rift_dfa_add_transition(DFAState* from, DFAState* to, char transition_char);

/**
 * Process input string through DFA from start state
 * @param start Initial DFA state
 * @param input Input string to process
 * @param length Length of input string
 * @return Final state reached or NULL on error
 */
DFAState* rift_dfa_process_input(DFAState* start, const char* input, size_t length);

/**
 * Check if DFA state is an accepting state
 * @param state State to check
 * @return true if accepting state, false otherwise
 */
bool rift_dfa_is_accepting_state(DFAState* state);

/**
 * Get token type associated with DFA state
 * @param state State to query
 * @return TokenType associated with state
 */
TokenType rift_dfa_get_token_type(DFAState* state);

/**
 * Set token type for a DFA state
 * @param state State to modify
 * @param token_type Token type to assign
 * @return true on success, false on failure
 */
bool rift_dfa_set_token_type(DFAState* state, TokenType token_type);

/* =================================================================
 * REGEX COMPOSITION OPERATIONS - PATTERN COMPILATION
 * =================================================================
 */

/**
 * Compile regex pattern into DFA-based composition
 * @param pattern Regex pattern string
 * @param flags Compilation flags
 * @return Compiled RegexComposition or NULL on failure
 */
RegexComposition* rift_regex_compile(const char* pattern, TokenFlags flags);

/**
 * Destroy compiled regex composition
 * @param regex Composition to destroy
 */
void rift_regex_destroy(RegexComposition* regex);

/**
 * Match input against compiled regex
 * @param regex Compiled regex composition
 * @param input Input string to match
 * @param length Length of input string
 * @return true on match, false otherwise
 */
bool rift_regex_match(const RegexComposition* regex, const char* input, size_t length);

/**
 * Extract matches from input using compiled regex
 * @param regex Compiled regex composition
 * @param input Input string
 * @param length Input length
 * @param matches Output array for match results
 * @param max_matches Maximum number of matches to extract
 * @return Number of matches found
 */
size_t rift_regex_extract_matches(const RegexComposition* regex, 
                                  const char* input, 
                                  size_t length,
                                  TokenTriplet* matches, 
                                  size_t max_matches);

/* =================================================================
 * TOKEN CREATION AND VALIDATION - TRIPLET OPERATIONS
 * =================================================================
 */

/**
 * Create token triplet with specified parameters
 * @param type Token type identifier (0-255)
 * @param mem_ptr Memory pointer/offset (0-65535)
 * @param value Token value/flags (0-255)
 * @return Created TokenTriplet
 */
TokenTriplet rift_token_create(uint8_t type, uint16_t mem_ptr, uint8_t value);

/**
 * Validate token triplet structure and values
 * @param token Token to validate
 * @return true if valid, false otherwise
 */
bool rift_token_validate(const TokenTriplet* token);

/**
 * Extract token type from triplet
 * @param token Token triplet
 * @return TokenType value
 */
TokenType rift_token_get_type(const TokenTriplet* token);

/**
 * Extract memory pointer from triplet
 * @param token Token triplet
 * @return Memory pointer value
 */
uint16_t rift_token_get_mem_ptr(const TokenTriplet* token);

/**
 * Extract value/flags from triplet
 * @param token Token triplet
 * @return Value/flags byte
 */
uint8_t rift_token_get_value(const TokenTriplet* token);

/**
 * Set token flags in triplet value field
 * @param token Token to modify
 * @param flags Flags to set
 * @return Modified token triplet
 */
TokenTriplet rift_token_set_flags(TokenTriplet token, TokenFlags flags);

/* =================================================================
 * PATTERN MATCHING RULES - HIGH-LEVEL RULE API
 * =================================================================
 */

/**
 * Register new tokenization rule with context
 * @param ctx Tokenizer context
 * @param pattern Regex pattern for rule
 * @param token_type Token type to generate
 * @param flags Rule-specific flags
 * @return true on successful registration, false on failure
 */
bool rift_rules_register_pattern(TokenizerContext* ctx, 
                                 const char* pattern,
                                 TokenType token_type,
                                 TokenFlags flags);

/**
 * Remove registered pattern rule
 * @param ctx Tokenizer context
 * @param pattern Pattern to remove
 * @return true on successful removal, false if not found
 */
bool rift_rules_unregister_pattern(TokenizerContext* ctx, const char* pattern);

/**
 * Apply all registered rules to input and generate tokens
 * @param ctx Tokenizer context
 * @param input Input string to tokenize
 * @param length Input string length
 * @return Number of tokens generated, or -1 on error
 */
ssize_t rift_rules_apply_all(TokenizerContext* ctx, const char* input, size_t length);

/**
 * Get number of registered rules in context
 * @param ctx Tokenizer context
 * @return Number of active rules
 */
size_t rift_rules_get_count(const TokenizerContext* ctx);

/**
 * Clear all registered rules from context
 * @param ctx Tokenizer context
 * @return true on success, false on failure
 */
bool rift_rules_clear_all(TokenizerContext* ctx);

/* =================================================================
 * RULE COMPOSITION OPERATIONS - ADVANCED PATTERN HANDLING
 * =================================================================
 */

/**
 * Compose multiple patterns using logical operations
 * @param patterns Array of pattern strings
 * @param pattern_count Number of patterns
 * @param operation Composition operation (AND, OR, XOR, NAND)
 * @param flags Composition flags
 * @return Composed RegexComposition or NULL on failure
 */
RegexComposition* rift_rules_compose_patterns(const char** patterns,
                                              size_t pattern_count,
                                              TokenType operation,
                                              TokenFlags flags);

/**
 * Validate rule composition for correctness and performance
 * @param composition Composition to validate
 * @return true if valid, false otherwise
 */
bool rift_rules_validate_composition(const RegexComposition* composition);

/**
 * Optimize rule composition for better performance
 * @param composition Composition to optimize
 * @return Optimized composition or NULL on failure
 */
RegexComposition* rift_rules_optimize_composition(RegexComposition* composition);

/* =================================================================
 * ERROR HANDLING AND DIAGNOSTICS
 * =================================================================
 */

/**
 * Get last error message from rule processing
 * @param ctx Tokenizer context
 * @return Error message string or NULL if no error
 */
const char* rift_rules_get_last_error(const TokenizerContext* ctx);

/**
 * Get last error code from rule processing
 * @param ctx Tokenizer context
 * @return TokenizerErrorCode value
 */
TokenizerErrorCode rift_rules_get_last_error_code(const TokenizerContext* ctx);

/**
 * Clear error state in tokenizer context
 * @param ctx Tokenizer context
 */
void rift_rules_clear_error(TokenizerContext* ctx);

/* =================================================================
 * SIMPLE MATCHING HELPERS (LEGACY TEST SUPPORT)
 * ================================================================= */

typedef struct {
    TokenTriplet* tokens;
    size_t count;
    bool success;
    char* error_message;
} TokenizationResult;

int init_tokenizer_rules(void);
void cleanup_tokenizer_rules(void);

int match_token_pattern(const char* src, TokenTriplet* out_token);
int match_token_pattern_ex(const char* src, const char* pattern,
                           uint32_t flags, PatternMatchResult* result);

TokenizationResult tokenize_source(const char* src, size_t length);
void free_tokenization_result(TokenizationResult* result);
int tokenize_source_into(const char* src, TokenTriplet* tokens,
                         size_t max_tokens, size_t* token_count);

TokenTriplet R_encode_safe(TokenType type, uint16_t mem_ptr, uint8_t value);
bool validate_token_separation(const TokenTriplet* token);
TokenType classify_null_nil_semantic(const char* text, size_t length);

typedef struct DFAStateMachine DFAStateMachine;
DFAStateMachine* dfa_create(void);
bool dfa_process_char(DFAStateMachine* dfa, char c);
bool dfa_is_accepting(DFAStateMachine* dfa);
void dfa_reset(DFAStateMachine* dfa);
void dfa_destroy(DFAStateMachine* dfa);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_0_CORE_TOKENIZER_RULES_H */