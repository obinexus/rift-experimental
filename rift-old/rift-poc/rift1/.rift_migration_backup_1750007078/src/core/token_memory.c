#include "rift1/core/rift.h"
// src/core/tokenmemory.c - TokenMemory Bridge Implementation
// OBINexus AEGIS Framework - Stage 0 → Stage 1 Token Processing
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// ===== TOKENMEMORY: STAGE 0 → STAGE 1 BRIDGE =====

RiftTokenMemory* rift_tokenmemory_create(void) {
    RiftTokenMemory* memory = calloc(1, sizeof(RiftTokenMemory));
    if (!memory) return NULL;
    
    memory->total_capacity = 1024;  // Initial capacity
    memory->is_optimized = false;
    
    // Allocate validation arrays
    memory->validation_results.type_validation = calloc(memory->total_capacity, sizeof(bool));
    memory->validation_results.value_validation = calloc(memory->total_capacity, sizeof(bool));
    memory->validation_results.sequence_validation = calloc(memory->total_capacity, sizeof(bool));
    memory->validation_results.error_messages = calloc(memory->total_capacity, sizeof(char*));
    
    if (!memory->validation_results.type_validation || 
        !memory->validation_results.value_validation ||
        !memory->validation_results.sequence_validation ||
        !memory->validation_results.error_messages) {
        rift_tokenmemory_destroy(memory);
        return NULL;
    }
    
    return memory;
}

void rift_tokenmemory_destroy(RiftTokenMemory* memory) {
    if (!memory) return;
    
    // Cleanup input tokens
    if (memory->input_tokens) {
        for (size_t i = 0; i < memory->input_count; i++) {
            rift_token_destroy(memory->input_tokens[i]);
        }
        free(memory->input_tokens);
    }
    
    // Cleanup validated tokens
    if (memory->validated_tokens) {
        for (size_t i = 0; i < memory->validated_count; i++) {
            rift_token_destroy(memory->validated_tokens[i]);
        }
        free(memory->validated_tokens);
    }
    
    // Cleanup enhanced tokens
    if (memory->enhanced_tokens) {
        for (size_t i = 0; i < memory->enhanced_count; i++) {
            rift_token_destroy(memory->enhanced_tokens[i]);
        }
        free(memory->enhanced_tokens);
    }
    
    // Cleanup validation results
    free(memory->validation_results.type_validation);
    free(memory->validation_results.value_validation);
    free(memory->validation_results.sequence_validation);
    
    if (memory->validation_results.error_messages) {
        for (size_t i = 0; i < memory->total_capacity; i++) {
            free(memory->validation_results.error_messages[i]);
        }
        free(memory->validation_results.error_messages);
    }
    
    free(memory);
}

RiftResult rift_tokenmemory_load_stage0(RiftTokenMemory* memory, const char* rift0_file) {
    if (!memory || !rift0_file) return RIFT_ERROR_NULL_POINTER;
    
    // Read .rift.0 file using existing IR infrastructure
    RiftIR* input_ir = rift_ir_create(rift0_file);
    if (!input_ir) return RIFT_ERROR_MEMORY_ALLOCATION;
    
    RiftResult read_result = rift_ir_read_file(input_ir, rift0_file);
    if (read_result != RIFT_SUCCESS) {
        rift_ir_destroy(input_ir);
        return read_result;
    }
    
    // Transfer tokens to tokenmemory
    memory->input_count = input_ir->token_count;
    memory->input_tokens = malloc(memory->input_count * sizeof(RiftToken*));
    if (!memory->input_tokens) {
        rift_ir_destroy(input_ir);
        return RIFT_ERROR_MEMORY_ALLOCATION;
    }
    
    // Deep copy tokens (preserve Stage 0 type/value separation)
    for (size_t i = 0; i < memory->input_count; i++) {
        RiftToken* original = input_ir->tokens[i];
        memory->input_tokens[i] = rift_token_create(
            original->type, 
            original->value, 
            original->line, 
            original->column
        );
        
        if (!memory->input_tokens[i]) {
            // Cleanup on failure
            for (size_t j = 0; j < i; j++) {
                rift_token_destroy(memory->input_tokens[j]);
            }
            free(memory->input_tokens);
            memory->input_tokens = NULL;
            memory->input_count = 0;
            rift_ir_destroy(input_ir);
            return RIFT_ERROR_MEMORY_ALLOCATION;
        }
        
        // Preserve AEGIS matched_state for AST minimization
        memory->input_tokens[i]->matched_state = original->matched_state;
    }
    
    rift_ir_destroy(input_ir);
    return RIFT_SUCCESS;
}

