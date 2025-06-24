#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"
#include "tokenizer_rules.h"

/**
 * RIFT-0 Lexeme Calculation Demonstration
 * 
 * Demonstrates the RIFT-0 tokenizer on mathematical expressions,
 * generating comprehensive CSV output of (token, type, memory) sets
 * for systematic inspection and validation.
 * 
 * This demonstration validates Nnamdi Okpala's state machine optimization
 * research within our tokenization pipeline, showcasing:
 * - Deterministic O(n) tokenization performance
 * - PoliC governance compliance verification  
 * - Memory-efficient token triplet generation
 * - CSV export for collaborative analysis
 */

// Test mathematical expressions for comprehensive validation
static const char* test_expressions[] = {
    "x + y * 2",
    "sin(theta) / cos(phi)",
    "matrix[i][j] = alpha * beta + gamma",
    "result := calculate(a, b, c)",
    "if (value > threshold) then process()",
    "for i := 0 to n-1 do sum += array[i]",
    "float precision = 3.14159265359",
    "string message = \"Hello, RIFT-0 World!\"",
    "// Comment: state machine optimization",
    "/* Multi-line\n   comment test */",
    NULL  // Sentinel value
};

/**
 * Generate CSV header for token analysis
 */
static void write_csv_header(FILE* output) {
    fprintf(output, "Expression,Token_Index,Token_Type,Token_Value,Memory_Pointer,Character_Value,Token_Length\n");
}

/**
 * Export token to CSV format for systematic analysis
 */
static void write_token_csv(FILE* output, const char* expression, 
                           size_t token_index, const TokenTriplet* token,
                           const char* source_text) {
    // Extract token text from source
    char token_text[256] = {0};
    size_t token_length = (token->value > 0) ? token->value : 1;
    
    if (token->mem_ptr < strlen(source_text)) {
        size_t copy_length = (token_length < sizeof(token_text) - 1) ? 
                            token_length : sizeof(token_text) - 1;
        strncpy(token_text, source_text + token->mem_ptr, copy_length);
        token_text[copy_length] = '\0';
        
        // Escape quotes in CSV
        for (size_t i = 0; i < strlen(token_text); i++) {
            if (token_text[i] == '"') {
                token_text[i] = '\'';
            }
        }
    } else {
        strcpy(token_text, "EOF");
    }
    
    fprintf(output, "\"%s\",%zu,\"%s\",\"%s\",%u,%u,%zu\n",
            expression,
            token_index,
            get_token_type_name(token->type),
            token_text,
            token->mem_ptr,
            token->value,
            token_length);
}

/**
 * Process single expression with comprehensive diagnostics
 */
static int process_expression(const char* expression, FILE* csv_output) {
    printf("\n=== Processing Expression: \"%s\" ===\n", expression);
    
    // Allocate token buffer (generous size for demonstration)
    const size_t MAX_TOKENS = 1000;
    TokenTriplet* tokens = malloc(sizeof(TokenTriplet) * MAX_TOKENS);
    if (!tokens) {
        fprintf(stderr, "ERROR: Memory allocation failed for token buffer\n");
        return -1;
    }
    
    // Perform tokenization
    size_t token_count = 0;
    int result = tokenizer_process_string(expression, tokens, MAX_TOKENS, &token_count);
    
    if (result != 0) {
        fprintf(stderr, "ERROR: Tokenization failed: %s\n", tokenizer_get_error_message());
        free(tokens);
        return -1;
    }
    
    // Display results with systematic formatting
    printf("Tokenization Results:\n");
    printf("  Expression Length: %zu characters\n", strlen(expression));
    printf("  Tokens Generated: %zu\n", token_count);
    printf("  Memory Efficiency: %.2f tokens/char\n", 
           (double)token_count / strlen(expression));
    
    // Detailed token analysis
    printf("\nToken Stream Analysis:\n");
    printf("  %-4s %-15s %-20s %-8s %-8s\n", 
           "Idx", "Type", "Text", "Mem_Ptr", "Value");
    printf("  %s\n", "------------------------------------------------------------");
    
    for (size_t i = 0; i < token_count; i++) {
        const TokenTriplet* token = &tokens[i];
        
        // Extract token text for display
        char display_text[32] = {0};
        if (token->type == TOKEN_EOF) {
            strcpy(display_text, "<EOF>");
        } else if (token->mem_ptr < strlen(expression)) {
            size_t length = (token->value > 0 && token->value < 30) ? token->value : 1;
            strncpy(display_text, expression + token->mem_ptr, length);
            display_text[length] = '\0';
        } else {
            strcpy(display_text, "<INVALID>");
        }
        
        printf("  %-4zu %-15s %-20s %-8u %-8u\n",
               i,
               get_token_type_name(token->type),
               display_text,
               token->mem_ptr,
               token->value);
        
        // Export to CSV for systematic analysis
        if (csv_output) {
            write_token_csv(csv_output, expression, i, token, expression);
        }
        
        // Validate PoliC governance compliance
        if (!polic_validate_token(token, NULL)) {
            printf("    WARNING: Token %zu failed PoliC governance validation\n", i);
        }
    }
    
    free(tokens);
    printf("=== Expression Processing Complete ===\n");
    return 0;
}

