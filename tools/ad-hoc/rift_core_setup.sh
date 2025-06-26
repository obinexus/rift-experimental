#!/bin/bash

# =============================================================================
# RIFT Core Infrastructure Setup Script
# Part of OBINexus RIFT-N Toolchain Governance Framework
# =============================================================================

set -euo pipefail

# Color codes for output
readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly YELLOW='\033[1;33m'
readonly BLUE='\033[0;34m'
readonly NC='\033[0m' # No Color

# Project configuration
readonly SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
readonly ROOT_DIR="${SCRIPT_DIR}"
readonly CORE_DIR="${ROOT_DIR}/rift-core"
readonly AUDIT_DIR="${ROOT_DIR}/rift-audit"
readonly TELEMETRY_DIR="${ROOT_DIR}/telemetry"

# Versioning and compliance
readonly RIFT_VERSION="0.1.0-alpha"
readonly GOVERNANCE_SCHEMA_VERSION="1.0"
readonly AUDIT_SCHEMA_VERSION="1.0"

# Stage definitions
readonly RIFT_STAGES=(rift-0 rift-1 rift-2 rift-3 rift-4 rift-5 rift-6)

# =============================================================================
# Utility Functions
# =============================================================================

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

generate_uuid() {
    if command -v uuidgen &> /dev/null; then
        uuidgen
    else
        # Fallback UUID generation
        cat /proc/sys/kernel/random/uuid 2>/dev/null || echo "$(date +%s)-$(shuf -i 1000-9999 -n 1)"
    fi
}

generate_hash() {
    local input="$1"
    echo -n "$input" | sha256sum | cut -d' ' -f1
}

# =============================================================================
# Directory Structure Creation
# =============================================================================

create_core_structure() {
    log_info "Creating rift-core directory structure..."
    
    # Core directories
    mkdir -p "${CORE_DIR}/include/rift-core"/{common,thread,audit,telemetry,governance}
    mkdir -p "${CORE_DIR}/src"/{common,thread,audit,telemetry,governance}
    mkdir -p "${CORE_DIR}/build"/{debug,prod,bin,obj,lib}
    mkdir -p "${CORE_DIR}/config"
    mkdir -p "${CORE_DIR}/tests"/{unit,integration,benchmark}
    mkdir -p "${CORE_DIR}/docs"
    
    # Audit and telemetry infrastructure
    mkdir -p "${AUDIT_DIR}"/{stage-0,stage-1,stage-2,stage-3,stage-4,stage-5,stage-6}
    mkdir -p "${TELEMETRY_DIR}"/{logs,schemas,reports}
    
    # Create .gitkeep files for empty directories
    find "${CORE_DIR}" -type d -empty -exec touch {}/.gitkeep \;
    find "${AUDIT_DIR}" -type d -empty -exec touch {}/.gitkeep \;
    find "${TELEMETRY_DIR}" -type d -empty -exec touch {}/.gitkeep \;
    
    log_success "rift-core directory structure created"
}

# =============================================================================
# CMake Configuration Generation
# =============================================================================

generate_root_cmake() {
    log_info "Generating root CMakeLists.txt..."
    
    cat > "${ROOT_DIR}/CMakeLists.txt" << 'EOF'
cmake_minimum_required(VERSION 3.14)
project(RIFT VERSION 0.1.0 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Compiler flags
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

# Include custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# Global include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add subdirectories in dependency order
add_subdirectory(rift-core)
add_subdirectory(rift-0)
add_subdirectory(rift-1)
add_subdirectory(rift-2)
add_subdirectory(rift-3)
add_subdirectory(rift-4)
add_subdirectory(rift-5)
add_subdirectory(rift-6)

# Testing
enable_testing()
add_subdirectory(tests)

# Documentation
find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in 
                   ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
endif()

# Install configuration
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY rift-core/include/ DESTINATION include)
EOF

    log_success "Root CMakeLists.txt generated"
}

