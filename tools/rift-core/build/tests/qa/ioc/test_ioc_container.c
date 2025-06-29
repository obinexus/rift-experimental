#include "rift-core/qa/ioc/rift_test_ioc.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
    printf("IoC Container Unit Tests\n");
    
    rift_test_ioc_t* container = rift_test_ioc_create();
    assert(container != NULL);
    
    bool init_result = rift_test_ioc_initialize(container);
    assert(init_result == true);
    
    // Test mock injection
    void* mock = rift_test_ioc_inject_mock(container, "tokenizer");
    assert(mock != NULL);
    free(mock);
    
    // Test stub injection
    void* stub = rift_test_ioc_inject_stub(container, "regex_matcher");
    assert(stub != NULL);
    free(stub);
    
    // Test fake injection
    void* fake = rift_test_ioc_inject_fake(container, "governance_policy");
    assert(fake != NULL);
    free(fake);
    
    rift_test_ioc_destroy(container);
    
    printf("✓ All IoC container tests passed\n");
    return 0;
}
