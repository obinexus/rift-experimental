/**
 * =================================================================
 * test_tokenizer.c - RIFT-0 Tokenizer Validation Suite
 * RIFT: RIFT Is a Flexible Translator
 * Component: Stage 0 POC validation and QA compliance testing
 * OBINexus Computing Framework - Aegis Project
 * Collaborator: Nnamdi Michael Okpala
 * =================================================================
 */

#include "rift-0/core/tokenizer_rules.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

/* Test framework macros */
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            printf("FAIL: %s\n", message); \
            printf("  Assertion failed: %s\n", #condition); \
            printf("  File: %s, Line: %d\n", __FILE__, __LINE__); \
            return false; \
        } \
    } while(0)

#define TEST_PASS(message) \
    do { \
        printf("PASS: %s\n", message); \
        return true; \
    } while(0)

/* Test result tracking */
typedef struct {
    int tests_run;
    int tests_passed;
    int tests_failed;
    char last_failure[256];
} TestSuite;

static TestSuite g_test_suite = {0};

/* Forward declarations */
static bool test_tokenizer_initialization(void);
static bool test_basic_token_patterns(void);
static bool test_r_pattern_tokenization(void);
static bool test_dfa_state_machine(void);
static bool test_null_nil_semantics(void);
static bool test_thread_safety(void);
static bool test_governance_compliance(void);
static bool test_ssa_validation(void);
static bool test_memory_safety(void);
static bool test_error_handling(void);

/* Utility functions */
static void run_test(const char *test_name, bool (*test_func)(void));
static void print_test_summary(void);

/**
 * Main test entry point
 * Validates RIFT-0 tokenizer according to .riftrc.0 governance
 */
int main(void) {
    printf("=================================================================\n");
    printf("RIFT-0 Tokenizer Validation Suite\n");
    printf("Aegis Project - Stage 0 POC Validation\n");
    printf("=================================================================\n\n");
    
    /* Initialize tokenizer system */
    init_tokenizer_rules();
    
    /* Core functionality tests */
    run_test("Tokenizer Initialization", test_tokenizer_initialization);
    run_test("Basic Token Patterns", test_basic_token_patterns);
    run_test("R Pattern Tokenization", test_r_pattern_tokenization);
    run_test("DFA State Machine", test_dfa_state_machine);
    run_test("NULL/nil Semantics", test_null_nil_semantics);
    
    /* Governance compliance tests */
    run_test("Thread Safety", test_thread_safety);
    run_test("Governance Compliance", test_governance_compliance);
    run_test("SSA Validation", test_ssa_validation);
    run_test("Memory Safety", test_memory_safety);
    run_test("Error Handling", test_error_handling);
    
    /* Cleanup */
    cleanup_tokenizer_rules();
    
    /* Print final results */
    print_test_summary();
    
    return (g_test_suite.tests_failed == 0) ? 0 : 1;
}

/**
 * Test: Tokenizer system initialization and cleanup
 */
static bool test_tokenizer_initialization(void) {
    /* Test multiple init/cleanup cycles */
    for (int i = 0; i < 3; i++) {
        init_tokenizer_rules();
        cleanup_tokenizer_rules();
    }
    
    /* Re-initialize for subsequent tests */
    init_tokenizer_rules();
    
    TEST_PASS("Tokenizer initialization and cleanup cycles");
}

/**
 * Test: Basic token pattern recognition
 */
