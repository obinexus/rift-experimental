# CMakeLists.txt - Root configuration
cmake_minimum_required(VERSION 3.16)
project(rift_bridge)

# WebAssembly configuration
if(DEFINED EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".wasm")
    set(CMAKE_C_COMPILER "${EMSCRIPTEN_PREFIX}/emcc")
    set(CMAKE_CXX_COMPILER "${EMSCRIPTEN_PREFIX}/em++")
endif()

# Multi-stage build function
function(add_rift_stage STAGE_NUM STAGE_NAME SOURCES)
    set(STAGE_DIR ${CMAKE_BINARY_DIR}/obj/stage${STAGE_NUM}_${STAGE_NAME})
    file(MAKE_DIRECTORY ${STAGE_DIR})
    
    # Object compilation
    add_library(${STAGE_NAME}_objects OBJECT ${SOURCES})
    set_target_properties(${STAGE_NAME}_objects PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${STAGE_DIR}
    )
    
    # WAT generation
    add_custom_command(OUTPUT ${STAGE_DIR}/${STAGE_NAME}.wat
        COMMAND ${CMAKE_CXX_COMPILER} -S ${SOURCES} -o ${STAGE_DIR}/${STAGE_NAME}.wat
        DEPENDS ${STAGE_NAME}_objects
    )
    
    # WASM compilation
    add_custom_command(OUTPUT ${STAGE_DIR}/${STAGE_NAME}.wasm
        COMMAND ${WAT2WASM} ${STAGE_DIR}/${STAGE_NAME}.wat -o ${STAGE_DIR}/${STAGE_NAME}.wasm
        DEPENDS ${STAGE_DIR}/${STAGE_NAME}.wat
    )
    
    # Register stage for linking
    set_property(GLOBAL APPEND PROPERTY RIFT_STAGES ${STAGE_NAME})
endfunction()

# Build stages 0-6
add_rift_stage(0 tokenizer "${TOKENIZER_SOURCES}")
add_rift_stage(1 parser "${PARSER_SOURCES}")
add_rift_stage(2 semantic "${SEMANTIC_SOURCES}")
add_rift_stage(3 optimizer "${OPTIMIZER_SOURCES}")
add_rift_stage(4 codegen "${CODEGEN_SOURCES}")
add_rift_stage(5 linker "${LINKER_SOURCES}")
add_rift_stage(6 runtime "${RUNTIME_SOURCES}")
