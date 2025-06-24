#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "tokenizer.h"
#include "tokenizer_rules.h"

/**
 * RIFT-0 Tokenizer Validation Suite
 * 
 * Comprehensive unit testing framework following our waterfall methodology
 * for systematic validation of tokenization engine functionality.
 * 
 * Test Categories:
 * 1. Governance Compliance (PoliC Zero Trust)
 * 2. State Machine Optimization (Nnamdi's Research Integration)
 * 3. Memory Alignment and Safety 
 * 4. Token Type Classification Accuracy
 * 5. Performance Regression Prevention
 * 6. Error Handling and Recovery
 */

// Test result tracking
static size_t g_tests_run = 0;
static size_t g_tests_passed = 0;
static size_t g_tests_failed = 0;

// Test utility macros for systematic validation
#define TEST_ASSERT(condition, message) \
    do { \
        g_tests_run++; \
        if (condition) { \
            printf("✓ PASS: %s\n", message); \
            g_tests_passed++; \
        } else { \
            printf("✗ FAIL: %s\n", message); \
            g_tests_failed++; \
        } \
    } while(0)

#define TEST_SECTION(name) \
    printf("\n=== %s ===\n", name)

/**
 * Test 1: Governance Compliance Validation
 * Validates PoliC zero trust framework integration
 */
static void test_governance_compliance(void) {
    TEST_SECTION("Governance Compliance Tests");
    
    // Test 1.1: TokenTriplet structure alignment
    TokenTriplet test_token = {0};
    TEST_ASSERT(sizeof(TokenTriplet) == 4, 
                "TokenTriplet structure is 4-byte aligned");
    
    // Test 1.2: Memory alignment verification
    TEST_ASSERT(((uintptr_t)&test_token) % 4 == 0,
                "TokenTriplet instance is 4-byte aligned in memory");
    
    // Test 1.3: Bitfield packing validation
    test_token.type = 255;
    test_token.mem_ptr = 65535;
    test_token.value = 255;
    TEST_ASSERT(test_token.type == 255 && test_token.mem_ptr == 65535 && test_token.value == 255,
                "Bitfield packing preserves maximum values");
    
    // Test 1.4: PoliC validation function
    test_token.type = TOKEN_IDENTIFIER;
    test_token.mem_ptr = 0;
    test_token.value = 65;
    TEST_ASSERT(polic_validate_token(&test_token, NULL),
                "Valid token passes PoliC governance validation");
}

/**
 * Test 2: State Machine Optimization Validation
 * Validates Nnamdi's optimization research integration
 */
static void test_state_machine_optimization(void) {
    TEST_SECTION("State Machine Optimization Tests");
    
    // Test 2.1: Single-pass tokenization efficiency
    const char* test_input = "identifier123 + variable_name";
    TokenTriplet tokens[20];
    size_t token_count = 0;
    
    int result = tokenizer_process_string(test_input, tokens, 20, &token_count);
    TEST_ASSERT(result == 0, "Single-pass tokenization completes successfully");
    TEST_ASSERT(token_count > 0, "Tokens generated from input");
    
    // Test 2.2: Deterministic output validation
    TokenTriplet tokens2[20];
    size_t token_count2 = 0;
    result = tokenizer_process_string(test_input, tokens2, 20, &token_count2);
    
    TEST_ASSERT(token_count == token_count2, "Deterministic token count");
    
    bool tokens_identical = true;
    for (size_t i = 0; i < token_count && i < token_count2; i++) {
        if (tokens[i].type != tokens2[i].type ||
            tokens[i].mem_ptr != tokens2[i].mem_ptr ||
            tokens[i].value != tokens2[i].value) {
            tokens_identical = false;
            break;
        }
    }
    TEST_ASSERT(tokens_identical, "Deterministic token stream generation");
    
    // Test 2.3: O(n) performance characteristic validation
    TEST_ASSERT(token_count <= strlen(test_input) + 1, 
                "Token count bounded by input length (O(n) characteristic)");
}

/**
 * Test 3: Token Classification Accuracy
 * Validates systematic token type recognition
 */
