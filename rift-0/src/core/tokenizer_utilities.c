/**
 * =================================================================
 * tokenizer_utilities.c - RIFT-0 Tokenizer Utility Implementation
 * RIFT: RIFT Is a Flexible Translator
 * Component: Buffer management, statistics, and utility functions
 * OBINexus Computing Framework - Stage 0 Implementation
 * =================================================================
 */

#include "rift-0/core/tokenizer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

/* =================================================================
 * BUFFER MANAGEMENT IMPLEMENTATION
 * =================================================================
 */

bool rift_tokenizer_resize_token_buffer(TokenizerContext* ctx, size_t new_capacity) {
    if (!ctx || new_capacity == 0 || new_capacity > RIFT_TOKENIZER_MAX_TOKENS) {
        if (ctx) {
            snprintf(ctx->error_message, RIFT_TOKENIZER_ERROR_MSG_SIZE,
                    "Invalid token buffer capacity: %zu (max: %d)", 
                    new_capacity, RIFT_TOKENIZER_MAX_TOKENS);
            ctx->error_code = RIFT_TOKENIZER_ERROR_INVALID_INPUT;
            ctx->has_error = true;
        }
        return false;
    }
    
    if (atomic_load(&ctx->thread_safe_mode)) {
        if (!rift_tokenizer_lock(ctx)) return false;
    }
    
    /* Preserve existing tokens up to the smaller of old/new capacity */
    size_t preserve_count = (ctx->token_count < new_capacity) ? 
                           ctx->token_count : new_capacity;
    
    TokenTriplet* new_buffer = calloc(new_capacity, sizeof(TokenTriplet));
    if (!new_buffer) {
        snprintf(ctx->error_message, RIFT_TOKENIZER_ERROR_MSG_SIZE,
                "Failed to allocate token buffer: %s", strerror(errno));
        ctx->error_code = RIFT_TOKENIZER_ERROR_MEMORY_ALLOCATION_FAILED;
        ctx->has_error = true;
        
        if (atomic_load(&ctx->thread_safe_mode)) {
            rift_tokenizer_unlock(ctx);
        }
        return false;
    }
    
    /* Copy existing tokens */
    if (preserve_count > 0) {
        memcpy(new_buffer, ctx->tokens, preserve_count * sizeof(TokenTriplet));
    }
    
    /* Update context */
    free(ctx->tokens);
    ctx->tokens = new_buffer;
    ctx->token_capacity = new_capacity;
    ctx->token_count = preserve_count;
    
    /* Update statistics */
    size_t new_allocated = new_capacity * sizeof(TokenTriplet);
    if (new_allocated > ctx->stats.memory_allocated) {
        ctx->stats.memory_peak = new_allocated;
    }
    ctx->stats.memory_allocated = new_allocated;
    
    if (atomic_load(&ctx->thread_safe_mode)) {
        rift_tokenizer_unlock(ctx);
    }
    
    return true;
}

bool rift_tokenizer_resize_pattern_buffer(TokenizerContext* ctx, size_t new_capacity) {
    if (!ctx || new_capacity == 0 || new_capacity > RIFT_TOKENIZER_MAX_PATTERNS) {
        if (ctx) {
            snprintf(ctx->error_message, RIFT_TOKENIZER_ERROR_MSG_SIZE,
                    "Invalid pattern buffer capacity: %zu (max: %d)", 
                    new_capacity, RIFT_TOKENIZER_MAX_PATTERNS);
            ctx->error_code = RIFT_TOKENIZER_ERROR_INVALID_INPUT;
            ctx->has_error = true;
        }
        return false;
    }
    
    if (atomic_load(&ctx->thread_safe_mode)) {
        if (!rift_tokenizer_lock(ctx)) return false;
    }
    
    /* Check if downsizing would lose patterns */
    if (new_capacity < ctx->pattern_count) {
        snprintf(ctx->error_message, RIFT_TOKENIZER_ERROR_MSG_SIZE,
                "Cannot downsize pattern buffer: would lose %zu patterns", 
                ctx->pattern_count - new_capacity);
        ctx->error_code = RIFT_TOKENIZER_ERROR_INVALID_INPUT;
        ctx->has_error = true;
        
        if (atomic_load(&ctx->thread_safe_mode)) {
            rift_tokenizer_unlock(ctx);
        }
        return false;
    }
    
    RegexComposition** new_buffer = calloc(new_capacity, sizeof(RegexComposition*));
    if (!new_buffer) {
        snprintf(ctx->error_message, RIFT_TOKENIZER_ERROR_MSG_SIZE,
                "Failed to allocate pattern buffer: %s", strerror(errno));
        ctx->error_code = RIFT_TOKENIZER_ERROR_MEMORY_ALLOCATION_FAILED;
        ctx->has_error = true;
        
        if (atomic_load(&ctx->thread_safe_mode)) {
            rift_tokenizer_unlock(ctx);
        }
        return false;
    }
    
    /* Copy existing patterns */
    if (ctx->pattern_count > 0) {
        memcpy(new_buffer, ctx->regex_patterns, 
               ctx->pattern_count * sizeof(RegexComposition*));
    }
    
    /* Update context */
    free(ctx->regex_patterns);
    ctx->regex_patterns = new_buffer;
    ctx->pattern_capacity = new_capacity;
    
    if (atomic_load(&ctx->thread_safe_mode)) {
        rift_tokenizer_unlock(ctx);
    }
    
    return true;
}

