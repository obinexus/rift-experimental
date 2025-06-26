cmake_minimum_required(VERSION 3.14)
project(RIFT VERSION 2.1.0 LANGUAGES C)

# RIFT Core Configuration
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Compiler flags
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

# Global include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rift-core/include)

# Add rift-core first (dependency for all stages)
add_subdirectory(rift-core)

# Add RIFT stages
add_subdirectory(rift-0)
add_subdirectory(rift-1)
add_subdirectory(rift-2)
add_subdirectory(rift-3)
add_subdirectory(rift-4)
add_subdirectory(rift-5)
add_subdirectory(rift-6)

# Testing
enable_testing()
add_subdirectory(tests)

# Install headers
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY rift-core/include/ DESTINATION include)
