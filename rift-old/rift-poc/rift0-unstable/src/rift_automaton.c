#include "../include/rift0/rift.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

RiftAutomaton* rift_automaton_create(void) {
    RiftAutomaton* automaton = calloc(1, sizeof(RiftAutomaton));
    if (!automaton) return NULL;
    
    automaton->state_capacity = 16;
    automaton->states = calloc(automaton->state_capacity, sizeof(RiftState*));
    
    automaton->transition_capacity = 32;
    automaton->transitions = calloc(automaton->transition_capacity, sizeof(RiftTransition*));
    
    automaton->final_states = calloc(16, sizeof(RiftState*));
    automaton->input_alphabet = strdup("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789._-+*/=<>!&|(){}[];,");
    
    return automaton;
}

void rift_automaton_destroy(RiftAutomaton* automaton) {
    if (!automaton) return;
    
    // Free states
    for (size_t i = 0; i < automaton->state_count; i++) {
        if (automaton->states[i]) {
            free(automaton->states[i]->pattern_string);
            if (automaton->states[i]->pattern) {
                rift_pattern_destroy(automaton->states[i]->pattern);
            }
            free(automaton->states[i]);
        }
    }
    free(automaton->states);
    
    // Free transitions
    for (size_t i = 0; i < automaton->transition_count; i++) {
        if (automaton->transitions[i]) {
            free(automaton->transitions[i]->input_symbol);
            free(automaton->transitions[i]);
        }
    }
    free(automaton->transitions);
    
    free(automaton->final_states);
    free(automaton->input_alphabet);
    free(automaton);
}

RiftState* rift_automaton_add_state(RiftAutomaton* automaton, 
                                   const char* pattern_str, 
                                   bool is_final) {
    assert(automaton && pattern_str);
    
    // Resize if needed
    if (automaton->state_count >= automaton->state_capacity) {
        automaton->state_capacity *= 2;
        automaton->states = realloc(automaton->states, 
                                   automaton->state_capacity * sizeof(RiftState*));
    }
    
    RiftState* state = calloc(1, sizeof(RiftState));
    state->id = automaton->state_count;
    state->pattern_string = strdup(pattern_str);
    state->is_final = is_final;
    state->is_initial = (automaton->state_count == 0); // First state is initial
    
    // Parse RIFT pattern if it looks like R"..."
    if (strncmp(pattern_str, "R\"", 2) == 0) {
        state->pattern = rift_parse_pattern(pattern_str);
        if (state->pattern) {
            rift_pattern_compile(state->pattern);
        }
    }
    
    automaton->states[automaton->state_count] = state;
    
    if (state->is_initial) {
        automaton->initial_state = state;
    }
    
    if (is_final) {
        automaton->final_states[automaton->final_count++] = state;
    }
    
    automaton->state_count++;
    return state;
}

bool rift_automaton_add_transition(RiftAutomaton* automaton,
                                  RiftState* from, 
                                  RiftState* to,
                                  const char* input_symbol) {
    assert(automaton && from && to && input_symbol);
    
    if (automaton->transition_count >= automaton->transition_capacity) {
        automaton->transition_capacity *= 2;
        automaton->transitions = realloc(automaton->transitions,
                                        automaton->transition_capacity * sizeof(RiftTransition*));
    }
    
    RiftTransition* transition = calloc(1, sizeof(RiftTransition));
    transition->from_state = from;
    transition->to_state = to;
    transition->input_symbol = strdup(input_symbol);
    
    automaton->transitions[automaton->transition_count++] = transition;
    return true;
}
