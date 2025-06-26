# =================================================================
# CMakeLists.txt - rift-3
# RIFT: RIFT Is a Flexible Translator
# Component: validator
# OBINexus Computing Framework - Build Orchestration
# =================================================================

cmake_minimum_required(VERSION 3.16)

project(rift-3
    VERSION ${CMAKE_PROJECT_VERSION}
    DESCRIPTION "RIFT Validator Stage"
    LANGUAGES C
)

# Include common pipeline configuration
include(${CMAKE_SOURCE_DIR}/cmake/common/compiler_pipeline.cmake)

# Add this stage using the common macro
add_rift_stage(rift-3 validator)

# Add validation if target was created
if(TARGET rift-3_static)
    add_rift_validation(rift-3)
endif()

message(STATUS "rift-3 configuration complete")
