/**
 * =================================================================
 * test_policy2_matrix.c - RIFT-0 Policy2 QA Matrix Implementation
 * RIFT: RIFT Is a Flexible Translator
 * Component: Systematic validation framework for tokenization accuracy
 * OBINexus Computing Framework - Stage 0 Implementation
 * 
 * This module implements the Policy2 QA matrix testing methodology
 * using four truth categories for comprehensive validation:
 * - truePositive: Valid input yields correct token triplet
 * - falseNegative: Valid input fails to match rule (should match)
 * - trueNegative: Invalid input correctly rejected
 * - falsePositive: Invalid input incorrectly accepted
 * =================================================================
 */

#include "rift-0/core/tokenizer_rules.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>

/* QA Test Result Structure */
typedef struct {
    char test_name[128];
    char input_text[256];
    TokenType expected_type;
    char category[32];
    bool passed;
    double execution_time_ms;
    char error_message[512];
} QATestResult;

/* QA Matrix Statistics */
typedef struct {
    int true_positives;
    int false_negatives;
    int true_negatives;
    int false_positives;
    int total_tests;
    double total_execution_time;
    double average_execution_time;
    double max_execution_time;
    double min_execution_time;
} QAMatrixStats;

/* Global test results storage */
static QATestResult g_test_results[1000];
static size_t g_test_count = 0;
static QAMatrixStats g_stats = {0};
static pthread_mutex_t g_test_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * Policy2 Test Case Definitions
 * Comprehensive test matrix covering all token types and edge cases
 */
