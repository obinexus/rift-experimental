#ifndef POCRIFT_H
#define POCRIFT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct State {
    char* pattern;
    bool is_final;
    size_t id;
} State;

typedef struct RegexAutomaton {
    State** states;
    size_t state_count;
    size_t state_capacity;
    State* initial_state;
    State* current_state;
} RegexAutomaton;

RegexAutomaton* automaton_create(void);
void automaton_destroy(RegexAutomaton* automaton);
State* automaton_add_state(RegexAutomaton* automaton, const char* pattern, bool is_final);
State* automaton_get_next_state(RegexAutomaton* automaton, const char* input);
const char* get_token_type_name(const char* pattern);

#endif
