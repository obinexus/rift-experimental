# CMakeLists.txt - RIFT Compiler Pipeline
# RIFT: RIFT Is a Flexible Translator
# OBINexus Computing Framework - Build Orchestration

cmake_minimum_required(VERSION 3.16)

project(RIFT_Compiler
    VERSION 4.0.0
    DESCRIPTION "RIFT Is a Flexible Translator - Modular Compiler Pipeline"
    LANGUAGES C
)

# Build configuration
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Include common configuration
include(cmake/common/compiler_pipeline.cmake)

# Compiler flags
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic -DRIFT_DEBUG=1")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG -Wall")

# Security flags
add_compile_options(
    -fstack-protector-strong
    -D_FORTIFY_SOURCE=2
    -fPIE
)

add_link_options(
    -Wl,-z,relro
    -Wl,-z,now
    -pie
)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/rift/core
)

# External dependencies
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

# Add subdirectories for each pipeline stage
add_subdirectory(rift-0)  # Tokenization
add_subdirectory(rift-1)  # Parsing
add_subdirectory(rift-2)  # Semantic Analysis
add_subdirectory(rift-3)  # Validation
add_subdirectory(rift-4)  # Bytecode Generation
add_subdirectory(rift-5)  # Verification
add_subdirectory(rift-6)  # Emission

# Core library components
add_subdirectory(src/core)
add_subdirectory(src/cli)
add_subdirectory(src/config)

# Testing
enable_testing()
add_subdirectory(tests)

# Documentation
find_package(Doxygen)
if(DOXYGEN_FOUND)
    add_custom_target(docs
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation"
    )
endif()

# Install configuration
install(DIRECTORY include/rift DESTINATION include)
install(FILES .riftrc DESTINATION share/rift)

# Build summary
message(STATUS "RIFT Compiler Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C Standard: ${CMAKE_C_STANDARD}")
message(STATUS "  OpenSSL: ${OPENSSL_VERSION}")
