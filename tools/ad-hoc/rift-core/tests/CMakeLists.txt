# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Unit tests
add_executable(test_core
    unit/test_core.c
    unit/test_thread_lifecycle.c
    unit/test_audit_tracer.c
)

target_link_libraries(test_core rift-core)

# Integration tests
add_executable(test_integration
    integration/test_stage_pipeline.c
)

target_link_libraries(test_integration rift-core)

# Add tests to CTest
add_test(NAME CoreTests COMMAND test_core)
add_test(NAME IntegrationTests COMMAND test_integration)
