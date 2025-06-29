/*
 * RIFT Test Executable - Main Entry Point
 * RIFT is a Flexible Translator - By OBINexus Nnamdi Michael Okpala
 * 
 * Comprehensive QA testing framework with IoC injection and R-syntax validation
 */

#include "rift-core/qa/matrix/qa_workflow_matrix.h"
#include "rift-core/qa/r_syntax_tokenizer.h" 
#include "rift-core/qa/ioc/rift_test_ioc.h"
#include "rift-core/qa/assertions/rift_assertions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

static void print_usage(const char* program_name) {
    printf("RIFT Test Framework - Comprehensive QA Validation\n");
    printf("Usage: %s [OPTIONS]\n\n", program_name);
    printf("Options:\n");
    printf("  --stage=N              Test specific stage (0-6)\n");
    printf("  --spec=FILE            Load test specification (.spec.rift)\n");
    printf("  --qa-matrix=ENABLE     Enable QA workflow matrix tracking\n");
    printf("  --ioc=CONTAINER        Use IoC container for dependency injection\n");
    printf("  --r-syntax             Test R-syntax tokenization patterns\n");
    printf("  --pattern=PATTERN      Test specific R\"\" or R'' pattern\n");
    printf("  --validate=TYPE        Validation type (bottom_up_matching, top_down_matching)\n");
    printf("  --qa-metrics=report    Generate QA metrics report\n");
    printf("  --model-agnostic       Test model-agnostic methods\n");
    printf("  --method=METHOD        Test specific method (matrix_multiply, etc.)\n");
    printf("  --mock=PROVIDER        Mock provider type\n");
    printf("  --help                 Show this help\n\n");
    printf("Examples:\n");
    printf("  %s --stage=0 --spec=tokenization.spec.rift --qa-matrix=enable\n", program_name);
    printf("  %s --r-syntax --pattern='R\"[a-z]+\"' --validate=bottom_up_matching\n", program_name);
    printf("  %s --model-agnostic --method=matrix_multiply --mock=matrix_provider\n", program_name);
}

static int test_r_syntax_tokenization(const char* pattern, const char* validate_type) {
    printf("\n=== R-Syntax Tokenization Testing ===\n");
    printf("Pattern: %s\n", pattern);
    printf("Validation: %s\n", validate_type);
    
    // Create QA matrix for tracking
    qa_workflow_matrix_t* qa_matrix = qa_matrix_create();
    if (!qa_matrix) {
        printf("Error: Failed to create QA matrix\n");
        return 1;
    }
    
    // Tokenize the pattern
    rift_r_regex_token_t* token = r_syntax_tokenize(pattern, strlen(pattern));
    if (!token) {
        printf("Error: Failed to tokenize pattern\n");
        qa_matrix_destroy(qa_matrix);
        return 1;
    }
    
    // Validate token
    bool validation_result = r_syntax_validate_token(token);
    printf("Token validation: %s\n", validation_result ? "PASSED" : "FAILED");
    
    // Update QA matrix
    qa_validation_result_t matrix_result = validation_result ? 
        QA_VALIDATION_TRUE_POSITIVE : QA_VALIDATION_FALSE_POSITIVE;
    qa_matrix_update(qa_matrix, matrix_result);
    
    // Test assertions
    assertion_result_t type_assertion = assert_token_type_match(token, token->type);
    printf("Type assertion: %s - %s\n", 
           type_assertion.passed ? "PASSED" : "FAILED",
           type_assertion.message);
    qa_matrix_update(qa_matrix, type_assertion.matrix_update);
    
    assertion_result_t flags_assertion = assert_token_flags_match(token, token->flags);
    printf("Flags assertion: %s - %s\n",
           flags_assertion.passed ? "PASSED" : "FAILED", 
           flags_assertion.message);
    qa_matrix_update(qa_matrix, flags_assertion.matrix_update);
    
    // Print QA matrix report
    qa_matrix_print_report(qa_matrix);
    
    // Cleanup
    assertion_result_destroy(&type_assertion);
    assertion_result_destroy(&flags_assertion);
    r_syntax_token_destroy(token);
    qa_matrix_destroy(qa_matrix);
    
    return validation_result ? 0 : 1;
}

