cmake_minimum_required(VERSION 3.12)
project(RiftCoreQA VERSION 1.0.0 LANGUAGES C)

# RIFT Core QA Framework Build Configuration
# Enhanced for rifttest integration and comprehensive testing

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build options
option(BUILD_QA_TESTS "Build QA testing framework" ON)
option(BUILD_RIFTTEST "Build rifttest integration" ON)
option(BUILD_IOC_CONTAINER "Build IoC container" ON)

# Include directories
include_directories(include)

# QA Framework core library
set(QA_CORE_SOURCES
    src/qa/matrix/qa_workflow_matrix.c
    src/qa/r_syntax/r_syntax_tokenizer.c
    src/qa/ioc/rift_test_ioc.c
    src/qa/assertions/rift_assertions.c
    src/common/core.c
    src/thread/lifecycle.c
)

add_library(rift_qa_core STATIC ${QA_CORE_SOURCES})

target_include_directories(rift_qa_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# rifttest executable
if(BUILD_RIFTTEST)
    add_executable(rifttest
        src/rifttest/main.c
    )
    
    target_link_libraries(rifttest rift_qa_core)
    
    # Install rifttest
    install(TARGETS rifttest
        RUNTIME DESTINATION bin
    )
endif()

# QA Tests
if(BUILD_QA_TESTS)
    enable_testing()
    
    # R-syntax tokenization tests
    add_executable(test_r_syntax_tokenization
        tests/qa/r_syntax/test_r_syntax_tokenization.c
    )
    target_link_libraries(test_r_syntax_tokenization rift_qa_core)
    add_test(NAME RSyntaxTokenization COMMAND test_r_syntax_tokenization)
    
    # QA Matrix tests
    add_executable(test_qa_matrix
        tests/qa/matrix/test_qa_matrix.c
    )
    target_link_libraries(test_qa_matrix rift_qa_core)
    add_test(NAME QAMatrix COMMAND test_qa_matrix)
    
    # IoC Container tests
    add_executable(test_ioc_container
        tests/qa/ioc/test_ioc_container.c
    )
    target_link_libraries(test_ioc_container rift_qa_core)
    add_test(NAME IoCContainer COMMAND test_ioc_container)
    
    # Model-agnostic tests
    add_executable(test_model_agnostic
        tests/qa/model_agnostic/test_model_agnostic.c
    )
    target_link_libraries(test_model_agnostic rift_qa_core)
    add_test(NAME ModelAgnostic COMMAND test_model_agnostic)
endif()

# Install configuration
install(TARGETS rift_qa_core
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# Generate pkg-config file
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/rift-core-qa.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/rift-core-qa.pc"
    @ONLY
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/rift-core-qa.pc"
    DESTINATION lib/pkgconfig
)
