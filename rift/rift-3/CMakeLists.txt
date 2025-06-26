# CMakeLists.txt - rift-3
# RIFT: RIFT Is a Flexible Translator
# Stage: validation

cmake_minimum_required(VERSION 3.16)

project(rift-3
    VERSION 4.0.0
    DESCRIPTION "validation stage for RIFT compiler"
    LANGUAGES C
)

# Include common configuration
include(${CMAKE_SOURCE_DIR}/cmake/common/compiler_pipeline.cmake)

# Validate component structure
validate_rift_component(validator)

# Add RIFT stage
add_rift_stage(rift-3 validation validator)

# Add tests
add_rift_tests(rift-3)

# Stage-specific configuration
if("rift-3" STREQUAL "rift-4")
    # Bytecode generation requires additional crypto libraries
    target_compile_definitions(rift-3_static PRIVATE RIFT_BYTECODE_CRYPTO=1)
endif()

if("rift-3" STREQUAL "rift-1")
    # Parser requires thread support for dual-mode parsing
    target_compile_definitions(rift-3_static PRIVATE RIFT_DUAL_MODE_PARSING=1)
endif()

message(STATUS "rift-3 configuration complete")