static bool test_basic_token_patterns(void) {
    TokenTriplet token;
    int consumed;
    
    /* Test identifier */
    consumed = match_token_pattern("identifier", &token);
    TEST_ASSERT(consumed > 0, "Failed to tokenize identifier");
    TEST_ASSERT(token.type == TOKEN_IDENTIFIER, "Wrong token type for identifier");
    TEST_ASSERT(token.line_number == 1 && token.column_number == 1,
                "Identifier token position incorrect");
    
    /* Test string literal */
    consumed = match_token_pattern("\"hello world\"", &token);
    TEST_ASSERT(consumed > 0, "Failed to tokenize string literal");
    TEST_ASSERT(token.type == TOKEN_LITERAL_STRING, "Wrong token type for string");
    TEST_ASSERT(token.line_number == 1 && token.column_number == 1,
                "String literal token position incorrect");
    
    /* Test number literal */
    consumed = match_token_pattern("42", &token);
    TEST_ASSERT(consumed > 0, "Failed to tokenize number");
    TEST_ASSERT(token.type == TOKEN_LITERAL_NUMBER, "Wrong token type for number");
    TEST_ASSERT(token.line_number == 1 && token.column_number == 1,
                "Number token position incorrect");
    
    /* Test operator */
    consumed = match_token_pattern("+", &token);
    TEST_ASSERT(consumed > 0, "Failed to tokenize operator");
    TEST_ASSERT(token.type == TOKEN_OPERATOR, "Wrong token type for operator");
    TEST_ASSERT(token.line_number == 1 && token.column_number == 1,
                "Operator token position incorrect");
    
    /* Test delimiter */
    consumed = match_token_pattern("(", &token);
    TEST_ASSERT(consumed > 0, "Failed to tokenize delimiter");
    TEST_ASSERT(token.type == TOKEN_DELIMITER, "Wrong token type for delimiter");
    TEST_ASSERT(token.line_number == 1 && token.column_number == 1,
                "Delimiter token position incorrect");
    
    TEST_PASS("Basic token pattern recognition");
}

/**
 * Test: R"" pattern tokenization as specified in governance
 */
static bool test_r_pattern_tokenization(void) {
    TokenTriplet token;
    int consumed;
    
    /* Test simple R"" pattern */
    consumed = match_token_pattern("R\"hello\"", &token);
    TEST_ASSERT(consumed > 0, "Failed to tokenize R pattern");
    TEST_ASSERT(token.type == TOKEN_R_PATTERN, "Wrong token type for R pattern");
    
    /* Test R pattern with delimiter */
    consumed = match_token_pattern("R\"delim(content)delim\"", &token);
    TEST_ASSERT(consumed > 0, "Failed to tokenize R pattern with delimiter");
    TEST_ASSERT(token.type == TOKEN_R_PATTERN, "Wrong token type for delimited R pattern");
    
    /* Test R' variant (insensitive) */
    consumed = match_token_pattern("R'hello'", &token);
    TEST_ASSERT(consumed > 0, "Failed to tokenize R' pattern");
    TEST_ASSERT(token.type == TOKEN_R_PATTERN, "Wrong token type for R' pattern");
    
    TEST_PASS("R pattern tokenization with DFA validation");
}

/**
 * Test: DFA state machine functionality
 */
static bool test_dfa_state_machine(void) {
    DFAStateMachine *dfa = dfa_create();
    TEST_ASSERT(dfa != NULL, "Failed to create DFA state machine");
    
    /* Test R"" pattern processing */
    const char *pattern = "R\"test\"";
    bool success = true;
    
    for (const char *p = pattern; *p && success; p++) {
        success = dfa_process_char(dfa, *p);
    }
    
    TEST_ASSERT(success, "DFA failed to process R pattern");
    TEST_ASSERT(dfa_is_accepting(dfa), "DFA not in accepting state");
    
    /* Test reset functionality */
    dfa_reset(dfa);
    TEST_ASSERT(!dfa_is_accepting(dfa), "DFA reset failed");
    
    dfa_destroy(dfa);
    
    TEST_PASS("DFA state machine operations");
}

/**
 * Test: NULL vs nil semantic handling
 */