static struct {
    const char* name;
    const char* input;
    TokenType expected_type;
    const char* category;
    const char* description;
} policy2_test_cases[] = {
    /* True Positive Tests - Valid input produces correct tokens */
    {"ID_SIMPLE", "identifier", TOKEN_IDENTIFIER, "truePositive", 
     "Simple identifier tokenization"},
    {"ID_UNDERSCORE", "_private_var", TOKEN_IDENTIFIER, "truePositive", 
     "Identifier with underscores"},
    {"ID_ALPHANUMERIC", "var123", TOKEN_IDENTIFIER, "truePositive", 
     "Alphanumeric identifier"},
    {"NULL_KEYWORD", "NULL", TOKEN_NULL_KEYWORD, "truePositive", 
     "NULL keyword recognition"},
    {"NIL_KEYWORD", "nil", TOKEN_NIL_KEYWORD, "truePositive", 
     "nil keyword recognition"},
    {"NUMBER_INTEGER", "42", TOKEN_LITERAL_NUMBER, "truePositive", 
     "Integer literal"},
    {"NUMBER_FLOAT", "3.14159", TOKEN_LITERAL_NUMBER, "truePositive", 
     "Floating point literal"},
    {"OPERATOR_PLUS", "+", TOKEN_OPERATOR, "truePositive", 
     "Plus operator"},
    {"OPERATOR_MINUS", "-", TOKEN_OPERATOR, "truePositive", 
     "Minus operator"},
    {"OPERATOR_MULTIPLY", "*", TOKEN_OPERATOR, "truePositive", 
     "Multiply operator"},
    {"OPERATOR_DIVIDE", "/", TOKEN_OPERATOR, "truePositive", 
     "Divide operator"},
    {"OPERATOR_ASSIGN", "=", TOKEN_OPERATOR, "truePositive", 
     "Assignment operator"},
    {"DELIMITER_LPAREN", "(", TOKEN_DELIMITER, "truePositive", 
     "Left parenthesis"},
    {"DELIMITER_RPAREN", ")", TOKEN_DELIMITER, "truePositive", 
     "Right parenthesis"},
    {"DELIMITER_LBRACE", "{", TOKEN_DELIMITER, "truePositive", 
     "Left brace"},
    {"DELIMITER_RBRACE", "}", TOKEN_DELIMITER, "truePositive", 
     "Right brace"},
    {"DELIMITER_SEMICOLON", ";", TOKEN_DELIMITER, "truePositive", 
     "Semicolon delimiter"},
    {"WHITESPACE_SPACE", " ", TOKEN_WHITESPACE, "truePositive", 
     "Space character"},
    {"WHITESPACE_TAB", "\t", TOKEN_WHITESPACE, "truePositive", 
     "Tab character"},
    {"WHITESPACE_NEWLINE", "\n", TOKEN_WHITESPACE, "truePositive", 
     "Newline character"},
     
    /* R-Pattern Tests */
    {"R_PATTERN_SIMPLE", "R\"delimiter(content)delimiter\"", TOKEN_R_PATTERN, "truePositive",
     "Simple R-pattern with delimiter"},
    {"R_PATTERN_NESTED", "R\"abc(nested(content))abc\"", TOKEN_R_PATTERN, "truePositive",
     "R-pattern with nested delimiters"},
     
    /* True Negative Tests - Invalid input correctly rejected */
    {"INVALID_NUMBER_START", "123abc", TOKEN_LITERAL_NUMBER, "trueNegative", 
     "Number cannot start with invalid characters"},
    {"INVALID_IDENTIFIER_START", "123var", TOKEN_IDENTIFIER, "trueNegative", 
     "Identifier cannot start with digit"},
    {"INVALID_OPERATOR", "@", TOKEN_OPERATOR, "trueNegative", 
     "Invalid operator character"},
    {"EMPTY_INPUT", "", TOKEN_IDENTIFIER, "trueNegative", 
     "Empty input should not match identifier"},
    {"NULL_INPUT", NULL, TOKEN_IDENTIFIER, "trueNegative", 
     "NULL input should be rejected"},
     
    /* False Negative Tests - Valid input that should match but doesn't */
    {"UNICODE_IDENTIFIER", "variableñ", TOKEN_IDENTIFIER, "falseNegative", 
     "Unicode identifiers should be supported"},
    {"SCIENTIFIC_NOTATION", "1.23e-4", TOKEN_LITERAL_NUMBER, "falseNegative", 
     "Scientific notation should be recognized"},
     
    /* False Positive Tests - Invalid input incorrectly accepted */
    {"MALFORMED_R_PATTERN", "R\"mismatched(content)wrong\"", TOKEN_R_PATTERN, "falsePositive",
     "Malformed R-pattern with mismatched delimiters"},
    {"INCOMPLETE_OPERATOR", "++", TOKEN_OPERATOR, "falsePositive",
     "Incomplete compound operator"},
     
    /* Edge Cases and Boundary Conditions */
    {"MAX_IDENTIFIER_LENGTH", 
     "very_long_identifier_name_that_exceeds_normal_expectations_but_should_still_work_correctly",
     TOKEN_IDENTIFIER, "truePositive", "Maximum length identifier"},
    {"ZERO_NUMBER", "0", TOKEN_LITERAL_NUMBER, "truePositive", "Zero literal"},
    {"NEGATIVE_NUMBER", "-42", TOKEN_LITERAL_NUMBER, "truePositive", "Negative number"},
    {"HEX_NUMBER", "0xFF", TOKEN_LITERAL_NUMBER, "truePositive", "Hexadecimal literal"},
    {"BINARY_NUMBER", "0b1010", TOKEN_LITERAL_NUMBER, "truePositive", "Binary literal"},
    
    /* Case Sensitivity Tests */
    {"CASE_NULL_UPPER", "NULL", TOKEN_NULL_KEYWORD, "truePositive", "Uppercase NULL"},
    {"CASE_NULL_LOWER", "null", TOKEN_NULL_KEYWORD, "falseNegative", "Lowercase null"},
    {"CASE_NIL_LOWER", "nil", TOKEN_NIL_KEYWORD, "truePositive", "Lowercase nil"},
    {"CASE_NIL_UPPER", "NIL", TOKEN_NIL_KEYWORD, "falseNegative", "Uppercase NIL"},
    
    /* Multi-character sequences */
    {"COMPOUND_OPERATOR_EQ", "==", TOKEN_OPERATOR, "truePositive", "Equality operator"},
    {"COMPOUND_OPERATOR_NE", "!=", TOKEN_OPERATOR, "truePositive", "Inequality operator"},
    {"COMPOUND_OPERATOR_LE", "<=", TOKEN_OPERATOR, "truePositive", "Less-equal operator"},
    {"COMPOUND_OPERATOR_GE", ">=", TOKEN_OPERATOR, "truePositive", "Greater-equal operator"},
    
    /* Terminator */
    {NULL, NULL, TOKEN_UNKNOWN, NULL, NULL}
};

