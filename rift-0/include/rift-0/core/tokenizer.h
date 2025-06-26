/**
 * =================================================================
 * tokenizer.h - RIFT-0 Core Tokenizer Interface
 * RIFT: RIFT Is a Flexible Translator
 * Component: DFA-based tokenization with regex composition
 * OBINexus Computing Framework - Stage 0 Implementation
 * 
 * Toolchain: riftlang.exe → .so.a → rift.exe → gosilang
 * Build Orchestration: nlink → polybuild (AEGIS Framework)
 * =================================================================
 */

#ifndef RIFT_0_CORE_TOKENIZER_H
#define RIFT_0_CORE_TOKENIZER_H



/* Hierarchical dependency - follows Sinphasé ordering */
#include "rift-0/core/tokenizer_types.h"
#include "rift-0/core/tokenizer_rules.h"

#if !defined(_WIN32)
#include <unistd.h>
#endif

// Provide ssize_t if not defined (for Windows/MSVC)
#if defined(_MSC_VER) && !defined(_SSIZE_T_DEFINED)
#include <stddef.h>
typedef ptrdiff_t ssize_t;
#define _SSIZE_T_DEFINED
#endif

// For clock_gettime and CLOCK_MONOTONIC on Windows
#if defined(_WIN32)
#include <windows.h>
#include <time.h>
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif
struct timespec {
    time_t tv_sec;
    long tv_nsec;
};
static int clock_gettime(int clk_id, struct timespec* t) {
    LARGE_INTEGER freq, count;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&count);
    t->tv_sec = (time_t)(count.QuadPart / freq.QuadPart);
    t->tv_nsec = (long)(((count.QuadPart % freq.QuadPart) * 1000000000) / freq.QuadPart);
    return 0;
}
#else
#include <time.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif

/* =================================================================
 * TOKENIZER CONTEXT LIFECYCLE - PRIMARY INTERFACE
 * =================================================================
 */

/**
 * Create new tokenizer context with default configuration
 * @return Initialized TokenizerContext or NULL on failure
 */
TokenizerContext* rift_tokenizer_create(void);

/**
 * Create tokenizer context with specific capacity
 * @param token_capacity Initial token buffer capacity
 * @param pattern_capacity Initial pattern array capacity
 * @return Initialized TokenizerContext or NULL on failure
 */
TokenizerContext* rift_tokenizer_create_with_capacity(size_t token_capacity, 
                                                     size_t pattern_capacity);

/**
 * Destroy tokenizer context and free all resources
 * @param ctx Tokenizer context to destroy
 */
void rift_tokenizer_destroy(TokenizerContext* ctx);

/**
 * Reset tokenizer context to initial state (preserving capacity)
 * @param ctx Tokenizer context to reset
 * @return true on success, false on failure
 */
bool rift_tokenizer_reset(TokenizerContext* ctx);

/* =================================================================
 * CORE TOKENIZATION OPERATIONS - MAIN PROCESSING INTERFACE
 * =================================================================
 */

/**
 * Tokenize input string using registered rules
 * @param ctx Tokenizer context
 * @param input Input string to tokenize
 * @param length Input string length (or 0 for null-terminated)
 * @return Number of tokens generated, or -1 on error
 */
ssize_t rift_tokenizer_process(TokenizerContext* ctx, const char* input, size_t length);

/**
 * Tokenize input string with specific flags
 * @param ctx Tokenizer context
 * @param input Input string to tokenize
 * @param length Input string length
 * @param flags Processing flags
 * @return Number of tokens generated, or -1 on error
 */
ssize_t rift_tokenizer_process_with_flags(TokenizerContext* ctx, 
                                         const char* input, 
                                         size_t length,
                                         TokenFlags flags);

/**
 * Get generated tokens from context
 * @param ctx Tokenizer context
 * @param tokens Output array for tokens (may be NULL to query count)
 * @param max_tokens Maximum tokens to copy
 * @return Number of available tokens
 */
size_t rift_tokenizer_get_tokens(const TokenizerContext* ctx, 
                                 TokenTriplet* tokens, 
                                 size_t max_tokens);

/**
 * Get specific token by index
 * @param ctx Tokenizer context
 * @param index Token index
 * @param token Output token (may be NULL to check bounds)
 * @return true if index is valid, false otherwise
 */
bool rift_tokenizer_get_token_at(const TokenizerContext* ctx, 
                                size_t index, 
                                TokenTriplet* token);

/* =================================================================
 * TOKENIZER CONFIGURATION - RUNTIME BEHAVIOR CONTROL
 * =================================================================
 */

/**
 * Set global tokenizer flags
 * @param ctx Tokenizer context
 * @param flags Global flags to set
 * @return true on success, false on failure
 */
bool rift_tokenizer_set_flags(TokenizerContext* ctx, TokenFlags flags);

/**
 * Get current global tokenizer flags
 * @param ctx Tokenizer context
 * @return Current global flags
 */
TokenFlags rift_tokenizer_get_flags(const TokenizerContext* ctx);

/**
 * Enable debug mode with optional output callback
 * @param ctx Tokenizer context
 * @param enable Whether to enable debug mode
 * @return true on success, false on failure
 */
bool rift_tokenizer_set_debug_mode(TokenizerContext* ctx, bool enable);

/**
 * Enable strict parsing mode
 * @param ctx Tokenizer context
 * @param strict Whether to enable strict mode
 * @return true on success, false on failure
 */
bool rift_tokenizer_set_strict_mode(TokenizerContext* ctx, bool strict);

