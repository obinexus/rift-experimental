# OBINexus AEGIS Framework - Modular CMake Configuration
# rift1 Tokenizer → Parser → AST Pipeline
cmake_minimum_required(VERSION 3.16)
project(rift1 VERSION 1.0 LANGUAGES C)

# AEGIS Core Configuration
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include paths - AEGIS structure compliance
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/rift1)
include_directories(${PROJECT_SOURCE_DIR}/include/rift1/core)

# AEGIS Compiler flags for governance validation
set(AEGIS_COMPILE_FLAGS
    -Wall -Wextra -Wpedantic -Werror
    -DAEEGIS_COMPLIANCE_ENABLED=1
    -DRIFT1_VERSION="1.0"
    $<$<CONFIG:Debug>:-O0 -g -DDEBUG>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
)

# ================================
# AEGIS Core Tokenizer Components
# ================================

# 1. Lexer Foundation
add_library(rift1_lexer STATIC
    src/core/lexer/lexer.c
)
target_include_directories(rift1_lexer PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_options(rift1_lexer PRIVATE ${AEGIS_COMPILE_FLAGS})

# 2. Token Type System  
add_library(rift1_token_types STATIC
    src/core/token_types/token_type.c
)
target_include_directories(rift1_token_types PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(rift1_token_types PRIVATE rift1_lexer)
target_compile_options(rift1_token_types PRIVATE ${AEGIS_COMPILE_FLAGS})

# 3. Token Value Management
add_library(rift1_token_values STATIC
    src/core/token_values/token_value.c
)
target_include_directories(rift1_token_values PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(rift1_token_values PRIVATE rift1_token_types)
target_compile_options(rift1_token_values PRIVATE ${AEGIS_COMPILE_FLAGS})

# 4. Token Memory Orchestration
add_library(rift1_token_memory STATIC
    src/core/token_memory/token_memory.c
    src/core/token_memory.c  # Legacy compatibility
)
target_include_directories(rift1_token_memory PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(rift1_token_memory PRIVATE rift1_token_values)
target_compile_options(rift1_token_memory PRIVATE ${AEGIS_COMPILE_FLAGS})

# 5. Primary Tokenizer Orchestrator
add_library(rift1_tokenizer STATIC
    src/core/tokenizer/tokenizer.c
)
target_include_directories(rift1_tokenizer PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(rift1_tokenizer PRIVATE 
    rift1_lexer
    rift1_token_types
    rift1_token_values
    rift1_token_memory
)
target_compile_options(rift1_tokenizer PRIVATE ${AEGIS_COMPILE_FLAGS})

# ================================
# AEGIS Parser & AST Components
# ================================

# 6. AST Infrastructure
add_library(rift1_ast STATIC
    src/core/rift1_ast.c
    src/core/rift_ast.c  # Legacy compatibility
)
target_include_directories(rift1_ast PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(rift1_ast PRIVATE rift1_tokenizer)
target_compile_options(rift1_ast PRIVATE ${AEGIS_COMPILE_FLAGS})

# 7. Parser Engine
add_library(rift1_parser STATIC
    src/core/rift1_parsing.c
    src/core/rift_parser.c  # Legacy compatibility
)
target_include_directories(rift1_parser PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(rift1_parser PRIVATE rift1_ast)
target_compile_options(rift1_parser PRIVATE ${AEGIS_COMPILE_FLAGS})

# ================================
# AEGIS Core Infrastructure
# ================================

# 8. Core utilities and configuration
add_library(rift1_core STATIC
    src/core/rift_core.c
    src/core/rift_core_utils.c
    src/core/rift_automaton.c
    src/core/rift_ir.c
    src/core/config/rift_pipeline_simple.c
)
target_include_directories(rift1_core PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(rift1_core PRIVATE rift1_parser)
target_compile_options(rift1_core PRIVATE ${AEGIS_COMPILE_FLAGS})

# ================================
# AEGIS Extensions (R-Lang)
# ================================

# 9. Governance Validation Extension
add_library(rift1_governance STATIC
    src/core/r_extensions/governance/r_governance_validation.c
)
target_include_directories(rift1_governance PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_options(rift1_governance PRIVATE ${AEGIS_COMPILE_FLAGS})

# 10. UML Pattern Extension  
add_library(rift1_uml STATIC
    src/core/r_extensions/uml/r_uml_patterns.c
)
target_include_directories(rift1_uml PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_options(rift1_uml PRIVATE ${AEGIS_COMPILE_FLAGS})

# 11. R-Extensions Core
add_library(rift1_r_extensions STATIC
    src/core/r_extensions/r_extensions_core.c
)
target_include_directories(rift1_r_extensions PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(rift1_r_extensions PRIVATE 
    rift1_governance 
    rift1_uml
)
target_compile_options(rift1_r_extensions PRIVATE ${AEGIS_COMPILE_FLAGS})

# ================================
# AEGIS CLI Interface
# ================================

# 12. CLI Utilities
add_library(rift1_cli_core STATIC
    src/cli/cli_args.c
    src/cli/cli_interface.c
    src/cli/commands/command_handler.c
    src/cli/commands/config_handler.c
    src/cli/commands/pattern_manager.c
    src/cli/commands/config/cli_config.c
)
target_include_directories(rift1_cli_core PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(rift1_cli_core PRIVATE rift1_core rift1_r_extensions)
target_compile_options(rift1_cli_core PRIVATE ${AEGIS_COMPILE_FLAGS})

# ================================
# AEGIS Primary Executable
# ================================

# 13. Main executable
add_executable(rift1_cli
    src/cli/main.c
)
target_include_directories(rift1_cli PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(rift1_cli PRIVATE rift1_cli_core)
target_compile_options(rift1_cli PRIVATE ${AEGIS_COMPILE_FLAGS})

# Set executable name to match OBINexus naming convention
set_target_properties(rift1_cli PROPERTIES OUTPUT_NAME "rift1")

# ================================
# AEGIS Testing Infrastructure  
# ================================

enable_testing()
add_executable(rift1_test
    tests/test_rift1.c
)
target_link_libraries(rift1_test PRIVATE rift1_core)
add_test(NAME aegis_compliance_test COMMAND rift1_test)

# ================================
# AEGIS Installation Configuration
# ================================

install(TARGETS rift1_cli DESTINATION bin)
install(DIRECTORY include/ DESTINATION include)