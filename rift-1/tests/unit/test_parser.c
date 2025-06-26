/*
 * test_parser.c - Unit Tests for RIFT parsing
 * RIFT: RIFT Is a Flexible Translator
 * OBINexus Computing Framework - Test Implementation
 */

#include "rift-1/core/parser.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Test initialization and cleanup */
void test_parser_init_cleanup() {
    printf("Testing parser initialization and cleanup...\n");
    
    rift_parser_config_t config = {0};
    config.processing_flags = 0x01;
    config.validation_level = 3;
    
    rift_parser_context_t *ctx = rift_parser_init(&config);
    assert(ctx != NULL);
    assert(ctx->initialized == true);
    assert(ctx->aegis_compliant == true);
    
    rift_parser_cleanup(ctx);
    printf("✅ Initialization and cleanup test passed\n");
}

/* Test stage processing */
void test_parser_processing() {
    printf("Testing parser processing...\n");
    
    rift_parser_config_t config = {0};
    rift_parser_context_t *ctx = rift_parser_init(&config);
    assert(ctx != NULL);
    
    const char *input = "test input data";
    void *output = NULL;
    size_t output_size = 0;
    
    rift_parser_result_t result = rift_parser_process(
        ctx, input, strlen(input), &output, &output_size);
    
    assert(result == RIFT_PARSER_SUCCESS);
    assert(output != NULL);
    assert(output_size > 0);
    
    free(output);
    rift_parser_cleanup(ctx);
    printf("✅ Processing test passed\n");
}

/* Test validation */
void test_parser_validation() {
    printf("Testing parser validation...\n");
    
    rift_parser_config_t config = {0};
    config.validation_level = 3;
    
    rift_parser_context_t *ctx = rift_parser_init(&config);
    assert(ctx != NULL);
    
    rift_parser_result_t result = rift_parser_validate(ctx);
    assert(result == RIFT_PARSER_SUCCESS);
    
    rift_parser_cleanup(ctx);
    printf("✅ Validation test passed\n");
}

int main() {
    printf("Running unit tests for RIFT parsing (rift-1)\n");
    printf("========================================\n");
    
    test_parser_init_cleanup();
    test_parser_processing();
    test_parser_validation();
    
    printf("\n🎉 All parser unit tests passed!\n");
    return 0;
}
