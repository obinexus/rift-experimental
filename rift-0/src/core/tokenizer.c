/**
 * =================================================================
 * tokenizer.c - RIFT-0 Core Tokenizer Implementation
 * RIFT: RIFT Is a Flexible Translator
 * Component: Main tokenizer context and processing implementation
 * OBINexus Computing Framework - Stage 0 Implementation
 * =================================================================
 */
#if !defined(_WIN32)
  #ifndef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 199309L
  #endif
#endif
#include <time.h>

#include "rift-0/core/tokenizer.h"

// Ignore non-standard pragma warning (optional)
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h> // For ssize_t

/* =================================================================
 * INTERNAL HELPER FUNCTIONS - STATIC SCOPE
 * =================================================================
 */

static bool _tokenizer_init_context(TokenizerContext* ctx, 
                                   size_t token_capacity, 
                                   size_t pattern_capacity) {
    if (!ctx) return false;
    
    /* Initialize core state */
    ctx->tokens = calloc(token_capacity, sizeof(TokenTriplet));
    if (!ctx->tokens) {
        snprintf(ctx->error_message, RIFT_TOKENIZER_ERROR_MSG_SIZE,
                "Failed to allocate token buffer: %s", strerror(errno));
        ctx->error_code = RIFT_TOKENIZER_ERROR_MEMORY_ALLOCATION_FAILED;
        ctx->has_error = true;
        return false;
    }
    
    ctx->regex_patterns = calloc(pattern_capacity, sizeof(RegexComposition*));
    if (!ctx->regex_patterns) {
        free(ctx->tokens);
        snprintf(ctx->error_message, RIFT_TOKENIZER_ERROR_MSG_SIZE,
                "Failed to allocate pattern buffer: %s", strerror(errno));
        ctx->error_code = RIFT_TOKENIZER_ERROR_MEMORY_ALLOCATION_FAILED;
        ctx->has_error = true;
        return false;
    }
    
    /* Initialize capacities and counts */
    ctx->token_capacity = token_capacity;
    ctx->pattern_capacity = pattern_capacity;
    ctx->token_count = 0;
    ctx->pattern_count = 0;
    
    /* Initialize input state */
    ctx->input_buffer = NULL;
    ctx->input_length = 0;
    ctx->current_position = 0;
    ctx->line_number = 1;
    ctx->column_number = 1;
    
    /* Initialize DFA state */
    ctx->current_dfa_state = NULL;
    
    /* Initialize thread safety */
    if (pthread_mutex_init(&ctx->context_mutex, NULL) != 0) {
        free(ctx->tokens);
        free(ctx->regex_patterns);
        snprintf(ctx->error_message, RIFT_TOKENIZER_ERROR_MSG_SIZE,
                "Failed to initialize mutex: %s", strerror(errno));
        ctx->error_code = RIFT_TOKENIZER_ERROR_MEMORY_ALLOCATION_FAILED;
        ctx->has_error = true;
        return false;
    }
    
    atomic_store(&ctx->thread_safe_mode, false);
    ctx->owner_thread = pthread_self();
    
    /* Initialize error state */
    ctx->error_message[0] = '\0';
    ctx->error_code = RIFT_TOKENIZER_SUCCESS;
    ctx->has_error = false;
    
    /* Initialize statistics */
    memset(&ctx->stats, 0, sizeof(TokenizerStats));
    
    /* Initialize configuration */
    ctx->global_flags = TOKEN_FLAG_NONE;
    ctx->debug_mode = false;
    ctx->strict_mode = false;
    
    return true;
}

static void _tokenizer_cleanup_context(TokenizerContext* ctx) {
    if (!ctx) return;
    
    /* Clean up patterns */
    for (size_t i = 0; i < ctx->pattern_count; i++) {
        if (ctx->regex_patterns[i]) {
            rift_regex_destroy(ctx->regex_patterns[i]);
        }
    }
    
    /* Free buffers */
    free(ctx->tokens);
    free(ctx->regex_patterns);
    
    /* Destroy mutex */
    pthread_mutex_destroy(&ctx->context_mutex);
    
    /* Clear sensitive data */
    memset(ctx, 0, sizeof(TokenizerContext));
}

/* =================================================================
 * TOKENIZER CONTEXT LIFECYCLE IMPLEMENTATION
 * =================================================================
 */

TokenizerContext* rift_tokenizer_create(void) {
    return rift_tokenizer_create_with_capacity(RIFT_TOKENIZER_DEFAULT_CAPACITY,
                                              RIFT_TOKENIZER_MAX_PATTERNS);
}

TokenizerContext* rift_tokenizer_create_with_capacity(size_t token_capacity, 
                                                     size_t pattern_capacity) {
    if (token_capacity == 0 || token_capacity > RIFT_TOKENIZER_MAX_TOKENS) {
        return NULL;
    }
    
    if (pattern_capacity == 0 || pattern_capacity > RIFT_TOKENIZER_MAX_PATTERNS) {
        return NULL;
    }
    
    TokenizerContext* ctx = calloc(1, sizeof(TokenizerContext));
    if (!ctx) return NULL;
    
    if (!_tokenizer_init_context(ctx, token_capacity, pattern_capacity)) {
        free(ctx);
        return NULL;
    }
    
    return ctx;
}