double rift_tokenizer_get_token_utilization(const TokenizerContext* ctx, 
                                           size_t* used, 
                                           size_t* capacity) {
    if (!ctx) return 0.0;
    
    if (used) *used = ctx->token_count;
    if (capacity) *capacity = ctx->token_capacity;
    
    return ctx->token_capacity > 0 ? 
           (double)ctx->token_count / (double)ctx->token_capacity : 0.0;
}

bool rift_tokenizer_compact_buffers(TokenizerContext* ctx) {
    if (!ctx) return false;
    
    if (atomic_load(&ctx->thread_safe_mode)) {
        if (!rift_tokenizer_lock(ctx)) return false;
    }
    
    bool success = true;
    
    /* Compact token buffer if underutilized */
    if (ctx->token_count > 0 && ctx->token_count < ctx->token_capacity / 2) {
        size_t new_capacity = ctx->token_count * 2; /* 50% overhead */
        if (new_capacity < RIFT_TOKENIZER_DEFAULT_CAPACITY) {
            new_capacity = RIFT_TOKENIZER_DEFAULT_CAPACITY;
        }
        
        if (!rift_tokenizer_resize_token_buffer(ctx, new_capacity)) {
            success = false;
        }
    }
    
    /* Compact pattern buffer if underutilized */
    if (ctx->pattern_count > 0 && ctx->pattern_count < ctx->pattern_capacity / 2) {
        size_t new_capacity = ctx->pattern_count * 2; /* 50% overhead */
        if (new_capacity < 16) {
            new_capacity = 16;
        }
        
        if (!rift_tokenizer_resize_pattern_buffer(ctx, new_capacity)) {
            success = false;
        }
    }
    
    if (atomic_load(&ctx->thread_safe_mode)) {
        rift_tokenizer_unlock(ctx);
    }
    
    return success;
}

/* =================================================================
 * STATISTICS AND DIAGNOSTICS IMPLEMENTATION
 * =================================================================
 */

bool rift_tokenizer_get_statistics(const TokenizerContext* ctx, TokenizerStats* stats) {
    if (!ctx || !stats) return false;
    
    /* Copy current statistics */
    memcpy(stats, &ctx->stats, sizeof(TokenizerStats));
    
    /* Update current memory usage */
    stats->memory_allocated = (ctx->token_capacity * sizeof(TokenTriplet)) +
                             (ctx->pattern_capacity * sizeof(RegexComposition*));
    
    return true;
}

bool rift_tokenizer_reset_statistics(TokenizerContext* ctx) {
    if (!ctx) return false;
    
    if (atomic_load(&ctx->thread_safe_mode)) {
        if (!rift_tokenizer_lock(ctx)) return false;
    }
    
    /* Preserve current memory allocation values */
    size_t current_allocated = ctx->stats.memory_allocated;
    size_t current_peak = ctx->stats.memory_peak;
    
    /* Reset all statistics */
    memset(&ctx->stats, 0, sizeof(TokenizerStats));
    
    /* Restore memory values */
    ctx->stats.memory_allocated = current_allocated;
    ctx->stats.memory_peak = current_peak;
    
    if (atomic_load(&ctx->thread_safe_mode)) {
        rift_tokenizer_unlock(ctx);
    }
    
    return true;
}

