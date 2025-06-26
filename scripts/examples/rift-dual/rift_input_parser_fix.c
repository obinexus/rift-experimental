/*
 * RIFT Input Specification Parser - Corrected Implementation
 * OBINexus Computing - AEGIS Framework
 * 
 * Technical Fix: Proper handling of R"/pattern/flags" syntax
 * Issue Resolution: Delimiter recognition for embedded forward slashes
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef enum {
    RIFT_FLAG_GLOBAL = 0x01,          // 'g' - global processing
    RIFT_FLAG_MULTILINE = 0x02,       // 'm' - multiline mode
    RIFT_FLAG_INSENSITIVE = 0x04,     // 'i' - case insensitive
    RIFT_FLAG_BOTTOM_UP = 0x08,       // 'b' - shift-reduce parsing
    RIFT_FLAG_TOP_DOWN = 0x10         // 't' - recursive descent parsing
} RIFTParsingFlags;

typedef struct RIFTInputSpec {
    char* raw_content;                // Extracted from R"" or R''
    RIFTParsingFlags flags;           // Parsed from /gmi[bt]
    bool dual_mode_enabled;           // Both 'b' and 't' present
    char* delimiter_type;             // "R\"" or "R'"
} RIFTInputSpec;

// =====================================
// CORRECTED INPUT PARSING IMPLEMENTATION
// =====================================

bool rift_parse_input_specification(const char* input, RIFTInputSpec* spec) {
    if (!input || !spec) return false;
    
    printf("🔍 PARSING INPUT SPECIFICATION (CORRECTED)\n");
    printf("Input: %s\n", input);
    
    // Initialize spec structure
    memset(spec, 0, sizeof(RIFTInputSpec));
    
    // Parse R"" format with proper delimiter handling
    if (strncmp(input, "R\"", 2) == 0) {
        spec->delimiter_type = strdup("R\"");
        printf("Delimiter Type: %s\n", spec->delimiter_type);
        
        // Find the content start (after R")
        const char* content_start = input + 2;
        
        // Find the closing quote (end of entire expression)
        const char* closing_quote = strrchr(input, '"');
        if (!closing_quote || closing_quote <= content_start) {
            printf("❌ No closing quote found\n");
            return false;
        }
        
        // Now we need to parse the pattern: /content/flags
        if (*content_start != '/') {
            printf("❌ Expected pattern to start with '/'\n");
            return false;
        }
        
        // Skip the initial '/'
        content_start++;
        
        // Find the closing '/' before flags
        const char* content_end = content_start;
        const char* flags_start = NULL;
        
        // Search for the last '/' before the closing quote
        // This handles cases where the content itself contains '/'
        for (const char* p = closing_quote - 1; p > content_start; p--) {
            if (*p == '/') {
                content_end = p;
                flags_start = p + 1;
                break;
            }
        }
        
        if (!flags_start) {
            printf("❌ No flags delimiter '/' found\n");
            return false;
        }
        
        // Extract content between the first '/' and the last '/'
        size_t content_len = content_end - content_start;
        spec->raw_content = strndup(content_start, content_len);
        printf("Extracted Content: '%s'\n", spec->raw_content);
        
        // Parse flags between the last '/' and the closing '"'
        spec->flags = 0;
        printf("Parsing flags: '");
        
        for (const char* p = flags_start; p < closing_quote; p++) {
            printf("%c", *p);
            switch (*p) {
                case 'g': 
                    spec->flags |= RIFT_FLAG_GLOBAL; 
                    break;
                case 'm': 
                    spec->flags |= RIFT_FLAG_MULTILINE; 
                    break;
                case 'i': 
                    spec->flags |= RIFT_FLAG_INSENSITIVE; 
                    break;
                case '[': 
                    // Parse [bt] section
                    p++; // Skip '['
                    while (p < closing_quote && *p != ']') {
                        if (*p == 'b') {
                            spec->flags |= RIFT_FLAG_BOTTOM_UP;
                            printf("b");
                        }
                        if (*p == 't') {
                            spec->flags |= RIFT_FLAG_TOP_DOWN;
                            printf("t");
                        }
                        p++;
                    }
                    break;
            }
        }
        printf("'\n");
        
        spec->dual_mode_enabled = (spec->flags & RIFT_FLAG_BOTTOM_UP) && 
                                (spec->flags & RIFT_FLAG_TOP_DOWN);
        
        printf("Parsing Flags Analysis:\n");
        printf("  Global: %s\n", (spec->flags & RIFT_FLAG_GLOBAL) ? "YES" : "NO");
        printf("  Multiline: %s\n", (spec->flags & RIFT_FLAG_MULTILINE) ? "YES" : "NO");
        printf("  Case Insensitive: %s\n", (spec->flags & RIFT_FLAG_INSENSITIVE) ? "YES" : "NO");
        printf("  Bottom-Up Parsing: %s\n", (spec->flags & RIFT_FLAG_BOTTOM_UP) ? "YES" : "NO");
        printf("  Top-Down Parsing: %s\n", (spec->flags & RIFT_FLAG_TOP_DOWN) ? "YES" : "NO");
        printf("  Dual Mode: %s\n", spec->dual_mode_enabled ? "ENABLED" : "DISABLED");
        
        return true;
    }
    
    // Handle R'' format (alternative delimiter)
    else if (strncmp(input, "R'", 2) == 0) {
        spec->delimiter_type = strdup("R'");
        printf("Delimiter Type: %s\n", spec->delimiter_type);
        
        // Similar parsing logic for single quotes
        const char* content_start = input + 2;
        const char* closing_quote = strrchr(input, '\'');
        
        if (!closing_quote || closing_quote <= content_start) {
            printf("❌ No closing single quote found\n");
            return false;
        }
        
        // Parse /content/flags pattern within single quotes
        if (*content_start != '/') {
            printf("❌ Expected pattern to start with '/'\n");
            return false;
        }
        
        content_start++;
        const char* content_end = content_start;
        const char* flags_start = NULL;
        
        for (const char* p = closing_quote - 1; p > content_start; p--) {
            if (*p == '/') {
                content_end = p;
                flags_start = p + 1;
                break;
            }
        }
        
        if (!flags_start) {
            printf("❌ No flags delimiter '/' found\n");
            return false;
        }
        
        size_t content_len = content_end - content_start;
        spec->raw_content = strndup(content_start, content_len);
        printf("Extracted Content: '%s'\n", spec->raw_content);
        
        // Flag parsing logic (same as above)
        spec->flags = 0;
        for (const char* p = flags_start; p < closing_quote; p++) {
            switch (*p) {
                case 'g': spec->flags |= RIFT_FLAG_GLOBAL; break;
                case 'm': spec->flags |= RIFT_FLAG_MULTILINE; break;
                case 'i': spec->flags |= RIFT_FLAG_INSENSITIVE; break;
                case '[': 
                    p++;
                    while (p < closing_quote && *p != ']') {
                        if (*p == 'b') spec->flags |= RIFT_FLAG_BOTTOM_UP;
                        if (*p == 't') spec->flags |= RIFT_FLAG_TOP_DOWN;
                        p++;
                    }
                    break;
            }
        }
        
        spec->dual_mode_enabled = (spec->flags & RIFT_FLAG_BOTTOM_UP) && 
                                (spec->flags & RIFT_FLAG_TOP_DOWN);
        
        return true;
    }
    
    printf("❌ Input does not match expected R\"\" or R'' format\n");
    return false;
}

// =====================================
// TESTING AND VALIDATION FRAMEWORK
// =====================================

void test_input_parser(void) {
    printf("\n🧪 INPUT PARSER TESTING FRAMEWORK\n");
    printf("==================================\n");
    
    // Test cases for validation
    const char* test_inputs[] = {
        "R\"/let result = (x + y) * 42;/gmi[bt]\"",
        "R\"/simple_pattern/g\"",
        "R'/alternative_delimiter/mi[t]'",
        "R\"/complex/pattern/with/slashes/gmi[bt]\"",
        "R\"/pattern_without_flags/\""
    };
    
    size_t test_count = sizeof(test_inputs) / sizeof(test_inputs[0]);
    
    for (size_t i = 0; i < test_count; i++) {
        printf("\n--- Test Case %zu ---\n", i + 1);
        
        RIFTInputSpec spec;
        bool result = rift_parse_input_specification(test_inputs[i], &spec);
        
        printf("Result: %s\n", result ? "✅ SUCCESS" : "❌ FAILED");
        
        if (result) {
            printf("Content: '%s'\n", spec.raw_content);
            printf("Dual Mode: %s\n", spec.dual_mode_enabled ? "ENABLED" : "DISABLED");
            
            // Cleanup
            free(spec.raw_content);
            free(spec.delimiter_type);
        }
        
        printf("----------------------------------------\n");
    }
}

// =====================================
// MAIN EXECUTION FOR VALIDATION
// =====================================

int main() {
    printf("🎯 RIFT INPUT PARSER CORRECTION VALIDATION\n");
    printf("===========================================\n");
    printf("OBINexus Computing - AEGIS Framework\n");
    printf("Technical Issue Resolution: Input Specification Parsing\n\n");
    
    // Test the corrected parser with the original failing input
    const char* original_input = "R\"/let result = (x + y) * 42;/gmi[bt]\"";
    RIFTInputSpec spec;
    
    printf("🔧 TESTING ORIGINAL FAILING INPUT\n");
    printf("=================================\n");
    
    bool result = rift_parse_input_specification(original_input, &spec);
    
    if (result) {
        printf("\n✅ PARSING SUCCESS!\n");
        printf("==================\n");
        printf("Raw Content: '%s'\n", spec.raw_content);
        printf("Delimiter Type: %s\n", spec.delimiter_type);
        printf("Dual Mode Enabled: %s\n", spec.dual_mode_enabled ? "YES" : "NO");
        printf("Flags: 0x%02X\n", spec.flags);
        
        // Cleanup
        free(spec.raw_content);
        free(spec.delimiter_type);
    } else {
        printf("\n❌ PARSING STILL FAILING\n");
        printf("========================\n");
        printf("Additional debugging required.\n");
    }
    
    // Run comprehensive test suite
    test_input_parser();
    
    printf("\n🎯 CORRECTED PARSER VALIDATION COMPLETE\n");
    printf("======================================\n");
    printf("Technical Resolution Status: %s\n", result ? "RESOLVED" : "REQUIRES FURTHER INVESTIGATION");
    
    return result ? 0 : 1;
}