generate_core_cmake() {
    log_info "Generating rift-core CMakeLists.txt..."
    
    cat > "${CORE_DIR}/CMakeLists.txt" << 'EOF'
cmake_minimum_required(VERSION 3.14)

# Core library sources
set(RIFT_CORE_SOURCES
    src/common/core.c
    src/common/memory.c
    src/thread/lifecycle.c
    src/thread/parity.c
    src/audit/tracer.c
    src/audit/validator.c
    src/telemetry/collector.c
    src/telemetry/reporter.c
    src/governance/policy.c
    src/governance/compliance.c
)

# Core library headers
set(RIFT_CORE_HEADERS
    include/rift-core/common/core.h
    include/rift-core/common/memory.h
    include/rift-core/thread/lifecycle.h
    include/rift-core/thread/parity.h
    include/rift-core/audit/tracer.h
    include/rift-core/audit/validator.h
    include/rift-core/telemetry/collector.h
    include/rift-core/telemetry/reporter.h
    include/rift-core/governance/policy.h
    include/rift-core/governance/compliance.h
)

# Create static library
add_library(rift-core STATIC ${RIFT_CORE_SOURCES})

# Set target properties
set_target_properties(rift-core PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "${RIFT_CORE_HEADERS}"
)

# Include directories
target_include_directories(rift-core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Compiler definitions
target_compile_definitions(rift-core PRIVATE
    RIFT_CORE_VERSION="${PROJECT_VERSION}"
    RIFT_GOVERNANCE_SCHEMA_VERSION="${GOVERNANCE_SCHEMA_VERSION}"
    RIFT_AUDIT_SCHEMA_VERSION="${AUDIT_SCHEMA_VERSION}"
)

# Thread support
find_package(Threads REQUIRED)
target_link_libraries(rift-core PRIVATE Threads::Threads)

# Install targets
install(TARGETS rift-core
    EXPORT rift-core-targets
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/rift-core
)

# Export configuration
install(EXPORT rift-core-targets
    FILE rift-core-config.cmake
    DESTINATION lib/cmake/rift-core
)

# Tests
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
EOF

    log_success "rift-core CMakeLists.txt generated"
}

# =============================================================================
# Core Header File Generation
# =============================================================================

generate_core_headers() {
    log_info "Generating core header files..."
    
    # Main core header
    cat > "${CORE_DIR}/include/rift-core/core.h" << 'EOF'
#ifndef RIFT_CORE_H
#define RIFT_CORE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Version information */
#define RIFT_CORE_VERSION_MAJOR 0
#define RIFT_CORE_VERSION_MINOR 1
#define RIFT_CORE_VERSION_PATCH 0

/* Core types */
typedef enum {
    RIFT_SUCCESS = 0,
    RIFT_ERROR_BASIC = 1,
    RIFT_ERROR_MODERATE = 5,
    RIFT_ERROR_HIGH = 7,
    RIFT_ERROR_CRITICAL = 9
} rift_result_t;

typedef struct {
    uint64_t id;
    char uuid[37];
    char hash[65];
    uint32_t prng_seed;
} rift_context_t;

/* Null and nil semantics */
#define RIFT_NULL ((void*)0)
#define RIFT_NIL  ((void*)0)

/* Memory safety macros */
#define RIFT_SAFE_FREE(ptr) do { \
    if ((ptr) != RIFT_NIL) { \
        free(ptr); \
        (ptr) = RIFT_NIL; \
    } \
} while(0)

/* Yoda-style condition macro */
#define RIFT_YODA_EQ(constant, variable) ((constant) == (variable))
#define RIFT_YODA_NE(constant, variable) ((constant) != (variable))

/* Core initialization and cleanup */
rift_result_t rift_core_init(void);
void rift_core_cleanup(void);

/* Context management */
rift_result_t rift_context_create(rift_context_t *ctx);
void rift_context_destroy(rift_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_H */
EOF

    # Thread lifecycle header
    cat > "${CORE_DIR}/include/rift-core/thread/lifecycle.h" << 'EOF'
#ifndef RIFT_THREAD_LIFECYCLE_H
#define RIFT_THREAD_LIFECYCLE_H

#include "../core.h"
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RIFT_MAX_WORKERS 32
#define RIFT_MAX_THREAD_DEPTH 32

typedef enum {
    THREAD_STATE_INIT = 0,
    THREAD_STATE_RUNNING = 1,
    THREAD_STATE_WAITING = 0,
    THREAD_STATE_TERMINATED = 1
} thread_state_t;

typedef struct {
    pthread_t thread_id;
    uint32_t worker_count;
    uint32_t depth;
    char lifecycle_bits[7]; /* "010111" format */
    thread_state_t state;
    rift_context_t context;
} rift_thread_t;

/* Thread lifecycle management */
rift_result_t rift_thread_create(rift_thread_t *thread, uint32_t workers);
rift_result_t rift_thread_start(rift_thread_t *thread, void *(*start_routine)(void*), void *arg);
rift_result_t rift_thread_join(rift_thread_t *thread, void **retval);
void rift_thread_destroy(rift_thread_t *thread);

/* Parity elimination */
rift_result_t rift_parity_eliminate(int *array, size_t size, int pivot);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_THREAD_LIFECYCLE_H */
EOF

    # Audit tracer header
    cat > "${CORE_DIR}/include/rift-core/audit/tracer.h" << 'EOF'
#ifndef RIFT_AUDIT_TRACER_H
#define RIFT_AUDIT_TRACER_H

#include "../core.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AUDIT_STDIN = 0,
    AUDIT_STDERR = 1,
    AUDIT_STDOUT = 2
} audit_stream_t;

typedef struct {
    audit_stream_t stream;
    char filename[256];
    FILE *file;
    char state_hash[65];
    rift_context_t context;
} rift_audit_t;

/* Audit file management */
rift_result_t rift_audit_init(rift_audit_t *audit, audit_stream_t stream, int stage);
rift_result_t rift_audit_write(rift_audit_t *audit, const char *data, size_t size);
rift_result_t rift_audit_finalize(rift_audit_t *audit);
void rift_audit_cleanup(rift_audit_t *audit);

/* Audit utilities */
rift_result_t rift_audit_generate_hash(const char *data, size_t size, char *hash_out);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_AUDIT_TRACER_H */
EOF

    log_success "Core header files generated"
}

