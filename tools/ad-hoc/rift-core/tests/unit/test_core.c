#include "rift-core/core.h"
#include <assert.h>
#include <stdio.h>

void test_core_init() {
    assert(rift_core_init() == RIFT_SUCCESS);
    rift_core_cleanup();
    printf("✓ Core initialization test passed\n");
}

void test_context_create() {
    rift_context_t ctx;
    assert(rift_context_create(&ctx) == RIFT_SUCCESS);
    assert(ctx.id != 0);
    rift_context_destroy(&ctx);
    printf("✓ Context creation test passed\n");
}

void test_yoda_conditions() {
    int value = 42;
    assert(RIFT_YODA_EQ(42, value) == true);
    assert(RIFT_YODA_NE(0, value) == true);
    printf("✓ Yoda-style condition test passed\n");
}

int main() {
    printf("Running RIFT Core tests...\n");
    
    test_core_init();
    test_context_create();
    test_yoda_conditions();
    
    printf("All tests passed!\n");
    return 0;
}
