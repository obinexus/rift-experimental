#include "../include/rift0/rift.h"
#include <stdio.h>
#include <string.h>

void print_token(RiftToken* token) {
    if (!token) return;
    
    printf("Token: %-15s | Value: %-20s | Position: (%zu,%zu)", 
           token->type, token->value, token->line, token->column);
    
    if (token->matched_state) {
        printf(" | State: %u", token->matched_state->id);
        if (token->matched_state->pattern) {
            printf(" | Pattern: %s | Mode: %c", 
                   token->matched_state->pattern->regex_pattern,
                   token->matched_state->pattern->mode);
        }
    }
    printf("\n");
}

int main(void) {
    printf("🚀 RIFT Stage 0 - Tokenization/Lexical Analysis Demo\n");
    printf("📋 AEGIS Automaton Engine for Generative Interpretation & Syntax\n\n");
    
    // Create RIFT engine
    RiftEngine* engine = rift_engine_create();
    if (!engine) {
        fprintf(stderr, "Failed to create RIFT engine\n");
        return 1;
    }
    
    // Add some example RIFT patterns
    printf("🔧 Adding RIFT pattern states to automaton...\n");
    
    // Pattern 1: Uppercase letters and digits
    RiftState* state1 = rift_automaton_add_state(engine->automaton, 
        "R\"[A-Z0-9]+/gmi[t]\"", false);
    
    // Pattern 2: Whitespace (bottom-up mode)
    RiftState* state2 = rift_automaton_add_state(engine->automaton,
        "R\"\\s+/gm[b]\"", false);
    
    // Pattern 3: Special characters  
    RiftState* state3 = rift_automaton_add_state(engine->automaton,
        "R\"[^A-Z0-9\\s]+/gmi[t]\"", true); // Final state
    
    printf("✅ Added %zu states to automaton\n\n", engine->automaton->state_count);
    
    // Test input string
    const char* test_input = "HELLO123 world! #$% ABC";
    printf("📝 Processing input: \"%s\"\n\n", test_input);
    
    // Process the input
    if (!rift_engine_process_input(engine, test_input)) {
        fprintf(stderr, "Failed to process input\n");
        rift_engine_destroy(engine);
        return 1;
    }
    
    printf("🎯 Generated tokens:\n");
    printf("================================================================================\n");
    
    // Tokenize
    RiftToken* token;
    int token_count = 0;
    while ((token = rift_engine_next_token(engine)) != NULL) {
        print_token(token);
        rift_token_destroy(token);
        token_count++;
        
        if (token_count > 50) { // Safety limit
            printf("... (truncated for safety)\n");
            break;
        }
    }
    
    printf("================================================================================\n");
    printf("✅ Generated %d tokens\n\n", token_count);
    
    // Print automaton summary
    printf("📊 Automaton Summary:\n");
    printf("   States: %zu (Initial: %u, Final: %zu)\n", 
           engine->automaton->state_count,
           engine->automaton->initial_state ? engine->automaton->initial_state->id : 999,
           engine->automaton->final_count);
    printf("   Transitions: %zu\n", engine->automaton->transition_count);
    printf("   Alphabet: %s\n", engine->automaton->input_alphabet);
    
    // Cleanup
    rift_engine_destroy(engine);
    
    printf("\n🎉 RIFT Stage 0 tokenization complete!\n");
    printf("💡 Ready for Stage 1 (Parsing/Grammar Analysis)\n");
    
    return 0;
}
