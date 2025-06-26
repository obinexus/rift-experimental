// tests/rift1/lexer/test_lexer_basic.c - Lexer Component Tests
// OBINexus AEGIS Framework - Unit tests for lexer component
#include "../../../include/rift1/lexer/lexer.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

void test_lexer_creation() {
    printf("Testing lexer creation...\n");
    
    RiftLexer* lexer = rift_lexer_create(NULL);
    assert(lexer != NULL);
    
    rift_lexer_destroy(lexer);
    printf("✓ Lexer creation test passed\n");
}

void test_lexer_simple_input() {
    printf("Testing simple input processing...\n");
    
    RiftLexer* lexer = rift_lexer_create(NULL);
    assert(lexer != NULL);
    
    const char* input = "hello world test";
    bool result = rift_lexer_set_input(lexer, input, strlen(input));
    assert(result == true);
    
    // Extract first lexeme
    RiftLexeme* lexeme = rift_lexer_next_lexeme(lexer);
    assert(lexeme != NULL);
    assert(strncmp(lexeme->text, "hello", 5) == 0);
    assert(lexeme->length == 5);
    
    rift_lexeme_destroy(lexeme);
    rift_lexer_destroy(lexer);
    printf("✓ Simple input test passed\n");
}

void test_lexer_position_tracking() {
    printf("Testing position tracking...\n");
    
    RiftLexer* lexer = rift_lexer_create(NULL);
    const char* input = "first\nsecond\nthird";
    rift_lexer_set_input(lexer, input, strlen(input));
    
    RiftLexeme* lexeme1 = rift_lexer_next_lexeme(lexer);
    assert(lexeme1 != NULL);
    assert(lexeme1->position.line == 1);
    assert(lexeme1->position.column == 1);
    
    RiftLexeme* lexeme2 = rift_lexer_next_lexeme(lexer);
    assert(lexeme2 != NULL);
    assert(lexeme2->position.line == 2);
    
    rift_lexeme_destroy(lexeme1);
    rift_lexeme_destroy(lexeme2);
    rift_lexer_destroy(lexer);
    printf("✓ Position tracking test passed\n");
}

int main() {
    printf("Running RIFT1 Lexer Component Tests\n");
    printf("===================================\n");
    
    test_lexer_creation();
    test_lexer_simple_input();
    test_lexer_position_tracking();
    
    printf("\n✓ All lexer tests passed!\n");
    return 0;
}
