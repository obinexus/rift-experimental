# =================================================================
# CMakeLists.txt - rift-1
# RIFT: RIFT Is a Flexible Translator
# Component: parser
# OBINexus Computing Framework - Build Orchestration
# =================================================================

cmake_minimum_required(VERSION 3.16)

project(rift-1
    VERSION ${CMAKE_PROJECT_VERSION}
    DESCRIPTION "RIFT Parser Stage"
    LANGUAGES C
)

# Include common pipeline configuration
include(${CMAKE_SOURCE_DIR}/cmake/common/compiler_pipeline.cmake)

# Add this stage using the common macro
add_rift_stage(rift-1 parser)

# Add validation if target was created
if(TARGET rift-1_static)
    add_rift_validation(rift-1)
endif()

message(STATUS "rift-1 configuration complete")
