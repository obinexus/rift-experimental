# =================================================================
# CMakeLists.txt - rift-4
# RIFT: RIFT Is a Flexible Translator
# Component: bytecode
# OBINexus Computing Framework - Build Orchestration
# =================================================================

cmake_minimum_required(VERSION 3.16)

project(rift-4
    VERSION ${CMAKE_PROJECT_VERSION}
    DESCRIPTION "RIFT Bytecode Stage"
    LANGUAGES C
)

# Include common pipeline configuration
include(${CMAKE_SOURCE_DIR}/cmake/common/compiler_pipeline.cmake)

# Add this stage using the common macro
add_rift_stage(rift-4 bytecode)

# Add validation if target was created
if(TARGET rift-4_static)
    add_rift_validation(rift-4)
endif()

message(STATUS "rift-4 configuration complete")
