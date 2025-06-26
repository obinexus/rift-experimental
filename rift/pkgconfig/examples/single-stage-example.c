/*
 * RIFT Single Stage Usage Example
 * Demonstrates linking against rift-0 (tokenizer) using pkg-config
 * 
 * Compile with:
 * gcc -o tokenizer-example single-stage-example.c $(pkg-config --cflags --libs rift-0)
 */

#include <stdio.h>
#include <stdlib.h>

// RIFT Stage 0 headers (resolved via pkg-config)
#ifdef RIFT_STAGE_0_ENABLED
#include "tokenizer.h"
#endif

int main(int argc, char* argv[]) {
    printf("RIFT Single Stage Usage Example\n");
    printf("================================\n");
    
    if (argc < 2) {
        printf("Usage: %s <input-text>\n", argv[0]);
        return 1;
    }
    
    const char* input = argv[1];
    printf("Input: %s\n", input);
    
#ifdef RIFT_STAGE_0_ENABLED
    // Use RIFT tokenizer functionality
    char* tokenized_output = NULL;
    RiftResult result = rift_stage0_process(input, &tokenized_output);
    
    if (result == RIFT_SUCCESS && tokenized_output) {
        printf("Tokenized Output: %s\n", tokenized_output);
        free(tokenized_output);
    } else {
        printf("Tokenization failed\n");
        return 1;
    }
#else
    printf("RIFT Stage 0 not available\n");
#endif
    
    return 0;
}