bool rift_tokenizer_get_position(const TokenizerContext* ctx, 
                                size_t* position, 
                                size_t* line, 
                                size_t* column) {
    if (!ctx) return false;
    
    if (position) *position = ctx->current_position;
    if (line) *line = ctx->line_number;
    if (column) *column = ctx->column_number;
    
    return ctx->input_buffer != NULL;
}

/* =================================================================
 * UTILITY STRING CONVERSION FUNCTIONS
 * =================================================================
 */

size_t rift_tokenizer_token_flags_to_string(TokenFlags flags, 
                                           char* buffer, 
                                           size_t buffer_size) {
    if (!buffer || buffer_size == 0) return 0;
    
    buffer[0] = '\0';
    size_t written = 0;
    
    if (flags == TOKEN_FLAG_NONE) {
        written = snprintf(buffer, buffer_size, "NONE");
        return (written < buffer_size) ? written : buffer_size - 1;
    }
    
    bool first = true;
    
    if (flags & TOKEN_FLAG_GLOBAL) {
        written += snprintf(buffer + written, buffer_size - written, 
                           "%sGLOBAL", first ? "" : "|");
        first = false;
    }
    
    if (flags & TOKEN_FLAG_MULTILINE) {
        written += snprintf(buffer + written, buffer_size - written, 
                           "%sMULTILINE", first ? "" : "|");
        first = false;
    }
    
    if (flags & TOKEN_FLAG_IGNORECASE) {
        written += snprintf(buffer + written, buffer_size - written, 
                           "%sIGNORECASE", first ? "" : "|");
        first = false;
    }
    
    if (flags & TOKEN_FLAG_TOPDOWN) {
        written += snprintf(buffer + written, buffer_size - written, 
                           "%sTOPDOWN", first ? "" : "|");
        first = false;
    }
    
    if (flags & TOKEN_FLAG_BOTTOMUP) {
        written += snprintf(buffer + written, buffer_size - written, 
                           "%sBOTTOMUP", first ? "" : "|");
        first = false;
    }
    
    if (flags & TOKEN_FLAG_COMPOSED) {
        written += snprintf(buffer + written, buffer_size - written, 
                           "%sCOMPOSED", first ? "" : "|");
        first = false;
    }
    
    if (flags & TOKEN_FLAG_VALIDATED) {
        written += snprintf(buffer + written, buffer_size - written, 
                           "%sVALIDATED", first ? "" : "|");
        first = false;
    }
    
    if (flags & TOKEN_FLAG_ERROR) {
        written += snprintf(buffer + written, buffer_size - written, 
                           "%sERROR", first ? "" : "|");
        first = false;
    }
    
    return (written < buffer_size) ? written : buffer_size - 1;
}

size_t rift_tokenizer_print_token(const TokenTriplet* token, 
                                 char* buffer, 
                                 size_t buffer_size) {
    if (!token || !buffer || buffer_size == 0) return 0;
    
    const char* type_str = rift_tokenizer_token_type_to_string(
        (TokenType)token->type);
    
    char flags_str[256];
    rift_tokenizer_token_flags_to_string((TokenFlags)token->value, 
                                        flags_str, sizeof(flags_str));
    
    return snprintf(buffer, buffer_size,
                   "Token{type=%s(%u), mem_ptr=%u, flags=%s(%u)}", 
                   type_str, token->type, token->mem_ptr, 
                   flags_str, token->value);
}

/* =================================================================
 * ADVANCED DIAGNOSTIC FUNCTIONS
 * =================================================================
 */

