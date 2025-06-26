cmake_minimum_required(VERSION 3.14)
project(RIFT VERSION 2.1.0 LANGUAGES C)

# RIFT-Core Configuration
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# RIFT-Core compiler flags
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic -DDEBUG -DRIFT_CORE_ENABLED")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG -DRIFT_CORE_ENABLED")

# RIFT-Core definitions
add_compile_definitions(
    RIFT_VERSION="2.1.0-core"
    RIFT_GOVERNANCE_SCHEMA_VERSION="2.1"
    RIFT_AUDIT_SCHEMA_VERSION="2.1"
    RIFT_CORE_NATIVE=1
)

# Include RIFT-Core modules
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rift-core/include)

# Add RIFT-Core subdirectory
add_subdirectory(rift-core)
add_subdirectory(governance)

# Add RIFT stages in dependency order
add_subdirectory(rift-0)
add_subdirectory(rift-1)
add_subdirectory(rift-2)
add_subdirectory(rift-3)
add_subdirectory(rift-4)
add_subdirectory(rift-5)
add_subdirectory(rift-6)

# RIFT-Core testing
enable_testing()
add_subdirectory(tests)

# Install RIFT-Core headers
install(DIRECTORY rift-core/include/ DESTINATION include)
