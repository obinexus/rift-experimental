# =================================================================
# CMakeLists.txt - rift-6
# RIFT: RIFT Is a Flexible Translator
# Component: emitter
# OBINexus Computing Framework - Build Orchestration
# =================================================================

cmake_minimum_required(VERSION 3.16)

project(rift-6
    VERSION ${CMAKE_PROJECT_VERSION}
    DESCRIPTION "RIFT Emitter Stage"
    LANGUAGES C
)

# Include common pipeline configuration
include(${CMAKE_SOURCE_DIR}/cmake/common/compiler_pipeline.cmake)

# Add this stage using the common macro
add_rift_stage(rift-6 emitter)

# Add validation if target was created
if(TARGET rift-6_static)
    add_rift_validation(rift-6)
endif()

message(STATUS "rift-6 configuration complete")
