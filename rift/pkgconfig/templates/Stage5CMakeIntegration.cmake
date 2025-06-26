# RIFT Stage 5 (verifier) CMakeLists.txt Integration
# Add this to rift/rift-5/CMakeLists.txt or equivalent

# Stage 5 Library Target
add_library(rift-5_static STATIC
    src/core/verifier.c
    # Add other source files as needed
)

# Set target properties
set_target_properties(rift-5_static PROPERTIES
    OUTPUT_NAME "rift-5"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    POSITION_INDEPENDENT_CODE ON
)

# Include directories
target_include_directories(rift-5_static PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/rift/core/stage-5>
)

# Compiler definitions for AEGIS compliance
target_compile_definitions(rift-5_static PUBLIC
    RIFT_STAGE_5_ENABLED=1
    AEGIS_COMPLIANCE_ENABLED=1
)

# Install library
install(TARGETS rift-5_static
    EXPORT RiftStage5Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rift/core/stage-5
    FILES_MATCHING PATTERN "*.h"
)
