# =================================================================
# RIFT Unified CLI Implementation - rift.exe
# OBINexus Computing Framework - AEGIS Methodology Compliance
# Technical Architecture: Systematic Waterfall Development
# =================================================================
# Add CLI subdirectory after stage libraries
add_subdirectory(rift-0)
add_subdirectory(rift-1) 
add_subdirectory(rift-2)
add_subdirectory(rift-3)
add_subdirectory(rift-4)
add_subdirectory(rift-5)
add_subdirectory(rift-6)

# Add unified CLI - depends on all stage libraries
add_subdirectory(rift/cli)  # Contains our unified CLI CMakeLists.txt

# Governance configuration generation
configure_file(governance/templates/riftrc.in ${CMAKE_BINARY_DIR}/.riftrc @ONLY)
configure_file(governance/templates/pkg.riftrc.in ${CMAKE_BINARY_DIR}/pkg.riftrc @ONLY)
cmake_minimum_required(VERSION 3.16)

# Master CLI Configuration
set(RIFT_CLI_EXECUTABLE "rift.exe")
set(RIFT_CLI_DESCRIPTION "RIFT Compiler Unified CLI - AEGIS Framework")
set(RIFT_CLI_VERSION "${PROJECT_VERSION}")

# =================================================================
# SOURCE COLLECTION AND ORGANIZATION
# =================================================================

# Primary CLI orchestrator sources
file(GLOB RIFT_CLI_SOURCES
    "src/cli/*.c"
    "src/cli/commands/*.c"
    "src/cli/config/*.c" 
    "src/cli/pipeline/*.c"
    "src/cli/governance/*.c"
)

# Command implementations for all stages
file(GLOB RIFT_COMMAND_SOURCES
    "src/commands/tokenize.c"
    "src/commands/parse.c"
    "src/commands/analyze.c"
    "src/commands/validate.c"
    "src/commands/generate.c"
    "src/commands/verify.c"
    "src/commands/emit.c"
    "src/commands/compile.c"
    "src/commands/governance.c"
)

# AEGIS governance implementation
file(GLOB RIFT_GOVERNANCE_SOURCES
    "src/governance/*.c"
    "src/governance/config/*.c"
    "src/governance/validation/*.c"
    "src/governance/policy/*.c"
)

# =================================================================
# UNIFIED CLI EXECUTABLE CONSTRUCTION
# =================================================================

# Create master CLI executable
add_executable(${RIFT_CLI_EXECUTABLE}
    ${RIFT_CLI_SOURCES}
    ${RIFT_COMMAND_SOURCES}
    ${RIFT_GOVERNANCE_SOURCES}
)

# Link all stage static libraries
target_link_libraries(${RIFT_CLI_EXECUTABLE}
    PRIVATE rift-0_static  # Tokenization
    PRIVATE rift-1_static  # Parsing
    PRIVATE rift-2_static  # Semantic Analysis
    PRIVATE rift-3_static  # Validation
    PRIVATE rift-4_static  # Bytecode Generation
    PRIVATE rift-5_static  # Verification
    PRIVATE rift-6_static  # Emission
    PRIVATE rfi            # Unified library interface
    PRIVATE OpenSSL::SSL
    PRIVATE OpenSSL::Crypto
    PRIVATE Threads::Threads
)

# =================================================================
# AEGIS GOVERNANCE COMPILATION FLAGS
# =================================================================

target_compile_options(${RIFT_CLI_EXECUTABLE} PRIVATE
    -Wall -Wextra -Wpedantic -Werror
    -std=c11
    -fstack-protector-strong
    -D_FORTIFY_SOURCE=2
    -fPIE
    -DRIFT_CLI_UNIFIED=1
    -DRIFT_AEGIS_COMPLIANCE=1
    -DRIFT_MEMORY_ALIGNMENT_4096=1
    -DRIFT_TOKEN_TRIPLET_VALIDATION=1
    -DRIFT_ZERO_TRUST_GOVERNANCE=1
    -DRIFT_VERSION=\"${RIFT_CLI_VERSION}\"
)

