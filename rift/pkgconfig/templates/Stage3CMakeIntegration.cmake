# RIFT Stage 3 (validator) CMakeLists.txt Integration
# Add this to rift/rift-3/CMakeLists.txt or equivalent

# Stage 3 Library Target
add_library(rift-3_static STATIC
    src/core/validator.c
    # Add other source files as needed
)

# Set target properties
set_target_properties(rift-3_static PROPERTIES
    OUTPUT_NAME "rift-3"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    POSITION_INDEPENDENT_CODE ON
)

# Include directories
target_include_directories(rift-3_static PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/rift/core/stage-3>
)

# Compiler definitions for AEGIS compliance
target_compile_definitions(rift-3_static PUBLIC
    RIFT_STAGE_3_ENABLED=1
    AEGIS_COMPLIANCE_ENABLED=1
)

# Install library
install(TARGETS rift-3_static
    EXPORT RiftStage3Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rift/core/stage-3
    FILES_MATCHING PATTERN "*.h"
)
