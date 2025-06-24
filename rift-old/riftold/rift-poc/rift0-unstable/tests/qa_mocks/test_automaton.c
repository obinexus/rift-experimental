#include "../../include/rift0/rift.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

// QA Mock: Test RIFT pattern parsing
void test_pattern_parsing(void) {
    printf("🧪 Testing RIFT pattern parsing...\n");
    
    RiftPattern* pattern = rift_parse_pattern("R\"[A-Z]+/gmi[t]\"");
    assert(pattern != NULL);
    assert(strcmp(pattern->regex_pattern, "[A-Z]+") == 0);
    assert(strcmp(pattern->flags, "gmi") == 0);
    assert(pattern->mode == RIFT_MODE_TOP_DOWN);
    
    rift_pattern_destroy(pattern);
    printf("✅ Pattern parsing test passed\n");
}

// QA Mock: Test automaton state management
void test_automaton_states(void) {
    printf("🧪 Testing automaton state management...\n");
    
    RiftAutomaton* automaton = rift_automaton_create();
    assert(automaton != NULL);
    
    RiftState* state1 = rift_automaton_add_state(automaton, "R\"test/g[t]\"", false);
    RiftState* state2 = rift_automaton_add_state(automaton, "R\"final/g[t]\"", true);
    
    assert(automaton->state_count == 2);
    assert(state1->is_initial == true);  // First state is initial
    assert(state2->is_final == true);
    assert(automaton->final_count == 1);
    
    rift_automaton_destroy(automaton);
    printf("✅ Automaton state test passed\n");
}

// QA Mock: Test tokenization
void test_tokenization(void) {
    printf("🧪 Testing tokenization engine...\n");
    
    RiftEngine* engine = rift_engine_create();
    assert(engine != NULL);
    
    // Add a simple pattern
    rift_automaton_add_state(engine->automaton, "R\"[A-Z]+/g[t]\"", false);
    
    assert(rift_engine_process_input(engine, "HELLO WORLD"));
    
    RiftToken* token = rift_engine_next_token(engine);
    assert(token != NULL);
    assert(strcmp(token->type, "PATTERN_MATCH") == 0);
    
    rift_token_destroy(token);
    rift_engine_destroy(engine);
    printf("✅ Tokenization test passed\n");
}

int main(void) {
    printf("🔬 RIFT Stage 0 QA Mock Tests\n");
    printf("==============================\n");
    
    test_pattern_parsing();
    test_automaton_states();
    test_tokenization();
    
    printf("==============================\n");
    printf("✅ All QA mock tests passed!\n");
    
    return 0;
}
