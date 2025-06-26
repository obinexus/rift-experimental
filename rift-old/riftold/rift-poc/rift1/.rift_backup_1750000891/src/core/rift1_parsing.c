// src/core/rift1_parsing.c - Core Implementation
#include "../../include/rift1/rift.h"
#include <stdlib.h>
#include <string.h>

// AEGIS Pattern-Based Parser Implementation
typedef struct Rift1ParseEngine {
    RiftToken** input_tokens;
    size_t token_count;
    size_t current_position;
    
    // AEGIS Automaton Components
    RiftAutomaton* syntax_automaton;
    RiftPattern** grammar_patterns;
    size_t pattern_count;
    
    // AST Generation
    RiftASTNode* parse_tree;
    bool parsing_complete;
} Rift1ParseEngine;

// Stage 1 Core Functionality
Rift1ParseEngine* rift1_engine_create(void) {
    Rift1ParseEngine* engine = calloc(1, sizeof(Rift1ParseEngine));
    if (!engine) return NULL;
    
    // Initialize AEGIS automaton for syntax recognition
    engine->syntax_automaton = rift_automaton_create();
    engine->parsing_complete = false;
    
    return engine;
}

void rift1_engine_destroy(Rift1ParseEngine* engine) {
    if (!engine) return;
    
    // Cleanup AEGIS components
    rift_automaton_destroy(engine->syntax_automaton);
    if (engine->grammar_patterns) {
        for (size_t i = 0; i < engine->pattern_count; i++) {
            rift_pattern_destroy(engine->grammar_patterns[i]);
        }
        free(engine->grammar_patterns);
    }
    
    free(engine);
}

// Main Stage 1 Processing Function
RiftResult rift1_process(Rift1ParseEngine* engine, 
                        const char* input_file, 
                        const char* output_file) {
    if (!engine || !input_file || !output_file) {
        return RIFT_ERROR_NULL_POINTER;
    }
    
    // Stage 1: Read .rift.0 tokens from Stage 0
    RiftIR* input_ir = rift_ir_create(input_file);
    RiftResult read_result = rift_ir_read_file(input_ir, input_file);
    if (read_result != RIFT_SUCCESS) {
        rift_ir_destroy(input_ir);
        return read_result;
    }
    
    // Stage 2: Apply AEGIS pattern matching for syntax recognition
    engine->input_tokens = input_ir->tokens;
    engine->token_count = input_ir->token_count;
    
    // Stage 3: Generate AST through pattern-based composition
    RiftResult parse_result = rift1_parse_tokens(engine);
    if (parse_result != RIFT_SUCCESS) {
        rift_ir_destroy(input_ir);
        return parse_result;
    }
    
    // Stage 4: Generate .rift.1 output
    RiftIR* output_ir = rift_ir_create(input_file);
    output_ir->stage = strdup("rift.1");
    // Transfer AST nodes to IR tokens for Stage 3 consumption
    
    RiftResult write_result = rift_ir_write_file(output_ir, output_file);
    
    rift_ir_destroy(input_ir);
    rift_ir_destroy(output_ir);
    
    return write_result;
}