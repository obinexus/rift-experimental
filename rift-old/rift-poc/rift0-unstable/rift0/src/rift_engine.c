#include "../include/rift0/rift.h"
#include <ctype.h>

RiftEngine* rift_engine_create(void) {
    RiftEngine* engine = calloc(1, sizeof(RiftEngine));
    if (!engine) return NULL;
    
    engine->automaton = rift_automaton_create();
    if (!engine->automaton) {
        free(engine);
        return NULL;
    }
    
    engine->token_capacity = 64;
    engine->tokens = calloc(engine->token_capacity, sizeof(RiftToken*));
    if (!engine->tokens) {
        rift_automaton_destroy(engine->automaton);
        free(engine);
        return NULL;
    }
    
    return engine;
}

void rift_engine_destroy(RiftEngine* engine) {
    if (!engine) return;
    
    rift_automaton_destroy(engine->automaton);
    
    for (size_t i = 0; i < engine->token_count; i++) {
        rift_token_destroy(engine->tokens[i]);
    }
    free(engine->tokens);
    
    free(engine->current_input);
    free(engine);
}

RiftResult rift_engine_process_input(RiftEngine* engine, const char* input) {
    if (!engine || !input) return RIFT_ERROR_NULL_POINTER;
    
    free(engine->current_input);
    engine->current_input = strdup(input);
    if (!engine->current_input) return RIFT_ERROR_MEMORY_ALLOCATION;
    
    engine->input_length = strlen(input);
    engine->input_position = 0;
    
    // Reset automaton state
    engine->automaton->current_state = engine->automaton->initial_state;
    
    return RIFT_SUCCESS;
}

RiftToken* rift_engine_next_token(RiftEngine* engine) {
    if (!engine || !engine->current_input || 
        engine->input_position >= engine->input_length) {
        return NULL;
    }
    
    // Skip whitespace (simple tokenizer)
    while (engine->input_position < engine->input_length && 
           isspace(engine->current_input[engine->input_position])) {
        engine->input_position++;
    }
    
    if (engine->input_position >= engine->input_length) {
        return NULL;
    }
    
    // Try to match against automaton states
    for (size_t i = 0; i < engine->automaton->state_count; i++) {
        RiftState* state = engine->automaton->states[i];
        if (!state || !state->pattern || !state->pattern->is_compiled) continue;
        
        regmatch_t match;
        const char* current_pos = engine->current_input + engine->input_position;
        
        if (regexec(&state->pattern->compiled_regex, current_pos, 1, &match, 0) == 0 &&
            match.rm_so == 0) { // Match at beginning
            
            size_t match_len = (size_t)(match.rm_eo - match.rm_so);
            char* value = calloc(match_len + 1, 1);
            if (!value) return NULL;
            
            strncpy(value, current_pos, match_len);
            
            RiftToken* token = rift_token_create("PATTERN_MATCH", value, 
                                               1, engine->input_position + 1);
            if (token) {
                token->matched_state = state;
                engine->input_position += match_len;
            }
            
            free(value);
            return token;
        }
    }
    
    // Fallback: single character token
    char single_char[2] = {engine->current_input[engine->input_position], '\0'};
    RiftToken* token = rift_token_create("CHAR", single_char, 
                                       1, engine->input_position + 1);
    if (token) {
        engine->input_position++;
    }
    
    return token;
}

RiftToken* rift_token_create(const char* type, const char* value, 
                           size_t line, size_t column) {
    RiftToken* token = calloc(1, sizeof(RiftToken));
    if (!token) return NULL;
    
    token->type = strdup(type ? type : "UNKNOWN");
    if (!token->type) {
        free(token);
        return NULL;
    }
    
    token->value = strdup(value ? value : "");
    if (!token->value) {
        free(token->type);
        free(token);
        return NULL;
    }
    
    token->lexeme = strdup(token->value);
    if (!token->lexeme) {
        free(token->type);
        free(token->value);
        free(token);
        return NULL;
    }
    
    token->line = line;
    token->column = column;
    
    return token;
}

void rift_token_destroy(RiftToken* token) {
    if (!token) return;
    
    free(token->type);
    free(token->value);
    free(token->lexeme);
    free(token);
}
