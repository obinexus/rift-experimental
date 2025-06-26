# CMakeLists.txt - rift-2
# RIFT: RIFT Is a Flexible Translator
# Stage: semantic_analysis

cmake_minimum_required(VERSION 3.16)

project(rift-2
    VERSION 4.0.0
    DESCRIPTION "semantic_analysis stage for RIFT compiler"
    LANGUAGES C
)

# Include common configuration
include(${CMAKE_SOURCE_DIR}/cmake/common/compiler_pipeline.cmake)

# Validate component structure
validate_rift_component(semantic)

# Add RIFT stage
add_rift_stage(rift-2 semantic_analysis semantic)

# Add tests
add_rift_tests(rift-2)

# Stage-specific configuration
if("rift-2" STREQUAL "rift-4")
    # Bytecode generation requires additional crypto libraries
    target_compile_definitions(rift-2_static PRIVATE RIFT_BYTECODE_CRYPTO=1)
endif()

if("rift-2" STREQUAL "rift-1")
    # Parser requires thread support for dual-mode parsing
    target_compile_definitions(rift-2_static PRIVATE RIFT_DUAL_MODE_PARSING=1)
endif()

message(STATUS "rift-2 configuration complete")
