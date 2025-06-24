#ifndef RIFT_H
#define RIFT_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <regex.h>

// RIFT Pattern Syntax: R"pattern/flags[mode]"
// mode: 't' = top-down matching, 'b' = bottom-up matching

typedef enum {
    RIFT_MODE_TOP_DOWN = 't',
    RIFT_MODE_BOTTOM_UP = 'b'
} RiftMatchMode;

typedef struct RiftPattern {
    char* raw_pattern;       // Original R"..." string
    char* regex_pattern;     // Extracted regex pattern
    char* flags;             // gmi flags
    RiftMatchMode mode;      // t or b
    regex_t compiled_regex;  // Compiled POSIX regex
    bool is_compiled;
} RiftPattern;

// AEGIS Automaton State (Q in 5-tuple)
typedef struct RiftState {
    uint32_t id;
    char* pattern_string;
    RiftPattern* pattern;
    bool is_final;
    bool is_initial;
    void* semantic_data;     // For IR generation
} RiftState;

// AEGIS Transition (δ in 5-tuple)
typedef struct RiftTransition {
    RiftState* from_state;
    char* input_symbol;      // Σ alphabet
    RiftState* to_state;
    bool (*condition)(const char* input, size_t pos);
} RiftTransition;

// AEGIS Regex Automaton (5-tuple: Q, Σ, δ, q0, F)
typedef struct RiftAutomaton {
    RiftState** states;          // Q: finite set of states
    size_t state_count;
    size_t state_capacity;
    
    RiftTransition** transitions; // δ: transition function  
    size_t transition_count;
    size_t transition_capacity;
    
    RiftState* initial_state;    // q0: initial state
    RiftState** final_states;    // F: set of accepting states
    size_t final_count;
    
    RiftState* current_state;    // Current execution state
    char* input_alphabet;        // Σ: input alphabet
} RiftAutomaton;

// Token for IR Generation
typedef struct RiftToken {
    char* type;
    char* value;
    char* lexeme;
    size_t line;
    size_t column;
    size_t position;
    RiftState* matched_state;
} RiftToken;

// AEGIS RegexAutomatonEngine Component
typedef struct RiftEngine {
    RiftAutomaton* automaton;
    RiftToken** tokens;
    size_t token_count;
    size_t token_capacity;
    char* current_input;
    size_t input_position;
    size_t input_length;
} RiftEngine;

// ===== CORE API FUNCTIONS =====

// Pattern parsing (R"pattern/flags[mode]")
RiftPattern* rift_parse_pattern(const char* pattern_string);
void rift_pattern_destroy(RiftPattern* pattern);
bool rift_pattern_compile(RiftPattern* pattern);

// Automaton management
RiftAutomaton* rift_automaton_create(void);
void rift_automaton_destroy(RiftAutomaton* automaton);
RiftState* rift_automaton_add_state(RiftAutomaton* automaton, 
                                   const char* pattern_str, 
                                   bool is_final);
bool rift_automaton_add_transition(RiftAutomaton* automaton,
                                  RiftState* from, 
                                  RiftState* to,
                                  const char* input_symbol);

// Engine operations
RiftEngine* rift_engine_create(void);
void rift_engine_destroy(RiftEngine* engine);
bool rift_engine_process_input(RiftEngine* engine, const char* input);
RiftToken* rift_engine_next_token(RiftEngine* engine);

// Token management
RiftToken* rift_token_create(const char* type, const char* value, 
                           size_t line, size_t column);
void rift_token_destroy(RiftToken* token);

#endif // RIFT_H
