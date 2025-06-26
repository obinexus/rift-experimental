cmake_minimum_required(VERSION 3.14)

# RIFT Core library sources
file(GLOB_RECURSE RIFT_CORE_SOURCES 
    "src/rift-core/*.c"
)

# RIFT Core library headers
file(GLOB_RECURSE RIFT_CORE_HEADERS 
    "include/rift-core/*.h"
)

# Create rift-core static library
add_library(rift-core STATIC ${RIFT_CORE_SOURCES})

# Set target properties
set_target_properties(rift-core PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Include directories
target_include_directories(rift-core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Thread support
find_package(Threads REQUIRED)
target_link_libraries(rift-core PRIVATE Threads::Threads)

# Install targets
install(TARGETS rift-core
    EXPORT rift-core-targets
    ARCHIVE DESTINATION lib
)

install(EXPORT rift-core-targets
    FILE rift-core-config.cmake
    DESTINATION lib/cmake/rift-core
)
