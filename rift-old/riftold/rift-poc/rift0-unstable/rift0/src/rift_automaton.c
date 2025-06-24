#include "../include/rift0/rift.h"
#include <assert.h>

RiftAutomaton* rift_automaton_create(void) {
    RiftAutomaton* automaton = calloc(1, sizeof(RiftAutomaton));
    if (!automaton) return NULL;
    
    automaton->state_capacity = 16;
    automaton->states = calloc(automaton->state_capacity, sizeof(RiftState*));
    if (!automaton->states) {
        free(automaton);
        return NULL;
    }
    
    automaton->transition_capacity = 32;
    automaton->transitions = calloc(automaton->transition_capacity, sizeof(RiftTransition*));
    if (!automaton->transitions) {
        free(automaton->states);
        free(automaton);
        return NULL;
    }
    
    automaton->final_capacity = 16;
    automaton->final_states = calloc(automaton->final_capacity, sizeof(RiftState*));
    if (!automaton->final_states) {
        free(automaton->states);
        free(automaton->transitions);
        free(automaton);
        return NULL;
    }
    
    automaton->input_alphabet = strdup("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789._-+*/=<>!&|(){}[];,");
    if (!automaton->input_alphabet) {
        free(automaton->states);
        free(automaton->transitions);
        free(automaton->final_states);
        free(automaton);
        return NULL;
    }
    
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
    if (!automaton || !pattern_str) return NULL;
    
    // Resize if needed
    if (automaton->state_count >= automaton->state_capacity) {
        automaton->state_capacity *= 2;
        RiftState** new_states = realloc(automaton->states, 
                                        automaton->state_capacity * sizeof(RiftState*));
        if (!new_states) return NULL;
        automaton->states = new_states;
    }
    
    RiftState* state = calloc(1, sizeof(RiftState));
    if (!state) return NULL;
    
    state->id = (uint32_t)automaton->state_count;
    state->pattern_string = strdup(pattern_str);
    if (!state->pattern_string) {
        free(state);
        return NULL;
    }
    
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
        // Resize final states if needed
        if (automaton->final_count >= automaton->final_capacity) {
            automaton->final_capacity *= 2;
            RiftState** new_final = realloc(automaton->final_states,
                                          automaton->final_capacity * sizeof(RiftState*));
            if (!new_final) {
                // State was already added, don't fail completely
                automaton->state_count++;
                return state;
            }
            automaton->final_states = new_final;
        }
        automaton->final_states[automaton->final_count++] = state;
    }
    
    automaton->state_count++;
    return state;
}

RiftResult rift_automaton_add_transition(RiftAutomaton* automaton,
                                        RiftState* from, 
                                        RiftState* to,
                                        const char* input_symbol) {
    if (!automaton || !from || !to || !input_symbol) {
        return RIFT_ERROR_NULL_POINTER;
    }
    
    if (automaton->transition_count >= automaton->transition_capacity) {
        automaton->transition_capacity *= 2;
        RiftTransition** new_transitions = realloc(automaton->transitions,
                                                  automaton->transition_capacity * sizeof(RiftTransition*));
        if (!new_transitions) return RIFT_ERROR_MEMORY_ALLOCATION;
        automaton->transitions = new_transitions;
    }
    
    RiftTransition* transition = calloc(1, sizeof(RiftTransition));
    if (!transition) return RIFT_ERROR_MEMORY_ALLOCATION;
    
    transition->from_state = from;
    transition->to_state = to;
    transition->input_symbol = strdup(input_symbol);
    if (!transition->input_symbol) {
        free(transition);
        return RIFT_ERROR_MEMORY_ALLOCATION;
    }
    
    automaton->transitions[automaton->transition_count++] = transition;
    return RIFT_SUCCESS;
}