void rift_tokenizer_destroy(TokenizerContext* ctx) {
    if (!ctx) return;
    
    _tokenizer_cleanup_context(ctx);
    free(ctx);
}

bool rift_tokenizer_reset(TokenizerContext* ctx) {
    if (!ctx) return false;
    
    if (atomic_load(&ctx->thread_safe_mode)) {
        if (!rift_tokenizer_lock(ctx)) return false;
    }
    
    /* Reset token state */
    ctx->token_count = 0;
    ctx->current_position = 0;
    ctx->line_number = 1;
    ctx->column_number = 1;
    ctx->current_dfa_state = NULL;
    
    /* Reset error state */
    ctx->error_message[0] = '\0';
    ctx->error_code = RIFT_TOKENIZER_SUCCESS;
    ctx->has_error = false;
    
    /* Clear input reference */
    ctx->input_buffer = NULL;
    ctx->input_length = 0;
    
    if (atomic_load(&ctx->thread_safe_mode)) {
        rift_tokenizer_unlock(ctx);
    }
    
    return true;
}

ssize_t rift_tokenizer_process_with_flags(TokenizerContext* ctx, 
                                         const char* input, 
                                         size_t length,
                                         TokenFlags flags) {
    (void)flags; // Prevent unused parameter warning

    if (!ctx || !input) {
        if (ctx) {
            snprintf(ctx->error_message, RIFT_TOKENIZER_ERROR_MSG_SIZE,
                    "Invalid input parameters: ctx=%p, input=%p", 
                    (void*)ctx, (void*)input);
            ctx->error_code = RIFT_TOKENIZER_ERROR_INVALID_INPUT;
            ctx->has_error = true;
        }
        return -1;
    }
    
    /* Auto-detect length for null-terminated strings */
    if (length == 0) {
        length = strlen(input);
    }
    
    if (atomic_load(&ctx->thread_safe_mode)) {
        if (!rift_tokenizer_lock(ctx)) return -1;
    }
    
    /* Reset for new processing */
    ctx->input_buffer = input;
    ctx->input_length = length;
    ctx->current_position = 0;
    ctx->token_count = 0;
    
    /* Record processing start time */
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    
    /* Apply all registered rules */
    ssize_t result = rift_rules_apply_all(ctx, input, length);
    
    /* Update statistics */
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    
    double processing_time = (end_time.tv_sec - start_time.tv_sec) + 
                           (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
    
    ctx->stats.processing_time += processing_time;
    ctx->stats.tokens_processed += length;
    ctx->stats.tokens_generated += (result > 0) ? result : 0;
    
    if (result < 0) {
        ctx->stats.error_count++;
    }
    
    if (atomic_load(&ctx->thread_safe_mode)) {
        rift_tokenizer_unlock(ctx);
    }
    
    return result;
}


    

size_t rift_tokenizer_get_tokens(const TokenizerContext* ctx, 
                                 TokenTriplet* tokens, 
                                 size_t max_tokens) {
    if (!ctx) return 0;
    
    size_t available_tokens = ctx->token_count;
    
    if (tokens && max_tokens > 0) {
        size_t copy_count = (available_tokens < max_tokens) ? available_tokens : max_tokens;
        memcpy(tokens, ctx->tokens, copy_count * sizeof(TokenTriplet));
    }
    
    return available_tokens;
}

bool rift_tokenizer_get_token_at(const TokenizerContext* ctx, 
                                size_t index, 
                                TokenTriplet* token) {
    if (!ctx || index >= ctx->token_count) return false;
    
    if (token) {
        *token = ctx->tokens[index];
    }
    
    return true;
}

/* =================================================================
 * CONFIGURATION FUNCTIONS IMPLEMENTATION
 * =================================================================
 */

bool rift_tokenizer_set_flags(TokenizerContext* ctx, TokenFlags flags) {
    if (!ctx) return false;
    
    if (atomic_load(&ctx->thread_safe_mode)) {
        if (!rift_tokenizer_lock(ctx)) return false;
    }
    
    ctx->global_flags = flags;
    
    if (atomic_load(&ctx->thread_safe_mode)) {
        rift_tokenizer_unlock(ctx);
    }
    
    return true;
}

TokenFlags rift_tokenizer_get_flags(const TokenizerContext* ctx) {
    if (!ctx) return TOKEN_FLAG_NONE;
    return ctx->global_flags;
}

bool rift_tokenizer_set_debug_mode(TokenizerContext* ctx, bool enable) {
    if (!ctx) return false;
    
    if (atomic_load(&ctx->thread_safe_mode)) {
        if (!rift_tokenizer_lock(ctx)) return false;
    }
    
    ctx->debug_mode = enable;
    
    if (atomic_load(&ctx->thread_safe_mode)) {
        rift_tokenizer_unlock(ctx);
    }
    
    return true;
}

bool rift_tokenizer_set_strict_mode(TokenizerContext* ctx, bool strict) {
    if (!ctx) return false;
    
    if (atomic_load(&ctx->thread_safe_mode)) {
        if (!rift_tokenizer_lock(ctx)) return false;
    }
    
    ctx->strict_mode = strict;
    
    if (atomic_load(&ctx->thread_safe_mode)) {
        rift_tokenizer_unlock(ctx);
    }
    
    return true;
}

bool rift_tokenizer_set_thread_safe_mode(TokenizerContext* ctx, bool thread_safe) {
    if (!ctx) return false;
    
    atomic_store(&ctx->thread_safe_mode, thread_safe);
    
    if (thread_safe) {
        ctx->owner_thread = pthread_self();
    }
    
    return true;
}

/* =================================================================
 * ERROR HANDLING IMPLEMENTATION
 * =================================================================
 */

bool rift_tokenizer_has_error(const TokenizerContext* ctx) {
    return ctx ? ctx->has_error : true;
}

const char* rift_tokenizer_get_error_message(const TokenizerContext* ctx) {
    if (!ctx || !ctx->has_error) return NULL;
    return ctx->error_message;
}

TokenizerErrorCode rift_tokenizer_get_error_code(const TokenizerContext* ctx) {
    if (!ctx) return RIFT_TOKENIZER_ERROR_NULL_CONTEXT;
    return ctx->error_code;
}

void rift_tokenizer_clear_error(TokenizerContext* ctx) {
    if (!ctx) return;
    
    ctx->error_message[0] = '\0';
    ctx->error_code = RIFT_TOKENIZER_SUCCESS;
    ctx->has_error = false;
}

/* =================================================================
 * UTILITY FUNCTIONS IMPLEMENTATION
 * =================================================================
 */

const char* rift_tokenizer_get_version(void) {
    return RIFT_TOKENIZER_VERSION;
}

const char* rift_tokenizer_token_type_to_string(TokenType token_type) {
    switch (token_type) {
        case TOKEN_UNKNOWN: return "UNKNOWN";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_KEYWORD: return "KEYWORD";
        case TOKEN_LITERAL_STRING: return "LITERAL_STRING";
        case TOKEN_LITERAL_NUMBER: return "LITERAL_NUMBER";
        case TOKEN_OPERATOR: return "OPERATOR";
        case TOKEN_PUNCTUATION: return "PUNCTUATION";
        case TOKEN_DELIMITER: return "DELIMITER";
        case TOKEN_WHITESPACE: return "WHITESPACE";
        case TOKEN_COMMENT: return "COMMENT";
        case TOKEN_EOF: return "EOF";
        case TOKEN_REGEX_START: return "REGEX_START";
        case TOKEN_REGEX_END: return "REGEX_END";
        case TOKEN_COMPOSE_AND: return "COMPOSE_AND";
        case TOKEN_COMPOSE_OR: return "COMPOSE_OR";
        case TOKEN_COMPOSE_XOR: return "COMPOSE_XOR";
        case TOKEN_COMPOSE_NAND: return "COMPOSE_NAND";
        case TOKEN_DFA_STATE: return "DFA_STATE";
        case TOKEN_ERROR: return "ERROR";
        default: return "INVALID";
    }
}

bool rift_tokenizer_validate_context(const TokenizerContext* ctx) {
    if (!ctx) return false;
    
    /* Check basic structure integrity */
    if (!ctx->tokens || !ctx->regex_patterns) return false;
    if (ctx->token_count > ctx->token_capacity) return false;
    if (ctx->pattern_count > ctx->pattern_capacity) return false;
    
    /* Check capacity limits */
    if (ctx->token_capacity > RIFT_TOKENIZER_MAX_TOKENS) return false;
    if (ctx->pattern_capacity > RIFT_TOKENIZER_MAX_PATTERNS) return false;
    
    return true;
}

/* =================================================================
 * THREAD SAFETY IMPLEMENTATION
 * =================================================================
 */

bool rift_tokenizer_lock(TokenizerContext* ctx) {
    if (!ctx || !atomic_load(&ctx->thread_safe_mode)) return false;
    return pthread_mutex_lock(&ctx->context_mutex) == 0;
}

bool rift_tokenizer_unlock(TokenizerContext* ctx) {
    if (!ctx || !atomic_load(&ctx->thread_safe_mode)) return false;
    return pthread_mutex_unlock(&ctx->context_mutex) == 0;
}

bool rift_tokenizer_trylock(TokenizerContext* ctx) {
    if (!ctx || !atomic_load(&ctx->thread_safe_mode)) return false;
    return pthread_mutex_trylock(&ctx->context_mutex) == 0;
}