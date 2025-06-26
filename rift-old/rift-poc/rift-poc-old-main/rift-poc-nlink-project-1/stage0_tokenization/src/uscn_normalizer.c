/**
 * @file uscn_normalizer.c
 * @brief Unicode-Only Structural Charset Normalizer implementation
 */

#include "pocrift/uscn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct USCN_Normalizer {
    bool strict_mode;
    char* canonical_table[256];
};

USCN_Normalizer* uscn_create(void) {
    USCN_Normalizer* normalizer = malloc(sizeof(USCN_Normalizer));
    if (!normalizer) return NULL;
    
    normalizer->strict_mode = true;
    
    // Initialize canonical mapping table
    for (int i = 0; i < 256; i++) {
        normalizer->canonical_table[i] = NULL;
    }
    
    // Common Unicode normalizations
    normalizer->canonical_table['%'] = strdup("PERCENT_ENCODE");
    
    return normalizer;
}

char* uscn_normalize(USCN_Normalizer* normalizer, const char* input) {
    if (!normalizer || !input) return NULL;
    
    size_t input_len = strlen(input);
    size_t output_len = input_len * 2; // Conservative estimate
    char* output = malloc(output_len);
    if (!output) return NULL;
    
    size_t out_pos = 0;
    for (size_t i = 0; i < input_len; i++) {
        unsigned char c = (unsigned char)input[i];
        
        // Handle percent encoding normalization
        if (c == '%' && i + 2 < input_len && 
            isxdigit(input[i+1]) && isxdigit(input[i+2])) {
            
            // Convert %XX to actual character
            char hex_str[3] = {input[i+1], input[i+2], '\0'};
            unsigned char decoded = (unsigned char)strtol(hex_str, NULL, 16);
            
            // Apply canonical normalization
            if (decoded == 0x2e) { // %2e -> '.'
                output[out_pos++] = '.';
            } else if (decoded == 0x2f) { // %2f -> '/'
                output[out_pos++] = '/';
            } else {
                output[out_pos++] = decoded;
            }
            i += 2; // Skip the hex digits
        } else {
            // Direct character copy
            output[out_pos++] = c;
        }
        
        // Ensure we don't overflow
        if (out_pos >= output_len - 1) {
            output_len *= 2;
            output = realloc(output, output_len);
            if (!output) return NULL;
        }
    }
    
    output[out_pos] = '\0';
    return output;
}

void uscn_free(USCN_Normalizer* normalizer) {
    if (!normalizer) return;
    
    for (int i = 0; i < 256; i++) {
        free(normalizer->canonical_table[i]);
    }
    free(normalizer);
}