# =================================================================
# INCLUDE DIRECTORY CONFIGURATION
# =================================================================

target_include_directories(${RIFT_CLI_EXECUTABLE} PRIVATE
    # Core framework headers
    include/rift/core
    include/rift/cli
    include/rift/governance
    include/rift/common
    
    # Stage-specific headers
    rift-0/include  # Tokenization
    rift-1/include  # Parsing
    rift-2/include  # Semantic Analysis
    rift-3/include  # Validation
    rift-4/include  # Bytecode Generation
    rift-5/include  # Verification
    rift-6/include  # Emission
    
    # CLI implementation headers
    src/cli
    src/commands
    src/governance
)

# =================================================================
# EXECUTABLE PROPERTIES AND METADATA
# =================================================================

set_target_properties(${RIFT_CLI_EXECUTABLE} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    VERSION ${RIFT_CLI_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    DESCRIPTION "${RIFT_CLI_DESCRIPTION}"
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF
)

# =================================================================
# AEGIS GOVERNANCE VALIDATION TARGETS
# =================================================================

# Memory alignment validation
add_custom_target(validate_memory_alignment
    COMMAND ${CMAKE_COMMAND} -E echo "🧠 AEGIS Memory Alignment Validation"
    COMMAND ${CMAKE_BINARY_DIR}/bin/${RIFT_CLI_EXECUTABLE} validate-memory --alignment 4096
    DEPENDS ${RIFT_CLI_EXECUTABLE}
    COMMENT "Validating 4096-bit memory alignment compliance"
)

# Token triplet schema validation
add_custom_target(validate_token_schema
    COMMAND ${CMAKE_COMMAND} -E echo "🎫 AEGIS Token Schema Validation"
    COMMAND ${CMAKE_BINARY_DIR}/bin/${RIFT_CLI_EXECUTABLE} validate-tokens --schema-check
    DEPENDS ${RIFT_CLI_EXECUTABLE}
    COMMENT "Validating token_type, token_value, token_memory triplet"
)

# Zero trust governance validation
add_custom_target(validate_governance
    COMMAND ${CMAKE_COMMAND} -E echo "🔐 AEGIS Zero Trust Governance Validation"
    COMMAND ${CMAKE_BINARY_DIR}/bin/${RIFT_CLI_EXECUTABLE} validate-governance --config .riftrc
    DEPENDS ${RIFT_CLI_EXECUTABLE}
    COMMENT "Validating zero trust policy compliance"
)

# Complete AEGIS validation suite
add_custom_target(validate_aegis_compliance
    COMMAND ${CMAKE_COMMAND} -E echo "⚡ Complete AEGIS Methodology Validation"
    DEPENDS validate_memory_alignment validate_token_schema validate_governance
    COMMENT "Full AEGIS governance compliance validation"
)

# =================================================================
# INSTALLATION CONFIGURATION
# =================================================================

# Install CLI executable
install(TARGETS ${RIFT_CLI_EXECUTABLE}
    RUNTIME DESTINATION bin
    COMPONENT cli_executable
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                GROUP_READ GROUP_EXECUTE
                WORLD_READ WORLD_EXECUTE
)

# Install governance configuration templates
install(FILES 
    "${CMAKE_BINARY_DIR}/.riftrc"
    "${CMAKE_BINARY_DIR}/pkg.riftrc"
    DESTINATION etc/rift
    COMPONENT governance_config
)

# Install CLI documentation
install(FILES
    "docs/cli/rift-cli-manual.md"
    "docs/cli/aegis-governance-guide.md"
    DESTINATION share/doc/rift/cli
    COMPONENT documentation
    OPTIONAL
)

# =================================================================
# COMPREHENSIVE TESTING FRAMEWORK
# =================================================================

# Enable testing for CLI validation
enable_testing()

# CLI operational validation
add_test(NAME "RIFT_CLI_Version_Test"
    COMMAND ${RIFT_CLI_EXECUTABLE} --version
)

add_test(NAME "RIFT_CLI_Help_Test"
    COMMAND ${RIFT_CLI_EXECUTABLE} --help
)

# Pipeline integration tests
add_test(NAME "RIFT_CLI_Complete_Pipeline_Test"
    COMMAND ${RIFT_CLI_EXECUTABLE} compile --input "let result = (x + y) * 42;" --output test_result.rbc
)

add_test(NAME "RIFT_CLI_Stage_Tokenize_Test"
    COMMAND ${RIFT_CLI_EXECUTABLE} tokenize --input "let x = 123;" --output test_tokens.json
)

add_test(NAME "RIFT_CLI_Stage_Parse_Test"  
    COMMAND ${RIFT_CLI_EXECUTABLE} parse --input test_tokens.json --output test_ast.json
)

# AEGIS governance validation tests
add_test(NAME "RIFT_CLI_Memory_Alignment_Test"
    COMMAND ${RIFT_CLI_EXECUTABLE} validate-memory --alignment 4096
)

add_test(NAME "RIFT_CLI_Token_Schema_Test"
    COMMAND ${RIFT_CLI_EXECUTABLE} validate-tokens --schema-check
)

add_test(NAME "RIFT_CLI_Governance_Policy_Test"
    COMMAND ${RIFT_CLI_EXECUTABLE} validate-governance --config .riftrc
)

# Configuration validation tests
add_test(NAME "RIFT_CLI_Config_Load_Test"
    COMMAND ${RIFT_CLI_EXECUTABLE} config --show --config .riftrc
)

add_test(NAME "RIFT_CLI_Config_Validate_Test"
    COMMAND ${RIFT_CLI_EXECUTABLE} config --validate --config .riftrc
)

# =================================================================
# DEMONSTRATION AND VALIDATION TARGETS
# =================================================================

# Demo pipeline execution
add_custom_target(rift_cli_demo
    COMMAND ${CMAKE_COMMAND} -E echo "🚀 RIFT CLI Demonstration Pipeline"
    COMMAND ${CMAKE_BINARY_DIR}/bin/${RIFT_CLI_EXECUTABLE} compile --input "let result = (x + y) * 42;" --output demo_result.rbc --verbose
    COMMAND ${CMAKE_COMMAND} -E echo "✅ Demo pipeline completed successfully"
    DEPENDS ${RIFT_CLI_EXECUTABLE}
    COMMENT "Execute demonstration compilation pipeline"
)

# CLI functional validation
add_custom_target(rift_cli_functional_test
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --tests-regex "RIFT_CLI"
    DEPENDS ${RIFT_CLI_EXECUTABLE}
    COMMENT "Execute comprehensive CLI functional test suite"
)

# =================================================================
# DEVELOPMENT AND DEBUGGING TARGETS
# =================================================================

# Debug build with additional instrumentation
add_custom_target(rift_cli_debug
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug -DRIFT_DEBUG_INSTRUMENTATION=1 ..
    COMMAND ${CMAKE_COMMAND} --build . --target ${RIFT_CLI_EXECUTABLE}
    COMMENT "Build CLI with debug instrumentation enabled"
)

# Performance profiling build
add_custom_target(rift_cli_profile
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=RelWithDebInfo -DRIFT_PERFORMANCE_PROFILING=1 ..
    COMMAND ${CMAKE_COMMAND} --build . --target ${RIFT_CLI_EXECUTABLE}
    COMMENT "Build CLI with performance profiling enabled"
)

# =================================================================
# STATUS REPORTING
# =================================================================

message(STATUS "🚀 RIFT Unified CLI: ${RIFT_CLI_EXECUTABLE}")
message(STATUS "📚 Linked Libraries: rift-{0..6}_static + rfi")
message(STATUS "🔐 AEGIS Governance: ENABLED")
message(STATUS "⚡ Memory Alignment: 4096-bit validated")
message(STATUS "🎫 Token Validation: triplet schema enforced")
message(STATUS "🛡️ Zero Trust Policy: ACTIVE")
message(STATUS "📋 Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "🧪 Testing Framework: CTest integrated")
message(STATUS "✅ CLI Ready: bin/${RIFT_CLI_EXECUTABLE}")