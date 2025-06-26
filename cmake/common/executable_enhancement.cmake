# CMake Executable Target Enhancement
# RIFT Compiler Pipeline - Stage Executable Generation
# Technical Integration: Nnamdi Okpala's AEGIS Framework

# Root CMakeLists.txt Enhancement Section
# =====================================

# Add executable generation for each stage
macro(create_stage_executable STAGE_NUM STAGE_NAME)
    set(STAGE_DIR "rift-${STAGE_NUM}")
    set(EXECUTABLE_NAME "rift-${STAGE_NUM}.exe")
    
    # CLI source files for the stage
    file(GLOB STAGE_CLI_SOURCES "${STAGE_DIR}/src/cli/*.c")
    
    if(STAGE_CLI_SOURCES)
        # Create executable target
        add_executable(${EXECUTABLE_NAME} ${STAGE_CLI_SOURCES})
        
        # Link against stage static library
        target_link_libraries(${EXECUTABLE_NAME} 
            PRIVATE rift-${STAGE_NUM}_static
        )
        
        # Include directories
        target_include_directories(${EXECUTABLE_NAME} 
            PRIVATE ${STAGE_DIR}/include
            PRIVATE include/rift/core
        )
        
        # AEGIS compliance flags
        target_compile_options(${EXECUTABLE_NAME} PRIVATE
            -Wall -Wextra -Wpedantic -Werror
            -DRIFT_STAGE=${STAGE_NUM}
            -DRIFT_STAGE_NAME="${STAGE_NAME}"
            -DRIFT_AEGIS_COMPLIANCE=1
        )
        
        # Set output directory
        set_target_properties(${EXECUTABLE_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        )
        
        # Install executable
        install(TARGETS ${EXECUTABLE_NAME}
            RUNTIME DESTINATION bin
            COMPONENT stage_executables
        )
        
        message(STATUS "Added executable: ${EXECUTABLE_NAME} for ${STAGE_NAME}")
    else()
        # Generate minimal CLI wrapper if no CLI sources exist
        set(WRAPPER_SOURCE "${CMAKE_BINARY_DIR}/generated/${STAGE_DIR}_cli.c")
        configure_file(
            "${CMAKE_SOURCE_DIR}/cmake/templates/stage_cli_wrapper.c.in"
            "${WRAPPER_SOURCE}"
            @ONLY
        )
        
        add_executable(${EXECUTABLE_NAME} ${WRAPPER_SOURCE})
        target_link_libraries(${EXECUTABLE_NAME} 
            PRIVATE rift-${STAGE_NUM}_static
        )
        
        set_target_properties(${EXECUTABLE_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        )
        
        install(TARGETS ${EXECUTABLE_NAME}
            RUNTIME DESTINATION bin
            COMPONENT stage_executables
        )
        
        message(STATUS "Generated wrapper executable: ${EXECUTABLE_NAME}")
    endif()
endmacro()

# Generate executables for all stages
create_stage_executable(0 "Tokenizer")
create_stage_executable(1 "Parser")
create_stage_executable(2 "Semantic Analyzer")
create_stage_executable(3 "Validator")
create_stage_executable(4 "Bytecode Generator")
create_stage_executable(5 "Verifier")
create_stage_executable(6 "Emitter")

# Demo pipeline target
add_custom_target(demo_pipeline
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/demo_pipeline_standardized.sh"
        "${CMAKE_BINARY_DIR}/demo_pipeline.sh"
    COMMAND chmod +x "${CMAKE_BINARY_DIR}/demo_pipeline.sh"
    DEPENDS rift-0.exe rift-1.exe rift-2.exe rift-3.exe 
            rift-4.exe rift-5.exe rift-6.exe
    COMMENT "Preparing AEGIS-compliant demo pipeline"
)

# Validation target for executable deployment
add_custom_target(validate_executables
    COMMAND ${CMAKE_COMMAND} -E echo "Validating executable deployment..."
    COMMAND test -f "${CMAKE_BINARY_DIR}/bin/rift-0.exe"
    COMMAND test -f "${CMAKE_BINARY_DIR}/bin/rift-1.exe"
    COMMAND test -f "${CMAKE_BINARY_DIR}/bin/rift-2.exe"
    COMMAND test -f "${CMAKE_BINARY_DIR}/bin/rift-3.exe"
    COMMAND test -f "${CMAKE_BINARY_DIR}/bin/rift-4.exe"
    COMMAND test -f "${CMAKE_BINARY_DIR}/bin/rift-5.exe"
    COMMAND test -f "${CMAKE_BINARY_DIR}/bin/rift-6.exe"
    COMMAND ${CMAKE_COMMAND} -E echo "✅ All stage executables validated"
    DEPENDS demo_pipeline
    COMMENT "AEGIS executable validation"
)