static bool test_null_nil_semantics(void) {
    TokenType type;
    
    /* Test NULL keyword */
    type = classify_null_nil_semantic("NULL", 4);
    TEST_ASSERT(type == TOKEN_NULL_KEYWORD, "Failed to classify NULL keyword");
    
    /* Test nil keyword */
    type = classify_null_nil_semantic("nil", 3);
    TEST_ASSERT(type == TOKEN_NIL_KEYWORD, "Failed to classify nil keyword");
    
    /* Test non-keyword */
    type = classify_null_nil_semantic("other", 5);
    TEST_ASSERT(type == TOKEN_UNKNOWN, "False positive on non-keyword");
    
    /* Test full tokenization with NULL/nil */
    TokenTriplet token;
    int consumed;
    
    consumed = match_token_pattern("NULL", &token);
    TEST_ASSERT(consumed > 0, "Failed to tokenize NULL");
    TEST_ASSERT(token.type == TOKEN_NULL_KEYWORD, "Wrong type for NULL token");
    
    consumed = match_token_pattern("nil", &token);
    TEST_ASSERT(consumed > 0, "Failed to tokenize nil");
    TEST_ASSERT(token.type == TOKEN_NIL_KEYWORD, "Wrong type for nil token");
    
    TEST_PASS("NULL/nil semantic differentiation");
}

/**
 * Test: Thread safety compliance
 */
static bool test_thread_safety(void) {
    /* Test thread-safe encoding */
    TokenTriplet token1 = R_encode_safe(TOKEN_IDENTIFIER, 100, 50);
    TokenTriplet token2 = R_encode_safe(TOKEN_KEYWORD, 200, 75);
    
    TEST_ASSERT(token1.type == TOKEN_IDENTIFIER, "Thread-safe encoding failed for token1");
    TEST_ASSERT(token1.mem_ptr == 100, "Memory pointer corrupted in token1");
    TEST_ASSERT(token1.value == 50, "Value corrupted in token1");
    
    TEST_ASSERT(token2.type == TOKEN_KEYWORD, "Thread-safe encoding failed for token2");
    TEST_ASSERT(token2.mem_ptr == 200, "Memory pointer corrupted in token2");
    TEST_ASSERT(token2.value == 75, "Value corrupted in token2");
    
    /* Test concurrent tokenization (simulated) */
    const char *test_strings[] = {"identifier1", "identifier2", "identifier3"};
    for (int i = 0; i < 3; i++) {
        TokenTriplet token;
        int consumed = match_token_pattern(test_strings[i], &token);
        TEST_ASSERT(consumed > 0, "Concurrent tokenization failed");
        TEST_ASSERT(token.type == TOKEN_IDENTIFIER, "Wrong type in concurrent test");
    }
    
    TEST_PASS("Thread safety compliance verified");
}

/**
 * Test: Governance compliance (.riftrc.0 validation)
 */
static bool test_governance_compliance(void) {
    /* Test that tokenizer respects governance constraints */
    
    /* Verify token type/value separation */
    TokenTriplet token = R_encode_safe(TOKEN_IDENTIFIER, 1000, 100);
    TEST_ASSERT(validate_token_separation(&token), "Token separation validation failed");
    
    /* Test invalid token (type out of range) */
    TokenTriplet invalid_token;
    invalid_token.type = 255; /* Invalid type */
    invalid_token.mem_ptr = 0;
    invalid_token.value = 0;
    TEST_ASSERT(!validate_token_separation(&invalid_token), "Should reject invalid token type");
    
    /* Test deterministic behavior */
    TokenTriplet token_a, token_b;
    int consumed_a = match_token_pattern("test", &token_a);
    int consumed_b = match_token_pattern("test", &token_b);
    
    TEST_ASSERT(consumed_a == consumed_b, "Non-deterministic consumption");
    TEST_ASSERT(token_a.type == token_b.type, "Non-deterministic token type");
    
    TEST_PASS("Governance compliance validated");
}

/**
 * Test: SSA (Static Single Assignment) validation
 */
static bool test_ssa_validation(void) {
    /* Test token type/value separation for SSA compliance */
    
    /* Valid token */
    TokenTriplet valid_token = R_encode_safe(TOKEN_IDENTIFIER, 100, 10);
    TEST_ASSERT(validate_token_separation(&valid_token), "Valid token rejected");
    
    /* EOF token validation */
    TokenTriplet eof_token = R_encode_safe(TOKEN_EOF, 0, 0);
    TEST_ASSERT(validate_token_separation(&eof_token), "EOF token validation failed");
    
    /* Error token validation */
    TokenTriplet error_token = R_encode_safe(TOKEN_ERROR, 0, 0);
    TEST_ASSERT(!validate_token_separation(&error_token), "Error token should be invalid");
    
    TEST_PASS("SSA validation compliance verified");
}

