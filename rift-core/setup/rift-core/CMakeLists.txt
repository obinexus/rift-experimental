cmake_minimum_required(VERSION 3.14)

# Core library sources
set(RIFT_CORE_SOURCES
    src/common/core.c
    src/common/memory.c
    src/thread/lifecycle.c
    src/thread/parity.c
    src/audit/tracer.c
    src/audit/validator.c
    src/telemetry/collector.c
    src/telemetry/reporter.c
    src/governance/policy.c
    src/governance/compliance.c
)

# Core library headers
set(RIFT_CORE_HEADERS
    include/rift-core/common/core.h
    include/rift-core/common/memory.h
    include/rift-core/thread/lifecycle.h
    include/rift-core/thread/parity.h
    include/rift-core/audit/tracer.h
    include/rift-core/audit/validator.h
    include/rift-core/telemetry/collector.h
    include/rift-core/telemetry/reporter.h
    include/rift-core/governance/policy.h
    include/rift-core/governance/compliance.h
)

# Create static library
add_library(rift-core STATIC ${RIFT_CORE_SOURCES})

# Set target properties
set_target_properties(rift-core PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "${RIFT_CORE_HEADERS}"
)

# Include directories
target_include_directories(rift-core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Compiler definitions
target_compile_definitions(rift-core PRIVATE
    RIFT_CORE_VERSION="${PROJECT_VERSION}"
    RIFT_GOVERNANCE_SCHEMA_VERSION="${GOVERNANCE_SCHEMA_VERSION}"
    RIFT_AUDIT_SCHEMA_VERSION="${AUDIT_SCHEMA_VERSION}"
)

# Thread support
find_package(Threads REQUIRED)
target_link_libraries(rift-core PRIVATE Threads::Threads)

# Install targets
install(TARGETS rift-core
    EXPORT rift-core-targets
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/rift-core
)

# Export configuration
install(EXPORT rift-core-targets
    FILE rift-core-config.cmake
    DESTINATION lib/cmake/rift-core
)

# Tests
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
