# RIFT Stage 4 (bytecode) CMakeLists.txt Integration
# Add this to rift/rift-4/CMakeLists.txt or equivalent

# Stage 4 Library Target
add_library(rift-4_static STATIC
    src/core/bytecode.c
    # Add other source files as needed
)

# Set target properties
set_target_properties(rift-4_static PROPERTIES
    OUTPUT_NAME "rift-4"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    POSITION_INDEPENDENT_CODE ON
)

# Include directories
target_include_directories(rift-4_static PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/rift/core/stage-4>
)

# Compiler definitions for AEGIS compliance
target_compile_definitions(rift-4_static PUBLIC
    RIFT_STAGE_4_ENABLED=1
    AEGIS_COMPLIANCE_ENABLED=1
)

# Install library
install(TARGETS rift-4_static
    EXPORT RiftStage4Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rift/core/stage-4
    FILES_MATCHING PATTERN "*.h"
)
