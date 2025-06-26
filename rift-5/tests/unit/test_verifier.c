/*
 * test_verifier.c - Unit Tests for RIFT verification
 * RIFT: RIFT Is a Flexible Translator
 * OBINexus Computing Framework - Test Implementation
 */

#include "rift-5/core/verifier.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Test initialization and cleanup */
void test_verifier_init_cleanup() {
    printf("Testing verifier initialization and cleanup...\n");
    
    rift_verifier_config_t config = {0};
    config.processing_flags = 0x01;
    config.validation_level = 3;
    
    rift_verifier_context_t *ctx = rift_verifier_init(&config);
    assert(ctx != NULL);
    assert(ctx->initialized == true);
    assert(ctx->aegis_compliant == true);
    
    rift_verifier_cleanup(ctx);
    printf("✅ Initialization and cleanup test passed\n");
}

/* Test stage processing */
void test_verifier_processing() {
    printf("Testing verifier processing...\n");
    
    rift_verifier_config_t config = {0};
    rift_verifier_context_t *ctx = rift_verifier_init(&config);
    assert(ctx != NULL);
    
    const char *input = "test input data";
    void *output = NULL;
    size_t output_size = 0;
    
    rift_verifier_result_t result = rift_verifier_process(
        ctx, input, strlen(input), &output, &output_size);
    
    assert(result == RIFT_VERIFIER_SUCCESS);
    assert(output != NULL);
    assert(output_size > 0);
    
    free(output);
    rift_verifier_cleanup(ctx);
    printf("✅ Processing test passed\n");
}

/* Test validation */
void test_verifier_validation() {
    printf("Testing verifier validation...\n");
    
    rift_verifier_config_t config = {0};
    config.validation_level = 3;
    
    rift_verifier_context_t *ctx = rift_verifier_init(&config);
    assert(ctx != NULL);
    
    rift_verifier_result_t result = rift_verifier_validate(ctx);
    assert(result == RIFT_VERIFIER_SUCCESS);
    
    rift_verifier_cleanup(ctx);
    printf("✅ Validation test passed\n");
}

int main() {
    printf("Running unit tests for RIFT verification (rift-5)\n");
    printf("========================================\n");
    
    test_verifier_init_cleanup();
    test_verifier_processing();
    test_verifier_validation();
    
    printf("\n🎉 All verifier unit tests passed!\n");
    return 0;
}