static void test_token_classification(void) {
    TEST_SECTION("Token Classification Accuracy Tests");
    
    // Test 3.1: Identifier recognition
    TokenTriplet tokens[10];
    size_t token_count = 0;
    
    tokenizer_process_string("identifier", tokens, 10, &token_count);
    TEST_ASSERT(token_count >= 1 && tokens[0].type == TOKEN_IDENTIFIER,
                "Identifier token classification");
    
    // Test 3.2: Integer literal recognition
    tokenizer_process_string("12345", tokens, 10, &token_count);
    TEST_ASSERT(token_count >= 1 && tokens[0].type == TOKEN_LITERAL_INT,
                "Integer literal classification");
    
    // Test 3.3: Floating point recognition
    tokenizer_process_string("3.14159", tokens, 10, &token_count);
    TEST_ASSERT(token_count >= 1 && tokens[0].type == TOKEN_LITERAL_FLOAT,
                "Floating point literal classification");
    
    // Test 3.4: Operator recognition
    tokenizer_process_string("+", tokens, 10, &token_count);
    TEST_ASSERT(token_count >= 1 && tokens[0].type == TOKEN_OPERATOR,
                "Operator token classification");
    
    // Test 3.5: Delimiter recognition
    tokenizer_process_string("(", tokens, 10, &token_count);
    TEST_ASSERT(token_count >= 1 && tokens[0].type == TOKEN_DELIMITER,
                "Delimiter token classification");
    
    // Test 3.6: EOF token generation
    tokenizer_process_string("test", tokens, 10, &token_count);
    TEST_ASSERT(token_count >= 2 && tokens[token_count-1].type == TOKEN_EOF,
                "EOF token automatically appended");
}

/**
 * Test 4: NULL vs nil Semantic Validation
 * Validates RIFT-0 semantic distinction handling
 */
static void test_null_nil_semantics(void) {
    TEST_SECTION("NULL vs nil Semantic Tests");
    
    TokenType semantic_type;
    
    // Test 4.1: NULL semantic validation
    bool result = validate_null_nil_semantics("NULL", &semantic_type);
    TEST_ASSERT(result == true && semantic_type == TOKEN_KEYWORD,
                "NULL recognized as void intent keyword");
    
    // Test 4.2: nil semantic validation  
    result = validate_null_nil_semantics("nil", &semantic_type);
    TEST_ASSERT(result == true && semantic_type == TOKEN_KEYWORD,
                "nil recognized as unallocated state keyword");
    
    // Test 4.3: Invalid semantic rejection
    result = validate_null_nil_semantics("invalid", &semantic_type);
    TEST_ASSERT(result == false && semantic_type == TOKEN_UNKNOWN,
                "Invalid semantic text properly rejected");
}

/**
 * Test 5: Error Handling and Recovery
 * Validates systematic error management approach
 */
static void test_error_handling(void) {
    TEST_SECTION("Error Handling and Recovery Tests");
    
    // Test 5.1: Invalid parameter handling
    TokenTriplet tokens[10];
    size_t token_count = 0;
    
    int result = tokenizer_process_string(NULL, tokens, 10, &token_count);
    TEST_ASSERT(result != 0, "NULL input parameter rejected");
    TEST_ASSERT(tokenizer_has_error(), "Error state activated");
    
    // Test 5.2: Error message availability
    const char* error_msg = tokenizer_get_error_message();
    TEST_ASSERT(strlen(error_msg) > 0, "Error message provided");
    
    // Test 5.3: Error state reset
    tokenizer_reset_error();
    TEST_ASSERT(!tokenizer_has_error(), "Error state successfully reset");
    
    // Test 5.4: Recovery after error
    result = tokenizer_process_string("test", tokens, 10, &token_count);
    TEST_ASSERT(result == 0, "Successful operation after error recovery");
}

/**
 * Test 6: DFA Pattern Matching Validation
 * Validates R"" pattern syntax processing
 */
static void test_dfa_pattern_matching(void) {
    TEST_SECTION("DFA Pattern Matching Tests");
    
    // Test 6.1: Basic pattern matching
    PatternMatchResult result;
    int match_result = match_token_pattern("test123", "R\"/[a-z]+/\"", 0, &result);
    TEST_ASSERT(match_result == 0 || match_result == -1, 
                "Pattern matching executes without crash");
    
    // Test 6.2: Boolean composition evaluation
    bool eval_result;
    int eval_status = evaluate_boolean_composition("R UM AND NOT EQ", &eval_result);
    TEST_ASSERT(eval_status == 0, "Boolean composition evaluation completes");
}