/**
 * Configure thread safety mode
 * @param ctx Tokenizer context
 * @param thread_safe Whether to enable thread safety
 * @return true on success, false on failure
 */
bool rift_tokenizer_set_thread_safe_mode(TokenizerContext* ctx, bool thread_safe);

/* =================================================================
 * BUFFER MANAGEMENT - MEMORY AND CAPACITY CONTROL
 * =================================================================
 */

/**
 * Resize token buffer capacity
 * @param ctx Tokenizer context
 * @param new_capacity New token buffer capacity
 * @return true on success, false on failure
 */
bool rift_tokenizer_resize_token_buffer(TokenizerContext* ctx, size_t new_capacity);

/**
 * Resize pattern array capacity
 * @param ctx Tokenizer context
 * @param new_capacity New pattern array capacity
 * @return true on success, false on failure
 */
bool rift_tokenizer_resize_pattern_buffer(TokenizerContext* ctx, size_t new_capacity);

/**
 * Get current token buffer utilization
 * @param ctx Tokenizer context
 * @param used Output for used token count (may be NULL)
 * @param capacity Output for total capacity (may be NULL)
 * @return Buffer utilization ratio (0.0 to 1.0)
 */
double rift_tokenizer_get_token_utilization(const TokenizerContext* ctx, 
                                           size_t* used, 
                                           size_t* capacity);

/**
 * Compact token buffer to remove unused space
 * @param ctx Tokenizer context
 * @return true on success, false on failure
 */
bool rift_tokenizer_compact_buffers(TokenizerContext* ctx);

/* =================================================================
 * STATISTICS AND DIAGNOSTICS - PERFORMANCE MONITORING
 * =================================================================
 */

/**
 * Get tokenizer runtime statistics
 * @param ctx Tokenizer context
 * @param stats Output statistics structure
 * @return true on success, false on failure
 */
bool rift_tokenizer_get_statistics(const TokenizerContext* ctx, TokenizerStats* stats);

/**
 * Reset tokenizer statistics counters
 * @param ctx Tokenizer context
 * @return true on success, false on failure
 */
bool rift_tokenizer_reset_statistics(TokenizerContext* ctx);

/**
 * Get current parsing position information
 * @param ctx Tokenizer context
 * @param position Output for character position (may be NULL)
 * @param line Output for line number (may be NULL)
 * @param column Output for column number (may be NULL)
 * @return true if position tracking is active, false otherwise
 */
bool rift_tokenizer_get_position(const TokenizerContext* ctx, 
                                size_t* position, 
                                size_t* line, 
                                size_t* column);

/* =================================================================
 * ERROR HANDLING - TOKENIZER-LEVEL ERROR MANAGEMENT
 * =================================================================
 */

/**
 * Check if tokenizer has error state
 * @param ctx Tokenizer context
 * @return true if error state is active, false otherwise
 */
bool rift_tokenizer_has_error(const TokenizerContext* ctx);

/**
 * Get last tokenizer error message
 * @param ctx Tokenizer context
 * @return Error message string or NULL if no error
 */
const char* rift_tokenizer_get_error_message(const TokenizerContext* ctx);

/**
 * Get last tokenizer error code
 * @param ctx Tokenizer context
 * @return TokenizerErrorCode value
 */
TokenizerErrorCode rift_tokenizer_get_error_code(const TokenizerContext* ctx);

/**
 * Clear tokenizer error state
 * @param ctx Tokenizer context
 */
void rift_tokenizer_clear_error(TokenizerContext* ctx);

/* =================================================================
 * UTILITY FUNCTIONS - CONVENIENCE AND DEBUGGING HELPERS
 * =================================================================
 */

/**
 * Get tokenizer version string
 * @return Version string (e.g., "0.4.0")
 */
const char* rift_tokenizer_get_version(void);

/**
 * Convert token type to string representation
 * @param token_type Token type to convert
 * @return String representation or "UNKNOWN"
 */
const char* rift_tokenizer_token_type_to_string(TokenType token_type);

/**
 * Convert token flags to string representation
 * @param flags Token flags to convert
 * @param buffer Output buffer for string
 * @param buffer_size Size of output buffer
 * @return Number of characters written (excluding null terminator)
 */
size_t rift_tokenizer_token_flags_to_string(TokenFlags flags, 
                                           char* buffer, 
                                           size_t buffer_size);

/**
 * Print token triplet for debugging
 * @param token Token to print
 * @param buffer Output buffer
 * @param buffer_size Size of output buffer
 * @return Number of characters written
 */
size_t rift_tokenizer_print_token(const TokenTriplet* token, 
                                 char* buffer, 
                                 size_t buffer_size);

/**
 * Validate tokenizer context integrity
 * @param ctx Tokenizer context to validate
 * @return true if context is valid, false otherwise
 */
bool rift_tokenizer_validate_context(const TokenizerContext* ctx);

/* =================================================================
 * THREAD SAFETY UTILITIES - CONCURRENT ACCESS SUPPORT
 * =================================================================
 */

/**
 * Lock tokenizer context for exclusive access
 * @param ctx Tokenizer context
 * @return true on successful lock, false on failure
 */
bool rift_tokenizer_lock(TokenizerContext* ctx);

/**
 * Unlock tokenizer context
 * @param ctx Tokenizer context
 * @return true on successful unlock, false on failure
 */
bool rift_tokenizer_unlock(TokenizerContext* ctx);

/**
 * Try to lock tokenizer context without blocking
 * @param ctx Tokenizer context
 * @return true if lock acquired, false if would block
 */
bool rift_tokenizer_trylock(TokenizerContext* ctx);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_0_CORE_TOKENIZER_H */