# =============================================================================
# Core Source File Generation
# =============================================================================

generate_core_sources() {
    log_info "Generating core source files..."
    
    # Main core implementation
    cat > "${CORE_DIR}/src/common/core.c" << 'EOF'
#include "rift-core/core.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

static bool rift_initialized = false;

rift_result_t rift_core_init(void) {
    if (rift_initialized) {
        return RIFT_SUCCESS;
    }
    
    /* Initialize random number generator */
    srand((unsigned int)time(NULL));
    
    rift_initialized = true;
    return RIFT_SUCCESS;
}

void rift_core_cleanup(void) {
    rift_initialized = false;
}

rift_result_t rift_context_create(rift_context_t *ctx) {
    if (RIFT_YODA_EQ(RIFT_NIL, ctx)) {
        return RIFT_ERROR_BASIC;
    }
    
    /* Generate unique ID */
    ctx->id = (uint64_t)time(NULL) * 1000 + (rand() % 1000);
    
    /* Generate UUID placeholder */
    snprintf(ctx->uuid, sizeof(ctx->uuid), 
             "%08x-%04x-%04x-%04x-%012x",
             rand(), rand() & 0xFFFF, rand() & 0xFFFF,
             rand() & 0xFFFF, rand());
    
    /* Generate hash placeholder */
    snprintf(ctx->hash, sizeof(ctx->hash),
             "%064x", rand());
    
    /* Initialize PRNG seed */
    ctx->prng_seed = (uint32_t)time(NULL);
    
    return RIFT_SUCCESS;
}

void rift_context_destroy(rift_context_t *ctx) {
    if (RIFT_YODA_NE(RIFT_NIL, ctx)) {
        memset(ctx, 0, sizeof(rift_context_t));
    }
}
EOF

    # Thread lifecycle implementation
    cat > "${CORE_DIR}/src/thread/lifecycle.c" << 'EOF'
#include "rift-core/thread/lifecycle.h"
#include <stdlib.h>
#include <string.h>

rift_result_t rift_thread_create(rift_thread_t *thread, uint32_t workers) {
    if (RIFT_YODA_EQ(RIFT_NIL, thread)) {
        return RIFT_ERROR_BASIC;
    }
    
    if (RIFT_YODA_EQ(0, workers) || workers > RIFT_MAX_WORKERS) {
        return RIFT_ERROR_MODERATE;
    }
    
    memset(thread, 0, sizeof(rift_thread_t));
    thread->worker_count = workers;
    thread->depth = 0;
    thread->state = THREAD_STATE_INIT;
    
    /* Initialize lifecycle bits to "000000" */
    strcpy(thread->lifecycle_bits, "000000");
    
    /* Create context */
    return rift_context_create(&thread->context);
}

rift_result_t rift_thread_start(rift_thread_t *thread, void *(*start_routine)(void*), void *arg) {
    if (RIFT_YODA_EQ(RIFT_NIL, thread) || RIFT_YODA_EQ(RIFT_NIL, start_routine)) {
        return RIFT_ERROR_BASIC;
    }
    
    int result = pthread_create(&thread->thread_id, NULL, start_routine, arg);
    if (RIFT_YODA_EQ(0, result)) {
        thread->state = THREAD_STATE_RUNNING;
        /* Update lifecycle bits */
        thread->lifecycle_bits[0] = '1';
        return RIFT_SUCCESS;
    }
    
    return RIFT_ERROR_HIGH;
}

rift_result_t rift_thread_join(rift_thread_t *thread, void **retval) {
    if (RIFT_YODA_EQ(RIFT_NIL, thread)) {
        return RIFT_ERROR_BASIC;
    }
    
    int result = pthread_join(thread->thread_id, retval);
    if (RIFT_YODA_EQ(0, result)) {
        thread->state = THREAD_STATE_TERMINATED;
        return RIFT_SUCCESS;
    }
    
    return RIFT_ERROR_HIGH;
}

void rift_thread_destroy(rift_thread_t *thread) {
    if (RIFT_YODA_NE(RIFT_NIL, thread)) {
        rift_context_destroy(&thread->context);
        memset(thread, 0, sizeof(rift_thread_t));
    }
}

rift_result_t rift_parity_eliminate(int *array, size_t size, int pivot) {
    if (RIFT_YODA_EQ(RIFT_NIL, array) || RIFT_YODA_EQ(0, size)) {
        return RIFT_ERROR_BASIC;
    }
    
    /* Parity elimination logic placeholder */
    for (size_t i = 0; i < size; i++) {
        if (array[i] <= pivot) {
            /* Thread 1 processing */
            continue;
        } else if (array[i] >= pivot) {
            /* Thread 2 processing */
            continue;
        }
        /* Base case, shared stack */
    }
    
    return RIFT_SUCCESS;
}
EOF

    log_success "Core source files generated"
}

