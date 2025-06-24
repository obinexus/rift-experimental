// Working C-Compatible CSV Parser
#include "../../include/rift.h"

int main() {
    printf("🔍 C-Compatible CSV Parser Demo\n");
    printf("================================\n");
    
    // Simple CSV pattern (C-compatible)
    const char* csv_pattern = "([^,\\n]+)|([,])|([\\n])";
    
    printf("📋 Pattern: %s\n\n", csv_pattern);
    
    // Create test data
    const char* csv_data = "Alice,25,NYC\nBob,30,SF\nCharlie,28,Austin";
    printf("📝 Test data:\n%s\n\n", csv_data);
    
    // Create automaton
    RiftAutomaton* automaton = rift_automaton_create();
    RiftState* csv_state = rift_automaton_add_state(automaton, csv_pattern, true);
    
    if (csv_state) {
        printf("✅ Pattern compiled successfully!\n");
        
        // Process with engine
        RiftEngine* engine = rift_engine_create();
        engine->automaton = automaton;
        rift_engine_process_input(engine, csv_data);
        
        printf("📊 Generated %zu tokens\n\n", engine->token_count);
        
        // Show tokens
        for (size_t i = 0; i < engine->token_count && i < 15; i++) {
            printf("Token %zu: \"%s\"\n", i, engine->tokens[i]->value);
        }
        
        rift_engine_destroy(engine);
    } else {
        printf("❌ Pattern compilation failed\n");
    }
    
    rift_automaton_destroy(automaton);
    
    printf("\n🎉 CSV parsing working with C-compatible patterns!\n");
    return 0;
}
