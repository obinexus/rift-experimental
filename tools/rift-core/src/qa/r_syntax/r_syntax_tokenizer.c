/*
 * RIFT R-Syntax Tokenization Implementation
 */

#include "rift-core/qa/r_syntax_tokenizer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

rift_r_regex_token_t* r_syntax_tokenize(const char* input, size_t input_length) {
    if (!input || input_length == 0) {
        return NULL;
    }
    
    rift_r_regex_token_t* token = calloc(1, sizeof(rift_r_regex_token_t));
    if (!token) {
        return NULL;
    }
    
    // Detect R-syntax pattern type
    if (input_length >= 3 && input[0] == 'R' && input[1] == '"') {
        // R"pattern" - bottom-up matching
        token->type = R_REGEX_BOTTOM_UP;
        token->flags = BOTTOM_MATCHING | SCOPE_DEFAULT;
        
        // Extract pattern between quotes
        const char* pattern_start = input + 2;
        const char* pattern_end = strrchr(input, '"');
        if (pattern_end && pattern_end > pattern_start) {
            size_t pattern_len = pattern_end - pattern_start;
            token->pattern = malloc(pattern_len + 1);
            if (token->pattern) {
                strncpy(token->pattern, pattern_start, pattern_len);
                token->pattern[pattern_len] = '\0';
                token->pattern_length = pattern_len;
            }
        }
    } else if (input_length >= 3 && input[0] == 'R' && input[1] == '\'') {
        // R'pattern' - top-down matching
        token->type = R_REGEX_TOP_DOWN;
        token->flags = TOP_DOWN_MATCHING | SCOPE_USER_OUTPUT;
        
        // Extract pattern between apostrophes
        const char* pattern_start = input + 2;
        const char* pattern_end = strrchr(input, '\'');
        if (pattern_end && pattern_end > pattern_start) {
            size_t pattern_len = pattern_end - pattern_start;
            token->pattern = malloc(pattern_len + 1);
            if (token->pattern) {
                strncpy(token->pattern, pattern_start, pattern_len);
                token->pattern[pattern_len] = '\0';
                token->pattern_length = pattern_len;
            }
        }
    } else {
        token->type = R_REGEX_INVALID;
    }
    
    // Initialize memory model
    token->memory.memory_size = sizeof(rift_r_regex_token_t);
    token->scope.default_channel = (token->flags & SCOPE_DEFAULT) != 0;
    token->scope.user_output_channel = (token->flags & SCOPE_USER_OUTPUT) != 0;
    
    return token;
}

bool r_syntax_validate_token(const rift_r_regex_token_t* token) {
    if (!token) {
        return false;
    }
    
    // Validate token type
    if (token->type == R_REGEX_INVALID) {
        return false;
    }
    
    // Validate pattern exists
    if (!token->pattern || token->pattern_length == 0) {
        return false;
    }
    
    // Validate flags consistency
    if (token->type == R_REGEX_BOTTOM_UP && !(token->flags & BOTTOM_MATCHING)) {
        return false;
    }
    
    if (token->type == R_REGEX_TOP_DOWN && !(token->flags & TOP_DOWN_MATCHING)) {
        return false;
    }
    
    return true;
}

void r_syntax_token_destroy(rift_r_regex_token_t* token) {
    if (token) {
        if (token->pattern) {
            free(token->pattern);
        }
        free(token);
    }
}

bool r_syntax_match_bottom_up(const char* pattern, const char* input) {
    // Simplified bottom-up pattern matching implementation
    if (!pattern || !input) {
        return false;
    }
    
    // For demo: basic substring search for bottom-up matching
    return strstr(input, pattern) != NULL;
}

bool r_syntax_match_top_down(const char* pattern, const char* input) {
    // Simplified top-down pattern matching implementation
    if (!pattern || !input) {
        return false;
    }
    
    // For demo: prefix matching for top-down matching
    return strncmp(input, pattern, strlen(pattern)) == 0;
}
