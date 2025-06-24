#include "../../include/rift.h"

int main(void) {
    printf("🚀 RIFT Stage 0 Demo\n");
    printf("====================\n");
    
    RiftAutomaton* automaton = rift_automaton_create();
    printf("🔧 Created automaton\n");
    
    RiftEngine* engine = rift_engine_create();
    engine->automaton = automaton;
    
    const char* test_input = "hello world";
    printf("📝 Processing: \"%s\"\n", test_input);
    
    rift_engine_process_input(engine, test_input);
    
    printf("🎯 Generated %zu tokens\n", engine->token_count);
    
    for (size_t i = 0; i < engine->token_count && i < 10; i++) {
        RiftToken* token = engine->tokens[i];
        printf("  Token %zu: %s = \"%s\"\n", i, token->type, token->value);
    }
    
    printf("✅ Demo complete\n");
    
    rift_engine_destroy(engine);
    rift_automaton_destroy(automaton);
    
    return 0;
}
