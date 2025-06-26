# compiler_pipeline.cmake
# RIFT Common Build Configuration
# OBINexus Computing Framework

# RIFT pipeline stage macro
macro(add_rift_stage STAGE_NAME STAGE_TYPE COMPONENT)
    set(STAGE_SOURCES "")
    set(STAGE_HEADERS "")
    
    # Collect source files
    file(GLOB_RECURSE STAGE_SOURCES 
        "${CMAKE_CURRENT_SOURCE_DIR}/src/core/${COMPONENT}/*.c"
    )
    
    file(GLOB_RECURSE STAGE_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/${STAGE_NAME}/core/*.h"
    )
    
    # Create static library
    add_library(${STAGE_NAME}_static STATIC ${STAGE_SOURCES})
    target_include_directories(${STAGE_NAME}_static PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
    )
    
    # Create shared library
    add_library(${STAGE_NAME}_shared SHARED ${STAGE_SOURCES})
    target_include_directories(${STAGE_NAME}_shared PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
    )
    
    # Create executable
    add_executable(${STAGE_NAME}.exe ${STAGE_SOURCES})
    target_include_directories(${STAGE_NAME}.exe PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
    )
    
    # Link dependencies
    target_link_libraries(${STAGE_NAME}_static 
        OpenSSL::SSL OpenSSL::Crypto Threads::Threads
    )
    target_link_libraries(${STAGE_NAME}_shared 
        OpenSSL::SSL OpenSSL::Crypto Threads::Threads
    )
    target_link_libraries(${STAGE_NAME}.exe 
        ${STAGE_NAME}_static OpenSSL::SSL OpenSSL::Crypto Threads::Threads
    )
    
    # Set output directories
    set_target_properties(${STAGE_NAME}_static PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
    )
    set_target_properties(${STAGE_NAME}_shared PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
    )
    set_target_properties(${STAGE_NAME}.exe PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    )
endmacro()

# RIFT component validation macro
macro(validate_rift_component COMPONENT_NAME)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/core/${COMPONENT_NAME}")
        message(FATAL_ERROR "Missing component source: ${COMPONENT_NAME}")
    endif()
    
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/rift/core/${COMPONENT_NAME}")
        message(FATAL_ERROR "Missing component headers: ${COMPONENT_NAME}")
    endif()
endmacro()

# RIFT testing framework
macro(add_rift_tests STAGE_NAME)
    file(GLOB_RECURSE TEST_SOURCES 
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/unit/*.c"
    )
    
    foreach(TEST_SOURCE ${TEST_SOURCES})
        get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
        add_executable(${STAGE_NAME}_${TEST_NAME} ${TEST_SOURCE})
        target_link_libraries(${STAGE_NAME}_${TEST_NAME} ${STAGE_NAME}_static)
        add_test(NAME ${STAGE_NAME}_${TEST_NAME} COMMAND ${STAGE_NAME}_${TEST_NAME})
    endforeach()
endmacro()