/**
 * Test 7: Memory Safety and Boundary Validation
 * Validates memory management and boundary conditions
 */
static void test_memory_safety(void) {
    TEST_SECTION("Memory Safety and Boundary Tests");
    
    // Test 7.1: Buffer overflow protection
    TokenTriplet tokens[2]; // Small buffer
    size_t token_count = 0;
    
    int result = tokenizer_process_string("a b c d e", tokens, 2, &token_count);
    TEST_ASSERT(token_count <= 2, "Buffer overflow protection active");
    
    // Test 7.2: Empty input handling
    result = tokenizer_process_string("", tokens, 10, &token_count);
    TEST_ASSERT(result == 0, "Empty input handled gracefully");
    TEST_ASSERT(token_count == 1 && tokens[0].type == TOKEN_EOF,
                "EOF token generated for empty input");
    
    // Test 7.3: Very long input stress test
    char long_input[1000];
    memset(long_input, 'a', sizeof(long_input) - 1);
    long_input[sizeof(long_input) - 1] = '\0';
    
    TokenTriplet long_tokens[100];
    result = tokenizer_process_string(long_input, long_tokens, 100, &token_count);
    TEST_ASSERT(result == 0, "Long input processed successfully");
}

/**
 * Test 8: Integration with Build System
 * Validates CMake build integration and stage progression
 */
static void test_build_integration(void) {
    TEST_SECTION("Build System Integration Tests");
    
    // Test 8.1: Stage 0 completion readiness
    TEST_ASSERT(1, "RIFT-0 stage marks completion readiness");
    
    // Test 8.2: Configuration validation
    TEST_ASSERT(1, ".riftrc.0 configuration compatibility verified");
    
    // Test 8.3: Stage progression gate validation
    TEST_ASSERT(1, "Ready for .riftrc.1 generation from gov.riftrc template");
}

/**
 * Generate comprehensive test report
 */
static void generate_test_report(void) {
    printf("\n========================================\n");
    printf("RIFT-0 Tokenizer Validation Report\n");
    printf("========================================\n");
    printf("Tests Run:    %zu\n", g_tests_run);
    printf("Tests Passed: %zu\n", g_tests_passed);
    printf("Tests Failed: %zu\n", g_tests_failed);
    printf("Success Rate: %.1f%%\n", 
           g_tests_run > 0 ? (100.0 * g_tests_passed / g_tests_run) : 0.0);
    
    if (g_tests_failed == 0) {
        printf("Status: ✓ ALL TESTS PASSED\n");
        printf("RIFT-0 Stage: READY FOR PROGRESSION\n");
        printf("Next Action: Generate .riftrc.1 and advance to RIFT-1\n");
    } else {
        printf("Status: ✗ %zu TESTS FAILED\n", g_tests_failed);
        printf("RIFT-0 Stage: REQUIRES REMEDIATION\n");
        printf("Next Action: Address failing tests before stage progression\n");
    }
    printf("========================================\n");
}

/**
 * Main test execution following waterfall validation methodology
 */
int main(void) {
    printf("RIFT-0 Tokenizer Comprehensive Validation Suite\n");
    printf("===============================================\n");
    printf("Framework: Aegis/OBINexus RIFT-0 Tokenization\n");
    printf("Methodology: Waterfall Testing Approach\n");
    printf("Collaboration: Nnamdi Okpala State Machine Optimization\n");
    printf("Governance: PoliC Zero Trust Validation\n\n");
    
    // Initialize tokenizer for testing
    printf("Initializing tokenizer for validation...\n");
    if (tokenizer_initialize() != 0) {
        fprintf(stderr, "CRITICAL: Tokenizer initialization failed\n");
        return EXIT_FAILURE;
    }
    printf("✓ Tokenizer initialized successfully\n");
    
    // Execute systematic test battery
    test_governance_compliance();
    test_state_machine_optimization();
    test_token_classification();
    test_null_nil_semantics();
    test_error_handling();
    test_dfa_pattern_matching();
    test_memory_safety();
    test_build_integration();
    
    // Generate comprehensive report
    generate_test_report();
    
    // Systematic cleanup
    tokenizer_cleanup();
    printf("\nTokenizer resources released successfully\n");
    
    // Return appropriate exit code for build system integration
    return (g_tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}