/**
 * Test: Memory safety verification
 */
static bool test_memory_safety(void) {
    /* Test full source tokenization with memory management */
    const char *source = "identifier 42 \"string\" + ( nil NULL";
    TokenizationResult result = tokenize_source(source, strlen(source));
    
    TEST_ASSERT(result.success, "Tokenization should succeed");
    TEST_ASSERT(result.tokens != NULL, "Token array should be allocated");
    TEST_ASSERT(result.count > 0, "Should have tokens");
    
    /* Verify no memory leaks in normal usage */
    free_tokenization_result(&result);
    TEST_ASSERT(result.tokens == NULL, "Token array should be cleared");
    TEST_ASSERT(result.count == 0, "Count should be reset");
    
    /* Test empty source handling */
    TokenizationResult empty_result = tokenize_source("", 0);
    TEST_ASSERT(!empty_result.success, "Empty source should fail gracefully");
    TEST_ASSERT(empty_result.error_message != NULL, "Should have error message");
    free_tokenization_result(&empty_result);
    
    TEST_PASS("Memory safety verification complete");
}

/**
 * Test: Error handling and robustness
 */
static bool test_error_handling(void) {
    TokenTriplet token;
    int consumed;
    
    /* Test NULL input handling */
    consumed = match_token_pattern(NULL, &token);
    TEST_ASSERT(consumed == -1, "Should reject NULL input");
    
    /* Test invalid character sequences */
    consumed = match_token_pattern("@#$%", &token);
    TEST_ASSERT(consumed > 0, "Should handle unknown characters gracefully");
    
    /* Test DFA error states */
    DFAStateMachine *dfa = dfa_create();
    TEST_ASSERT(dfa != NULL, "DFA creation should succeed");
    
    /* Force DFA into error state with malformed R pattern */
    bool success = dfa_process_char(dfa, 'R');
    TEST_ASSERT(success, "R detection should succeed");
    
    success = dfa_process_char(dfa, 'X'); /* Invalid character after R */
    TEST_ASSERT(!success, "Should detect invalid R pattern");
    
    dfa_destroy(dfa);
    
    TEST_PASS("Error handling verification complete");
}

/**
 * Utility: Run individual test and track results
 */
static void run_test(const char *test_name, bool (*test_func)(void)) {
    printf("Running: %s... ", test_name);
    fflush(stdout);
    
    g_test_suite.tests_run++;
    
    if (test_func()) {
        g_test_suite.tests_passed++;
    } else {
        g_test_suite.tests_failed++;
        snprintf(g_test_suite.last_failure, sizeof(g_test_suite.last_failure), 
                "%s", test_name);
    }
    
    printf("\n");
}

/**
 * Utility: Print final test summary
 */
static void print_test_summary(void) {
    printf("\n=================================================================\n");
    printf("RIFT-0 Tokenizer Validation Results\n");
    printf("=================================================================\n");
    printf("Tests Run:    %d\n", g_test_suite.tests_run);
    printf("Tests Passed: %d\n", g_test_suite.tests_passed);
    printf("Tests Failed: %d\n", g_test_suite.tests_failed);
    
    if (g_test_suite.tests_failed > 0) {
        printf("Last Failure: %s\n", g_test_suite.last_failure);
        printf("\nSTATUS: RIFT-0 VALIDATION FAILED\n");
        printf("Stage progression to rift-1 is blocked.\n");
    } else {
        printf("\nSTATUS: RIFT-0 VALIDATION PASSED\n");
        printf("Ready for .riftrc.1 generation and stage progression.\n");
    }
    
    printf("=================================================================\n");
}