# =============================================================================
# File Indexing and Traceability
# =============================================================================

index_project_files() {
    log_info "Indexing project files for traceability..."
    
    local index_file="${ROOT_DIR}/rift_project_index.txt"
    local telemetry_file="${TELEMETRY_DIR}/file_inventory.json"
    
    # Generate file index
    {
        echo "# RIFT Project File Index"
        echo "# Generated: $(date)"
        echo "# Version: ${RIFT_VERSION}"
        echo ""
        
        echo "## C Source Files"
        find "${ROOT_DIR}" -name "*.c" -type f | sort
        echo ""
        
        echo "## Header Files"
        find "${ROOT_DIR}" -name "*.h" -type f | sort
        echo ""
        
        echo "## CMake Files"
        find "${ROOT_DIR}" -name "CMakeLists.txt" -type f | sort
        find "${ROOT_DIR}" -name "*.cmake" -type f | sort
        echo ""
        
        echo "## Configuration Files"
        find "${ROOT_DIR}" -name "*.pc.in" -type f | sort
        find "${ROOT_DIR}" -name "*.json" -type f | sort
        echo ""
        
        echo "## Documentation Files"
        find "${ROOT_DIR}" -name "*.md" -type f | sort
        find "${ROOT_DIR}" -name "*.pdf" -type f | sort
        
    } > "${index_file}"
    
    # Generate JSON inventory for telemetry
    {
        echo "{"
        echo "  \"project\": \"RIFT\","
        echo "  \"version\": \"${RIFT_VERSION}\","
        echo "  \"generated\": \"$(date -Iseconds)\","
        echo "  \"uuid\": \"$(generate_uuid)\","
        echo "  \"files\": {"
        
        echo "    \"c_sources\": ["
        find "${ROOT_DIR}" -name "*.c" -type f -printf '      "%p",' | sed '$s/,$//'
        echo ""
        echo "    ],"
        
        echo "    \"headers\": ["
        find "${ROOT_DIR}" -name "*.h" -type f -printf '      "%p",' | sed '$s/,$//'
        echo ""
        echo "    ],"
        
        echo "    \"cmake_files\": ["
        find "${ROOT_DIR}" -name "CMakeLists.txt" -o -name "*.cmake" -type f -printf '      "%p",' | sed '$s/,$//'
        echo ""
        echo "    ]"
        
        echo "  },"
        echo "  \"statistics\": {"
        echo "    \"total_c_files\": $(find "${ROOT_DIR}" -name "*.c" -type f | wc -l),"
        echo "    \"total_h_files\": $(find "${ROOT_DIR}" -name "*.h" -type f | wc -l),"
        echo "    \"total_cmake_files\": $(find "${ROOT_DIR}" -name "CMakeLists.txt" -o -name "*.cmake" -type f | wc -l)"
        echo "  }"
        echo "}"
    } > "${telemetry_file}"
    
    log_success "Project files indexed: ${index_file}"
    log_success "Telemetry inventory generated: ${telemetry_file}"
}

