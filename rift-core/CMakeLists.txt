cmake_minimum_required(VERSION 3.14)

# RIFT-Core library sources
set(RIFT_CORE_SOURCES
    src/rift-core/core.c
    src/rift-core/audit.c
    src/rift-core/thread.c
    src/rift-core/telemetry.c
    src/rift-core/governance.c
    src/rift-core/accessibility.c
)

# RIFT-Core library headers
set(RIFT_CORE_HEADERS
    include/rift-core/core.h
    include/rift-core/audit.h
    include/rift-core/thread.h
    include/rift-core/telemetry.h
    include/rift-core/governance.h
    include/rift-core/accessibility.h
)

# Create RIFT-Core static library
add_library(rift-core STATIC ${RIFT_CORE_SOURCES})

# Set RIFT-Core target properties
set_target_properties(rift-core PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "${RIFT_CORE_HEADERS}"
)

# RIFT-Core include directories
target_include_directories(rift-core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# RIFT-Core compile definitions
target_compile_definitions(rift-core PRIVATE
    RIFT_CORE_VERSION="2.1.0-core"
    RIFT_CORE_INTERNAL=1
    RIFT_NATIVE_IMPLEMENTATION=1
)

# Thread support for RIFT-Core
find_package(Threads REQUIRED)
target_link_libraries(rift-core PRIVATE Threads::Threads)

# Install RIFT-Core targets
install(TARGETS rift-core
    EXPORT rift-core-targets
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/rift-core
)

# Export RIFT-Core configuration
install(EXPORT rift-core-targets
    FILE rift-core-config.cmake
    DESTINATION lib/cmake/rift-core
)
