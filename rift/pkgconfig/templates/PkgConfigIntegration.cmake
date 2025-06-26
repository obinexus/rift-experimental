# RIFT pkg-config Integration for CMake
# AEGIS Compliant Build Configuration
# OBINexus Computing Framework

# Function to configure pkg-config for a RIFT stage
function(configure_rift_stage_pkgconfig STAGE_ID STAGE_NAME)
    set(PC_TEMPLATE "${CMAKE_SOURCE_DIR}/rift/pkgconfig/rift-${STAGE_ID}.pc.in")
    set(PC_OUTPUT "${CMAKE_BINARY_DIR}/pkgconfig/rift-${STAGE_ID}.pc")
    
    # Ensure output directory exists
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/pkgconfig")
    
    # Configure the .pc file
    configure_file("${PC_TEMPLATE}" "${PC_OUTPUT}" @ONLY)
    
    # Install the .pc file
    install(FILES "${PC_OUTPUT}"
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
            COMPONENT Development)
    
    # Log configuration
    message(STATUS "RIFT Stage ${STAGE_ID}: pkg-config configured (${STAGE_NAME})")
endfunction()

# Function to configure all RIFT stages
function(configure_all_rift_stages)
    # Stage definitions matching the shell script
    set(RIFT_STAGE_0_NAME "tokenizer")
    set(RIFT_STAGE_1_NAME "parser")
    set(RIFT_STAGE_2_NAME "semantic")
    set(RIFT_STAGE_3_NAME "validator") 
    set(RIFT_STAGE_4_NAME "bytecode")
    set(RIFT_STAGE_5_NAME "verifier")
    set(RIFT_STAGE_6_NAME "emitter")
    
    # Configure each stage
    foreach(STAGE_NUM RANGE 0 6)
        configure_rift_stage_pkgconfig(${STAGE_NUM} ${RIFT_STAGE_${STAGE_NUM}_NAME})
    endforeach()
    
    message(STATUS "AEGIS Compliance: All RIFT stage pkg-config files configured")
endfunction()

# Validation function for pkg-config integration
function(validate_pkgconfig_installation)
    find_program(PKG_CONFIG_EXECUTABLE pkg-config)
    
    if(PKG_CONFIG_EXECUTABLE)
        message(STATUS "pkg-config found: ${PKG_CONFIG_EXECUTABLE}")
        
        # Get pkg-config version
        execute_process(
            COMMAND ${PKG_CONFIG_EXECUTABLE} --version
            OUTPUT_VARIABLE PKG_CONFIG_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        message(STATUS "pkg-config version: ${PKG_CONFIG_VERSION}")
        
        # Validate minimum version (0.29.0 or higher recommended)
        if(PKG_CONFIG_VERSION VERSION_LESS "0.29.0")
            message(WARNING "pkg-config version ${PKG_CONFIG_VERSION} may have compatibility issues. Recommend 0.29.0+")
        endif()
    else()
        message(WARNING "pkg-config not found. External library linking may be affected.")
    endif()
endfunction()
