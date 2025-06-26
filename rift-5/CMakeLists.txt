# =================================================================
# CMakeLists.txt - rift-5
# RIFT: RIFT Is a Flexible Translator
# Component: verifier
# OBINexus Computing Framework - Build Orchestration
# =================================================================

cmake_minimum_required(VERSION 3.16)

project(rift-5
    VERSION ${CMAKE_PROJECT_VERSION}
    DESCRIPTION "RIFT Verifier Stage"
    LANGUAGES C
)

# Include common pipeline configuration
include(${CMAKE_SOURCE_DIR}/cmake/common/compiler_pipeline.cmake)

# Add this stage using the common macro
add_rift_stage(rift-5 verifier)

# Add validation if target was created
if(TARGET rift-5_static)
    add_rift_validation(rift-5)
endif()

message(STATUS "rift-5 configuration complete")