/**
 * Execute individual QA test case
 */
static bool execute_qa_test(const char* test_name, const char* input, 
                           TokenType expected_type, const char* category) {
    if (!input && strcmp(category, "trueNegative") != 0) {
        // NULL input only valid for true negative tests
        return false;
    }
    
    clock_t start_time = clock();
    
    // Initialize test result
    QATestResult result = {0};
    strncpy(result.test_name, test_name, sizeof(result.test_name) - 1);
    if (input) {
        strncpy(result.input_text, input, sizeof(result.input_text) - 1);
    }
    result.expected_type = expected_type;
    strncpy(result.category, category, sizeof(result.category) - 1);
    
    bool test_passed = false;
    
    if (!input) {
        // NULL input test
        PatternMatchResult match_result;
        int match_status = match_token_pattern(NULL, "", 0, &match_result);
        
        if (strcmp(category, "trueNegative") == 0) {
            // Should fail for NULL input
            test_passed = (match_status != 0 || !match_result.success);
        } else {
            test_passed = false; // NULL should not be accepted for other categories
        }
    } else {
        // Regular tokenization test
        TokenTriplet tokens[10];
        size_t token_count = 0;
        
        int tokenize_result = tokenize_source(input, tokens, 10, &token_count);
        
        if (strcmp(category, "truePositive") == 0) {
            // Should successfully tokenize and match expected type
            test_passed = (tokenize_result == 0 && token_count > 0 && 
                          tokens[0].type == expected_type);
                          
        } else if (strcmp(category, "trueNegative") == 0) {
            // Should either fail to tokenize or produce different type
            test_passed = (tokenize_result != 0 || token_count == 0 || 
                          tokens[0].type != expected_type);
                          
        } else if (strcmp(category, "falseNegative") == 0) {
            // This input should match but currently doesn't (known limitation)
            test_passed = (tokenize_result != 0 || token_count == 0 || 
                          tokens[0].type != expected_type);
            // For false negatives, we expect failure (test passes if it fails to match)
            
        } else if (strcmp(category, "falsePositive") == 0) {
            // This input should not match but currently does (bug)
            test_passed = (tokenize_result == 0 && token_count > 0 && 
                          tokens[0].type == expected_type);
            // For false positives, we expect incorrect success (test passes if it incorrectly matches)
        }
    }
    
    clock_t end_time = clock();
    result.execution_time_ms = ((double)(end_time - start_time)) / CLOCKS_PER_SEC * 1000.0;
    result.passed = test_passed;
    
    if (!test_passed) {
        snprintf(result.error_message, sizeof(result.error_message),
                "Test failed: expected %s behavior for type %d", category, expected_type);
    }
    
    // Store result (thread-safe)
    pthread_mutex_lock(&g_test_mutex);
    if (g_test_count < sizeof(g_test_results) / sizeof(g_test_results[0])) {
        g_test_results[g_test_count++] = result;
    }
    pthread_mutex_unlock(&g_test_mutex);
    
    return test_passed;
}

/**
 * Calculate QA matrix statistics
 */
