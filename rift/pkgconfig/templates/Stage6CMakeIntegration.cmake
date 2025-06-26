# RIFT Stage 6 (emitter) CMakeLists.txt Integration
# Add this to rift/rift-6/CMakeLists.txt or equivalent

# Stage 6 Library Target
add_library(rift-6_static STATIC
    src/core/emitter.c
    # Add other source files as needed
)

# Set target properties
set_target_properties(rift-6_static PROPERTIES
    OUTPUT_NAME "rift-6"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    POSITION_INDEPENDENT_CODE ON
)

# Include directories
target_include_directories(rift-6_static PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/rift/core/stage-6>
)

# Compiler definitions for AEGIS compliance
target_compile_definitions(rift-6_static PUBLIC
    RIFT_STAGE_6_ENABLED=1
    AEGIS_COMPLIANCE_ENABLED=1
)

# Install library
install(TARGETS rift-6_static
    EXPORT RiftStage6Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rift/core/stage-6
    FILES_MATCHING PATTERN "*.h"
)