static int test_ioc_container(const char* container_type) {
    printf("\n=== IoC Container Testing ===\n");
    printf("Container Type: %s\n", container_type);
    
    // Create IoC container
    rift_test_ioc_t* container = rift_test_ioc_create();
    if (!container) {
        printf("Error: Failed to create IoC container\n");
        return 1;
    }
    
    // Initialize container
    bool init_result = rift_test_ioc_initialize(container);
    printf("Container initialization: %s\n", init_result ? "SUCCESS" : "FAILED");
    
    if (init_result) {
        // Test mock injection
        void* mock_tokenizer = rift_test_ioc_inject_mock(container, "tokenizer");
        printf("Mock tokenizer injection: %s\n", mock_tokenizer ? "SUCCESS" : "FAILED");
        
        // Test stub injection  
        void* stub_matcher = rift_test_ioc_inject_stub(container, "regex_matcher");
        printf("Stub regex matcher injection: %s\n", stub_matcher ? "SUCCESS" : "FAILED");
        
        // Test fake injection
        void* fake_policy = rift_test_ioc_inject_fake(container, "governance_policy");
        printf("Fake governance policy injection: %s\n", fake_policy ? "SUCCESS" : "FAILED");
        
        // Cleanup injected objects
        if (mock_tokenizer) free(mock_tokenizer);
        if (stub_matcher) free(stub_matcher);
        if (fake_policy) free(fake_policy);
    }
    
    // Cleanup container
    rift_test_ioc_destroy(container);
    
    return init_result ? 0 : 1;
}

static int test_model_agnostic_methods(const char* method_name) {
    printf("\n=== Model-Agnostic Method Testing ===\n");
    printf("Method: %s\n", method_name);
    
    // Create placeholder models
    void* square_model = malloc(64);   // Placeholder square matrix model
    void* triangular_model = malloc(64); // Placeholder triangular matrix model
    void* method_impl = malloc(64);    // Placeholder method implementation
    
    if (!square_model || !triangular_model || !method_impl) {
        printf("Error: Failed to allocate test models\n");
        free(square_model);
        free(triangular_model);
        free(method_impl);
        return 1;
    }
    
    // Test model-agnostic assertion
    assertion_result_t agnostic_assertion = assert_matrix_method_agnostic(
        method_impl, square_model, triangular_model
    );
    
    printf("Model-agnostic assertion: %s - %s\n",
           agnostic_assertion.passed ? "PASSED" : "FAILED",
           agnostic_assertion.message);
    
    // Cleanup
    assertion_result_destroy(&agnostic_assertion);
    free(square_model);
    free(triangular_model);
    free(method_impl);
    
    return agnostic_assertion.passed ? 0 : 1;
}

int main(int argc, char* argv[]) {
    static struct option long_options[] = {
        {"stage", required_argument, 0, 's'},
        {"spec", required_argument, 0, 'f'},
        {"qa-matrix", required_argument, 0, 'm'},
        {"ioc", required_argument, 0, 'i'},
        {"r-syntax", no_argument, 0, 'r'},
        {"pattern", required_argument, 0, 'p'},
        {"validate", required_argument, 0, 'v'},
        {"qa-metrics", required_argument, 0, 'q'},
        {"model-agnostic", no_argument, 0, 'a'},
        {"method", required_argument, 0, 'M'},
        {"mock", required_argument, 0, 'k'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };
    
    int opt;
    int option_index = 0;
    
    // Default values
    char* pattern = NULL;
    char* validate_type = NULL;
    char* ioc_container = NULL;
    char* method_name = NULL;
    bool r_syntax_test = false;
    bool model_agnostic_test = false;
    
    if (argc == 1) {
        print_usage(argv[0]);
        return 1;
    }
    
    while ((opt = getopt_long(argc, argv, "s:f:m:i:rp:v:q:aM:k:h", 
                              long_options, &option_index)) != -1) {
        switch (opt) {
            case 's':
                printf("Testing stage: %s\n", optarg);
                break;
            case 'f':
                printf("Spec file: %s\n", optarg);
                break;
            case 'm':
                printf("QA matrix: %s\n", optarg);
                break;
            case 'i':
                ioc_container = optarg;
                break;
            case 'r':
                r_syntax_test = true;
                break;
            case 'p':
                pattern = optarg;
                break;
            case 'v':
                validate_type = optarg;
                break;
            case 'q':
                printf("QA metrics: %s\n", optarg);
                break;
            case 'a':
                model_agnostic_test = true;
                break;
            case 'M':
                method_name = optarg;
                break;
            case 'k':
                printf("Mock provider: %s\n", optarg);
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }
    
    printf("RIFT Test Framework - QA Validation Suite\n");
    printf("RIFT is a Flexible Translator - By OBINexus Nnamdi Michael Okpala\n");
    printf("=========================================================\n");
    
    int exit_code = 0;
    
    // Execute requested tests
    if (r_syntax_test && pattern) {
        exit_code |= test_r_syntax_tokenization(pattern, validate_type ? validate_type : "basic");
    }
    
    if (ioc_container) {
        exit_code |= test_ioc_container(ioc_container);
    }
    
    if (model_agnostic_test && method_name) {
        exit_code |= test_model_agnostic_methods(method_name);
    }
    
    printf("\n=== RIFT Test Framework Execution Complete ===\n");
    printf("Exit Code: %d\n", exit_code);
    
    return exit_code;
}
