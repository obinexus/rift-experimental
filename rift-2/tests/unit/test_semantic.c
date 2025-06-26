/*
 * test_semantic.c - Unit Tests for RIFT semantic
 * RIFT: RIFT Is a Flexible Translator
 * OBINexus Computing Framework - Test Implementation
 */

#include "rift-2/core/semantic.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Test initialization and cleanup */
void test_semantic_init_cleanup() {
    printf("Testing semantic initialization and cleanup...\n");
    
    rift_semantic_config_t config = {0};
    config.processing_flags = 0x01;
    config.validation_level = 3;
    
    rift_semantic_context_t *ctx = rift_semantic_init(&config);
    assert(ctx != NULL);
    assert(ctx->initialized == true);
    assert(ctx->aegis_compliant == true);
    
    rift_semantic_cleanup(ctx);
    printf("✅ Initialization and cleanup test passed\n");
}

/* Test stage processing */
void test_semantic_processing() {
    printf("Testing semantic processing...\n");
    
    rift_semantic_config_t config = {0};
    rift_semantic_context_t *ctx = rift_semantic_init(&config);
    assert(ctx != NULL);
    
    const char *input = "test input data";
    void *output = NULL;
    size_t output_size = 0;
    
    rift_semantic_result_t result = rift_semantic_process(
        ctx, input, strlen(input), &output, &output_size);
    
    assert(result == RIFT_SEMANTIC_SUCCESS);
    assert(output != NULL);
    assert(output_size > 0);
    
    free(output);
    rift_semantic_cleanup(ctx);
    printf("✅ Processing test passed\n");
}

/* Test validation */
void test_semantic_validation() {
    printf("Testing semantic validation...\n");
    
    rift_semantic_config_t config = {0};
    config.validation_level = 3;
    
    rift_semantic_context_t *ctx = rift_semantic_init(&config);
    assert(ctx != NULL);
    
    rift_semantic_result_t result = rift_semantic_validate(ctx);
    assert(result == RIFT_SEMANTIC_SUCCESS);
    
    rift_semantic_cleanup(ctx);
    printf("✅ Validation test passed\n");
}

int main() {
    printf("Running unit tests for RIFT semantic (rift-2)\n");
    printf("========================================\n");
    
    test_semantic_init_cleanup();
    test_semantic_processing();
    test_semantic_validation();
    
    printf("\n🎉 All semantic unit tests passed!\n");
    return 0;
}
