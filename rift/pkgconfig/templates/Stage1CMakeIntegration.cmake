# RIFT Stage 1 (parser) CMakeLists.txt Integration
# Add this to rift/rift-1/CMakeLists.txt or equivalent

# Stage 1 Library Target
add_library(rift-1_static STATIC
    src/core/parser.c
    # Add other source files as needed
)

# Set target properties
set_target_properties(rift-1_static PROPERTIES
    OUTPUT_NAME "rift-1"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    POSITION_INDEPENDENT_CODE ON
)

# Include directories
target_include_directories(rift-1_static PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/rift/core/stage-1>
)

# Compiler definitions for AEGIS compliance
target_compile_definitions(rift-1_static PUBLIC
    RIFT_STAGE_1_ENABLED=1
    AEGIS_COMPLIANCE_ENABLED=1
)

# Install library
install(TARGETS rift-1_static
    EXPORT RiftStage1Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rift/core/stage-1
    FILES_MATCHING PATTERN "*.h"
)
