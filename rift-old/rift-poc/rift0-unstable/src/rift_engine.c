#include "../include/rift0/rift.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

RiftEngine* rift_engine_create(void) {
    RiftEngine* engine = calloc(1, sizeof(RiftEngine));
    if (!engine) return NULL;
    
    engine->automaton = rift_automaton_create();
    engine->token_capacity = 64;
    engine->tokens = calloc(engine->token_capacity, sizeof(RiftToken*));
    
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

bool rift_engine_process_input(RiftEngine* engine, const char* input) {
    if (!engine || !input) return false;
    
    free(engine->current_input);
    engine->current_input = strdup(input);
    engine->input_length = strlen(input);
    engine->input_position = 0;
    
    // Reset automaton state
    engine->automaton->current_state = engine->automaton->initial_state;
    
    return true;
}

RiftToken* rift_engine_next_token(RiftEngine* engine) {
    if (!engine || !engine->current_input || 
        engine->input_position >= engine->input_length) {
        return NULL;
    }
    
    const char* input = engine->current_input + engine->input_position;
    size_t remaining = engine->input_length - engine->input_position;
    
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
        if (!state->pattern || !state->pattern->is_compiled) continue;
        
        regmatch_t match;
        const char* current_pos = engine->current_input + engine->input_position;
        
        if (regexec(&state->pattern->compiled_regex, current_pos, 1, &match, 0) == 0 &&
            match.rm_so == 0) { // Match at beginning
            
            size_t match_len = match.rm_eo - match.rm_so;
            char* value = calloc(match_len + 1, 1);
            strncpy(value, current_pos, match_len);
            
            RiftToken* token = rift_token_create("PATTERN_MATCH", value, 
                                               1, engine->input_position + 1);
            token->matched_state = state;
            
            engine->input_position += match_len;
            free(value);
            
            return token;
        }
    }
    
    // Fallback: single character token
    char single_char[2] = {engine->current_input[engine->input_position], '\0'};
    RiftToken* token = rift_token_create("CHAR", single_char, 
                                       1, engine->input_position + 1);
    engine->input_position++;
    
    return token;
}

RiftToken* rift_token_create(const char* type, const char* value, 
                           size_t line, size_t column) {
    RiftToken* token = calloc(1, sizeof(RiftToken));
    token->type = strdup(type ? type : "UNKNOWN");
    token->value = strdup(value ? value : "");
    token->lexeme = strdup(token->value);
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