RiftResult rift_tokenmemory_validate_tokens(RiftTokenMemory* memory) {
    if (!memory || !memory->input_tokens) return RIFT_ERROR_NULL_POINTER;
    
    bool validation_passed = true;
    
    for (size_t i = 0; i < memory->input_count; i++) {
        RiftToken* token = memory->input_tokens[i];
        
        // Type validation: Ensure type is not null/empty
        memory->validation_results.type_validation[i] = 
            (token->type != NULL && strlen(token->type) > 0);
        
        // Value validation: Ensure value is not null (empty allowed)
        memory->validation_results.value_validation[i] = 
            (token->value != NULL);
        
        // Sequence validation: Basic syntax validation
        memory->validation_results.sequence_validation[i] = true;  // TODO: Implement grammar rules
        
        // Generate error messages for failed validations
        if (!memory->validation_results.type_validation[i] ||
            !memory->validation_results.value_validation[i] ||
            !memory->validation_results.sequence_validation[i]) {
            
            validation_passed = false;
            
            char error_buffer[512];
            snprintf(error_buffer, sizeof(error_buffer), 
                    "Token %zu validation failed: type=%s, value=%s, line=%zu, col=%zu",
                    i, 
                    token->type ? token->type : "(null)",
                    token->value ? token->value : "(null)",
                    token->line,
                    token->column);
            
            memory->validation_results.error_messages[i] = strdup(error_buffer);
        }
    }
    
    return validation_passed ? RIFT_SUCCESS : RIFT_ERROR_TOKEN_VALIDATION;
}

RiftResult rift_tokenmemory_enhance_tokens(RiftTokenMemory* memory) {
    if (!memory || !memory->input_tokens) return RIFT_ERROR_NULL_POINTER;
    
    // Allocate enhanced token array
    memory->enhanced_count = memory->input_count;
    memory->enhanced_tokens = malloc(memory->enhanced_count * sizeof(RiftToken*));
    if (!memory->enhanced_tokens) return RIFT_ERROR_MEMORY_ALLOCATION;
    
    // Create enhanced tokens with semantic data
    for (size_t i = 0; i < memory->input_count; i++) {
        RiftToken* original = memory->input_tokens[i];
        
        // Create enhanced token
        memory->enhanced_tokens[i] = rift_token_create(
            original->type, 
            original->value, 
            original->line, 
            original->column
        );
        
        if (!memory->enhanced_tokens[i]) {
            // Cleanup on failure
            for (size_t j = 0; j < i; j++) {
                rift_token_destroy(memory->enhanced_tokens[j]);
            }
            free(memory->enhanced_tokens);
            memory->enhanced_tokens = NULL;
            memory->enhanced_count = 0;
            return RIFT_ERROR_MEMORY_ALLOCATION;
        }
        
        // Copy enhanced properties
        memory->enhanced_tokens[i]->matched_state = original->matched_state;
        memory->enhanced_tokens[i]->position = original->position;
        memory->enhanced_tokens[i]->length = original->length;
        
        // Add semantic enhancements based on token type
        if (strcmp(original->type, "IDENTIFIER") == 0) {
            memory->enhanced_tokens[i]->flags |= 0x01;  // Mark as identifier
        } else if (strcmp(original->type, "KEYWORD") == 0) {
            memory->enhanced_tokens[i]->flags |= 0x02;  // Mark as keyword
        } else if (strcmp(original->type, "LITERAL") == 0) {
            memory->enhanced_tokens[i]->flags |= 0x04;  // Mark as literal
        }
        
        // Set processing mode (classical by default, quantum if specified)
        memory->enhanced_tokens[i]->processing_mode = RIFT_MODE_CLASSICAL;
        if (strstr(original->value, "quantum:") != NULL) {
            memory->enhanced_tokens[i]->processing_mode = RIFT_MODE_QUANTUM;
        }
    }
    
    memory->is_optimized = true;
    return RIFT_SUCCESS;
}

// ===== AST NODE IMPLEMENTATION =====

RiftASTNode* rift_ast_node_create(RiftASTNodeType type, const char* value) {
    RiftASTNode* node = calloc(1, sizeof(struct RiftASTNode));
    if (!node) return NULL;
    
    node->type = type;
    if (value) {
        node->value = strdup(value);
        if (!node->value) {
            free(node);
            return NULL;
        }
    }
    
    node->child_capacity = 8;  // Initial capacity
    node->children = calloc(node->child_capacity, sizeof(RiftASTNode*));
    if (!node->children) {
        free(node->value);
        free(node);
        return NULL;
    }
    
    // Initialize classical mode by default
    node->mode_data.classical.is_resolved = false;
    node->optimization_weight = 1.0;
    
    return node;
}

