// tests/rift1/token_types/test_token_types_basic.c - Token Types Tests
// OBINexus AEGIS Framework - Unit tests for token type classification
#include "../../../include/rift1/token_types/token_type.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

void test_token_type_matcher_creation() {
    printf("Testing token type matcher creation...\n");
    
    RiftTokenTypeMatcher* matcher = rift_token_type_matcher_create();
    assert(matcher != NULL);
    
    rift_token_type_matcher_destroy(matcher);
    printf("✓ Token type matcher creation test passed\n");
}

void test_keyword_recognition() {
    printf("Testing keyword recognition...\n");
    
    RiftTokenTypeMatcher* matcher = rift_token_type_matcher_create();
    assert(matcher != NULL);
    
    // Create test lexemes
    RiftPosition pos = {1, 1, 0, "test"};
    RiftLexeme* if_lexeme = rift_lexeme_create("if", 2, pos);
    RiftLexeme* while_lexeme = rift_lexeme_create("while", 5, pos);
    RiftLexeme* identifier_lexeme = rift_lexeme_create("myVar", 5, pos);
    
    // Test classification
    assert(rift_token_type_match(matcher, if_lexeme) == RIFT_TOKEN_KEYWORD);
    assert(rift_token_type_match(matcher, while_lexeme) == RIFT_TOKEN_KEYWORD);
    assert(rift_token_type_match(matcher, identifier_lexeme) == RIFT_TOKEN_IDENTIFIER);
    
    rift_lexeme_destroy(if_lexeme);
    rift_lexeme_destroy(while_lexeme);
    rift_lexeme_destroy(identifier_lexeme);
    rift_token_type_matcher_destroy(matcher);
    printf("✓ Keyword recognition test passed\n");
}

void test_literal_recognition() {
    printf("Testing literal recognition...\n");
    
    RiftTokenTypeMatcher* matcher = rift_token_type_matcher_create();
    RiftPosition pos = {1, 1, 0, "test"};
    
    // Test integer
    RiftLexeme* int_lexeme = rift_lexeme_create("42", 2, pos);
    assert(rift_token_type_match(matcher, int_lexeme) == RIFT_TOKEN_INTEGER);
    
    // Test float
    RiftLexeme* float_lexeme = rift_lexeme_create("3.14", 4, pos);
    assert(rift_token_type_match(matcher, float_lexeme) == RIFT_TOKEN_FLOAT);
    
    // Test string
    RiftLexeme* string_lexeme = rift_lexeme_create("\"hello\"", 7, pos);
    assert(rift_token_type_match(matcher, string_lexeme) == RIFT_TOKEN_STRING);
    
    // Test boolean
    RiftLexeme* bool_lexeme = rift_lexeme_create("true", 4, pos);
    assert(rift_token_type_match(matcher, bool_lexeme) == RIFT_TOKEN_BOOLEAN);
    
    rift_lexeme_destroy(int_lexeme);
    rift_lexeme_destroy(float_lexeme);
    rift_lexeme_destroy(string_lexeme);
    rift_lexeme_destroy(bool_lexeme);
    rift_token_type_matcher_destroy(matcher);
    printf("✓ Literal recognition test passed\n");
}

int main() {
    printf("Running RIFT1 Token Types Component Tests\n");
    printf("=========================================\n");
    
    test_token_type_matcher_creation();
    test_keyword_recognition();
    test_literal_recognition();
    
    printf("\n✓ All token type tests passed!\n");
    return 0;
}