bool rift_tokenizer_dump_state(const TokenizerContext* ctx, 
                               char* buffer, 
                               size_t buffer_size) {
    if (!ctx || !buffer || buffer_size < 512) return false;
    
    size_t written = 0;
    
    written += snprintf(buffer + written, buffer_size - written,
                       "=== RIFT-0 Tokenizer State Dump ===\n");
    
    written += snprintf(buffer + written, buffer_size - written,
                       "Version: %s\n", RIFT_TOKENIZER_VERSION);
    
    written += snprintf(buffer + written, buffer_size - written,
                       "Token Buffer: %zu/%zu (%.1f%% utilized)\n",
                       ctx->token_count, ctx->token_capacity,
                       rift_tokenizer_get_token_utilization(ctx, NULL, NULL) * 100.0);
    
    written += snprintf(buffer + written, buffer_size - written,
                       "Pattern Buffer: %zu/%zu patterns\n",
                       ctx->pattern_count, ctx->pattern_capacity);
    
    written += snprintf(buffer + written, buffer_size - written,
                       "Current Position: %zu (line %zu, col %zu)\n",
                       ctx->current_position, ctx->line_number, ctx->column_number);
    
    written += snprintf(buffer + written, buffer_size - written,
                       "Thread Safe Mode: %s\n",
                       atomic_load(&ctx->thread_safe_mode) ? "enabled" : "disabled");
    
    written += snprintf(buffer + written, buffer_size - written,
                       "Debug Mode: %s\n", ctx->debug_mode ? "enabled" : "disabled");
    
    written += snprintf(buffer + written, buffer_size - written,
                       "Strict Mode: %s\n", ctx->strict_mode ? "enabled" : "disabled");
    
    char flags_str[256];
    rift_tokenizer_token_flags_to_string(ctx->global_flags, flags_str, sizeof(flags_str));
    written += snprintf(buffer + written, buffer_size - written,
                       "Global Flags: %s\n", flags_str);
    
    written += snprintf(buffer + written, buffer_size - written,
                       "Error State: %s\n", ctx->has_error ? "ERROR" : "OK");
    
    if (ctx->has_error) {
        written += snprintf(buffer + written, buffer_size - written,
                           "Last Error: %s (code %u)\n",
                           ctx->error_message, ctx->error_code);
    }
    
    written += snprintf(buffer + written, buffer_size - written,
                       "Statistics:\n");
    written += snprintf(buffer + written, buffer_size - written,
                       "  Tokens Processed: %zu\n", ctx->stats.tokens_processed);
    written += snprintf(buffer + written, buffer_size - written,
                       "  Tokens Generated: %zu\n", ctx->stats.tokens_generated);
    written += snprintf(buffer + written, buffer_size - written,
                       "  Memory Allocated: %zu bytes\n", ctx->stats.memory_allocated);
    written += snprintf(buffer + written, buffer_size - written,
                       "  Memory Peak: %zu bytes\n", ctx->stats.memory_peak);
    written += snprintf(buffer + written, buffer_size - written,
                       "  DFA States Created: %zu\n", ctx->stats.dfa_states_created);
    written += snprintf(buffer + written, buffer_size - written,
                       "  Processing Time: %.3f seconds\n", ctx->stats.processing_time);
    written += snprintf(buffer + written, buffer_size - written,
                       "  Error Count: %u\n", ctx->stats.error_count);
    
    written += snprintf(buffer + written, buffer_size - written,
                       "=== End State Dump ===\n");
    
    return written < buffer_size;
}

/* =================================================================
 * PERFORMANCE PROFILING UTILITIES
 * =================================================================
 */

bool rift_tokenizer_benchmark_processing(TokenizerContext* ctx,
                                        const char* test_input,
                                        size_t iterations,
                                        double* avg_time_ms) {
    if (!ctx || !test_input || iterations == 0 || !avg_time_ms) return false;
    
    size_t input_length = strlen(test_input);
    struct timespec start, end;
    double total_time = 0.0;
    
    for (size_t i = 0; i < iterations; i++) {
        rift_tokenizer_reset(ctx);
        
        clock_gettime(CLOCK_MONOTONIC, &start);
        ssize_t result = rift_tokenizer_process(ctx, test_input, input_length);
        clock_gettime(CLOCK_MONOTONIC, &end);
        
        if (result < 0) return false;
        
        double iteration_time = (end.tv_sec - start.tv_sec) * 1000.0 +
                               (end.tv_nsec - start.tv_nsec) / 1e6;
        total_time += iteration_time;
    }
    
    *avg_time_ms = total_time / iterations;
    return true;
}