void rift_ast_node_destroy(RiftASTNode* node) {
    if (!node) return;
    
    // Recursively destroy children
    for (size_t i = 0; i < node->child_count; i++) {
        rift_ast_node_destroy(node->children[i]);
    }
    
    free(node->children);
    free(node->value);
    
    // Cleanup quantum mode data if applicable
    if (node->type == AST_NODE_QUANTUM_SUPERPOSITION && 
        node->mode_data.quantum.superposition_states) {
        free(node->mode_data.quantum.superposition_states);
        free(node->mode_data.quantum.probabilities);
        free(node->mode_data.quantum.entangled_nodes);
    }
    
    free(node);
}

RiftResult rift_ast_node_add_child(RiftASTNode* parent, RiftASTNode* child) {
    if (!parent || !child) return RIFT_ERROR_NULL_POINTER;
    
    // Resize children array if needed
    if (parent->child_count >= parent->child_capacity) {
        size_t new_capacity = parent->child_capacity * 2;
        RiftASTNode** new_children = realloc(parent->children, 
                                           new_capacity * sizeof(RiftASTNode*));
        if (!new_children) return RIFT_ERROR_MEMORY_ALLOCATION;
        
        parent->children = new_children;
        parent->child_capacity = new_capacity;
    }
    
    parent->children[parent->child_count] = child;
    child->parent = parent;
    parent->child_count++;
    
    return RIFT_SUCCESS;
}

RiftResult rift_ast_optimize(RiftASTNode* root) {
    if (!root) return RIFT_ERROR_NULL_POINTER;
    
    // AEGIS State Machine Minimization: Apply optimization techniques
    // 1. Node Reduction: Eliminate unnecessary nodes
    // 2. Path Optimization: Minimize state checks
    // 3. Memory Efficiency: Reduce allocation overhead
    
    // Recursively optimize children first
    for (size_t i = 0; i < root->child_count; i++) {
        RiftResult result = rift_ast_optimize(root->children[i]);
        if (result != RIFT_SUCCESS) return result;
    }
    
    // Apply local optimizations
    if (root->child_count == 1 && root->type == AST_NODE_BLOCK) {
        // Single-child block can be flattened
        root->optimization_weight *= 0.5;
    }
    
    return RIFT_SUCCESS;
}

// ===== RIFT1 PARSE ENGINE IMPLEMENTATION =====

Rift1ParseEngine* rift1_engine_create(void) {
    Rift1ParseEngine* engine = calloc(1, sizeof(Rift1ParseEngine));
    if (!engine) return NULL;
    
    engine->syntax_automaton = rift_automaton_create();
    if (!engine->syntax_automaton) {
        free(engine);
        return NULL;
    }
    
    engine->parsing_complete = false;
    engine->enable_state_minimization = true;   // AEGIS optimization enabled
    engine->enable_ast_optimization = true;
    
    return engine;
}

void rift1_engine_destroy(Rift1ParseEngine* engine) {
    if (!engine) return;
    
    rift_automaton_destroy(engine->syntax_automaton);
    
    if (engine->grammar_patterns) {
        for (size_t i = 0; i < engine->pattern_count; i++) {
            rift_pattern_destroy(engine->grammar_patterns[i]);
        }
        free(engine->grammar_patterns);
    }
    
    rift_ast_node_destroy(engine->parse_tree);
    
    if (engine->parse_errors) {
        for (size_t i = 0; i < engine->error_count; i++) {
            free(engine->parse_errors[i]);
        }
        free(engine->parse_errors);
    }
    
    free(engine);
}

RiftResult rift1_engine_load_tokenmemory(Rift1ParseEngine* engine, RiftTokenMemory* memory) {
    if (!engine || !memory) return RIFT_ERROR_NULL_POINTER;
    
    engine->token_memory = memory;
    return RIFT_SUCCESS;
}