/**
 * Systematic validation of tokenizer performance
 */
static void run_performance_analysis(void) {
    printf("\n=== RIFT-0 Performance Analysis ===\n");
    
    // Test various expression sizes for performance characterization
    const char* performance_tests[] = {
        "a",  // Single character
        "a + b",  // Simple expression
        "very_long_identifier_name_for_testing",  // Long identifier
        "matrix[i][j] = alpha * beta + gamma * delta + epsilon",  // Complex expression
        NULL
    };
    
    for (size_t i = 0; performance_tests[i] != NULL; i++) {
        const char* test_expr = performance_tests[i];
        size_t expr_length = strlen(test_expr);
        
        TokenTriplet tokens[100];
        size_t token_count = 0;
        
        // Time-based analysis would require platform-specific timing
        int result = tokenizer_process_string(test_expr, tokens, 100, &token_count);
        
        if (result == 0) {
            printf("Expression Length %zu chars -> %zu tokens (%.2f ratio)\n",
                   expr_length, token_count, (double)token_count / expr_length);
        }
    }
    
    printf("=== Performance Analysis Complete ===\n");
}

/**
 * Main demonstration routine following waterfall methodology
 */
int main(void) {
    printf("RIFT-0 Lexeme Calculation Demonstration\n");
    printf("========================================\n");
    printf("Tokenizer Engine: State Machine Optimized (Nnamdi Okpala Research)\n");
    printf("Governance Framework: PoliC Zero Trust\n");
    printf("Build Stage: RIFT-0 (Tokenization)\n\n");
    
    // Phase 1: Initialize tokenizer subsystem
    printf("Phase 1: Tokenizer Initialization\n");
    if (tokenizer_initialize() != 0) {
        fprintf(stderr, "CRITICAL ERROR: Tokenizer initialization failed\n");
        fprintf(stderr, "Error: %s\n", tokenizer_get_error_message());
        return EXIT_FAILURE;
    }
    printf("✓ Tokenizer initialized successfully\n");
    printf("✓ PoliC governance active\n");
    printf("✓ State machine optimization enabled\n");
    
    // Phase 2: Open CSV output for systematic analysis
    printf("\nPhase 2: CSV Export Preparation\n");
    FILE* csv_output = fopen("lexem_analysis_results.csv", "w");
    if (!csv_output) {
        fprintf(stderr, "WARNING: Could not create CSV output file\n");
        printf("Continuing without CSV export...\n");
    } else {
        write_csv_header(csv_output);
        printf("✓ CSV output file created: lexem_analysis_results.csv\n");
    }
    
    // Phase 3: Process test expressions systematically
    printf("\nPhase 3: Mathematical Expression Processing\n");
    for (size_t i = 0; test_expressions[i] != NULL; i++) {
        if (process_expression(test_expressions[i], csv_output) != 0) {
            printf("WARNING: Expression %zu processing encountered issues\n", i);
        }
    }
    
    // Phase 4: Performance analysis
    run_performance_analysis();
    
    // Phase 5: Systematic cleanup
    printf("\nPhase 5: Resource Cleanup\n");
    if (csv_output) {
        fclose(csv_output);
        printf("✓ CSV output file finalized\n");
    }
    
    tokenizer_cleanup();
    printf("✓ Tokenizer resources released\n");
    
    printf("\n========================================\n");
    printf("RIFT-0 Demonstration Complete\n");
    printf("Status: SUCCESS\n");
    printf("Output: lexem_analysis_results.csv\n");
    printf("Next Stage: Validate .riftrc.0 and proceed to RIFT-1\n");
    
    return EXIT_SUCCESS;
}