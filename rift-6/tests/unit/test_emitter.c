/*
 * test_emitter.c - Unit Tests for RIFT emission
 * RIFT: RIFT Is a Flexible Translator
 * OBINexus Computing Framework - Test Implementation
 */

#include "rift-6/core/emitter.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Test initialization and cleanup */
void test_emitter_init_cleanup() {
    printf("Testing emitter initialization and cleanup...\n");
    
    rift_emitter_config_t config = {0};
    config.processing_flags = 0x01;
    config.validation_level = 3;
    
    rift_emitter_context_t *ctx = rift_emitter_init(&config);
    assert(ctx != NULL);
    assert(ctx->initialized == true);
    assert(ctx->aegis_compliant == true);
    
    rift_emitter_cleanup(ctx);
    printf("✅ Initialization and cleanup test passed\n");
}

/* Test stage processing */
void test_emitter_processing() {
    printf("Testing emitter processing...\n");
    
    rift_emitter_config_t config = {0};
    rift_emitter_context_t *ctx = rift_emitter_init(&config);
    assert(ctx != NULL);
    
    const char *input = "test input data";
    void *output = NULL;
    size_t output_size = 0;
    
    rift_emitter_result_t result = rift_emitter_process(
        ctx, input, strlen(input), &output, &output_size);
    
    assert(result == RIFT_EMITTER_SUCCESS);
    assert(output != NULL);
    assert(output_size > 0);
    
    free(output);
    rift_emitter_cleanup(ctx);
    printf("✅ Processing test passed\n");
}

/* Test validation */
void test_emitter_validation() {
    printf("Testing emitter validation...\n");
    
    rift_emitter_config_t config = {0};
    config.validation_level = 3;
    
    rift_emitter_context_t *ctx = rift_emitter_init(&config);
    assert(ctx != NULL);
    
    rift_emitter_result_t result = rift_emitter_validate(ctx);
    assert(result == RIFT_EMITTER_SUCCESS);
    
    rift_emitter_cleanup(ctx);
    printf("✅ Validation test passed\n");
}

int main() {
    printf("Running unit tests for RIFT emission (rift-6)\n");
    printf("========================================\n");
    
    test_emitter_init_cleanup();
    test_emitter_processing();
    test_emitter_validation();
    
    printf("\n🎉 All emitter unit tests passed!\n");
    return 0;
}