static void calculate_qa_statistics(void) {
    memset(&g_stats, 0, sizeof(g_stats));
    
    for (size_t i = 0; i < g_test_count; i++) {
        const QATestResult* result = &g_test_results[i];
        
        g_stats.total_tests++;
        g_stats.total_execution_time += result->execution_time_ms;
        
        if (g_stats.max_execution_time < result->execution_time_ms) {
            g_stats.max_execution_time = result->execution_time_ms;
        }
        
        if (g_stats.min_execution_time == 0 || g_stats.min_execution_time > result->execution_time_ms) {
            g_stats.min_execution_time = result->execution_time_ms;
        }
        
        if (result->passed) {
            if (strcmp(result->category, "truePositive") == 0) {
                g_stats.true_positives++;
            } else if (strcmp(result->category, "falseNegative") == 0) {
                g_stats.false_negatives++;
            } else if (strcmp(result->category, "trueNegative") == 0) {
                g_stats.true_negatives++;
            } else if (strcmp(result->category, "falsePositive") == 0) {
                g_stats.false_positives++;
            }
        }
    }
    
    if (g_stats.total_tests > 0) {
        g_stats.average_execution_time = g_stats.total_execution_time / g_stats.total_tests;
    }
}

/**
 * Print detailed QA matrix report
 */
static void print_qa_matrix_report(bool verbose) {
    printf("\n=================================================================\n");
    printf("RIFT-0 Policy2 QA Matrix Validation Report\n");
    printf("=================================================================\n\n");
    
    calculate_qa_statistics();
    
    printf("Test Summary:\n");
    printf("  Total Tests:       %d\n", g_stats.total_tests);
    printf("  True Positives:    %d\n", g_stats.true_positives);
    printf("  False Negatives:   %d\n", g_stats.false_negatives);
    printf("  True Negatives:    %d\n", g_stats.true_negatives);
    printf("  False Positives:   %d\n", g_stats.false_positives);
    printf("\n");
    
    printf("Performance Metrics:\n");
    printf("  Total Time:        %.2f ms\n", g_stats.total_execution_time);
    printf("  Average Time:      %.2f ms\n", g_stats.average_execution_time);
    printf("  Max Time:          %.2f ms\n", g_stats.max_execution_time);
    printf("  Min Time:          %.2f ms\n", g_stats.min_execution_time);
    printf("\n");
    
    // Calculate accuracy metrics
    int total_correct = g_stats.true_positives + g_stats.true_negatives;
    int total_incorrect = g_stats.false_negatives + g_stats.false_positives;
    double accuracy = (double)total_correct / (total_correct + total_incorrect) * 100.0;
    
    printf("Accuracy Analysis:\n");
    printf("  Correct Classifications: %d\n", total_correct);
    printf("  Incorrect Classifications: %d\n", total_incorrect);
    printf("  Overall Accuracy: %.1f%%\n", accuracy);
    printf("\n");
    
    if (verbose) {
        printf("Detailed Test Results:\n");
        printf("%-30s %-15s %-20s %-15s %-8s %-10s\n", 
               "Test Name", "Category", "Input", "Expected", "Result", "Time (ms)");
        printf("%-30s %-15s %-20s %-15s %-8s %-10s\n", 
               "------------------------------", "---------------", "--------------------", 
               "---------------", "--------", "----------");
        
        for (size_t i = 0; i < g_test_count; i++) {
            const QATestResult* result = &g_test_results[i];
            
            char input_display[21];
            if (strlen(result->input_text) == 0) {
                strcpy(input_display, "<empty>");
            } else if (strlen(result->input_text) > 20) {
                strncpy(input_display, result->input_text, 17);
                strcpy(input_display + 17, "...");
            } else {
                strcpy(input_display, result->input_text);
            }
            
            printf("%-30s %-15s %-20s %-15d %-8s %-10.2f\n",
                   result->test_name,
                   result->category,
                   input_display,
                   result->expected_type,
                   result->passed ? "PASS" : "FAIL",
                   result->execution_time_ms);
                   
            if (!result->passed && strlen(result->error_message) > 0) {
                printf("    Error: %s\n", result->error_message);
            }
        }
    }
    
    printf("\n=================================================================\n");
}

