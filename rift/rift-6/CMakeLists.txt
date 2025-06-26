# CMakeLists.txt - rift-6
# RIFT: RIFT Is a Flexible Translator
# Stage: emission

cmake_minimum_required(VERSION 3.16)

project(rift-6
    VERSION 4.0.0
    DESCRIPTION "emission stage for RIFT compiler"
    LANGUAGES C
)

# Include common configuration
include(${CMAKE_SOURCE_DIR}/cmake/common/compiler_pipeline.cmake)

# Validate component structure
validate_rift_component(emitter)

# Add RIFT stage
add_rift_stage(rift-6 emission emitter)

# Add tests
add_rift_tests(rift-6)

# Stage-specific configuration
if("rift-6" STREQUAL "rift-4")
    # Bytecode generation requires additional crypto libraries
    target_compile_definitions(rift-6_static PRIVATE RIFT_BYTECODE_CRYPTO=1)
endif()

if("rift-6" STREQUAL "rift-1")
    # Parser requires thread support for dual-mode parsing
    target_compile_definitions(rift-6_static PRIVATE RIFT_DUAL_MODE_PARSING=1)
endif()

message(STATUS "rift-6 configuration complete")