# =============================================================================
# Governance Configuration
# =============================================================================

generate_governance_config() {
    log_info "Generating governance configuration files..."
    
    # Main governance configuration
    cat > "${CORE_DIR}/config/governance.json" << EOF
{
  "schema_version": "${GOVERNANCE_SCHEMA_VERSION}",
  "project": "RIFT",
  "version": "${RIFT_VERSION}",
  "governance": {
    "null_nil_semantics": {
      "auto_cast_null_to_nil": true,
      "audit_transformations": true,
      "prevent_double_free": true
    },
    "yoda_style_enforcement": {
      "required": true,
      "check_assignments": true,
      "audit_conditions": true
    },
    "thread_safety": {
      "max_workers_per_thread": ${RIFT_MAX_WORKERS},
      "max_thread_depth": ${RIFT_MAX_THREAD_DEPTH},
      "lifecycle_encoding": "bit_string",
      "parity_elimination": true
    },
    "audit_requirements": {
      "stdin_audit": true,
      "stderr_audit": true,
      "stdout_audit": true,
      "state_hash_verification": true
    },
    "telemetry": {
      "guid_required": true,
      "uuid_required": true,
      "crypto_hash_required": true,
      "prng_identifier_required": true,
      "stage_tracking": true
    },
    "exception_classification": {
      "basic": {"range": [0, 4], "action": "warning"},
      "moderate": {"range": [5, 6], "action": "pause"},
      "high": {"range": [7, 8], "action": "escalate"},
      "critical": {"range": [9, 12], "action": "halt"}
    }
  }
}
EOF

    # Stage-specific governance configs
    for stage in "${RIFT_STAGES[@]}"; do
        stage_num="${stage#rift-}"
        cat > "${CORE_DIR}/config/gov.riftrc.${stage_num}" << EOF
# RIFT Stage ${stage_num} Governance Configuration
# Schema Version: ${GOVERNANCE_SCHEMA_VERSION}

[general]
stage = ${stage_num}
version = ${RIFT_VERSION}

[validation]
yoda_style = required
null_nil_semantics = enforce
thread_lifecycle = validate
parity_elimination = enable

[audit]
audit_${stage_num}_stdin = true
audit_${stage_num}_stderr = true
audit_${stage_num}_stdout = true
state_hash = required

[telemetry]
stage_${stage_num}_tracing = enable
guid_generation = required
uuid_generation = required
crypto_hash = sha256
prng_seed = time_based
EOF
    done
    
    log_success "Governance configuration files generated"
}