/**
 * Public interface: Policy2 QA validation for single test case
 */
int policy2_qa_validate(const char* test_input, TokenType expected_type, 
                       const char* test_category) {
    if (!test_category) {
        return -1;
    }
    
    char test_name[64];
    snprintf(test_name, sizeof(test_name), "SINGLE_TEST_%zu", g_test_count);
    
    bool result = execute_qa_test(test_name, test_input, expected_type, test_category);
    return result ? 0 : -1;
}

/**
 * Public interface: Run complete Policy2 QA matrix
 */
int run_policy2_qa_matrix(bool verbose) {
    printf("RIFT-0 Policy2 QA Matrix - Initializing tokenizer...\n");
    
    // Initialize tokenizer
    int init_result = init_tokenizer_rules();
    if (init_result != 0) {
        fprintf(stderr, "Error: Failed to initialize tokenizer for QA testing\n");
        return -1;
    }
    
    printf("Running Policy2 QA matrix tests...\n\n");
    
    g_test_count = 0;
    int passed = 0;
    int failed = 0;
    
    // Execute all test cases
    for (int i = 0; policy2_test_cases[i].name != NULL; i++) {
        const char* name = policy2_test_cases[i].name;
        const char* input = policy2_test_cases[i].input;
        TokenType expected = policy2_test_cases[i].expected_type;
        const char* category = policy2_test_cases[i].category;
        
        if (verbose) {
            printf("Executing test: %s (%s)\n", name, category);
        }
        
        bool test_result = execute_qa_test(name, input, expected, category);
        
        if (test_result) {
            passed++;
            if (verbose) printf("  PASS\n");
        } else {
            failed++;
            if (verbose) printf("  FAIL\n");
        }
    }
    
    // Print comprehensive report
    print_qa_matrix_report(verbose);
    
    printf("QA Matrix Execution Complete:\n");
    printf("  Tests Passed: %d\n", passed);
    printf("  Tests Failed: %d\n", failed);
    printf("  Success Rate: %.1f%%\n", (double)passed / (passed + failed) * 100.0);
    
    // Cleanup
    cleanup_tokenizer_rules();
    
    return (failed == 0) ? 0 : -1;
}

/**
 * Export QA results to CSV for analysis
 */
int export_qa_results_csv(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s' for writing\n", filename);
        return -1;
    }
    
    // Write CSV header
    fprintf(file, "test_name,category,input_text,expected_type,passed,execution_time_ms,error_message\n");
    
    // Write test results
    for (size_t i = 0; i < g_test_count; i++) {
        const QATestResult* result = &g_test_results[i];
        
        fprintf(file, "\"%s\",\"%s\",\"%s\",%d,%s,%.2f,\"%s\"\n",
                result->test_name,
                result->category,
                result->input_text,
                result->expected_type,
                result->passed ? "true" : "false",
                result->execution_time_ms,
                result->error_message);
    }
    
    fclose(file);
    printf("QA results exported to: %s\n", filename);
    return 0;
}

/**
 * Main function for standalone QA testing
 */
#ifdef QA_STANDALONE
int main(int argc, char* argv[]) {
    bool verbose = false;
    const char* csv_output = NULL;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            csv_output = argv[++i];
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printf("Usage: %s [-v|--verbose] [-o output.csv] [-h|--help]\n", argv[0]);
            printf("  -v, --verbose    Enable verbose output\n");
            printf("  -o FILE          Export results to CSV file\n");
            printf("  -h, --help       Show this help\n");
            return 0;
        }
    }
    
    // Run QA matrix
    int result = run_policy2_qa_matrix(verbose);
    
    // Export results if requested
    if (csv_output) {
        export_qa_results_csv(csv_output);
    }
    
    return result;
}
#endif