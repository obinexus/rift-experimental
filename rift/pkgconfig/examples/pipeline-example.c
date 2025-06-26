/*
 * RIFT Pipeline Usage Example
 * Demonstrates full pipeline using all stages via pkg-config
 * 
 * Compile with:
 * gcc -o pipeline-example pipeline-example.c $(pkg-config --cflags --libs rift-0 rift-1 rift-2 rift-3 rift-4 rift-5 rift-6)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include headers for all stages (resolved via pkg-config)
#ifdef RIFT_STAGE_0_ENABLED
#include "tokenizer.h"
#endif
#ifdef RIFT_STAGE_1_ENABLED  
#include "parser.h"
#endif
// ... additional stage includes as needed

int main(int argc, char* argv[]) {
    printf("RIFT Full Pipeline Example\n");
    printf("==========================\n");
    
    if (argc < 2) {
        printf("Usage: %s <input-text>\n", argv[0]);
        return 1;
    }
    
    const char* input = argv[1];
    char* stage_output = strdup(input);
    
    printf("Processing through RIFT pipeline...\n");
    printf("Input: %s\n", input);
    
    // Stage 0: Tokenizer
#ifdef RIFT_STAGE_0_ENABLED
    char* tokenized = NULL;
    if (rift_stage0_process(stage_output, &tokenized) == RIFT_SUCCESS) {
        printf("Stage 0 (Tokenizer): %s\n", tokenized);
        free(stage_output);
        stage_output = tokenized;
    }
#endif
    
    // Stage 1: Parser  
#ifdef RIFT_STAGE_1_ENABLED
    char* parsed = NULL;
    if (rift_stage1_process(stage_output, &parsed) == RIFT_SUCCESS) {
        printf("Stage 1 (Parser): %s\n", parsed);
        free(stage_output);
        stage_output = parsed;
    }
#endif
    
    // Continue with remaining stages...
    
    printf("Final Output: %s\n", stage_output);
    free(stage_output);
    
    return 0;
}