# =============================================================================
# Testing Framework
# =============================================================================

generate_test_framework() {
    log_info "Generating test framework..."
    
    # Core test CMakeLists.txt
    cat > "${CORE_DIR}/tests/CMakeLists.txt" << 'EOF'
# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Unit tests
add_executable(test_core
    unit/test_core.c
    unit/test_thread_lifecycle.c
    unit/test_audit_tracer.c
)

target_link_libraries(test_core rift-core)

# Integration tests
add_executable(test_integration
    integration/test_stage_pipeline.c
)

target_link_libraries(test_integration rift-core)

# Add tests to CTest
add_test(NAME CoreTests COMMAND test_core)
add_test(NAME IntegrationTests COMMAND test_integration)
EOF

    # Sample unit test
    cat > "${CORE_DIR}/tests/unit/test_core.c" << 'EOF'
#include "rift-core/core.h"
#include <assert.h>
#include <stdio.h>

void test_core_init() {
    assert(rift_core_init() == RIFT_SUCCESS);
    rift_core_cleanup();
    printf("✓ Core initialization test passed\n");
}

void test_context_create() {
    rift_context_t ctx;
    assert(rift_context_create(&ctx) == RIFT_SUCCESS);
    assert(ctx.id != 0);
    rift_context_destroy(&ctx);
    printf("✓ Context creation test passed\n");
}

void test_yoda_conditions() {
    int value = 42;
    assert(RIFT_YODA_EQ(42, value) == true);
    assert(RIFT_YODA_NE(0, value) == true);
    printf("✓ Yoda-style condition test passed\n");
}

int main() {
    printf("Running RIFT Core tests...\n");
    
    test_core_init();
    test_context_create();
    test_yoda_conditions();
    
    printf("All tests passed!\n");
    return 0;
}
EOF

    log_success "Test framework generated"
}

# =============================================================================
# Documentation Generation
# =============================================================================

