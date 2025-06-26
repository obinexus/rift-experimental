# RIFT Stage 1 CMake Configuration
cmake_minimum_required(VERSION 3.10)
project(rift-stage-1-aegis VERSION 1.0.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Core library
file(GLOB CORE_SOURCES "src/core/*.c")
add_library(rift1 STATIC ${CORE_SOURCES})

# CLI executable
file(GLOB CLI_SOURCES "src/cli/*.c")
add_executable(rift1.exe ${CLI_SOURCES})
target_link_libraries(rift1.exe rift1 pthread m)

# Installation
install(TARGETS rift1 rift1.exe
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)
install(DIRECTORY include/rift1 DESTINATION include)
