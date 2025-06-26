# RIFT Stage 0 (tokenizer) CMakeLists.txt Integration
# Add this to rift/rift-0/CMakeLists.txt or equivalent

# Stage 0 Library Target
add_library(rift-0_static STATIC
    src/core/tokenizer.c
    # Add other source files as needed
)

# Set target properties
set_target_properties(rift-0_static PROPERTIES
    OUTPUT_NAME "rift-0"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    POSITION_INDEPENDENT_CODE ON
)

# Include directories
target_include_directories(rift-0_static PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/rift/core/stage-0>
)

# Compiler definitions for AEGIS compliance
target_compile_definitions(rift-0_static PUBLIC
    RIFT_STAGE_0_ENABLED=1
    AEGIS_COMPLIANCE_ENABLED=1
)

# Install library
install(TARGETS rift-0_static
    EXPORT RiftStage0Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rift/core/stage-0
    FILES_MATCHING PATTERN "*.h"
)
