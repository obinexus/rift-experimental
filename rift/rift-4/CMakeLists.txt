# CMakeLists.txt - rift-4
# RIFT: RIFT Is a Flexible Translator
# Stage: bytecode_generation

cmake_minimum_required(VERSION 3.16)

project(rift-4
    VERSION 4.0.0
    DESCRIPTION "bytecode_generation stage for RIFT compiler"
    LANGUAGES C
)

# Include common configuration
include(${CMAKE_SOURCE_DIR}/cmake/common/compiler_pipeline.cmake)

# Validate component structure
validate_rift_component(bytecode)

# Add RIFT stage
add_rift_stage(rift-4 bytecode_generation bytecode)

# Add tests
add_rift_tests(rift-4)

# Stage-specific configuration
if("rift-4" STREQUAL "rift-4")
    # Bytecode generation requires additional crypto libraries
    target_compile_definitions(rift-4_static PRIVATE RIFT_BYTECODE_CRYPTO=1)
endif()

if("rift-4" STREQUAL "rift-1")
    # Parser requires thread support for dual-mode parsing
    target_compile_definitions(rift-4_static PRIVATE RIFT_DUAL_MODE_PARSING=1)
endif()

message(STATUS "rift-4 configuration complete")
