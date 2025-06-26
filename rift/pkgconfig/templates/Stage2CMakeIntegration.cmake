# RIFT Stage 2 (semantic) CMakeLists.txt Integration
# Add this to rift/rift-2/CMakeLists.txt or equivalent

# Stage 2 Library Target
add_library(rift-2_static STATIC
    src/core/semantic.c
    # Add other source files as needed
)

# Set target properties
set_target_properties(rift-2_static PROPERTIES
    OUTPUT_NAME "rift-2"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    POSITION_INDEPENDENT_CODE ON
)

# Include directories
target_include_directories(rift-2_static PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/rift/core/stage-2>
)

# Compiler definitions for AEGIS compliance
target_compile_definitions(rift-2_static PUBLIC
    RIFT_STAGE_2_ENABLED=1
    AEGIS_COMPLIANCE_ENABLED=1
)

# Install library
install(TARGETS rift-2_static
    EXPORT RiftStage2Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rift/core/stage-2
    FILES_MATCHING PATTERN "*.h"
)
