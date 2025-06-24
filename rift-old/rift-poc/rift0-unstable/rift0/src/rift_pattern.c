#include "../include/rift0/rift.h"
#include <ctype.h>

// Parse R"pattern/flags[mode]" syntax
RiftPattern* rift_parse_pattern(const char* pattern_string) {
    if (!pattern_string || strncmp(pattern_string, "R\"", 2) != 0) {
        return NULL;
    }
    
    RiftPattern* pattern = calloc(1, sizeof(RiftPattern));
    if (!pattern) return NULL;
    
    pattern->raw_pattern = strdup(pattern_string);
    if (!pattern->raw_pattern) {
        free(pattern);
        return NULL;
    }
    
    // Find the pattern part between R" and the last "
    const char* start = pattern_string + 2; // Skip R"
    const char* end = NULL;
    
    // Find the closing quote, looking for the last quote before any flags/brackets
    const char* ptr = start;
    while (*ptr && *ptr != '"') {
        if (*ptr == '\\' && *(ptr + 1)) {
            ptr += 2; // Skip escaped character
        } else {
            ptr++;
        }
    }
    end = ptr;
    
    if (!end || *end != '"') {
        rift_pattern_destroy(pattern);
        return NULL;
    }
    
    // Extract the main pattern
    size_t pattern_len = (size_t)(end - start);
    pattern->regex_pattern = calloc(pattern_len + 1, 1);
    if (!pattern->regex_pattern) {
        rift_pattern_destroy(pattern);
        return NULL;
    }
    strncpy(pattern->regex_pattern, start, pattern_len);
    
    // Look for flags after the closing quote
    const char* flags_start = end + 1;
    const char* bracket_start = strchr(flags_start, '[');
    
    if (bracket_start) {
        // Extract flags (between quote and bracket)
        size_t flags_len = (size_t)(bracket_start - flags_start);
        if (flags_len > 0) {
            pattern->flags = calloc(flags_len + 1, 1);
            if (!pattern->flags) {
                rift_pattern_destroy(pattern);
                return NULL;
            }
            strncpy(pattern->flags, flags_start, flags_len);
        }
        
        // Extract mode (inside brackets)
        const char* bracket_end = strchr(bracket_start, ']');
        if (bracket_end && bracket_end > bracket_start + 1) {
            char mode_char = bracket_start[1];
            pattern->mode = (mode_char == 'b') ? RIFT_MODE_BOTTOM_UP : RIFT_MODE_TOP_DOWN;
        } else {
            pattern->mode = RIFT_MODE_TOP_DOWN; // Default
        }
    } else {
        // No brackets, everything after quote is flags
        if (*flags_start) {
            pattern->flags = strdup(flags_start);
            if (!pattern->flags) {
                rift_pattern_destroy(pattern);
                return NULL;
            }
        }
        pattern->mode = RIFT_MODE_TOP_DOWN; // Default
    }
    
    // Default empty flags if none specified
    if (!pattern->flags) {
        pattern->flags = strdup("");
        if (!pattern->flags) {
            rift_pattern_destroy(pattern);
            return NULL;
        }
    }
    
    return pattern;
}

RiftResult rift_pattern_compile(RiftPattern* pattern) {
    if (!pattern || !pattern->regex_pattern) {
        return RIFT_ERROR_NULL_POINTER;
    }
    
    int flags = REG_EXTENDED;
    
    // Process RIFT flags
    if (pattern->flags) {
        if (strchr(pattern->flags, 'i')) flags |= REG_ICASE;
        // Note: 'g' (global) and 'm' (multiline) are handled at match time
    }
    
    int result = regcomp(&pattern->compiled_regex, pattern->regex_pattern, flags);
    pattern->is_compiled = (result == 0);
    
    if (result != 0) {
        char error_buffer[256];
        regerror(result, &pattern->compiled_regex, error_buffer, sizeof(error_buffer));
        fprintf(stderr, "RIFT Pattern compilation failed: %s\n", error_buffer);
        return RIFT_ERROR_COMPILATION_FAILED;
    }
    
    return RIFT_SUCCESS;
}

void rift_pattern_destroy(RiftPattern* pattern) {
    if (!pattern) return;
    
    free(pattern->raw_pattern);
    free(pattern->regex_pattern);
    free(pattern->flags);
    
    if (pattern->is_compiled) {
        regfree(&pattern->compiled_regex);
    }
    
    free(pattern);
}
