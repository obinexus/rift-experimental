/*
 * test_bytecode.c - Unit Tests for RIFT bytecode
 * RIFT: RIFT Is a Flexible Translator
 * OBINexus Computing Framework - Test Implementation
 */

#include "rift-4/core/bytecode.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Test initialization and cleanup */
void test_bytecode_init_cleanup() {
    printf("Testing bytecode initialization and cleanup...\n");
    
    rift_bytecode_config_t config = {0};
    config.processing_flags = 0x01;
    config.validation_level = 3;
    
    rift_bytecode_context_t *ctx = rift_bytecode_init(&config);
    assert(ctx != NULL);
    assert(ctx->initialized == true);
    assert(ctx->aegis_compliant == true);
    
    rift_bytecode_cleanup(ctx);
    printf("✅ Initialization and cleanup test passed\n");
}

/* Test stage processing */
void test_bytecode_processing() {
    printf("Testing bytecode processing...\n");
    
    rift_bytecode_config_t config = {0};
    rift_bytecode_context_t *ctx = rift_bytecode_init(&config);
    assert(ctx != NULL);
    
    const char *input = "test input data";
    void *output = NULL;
    size_t output_size = 0;
    
    rift_bytecode_result_t result = rift_bytecode_process(
        ctx, input, strlen(input), &output, &output_size);
    
    assert(result == RIFT_BYTECODE_SUCCESS);
    assert(output != NULL);
    assert(output_size > 0);
    
    free(output);
    rift_bytecode_cleanup(ctx);
    printf("✅ Processing test passed\n");
}

/* Test validation */
void test_bytecode_validation() {
    printf("Testing bytecode validation...\n");
    
    rift_bytecode_config_t config = {0};
    config.validation_level = 3;
    
    rift_bytecode_context_t *ctx = rift_bytecode_init(&config);
    assert(ctx != NULL);
    
    rift_bytecode_result_t result = rift_bytecode_validate(ctx);
    assert(result == RIFT_BYTECODE_SUCCESS);
    
    rift_bytecode_cleanup(ctx);
    printf("✅ Validation test passed\n");
}

int main() {
    printf("Running unit tests for RIFT bytecode (rift-4)\n");
    printf("========================================\n");
    
    test_bytecode_init_cleanup();
    test_bytecode_processing();
    test_bytecode_validation();
    
    printf("\n🎉 All bytecode unit tests passed!\n");
    return 0;
}
