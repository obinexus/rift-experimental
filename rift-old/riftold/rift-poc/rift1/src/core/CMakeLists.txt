# RIFT1 Core Components CMake Configuration
# OBINexus AEGIS Framework - Modular tokenizer build

cmake_minimum_required(VERSION 3.16)

# Lexer component
add_library(rift1_lexer STATIC
    lexer/lexer.c
)
target_include_directories(rift1_lexer PUBLIC
    ${PROJECT_SOURCE_DIR}/include/rift1
)

# Token Types component
add_library(rift1_token_types STATIC
    token_types/token_type.c
)
target_include_directories(rift1_token_types PUBLIC
    ${PROJECT_SOURCE_DIR}/include/rift1
)
target_link_libraries(rift1_token_types 
    rift1_lexer
)

# Token Values component (to be implemented)
add_library(rift1_token_values STATIC
    token_values/token_value.c
)
target_include_directories(rift1_token_values PUBLIC
    ${PROJECT_SOURCE_DIR}/include/rift1
)
target_link_libraries(rift1_token_values 
    rift1_token_types
)

# Token Memory component (enhanced)
add_library(rift1_token_memory STATIC
    token_memory/token_memory.c
)
target_include_directories(rift1_token_memory PUBLIC
    ${PROJECT_SOURCE_DIR}/include/rift1
)
target_link_libraries(rift1_token_memory 
    rift1_token_values
)

# Tokenizer orchestrator
add_library(rift1_tokenizer STATIC
    tokenizer/tokenizer.c
)
target_include_directories(rift1_tokenizer PUBLIC
    ${PROJECT_SOURCE_DIR}/include/rift1
)
target_link_libraries(rift1_tokenizer 
    rift1_lexer
    rift1_token_types
    rift1_token_values
    rift1_token_memory
)

# Existing components integration
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/rift1_parsing.c")
    target_sources(rift1_tokenizer PRIVATE
        rift1_parsing.c
    )
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/rift1_tokenmemory.c")
    target_sources(rift1_token_memory PRIVATE
        rift1_tokenmemory.c
    )
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/rift1_ast.c")
    add_library(rift1_ast STATIC
        rift1_ast.c
    )
    target_include_directories(rift1_ast PUBLIC
        ${PROJECT_SOURCE_DIR}/include/rift1
    )
endif()

# Compiler options
target_compile_options(rift1_lexer PRIVATE
    -Wall -Wextra -Wpedantic
    $<$<CONFIG:Debug>:-O0 -g -DDEBUG>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
)

target_compile_options(rift1_token_types PRIVATE
    -Wall -Wextra -Wpedantic
    $<$<CONFIG:Debug>:-O0 -g -DDEBUG>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
)

# AEGIS compliance flags
target_compile_definitions(rift1_tokenizer PRIVATE
    RIFT1_VERSION="${RIFT1_VERSION}"
    AEGIS_COMPLIANCE_ENABLED=1
)