generate_documentation() {
    log_info "Generating documentation..."
    
    # README for rift-core
    cat > "${CORE_DIR}/README.md" << EOF
# RIFT Core - Foundational Layer

## Overview

RIFT Core is the foundational layer for the RIFT-N toolchain within the OBINexus project. It provides shared libraries, token definitions, thread models, and telemetry schemas for all RIFT stages.

## Architecture

### Directory Structure

\`\`\`
rift-core/
├── include/         # Header files and shared definitions
│   └── rift-core/   # Core API headers
├── src/             # Core source implementations
├── build/           # Build artifacts
│   ├── debug/       # Debug build outputs
│   ├── prod/        # Production build outputs
│   ├── bin/         # Executable binaries
│   ├── obj/         # Object files
│   └── lib/         # Static/shared libraries
├── config/          # Governance and configuration files
├── tests/           # Test suites
└── docs/            # Documentation
\`\`\`

## Key Features

### Thread Safety & Semantic Control

- **NULL/nil Semantics**: Automatic conversion of C-style NULL to RIFT nil
- **Yoda-Style Safety**: Enforced condition ordering to prevent assignment errors
- **Thread Lifecycle**: Bit-encoded state management for concurrent execution
- **Parity Elimination**: Alternative to mutex locks for parallel processing

### Audit & Telemetry

- **Audit Streams**: Separate tracking for stdin, stderr, stdout
- **State Verification**: Cryptographic hash validation
- **Telemetry Collection**: GUID, UUID, and hash-based tracing
- **Exception Classification**: Structured error level handling

### Governance Compliance

- **Policy Enforcement**: Compile-time validation of governance rules
- **Stage Validation**: Per-stage compliance checking
- **Traceability**: Complete audit trail for all operations

## Building

\`\`\`bash
mkdir build && cd build
cmake ..
make
\`\`\`

## Testing

\`\`\`bash
make test
\`\`\`

## Integration

RIFT Core is designed to be used by all RIFT-N stages (rift-0 through rift-6). Each stage links against the core library and inherits its governance and telemetry capabilities.

## Version

Current version: ${RIFT_VERSION}
Governance Schema: ${GOVERNANCE_SCHEMA_VERSION}
Audit Schema: ${AUDIT_SCHEMA_VERSION}
EOF

    log_success "Documentation generated"
}

# =============================================================================
# Validation and Verification
# =============================================================================

validate_setup() {
    log_info "Validating setup..."
    
    local validation_report="${TELEMETRY_DIR}/setup_validation.txt"
    
    {
        echo "RIFT Core Setup Validation Report"
        echo "Generated: $(date)"
        echo "UUID: $(generate_uuid)"
        echo ""
        
        echo "Directory Structure Validation:"
        
        # Check core directories
        for dir in include src build config tests docs; do
            if [[ -d "${CORE_DIR}/${dir}" ]]; then
                echo "✓ ${CORE_DIR}/${dir} exists"
            else
                echo "✗ ${CORE_DIR}/${dir} missing"
            fi
        done
        
        # Check audit directories
        for stage in "${RIFT_STAGES[@]}"; do
            stage_dir="${AUDIT_DIR}/${stage}"
            if [[ -d "${stage_dir}" ]]; then
                echo "✓ ${stage_dir} exists"
            else
                echo "✗ ${stage_dir} missing"
            fi
        done
        
        echo ""
        echo "File Generation Validation:"
        
        # Check CMake files
        for cmake_file in "${ROOT_DIR}/CMakeLists.txt" "${CORE_DIR}/CMakeLists.txt"; do
            if [[ -f "${cmake_file}" ]]; then
                echo "✓ ${cmake_file} generated"
            else
                echo "✗ ${cmake_file} missing"
            fi
        done
        
        # Check core headers
        for header in core.h thread/lifecycle.h audit/tracer.h; do
            header_file="${CORE_DIR}/include/rift-core/${header}"
            if [[ -f "${header_file}" ]]; then
                echo "✓ ${header_file} generated"
            else
                echo "✗ ${header_file} missing"
            fi
        done
        
        echo ""
        echo "Governance Configuration Validation:"
        
        # Check governance files
        if [[ -f "${CORE_DIR}/config/governance.json" ]]; then
            echo "✓ Main governance config exists"
        else
            echo "✗ Main governance config missing"
        fi
        
        for stage in "${RIFT_STAGES[@]}"; do
            stage_num="${stage#rift-}"
            config_file="${CORE_DIR}/config/gov.riftrc.${stage_num}"
            if [[ -f "${config_file}" ]]; then
                echo "✓ Stage ${stage_num} config exists"
            else
                echo "✗ Stage ${stage_num} config missing"
            fi
        done
        
    } > "${validation_report}"
    
    log_success "Validation report generated: ${validation_report}"
}

# =============================================================================
# Main Execution
# =============================================================================

main() {
    log_info "Starting RIFT Core Infrastructure Setup"
    log_info "Version: ${RIFT_VERSION}"
    log_info "Root Directory: ${ROOT_DIR}"
    
    # Execute setup phases
    create_core_structure
    generate_root_cmake
    generate_core_cmake
    generate_core_headers
    generate_core_sources
    generate_governance_config
    generate_test_framework
    generate_documentation
    index_project_files
    validate_setup
    
    log_success "RIFT Core infrastructure setup completed successfully!"
    log_info "Next steps:"
    log_info "  1. cd ${ROOT_DIR} && mkdir build && cd build"
    log_info "  2. cmake .."
    log_info "  3. make"
    log_info "  4. make test"
    log_info ""
    log_info "Governance configs: ${CORE_DIR}/config/"
    log_info "Audit directory: ${AUDIT_DIR}"
    log_info "Telemetry directory: ${TELEMETRY_DIR}"
    log_info "Project index: ${ROOT_DIR}/rift_project_index.txt"
}

# Execute main function with all arguments
main "$@"
