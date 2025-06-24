/**
 * @file pocrift_tokenizer.c
 * @brief POCRIFT tokenizer with USCN normalization
 */

#include "pocrift/pocrift.h"
#include "pocrift/uscn.h"
#include "pocrift/minimizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    RegexAutomaton* automaton;
    USCN_Normalizer* normalizer;
    TokenMinimizer* minimizer;
    bool zero_trust_mode;
} Stage0_Context;

Stage0_Context* stage0_create_context(bool zero_trust_mode) {
    Stage0_Context* ctx = malloc(sizeof(Stage0_Context));
    if (!ctx) return NULL;
    
    ctx->automaton = automaton_create();
    ctx->normalizer = uscn_create();
    ctx->minimizer = token_minimizer_create();
    ctx->zero_trust_mode = zero_trust_mode;
    
    // Add standard token patterns
    automaton_add_state(ctx->automaton, "^[a-zA-Z_][a-zA-Z0-9_]*$", false); // identifiers
    automaton_add_state(ctx->automaton, "^[0-9]+$", false); // numbers
    automaton_add_state(ctx->automaton, "^[+\\-*/=]$", false); // operators
    automaton_add_state(ctx->automaton, "^[{}();]$", false); // delimiters
    automaton_add_state(ctx->automaton, "^\"[^\"]*\"$", false); // strings
    automaton_add_state(ctx->automaton, "^//.*$", false); // comments
    
    return ctx;
}

bool stage0_process_file(Stage0_Context* ctx, const char* input_file, const char* output_file) {
    FILE* input = fopen(input_file, "r");
    if (!input) return false;
    
    FILE* output = fopen(output_file, "w");
    if (!output) {
        fclose(input);
        return false;
    }
    
    fprintf(output, "# RIFT Stage 0 Output - Tokenization with USCN\n");
    fprintf(output, "# Input: %s\n", input_file);
    fprintf(output, "# Zero Trust: %s\n", ctx->zero_trust_mode ? "ENABLED" : "DISABLED");
    fprintf(output, "# USCN Normalization: ENABLED\n\n");
    
    char line[1024];
    int line_number = 1;
    
    while (fgets(line, sizeof(line), input)) {
        // Apply USCN normalization first
        char* normalized = uscn_normalize(ctx->normalizer, line);
        if (!normalized) continue;
        
        // Tokenize normalized input
        char* token = strtok(normalized, " \t\n");
        while (token) {
            State* matched_state = automaton_get_next_state(ctx->automaton, token);
            if (matched_state) {
                // Apply token minimization
                char* minimized = token_minimizer_process(ctx->minimizer, token, matched_state->pattern);
                fprintf(output, "TOKEN_%s: %s (line: %d)\n", 
                       get_token_type_name(matched_state->pattern), 
                       minimized ? minimized : token, 
                       line_number);
                free(minimized);
            }
            token = strtok(NULL, " \t\n");
        }
        
        free(normalized);
        line_number++;
    }
    
    fclose(input);
    fclose(output);
    return true;
}

void stage0_free_context(Stage0_Context* ctx) {
    if (!ctx) return;
    automaton_destroy(ctx->automaton);
    uscn_free(ctx->normalizer);
    token_minimizer_free(ctx->minimizer);
    free(ctx);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input.rift> <output.rift.0> [--zero-trust]\n", argv[0]);
        return 1;
    }
    
    bool zero_trust = (argc > 3 && strcmp(argv[3], "--zero-trust") == 0);
    
    Stage0_Context* ctx = stage0_create_context(zero_trust);
    if (!ctx) {
        fprintf(stderr, "Failed to create Stage 0 context\n");
        return 1;
    }
    
    bool success = stage0_process_file(ctx, argv[1], argv[2]);
    stage0_free_context(ctx);
    
    if (success) {
        printf("Stage 0 tokenization completed: %s\n", argv[2]);
        return 0;
    } else {
        fprintf(stderr, "Stage 0 tokenization failed\n");
        return 1;
    }
}
