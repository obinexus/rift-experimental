# RIFT Root CMakeLists.txt pkg-config Integration Snippet
# Add this to your main CMakeLists.txt

# Include pkg-config integration
include(${CMAKE_SOURCE_DIR}/rift/pkgconfig/templates/PkgConfigIntegration.cmake)

# Validate pkg-config availability
validate_pkgconfig_installation()

# Configure pkg-config for all RIFT stages
configure_all_rift_stages()

# Set pkg-config path for development
set(ENV{PKG_CONFIG_PATH} "${CMAKE_BINARY_DIR}/pkgconfig:$ENV{PKG_CONFIG_PATH}")

# Create convenience target for pkg-config validation
add_custom_target(validate-pkgconfig
    COMMAND ${CMAKE_COMMAND} -E echo "Validating RIFT pkg-config installation..."
    COMMAND bash -c "for stage in {0..6}; do echo -n \"Stage \$stage: \"; pkg-config --exists rift-\$stage && echo \"✓ Found\" || echo \"✗ Missing\"; done"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Validating RIFT pkg-config files"
    VERBATIM
)
