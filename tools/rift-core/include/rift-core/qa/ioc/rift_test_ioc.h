/*
 * RIFT Testing IoC Container for Dependency Injection
 * Supports mocks, stubs, and fakes for comprehensive testing
 */

#ifndef RIFT_TEST_IOC_H
#define RIFT_TEST_IOC_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct mock_repository mock_repository_t;
typedef struct stub_factory stub_factory_t;
typedef struct fake_provider fake_provider_t;
typedef struct governance_injector governance_injector_t;

// IoC Container for rifttest
typedef struct {
    mock_repository_t* mock_repo;
    stub_factory_t* stub_factory;
    fake_provider_t* fake_provider;
    governance_injector_t* gov_injector;
    
    // Container state
    bool initialized;
    uint32_t injection_count;
} rift_test_ioc_t;

// Test specification structure
typedef struct {
    char* test_name;
    char* test_pattern;
    uint32_t expected_type;
    uint32_t expected_flags;
    void* test_data;
} test_specification_t;

// Test execution result
typedef struct {
    bool success;
    uint32_t assertion_count;
    uint32_t failure_count;
    char* error_message;
    void* result_data;
} test_execution_result_t;

// IoC Container operations
rift_test_ioc_t* rift_test_ioc_create(void);
void rift_test_ioc_destroy(rift_test_ioc_t* container);
bool rift_test_ioc_initialize(rift_test_ioc_t* container);

// Mock injection
void* rift_test_ioc_inject_mock(rift_test_ioc_t* container, const char* mock_type);
void* rift_test_ioc_inject_stub(rift_test_ioc_t* container, const char* stub_type);
void* rift_test_ioc_inject_fake(rift_test_ioc_t* container, const char* fake_type);

// Test execution with IoC
test_execution_result_t rift_test_ioc_execute_test(
    rift_test_ioc_t* container,
    test_specification_t* spec
);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_TEST_IOC_H */