RiftResult rift1_parse_tokens(Rift1ParseEngine* engine) {
    if (!engine || !engine->token_memory) return RIFT_ERROR_NULL_POINTER;
    
    // Ensure tokens are enhanced
    if (!engine->token_memory->enhanced_tokens) {
        RiftResult enhance_result = rift_tokenmemory_enhance_tokens(engine->token_memory);
        if (enhance_result != RIFT_SUCCESS) return enhance_result;
    }
    
    // Create root AST node
    engine->parse_tree = rift_ast_node_create(AST_NODE_PROGRAM, "program");
    if (!engine->parse_tree) return RIFT_ERROR_MEMORY_ALLOCATION;
    
    // Simple recursive descent parser for demonstration
    // TODO: Implement full AEGIS pattern-based parsing
    for (size_t i = 0; i < engine->token_memory->enhanced_count; i++) {
        RiftToken* token = engine->token_memory->enhanced_tokens[i];
        
        RiftASTNode* token_node;
        if (strcmp(token->type, "IDENTIFIER") == 0) {
            token_node = rift_ast_node_create(AST_NODE_IDENTIFIER, token->value);
        } else if (strcmp(token->type, "LITERAL") == 0) {
            token_node = rift_ast_node_create(AST_NODE_LITERAL, token->value);
        } else {
            token_node = rift_ast_node_create(AST_NODE_EXPRESSION, token->value);
        }
        
        if (!token_node) return RIFT_ERROR_MEMORY_ALLOCATION;
        
        // Link AST node to source token
        token_node->source_token = token;
        
        // Handle quantum mode tokens
        if (token->processing_mode == RIFT_MODE_QUANTUM) {
            token_node->type = AST_NODE_QUANTUM_SUPERPOSITION;
            // TODO: Initialize quantum superposition data
        }
        
        RiftResult add_result = rift_ast_node_add_child(engine->parse_tree, token_node);
        if (add_result != RIFT_SUCCESS) {
            rift_ast_node_destroy(token_node);
            return add_result;
        }
    }
    
    engine->parsing_complete = true;
    return RIFT_SUCCESS;
}

RiftResult rift1_generate_ast(Rift1ParseEngine* engine) {
    if (!engine || !engine->parsing_complete) return RIFT_ERROR_PARSE_FAILED;
    
    // Apply AEGIS optimizations if enabled
    if (engine->enable_ast_optimization) {
        return rift_ast_optimize(engine->parse_tree);
    }
    
    return RIFT_SUCCESS;
}

// ===== STAGE 0 → STAGE 1 PROCESSING =====

RiftResult rift1_process_stage0_to_stage1(const char* rift0_file, const char* rift1_file, RiftConfig* config) {
    if (!rift0_file || !rift1_file) return RIFT_ERROR_NULL_POINTER;
    
    // Create processing pipeline
    RiftTokenMemory* memory = rift_tokenmemory_create();
    if (!memory) return RIFT_ERROR_MEMORY_ALLOCATION;
    
    Rift1ParseEngine* engine = rift1_engine_create();
    if (!engine) {
        rift_tokenmemory_destroy(memory);
        return RIFT_ERROR_MEMORY_ALLOCATION;
    }
    
    RiftResult result = RIFT_SUCCESS;
    
    // Stage 1: Load .rift.0 tokens
    result = rift_tokenmemory_load_stage0(memory, rift0_file);
    if (result != RIFT_SUCCESS) goto cleanup;
    
    // Stage 2: Validate tokens (compile-time safety)
    result = rift_tokenmemory_validate_tokens(memory);
    if (result != RIFT_SUCCESS) goto cleanup;
    
    // Stage 3: Enhance tokens for parsing
    result = rift_tokenmemory_enhance_tokens(memory);
    if (result != RIFT_SUCCESS) goto cleanup;
    
    // Stage 4: Load into parse engine
    result = rift1_engine_load_tokenmemory(engine, memory);
    if (result != RIFT_SUCCESS) goto cleanup;
    
    // Stage 5: Parse tokens → AST
    result = rift1_parse_tokens(engine);
    if (result != RIFT_SUCCESS) goto cleanup;
    
    // Stage 6: Generate optimized AST
    result = rift1_generate_ast(engine);
    if (result != RIFT_SUCCESS) goto cleanup;
    
    // Stage 7: Write .rift.1 output
    RiftIR* output_ir = rift_ir_create(rift0_file);
    if (!output_ir) {
        result = RIFT_ERROR_MEMORY_ALLOCATION;
        goto cleanup;
    }
    
    free(output_ir->stage);
    output_ir->stage = strdup("rift.1");
    output_ir->token_memory = memory;
    output_ir->ast_root = engine->parse_tree;
    
    result = rift_ir_write_file(output_ir, rift1_file);
    
    // Don't destroy AST/memory as they're now owned by output_ir
    output_ir->token_memory = NULL;
    output_ir->ast_root = NULL;
    rift_ir_destroy(output_ir);
    
cleanup:
    rift1_engine_destroy(engine);
    rift_tokenmemory_destroy(memory);
    
    return result;
}