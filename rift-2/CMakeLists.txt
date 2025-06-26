# =================================================================
# CMakeLists.txt - rift-2
# RIFT: RIFT Is a Flexible Translator
# Component: semantic
# OBINexus Computing Framework - Build Orchestration
# =================================================================

cmake_minimum_required(VERSION 3.16)

project(rift-2
    VERSION ${CMAKE_PROJECT_VERSION}
    DESCRIPTION "RIFT Semantic Stage"
    LANGUAGES C
)

# Include common pipeline configuration
include(${CMAKE_SOURCE_DIR}/cmake/common/compiler_pipeline.cmake)

# Add this stage using the common macro
add_rift_stage(rift-2 semantic)

# Add validation if target was created
if(TARGET rift-2_static)
    add_rift_validation(rift-2)
endif()

message(STATUS "rift-2 configuration complete")
