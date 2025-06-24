// cli/command/pattern_manager.c
#include "cli/command/pattern_manager.h"

RiftPatternSyntax* rift_parse_rift_syntax(const char* rift_pattern) {
    if (!rift_pattern) return NULL;
    
    RiftPatternSyntax* syntax = calloc(1, sizeof(RiftPatternSyntax));
    if (!syntax) return NULL;
    
    syntax->raw_pattern = strdup(rift_pattern);
    
    // Parse RIFT syntax: R"pattern/flags[mode]" or just "pattern/flags[mode]"
    const char* start = rift_pattern;
    
    // Skip R" if present
    if (strncmp(start, "R\"", 2) == 0) {
        start += 2;
    } else if (*start == '"') {
        start += 1;
    }
    
    // Find the pattern end (first '/')
    const char* pattern_end = strchr(start, '/');
    if (!pattern_end) {
        // No flags, look for [mode] or end quote
        pattern_end = strchr(start, '[');
        if (!pattern_end) {
            pattern_end = strchr(start, '"');
            if (!pattern_end) {
                pattern_end = start + strlen(start);
            }
        }
        syntax->regex_pattern = strndup(start, pattern_end - start);
        syntax->flags = strdup("");
    } else {
        // Extract pattern part
        syntax->regex_pattern = strndup(start, pattern_end - start);
        
        // Extract flags part
        const char* flags_start = pattern_end + 1;
        const char* flags_end = strchr(flags_start, '[');
        if (!flags_end) {
            flags_end = strchr(flags_start, '"');
            if (!flags_end) {
                flags_end = flags_start + strlen(flags_start);
            }
        }
        syntax->flags = strndup(flags_start, flags_end - flags_start);
    }
    
    // Extract mode from [mode]
    const char* mode_start = strchr(rift_pattern, '[');
    if (mode_start && mode_start[1] && mode_start[2] == ']') {
        syntax->mode = mode_start[1];
    } else {
        syntax->mode = 't'; // Default to top-down
    }
    
    syntax->is_valid = (syntax->regex_pattern && strlen(syntax->regex_pattern) > 0);
    
    return syntax;
}

void rift_pattern_syntax_destroy(RiftPatternSyntax* syntax) {
    if (!syntax) return;
    
    free(syntax->raw_pattern);
    free(syntax->regex_pattern);
    free(syntax->flags);
    free(syntax);
}