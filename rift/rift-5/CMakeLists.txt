# CMakeLists.txt - rift-5
# RIFT: RIFT Is a Flexible Translator
# Stage: verification

cmake_minimum_required(VERSION 3.16)

project(rift-5
    VERSION 4.0.0
    DESCRIPTION "verification stage for RIFT compiler"
    LANGUAGES C
)

# Include common configuration
include(${CMAKE_SOURCE_DIR}/cmake/common/compiler_pipeline.cmake)

# Validate component structure
validate_rift_component(verifier)

# Add RIFT stage
add_rift_stage(rift-5 verification verifier)

# Add tests
add_rift_tests(rift-5)

# Stage-specific configuration
if("rift-5" STREQUAL "rift-4")
    # Bytecode generation requires additional crypto libraries
    target_compile_definitions(rift-5_static PRIVATE RIFT_BYTECODE_CRYPTO=1)
endif()

if("rift-5" STREQUAL "rift-1")
    # Parser requires thread support for dual-mode parsing
    target_compile_definitions(rift-5_static PRIVATE RIFT_DUAL_MODE_PARSING=1)
endif()

message(STATUS "rift-5 configuration complete")
