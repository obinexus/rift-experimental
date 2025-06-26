# =================================================================
# CMakeLists.txt - rift-0
# RIFT: RIFT Is a Flexible Translator
# Component: tokenizer
# OBINexus Computing Framework - Build Orchestration
# =================================================================

cmake_minimum_required(VERSION 3.16)

project(rift-0
    VERSION ${CMAKE_PROJECT_VERSION}
    DESCRIPTION "RIFT Tokenizer Stage"
    LANGUAGES C
)

# Include common pipeline configuration
include(${CMAKE_SOURCE_DIR}/cmake/common/compiler_pipeline.cmake)

install(FILES ${CMAKE_BINARY_DIR}/lib/pkgconfig/rift-0.pc
        DESTINATION lib/pkgconfig)

        
# Add this stage using the common macro
add_rift_stage(rift-0 tokenizer)

# Add validation if target was created
if(TARGET rift-0_static)
    add_rift_validation(rift-0)
endif()

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config_templates/rift-0.pc.in
  ${CMAKE_BINARY_DIR}/lib/pkgconfig/rift-0.pc @ONLY
)

message(STATUS "rift-0 configuration complete")
