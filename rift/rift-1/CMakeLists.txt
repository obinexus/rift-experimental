# CMakeLists.txt - rift-1
# RIFT: RIFT Is a Flexible Translator
# Stage: parsing

cmake_minimum_required(VERSION 3.16)

project(rift-1
    VERSION 4.0.0
    DESCRIPTION "parsing stage for RIFT compiler"
    LANGUAGES C
)

# Include common configuration
include(${CMAKE_SOURCE_DIR}/cmake/common/compiler_pipeline.cmake)

# Validate component structure
validate_rift_component(parser)

# Add RIFT stage
add_rift_stage(rift-1 parsing parser)

# Add tests
add_rift_tests(rift-1)

# Stage-specific configuration
if("rift-1" STREQUAL "rift-4")
    # Bytecode generation requires additional crypto libraries
    target_compile_definitions(rift-1_static PRIVATE RIFT_BYTECODE_CRYPTO=1)
endif()

if("rift-1" STREQUAL "rift-1")
    # Parser requires thread support for dual-mode parsing
    target_compile_definitions(rift-1_static PRIVATE RIFT_DUAL_MODE_PARSING=1)
endif()

message(STATUS "rift-1 configuration complete")
