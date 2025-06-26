/*
 * test_validator.c - Unit Tests for RIFT validation
 * RIFT: RIFT Is a Flexible Translator
 * OBINexus Computing Framework - Test Implementation
 */

#include "rift-3/core/validator.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Test initialization and cleanup */
void test_validator_init_cleanup() {
    printf("Testing validator initialization and cleanup...\n");
    
    rift_validator_config_t config = {0};
    config.processing_flags = 0x01;
    config.validation_level = 3;
    
    rift_validator_context_t *ctx = rift_validator_init(&config);
    assert(ctx != NULL);
    assert(ctx->initialized == true);
    assert(ctx->aegis_compliant == true);
    
    rift_validator_cleanup(ctx);
    printf("✅ Initialization and cleanup test passed\n");
}

/* Test stage processing */
void test_validator_processing() {
    printf("Testing validator processing...\n");
    
    rift_validator_config_t config = {0};
    rift_validator_context_t *ctx = rift_validator_init(&config);
    assert(ctx != NULL);
    
    const char *input = "test input data";
    void *output = NULL;
    size_t output_size = 0;
    
    rift_validator_result_t result = rift_validator_process(
        ctx, input, strlen(input), &output, &output_size);
    
    assert(result == RIFT_VALIDATOR_SUCCESS);
    assert(output != NULL);
    assert(output_size > 0);
    
    free(output);
    rift_validator_cleanup(ctx);
    printf("✅ Processing test passed\n");
}

/* Test validation */
void test_validator_validation() {
    printf("Testing validator validation...\n");
    
    rift_validator_config_t config = {0};
    config.validation_level = 3;
    
    rift_validator_context_t *ctx = rift_validator_init(&config);
    assert(ctx != NULL);
    
    rift_validator_result_t result = rift_validator_validate(ctx);
    assert(result == RIFT_VALIDATOR_SUCCESS);
    
    rift_validator_cleanup(ctx);
    printf("✅ Validation test passed\n");
}

int main() {
    printf("Running unit tests for RIFT validation (rift-3)\n");
    printf("========================================\n");
    
    test_validator_init_cleanup();
    test_validator_processing();
    test_validator_validation();
    
    printf("\n🎉 All validator unit tests passed!\n");
    return 0;
}
