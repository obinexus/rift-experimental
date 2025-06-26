#!/bin/bash

# =============================================================================
# RIFT-Core Pure Infrastructure Setup Script
# OBINexus RIFT-N Toolchain - Aegis Project Implementation
# Technical Lead: Nnamdi Michael Okpala
# Waterfall Phase: Core Infrastructure Deployment
# =============================================================================

set -euo pipefail

# =============================================================================
# RIFT-Core Configuration Constants
# =============================================================================

readonly SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
readonly PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
readonly RIFT_CORE_DIR="$PROJECT_ROOT/rift-core"
readonly RIFT_AUDIT_DIR="$PROJECT_ROOT/rift-audit"
readonly RIFT_TELEMETRY_DIR="$PROJECT_ROOT/rift-telemetry"

# Technical metadata for Aegis project
readonly RIFT_VERSION="2.1.0-core"
readonly RIFT_GOVERNANCE_SCHEMA="2.1"
readonly RIFT_AUDIT_SCHEMA="2.1"
readonly AEGIS_PROJECT_LEAD="Nnamdi Michael Okpala"

# RIFT-Core constants synchronized with C implementation
readonly RIFT_MAX_WORKERS=32
readonly RIFT_MAX_THREAD_DEPTH=32
readonly RIFT_STAGE_COUNT=7

# RIFT-exclusive color governance (no Polycall dependencies)
readonly RIFT_COLOR_CRITICAL='\033[0;31m'    # Red - Fatal errors
readonly RIFT_COLOR_HIGH='\033[0;33m'        # Orange - Escalated issues
readonly RIFT_COLOR_MODERATE='\033[1;33m'    # Yellow - Recoverable warnings
readonly RIFT_COLOR_INFO='\033[0;34m'        # Blue - Neutral diagnostics
readonly RIFT_COLOR_SUCCESS='\033[0;32m'     # Green - Validated transitions
readonly RIFT_COLOR_RESET='\033[0m'

# Command line options
DRY_RUN=false
VERBOSE=false
FORCE_REBUILD=false

# =============================================================================
# RIFT-Core Logging System (Polycall-Free)
# =============================================================================

rift_log_with_color() {
    local level="$1"
    local message="$2"
    local timestamp="$(date '+%Y-%m-%d %H:%M:%S')"
    local color=""
    
    case "$level" in
        "CRITICAL") color="$RIFT_COLOR_CRITICAL" ;;
        "HIGH")     color="$RIFT_COLOR_HIGH" ;;
        "MODERATE") color="$RIFT_COLOR_MODERATE" ;;
        "INFO")     color="$RIFT_COLOR_INFO" ;;
        "SUCCESS")  color="$RIFT_COLOR_SUCCESS" ;;
        *)          color="$RIFT_COLOR_INFO" ;;
    esac
    
    echo -e "${color}[RIFT-${level}]${RIFT_COLOR_RESET} ${timestamp} $message"
    
    # Log to RIFT audit stream if available
    if [[ -d "$RIFT_AUDIT_DIR" ]]; then
        echo "[${level}] ${timestamp} $message" >> "$RIFT_AUDIT_DIR/setup.audit"
    fi
}

rift_log_critical() { rift_log_with_color "CRITICAL" "$1"; }
rift_log_high() { rift_log_with_color "HIGH" "$1"; }
rift_log_moderate() { rift_log_with_color "MODERATE" "$1"; }
rift_log_info() { rift_log_with_color "INFO" "$1"; }
rift_log_success() { rift_log_with_color "SUCCESS" "$1"; }

rift_panic() {
    rift_log_critical "🚨 RIFT PANIC: $1"
    rift_log_critical "System entering emergency halt state"
    exit 9
}

# =============================================================================
# RIFT-Core Directory Structure Creation
# =============================================================================

create_rift_core_structure() {
    rift_log_info "Creating RIFT-Core directory infrastructure..."
    
    if [[ "$DRY_RUN" == true ]]; then
        rift_log_info "DRY-RUN: Would create RIFT-Core directory tree"
        cat << 'EOF'
rift-core/
├── include/rift-core/
│   ├── common/          # Core utilities and definitions
│   ├── thread/          # Thread lifecycle management
│   ├── audit/           # Audit stream handling (.audit-0, .audit-1, .audit-2)
│   ├── telemetry/       # RIFT telemetry system
│   ├── governance/      # Policy enforcement engine
│   └── accessibility/   # Color governance (RIFT-native)
├── src/rift-core/
│   ├── common/          # Core implementation
│   ├── thread/          # Thread safety & NULL/nil semantics
│   ├── audit/           # Audit trail generation
│   ├── telemetry/       # Structured logging
│   ├── governance/      # Policy validation
│   └── accessibility/   # Color system implementation
├── config/
│   ├── governance.json  # Main governance schema
│   ├── gov.riftrc.*     # Stage-specific configurations
│   └── audit.schema     # Audit stream schema
├── setup/
│   ├── setup-rift-core.sh  # This script
│   └── migrate-legacy.sh   # Legacy migration tools
├── cmake/
│   ├── RiftCore.cmake   # Core CMake module
│   ├── RiftStages.cmake # Stage configuration
│   └── RiftValidation.cmake # Validation utilities
└── docs/
    ├── rift-core-api.md # API documentation
    └── governance.md    # Governance framework docs

rift-audit/
├── audit-0/             # stdin audit streams
├── audit-1/             # stderr audit streams
├── audit-2/             # stdout audit streams
└── governance/          # governance audit logs

rift-telemetry/
├── logs/                # Structured log files
├── schemas/             # Telemetry schemas
└── reports/             # Generated reports
EOF
        return 0
    fi
    
    # Create RIFT-Core infrastructure
    mkdir -p "$RIFT_CORE_DIR"/{include/rift-core,src/rift-core,config,setup,cmake,docs}
    mkdir -p "$RIFT_CORE_DIR/include/rift-core"/{common,thread,audit,telemetry,governance,accessibility}
    mkdir -p "$RIFT_CORE_DIR/src/rift-core"/{common,thread,audit,telemetry,governance,accessibility}
    
    # Create audit infrastructure with .audit-N structure
    mkdir -p "$RIFT_AUDIT_DIR"/{audit-0,audit-1,audit-2,governance}
    
    # Create telemetry infrastructure
    mkdir -p "$RIFT_TELEMETRY_DIR"/{logs,schemas,reports}
    
    # Create .gitkeep files for empty directories
    find "$RIFT_CORE_DIR" -type d -empty -exec touch {}/.gitkeep \;
    find "$RIFT_AUDIT_DIR" -type d -empty -exec touch {}/.gitkeep \;
    find "$RIFT_TELEMETRY_DIR" -type d -empty -exec touch {}/.gitkeep \;
    
    rift_log_success "RIFT-Core directory structure created"
}

# =============================================================================
# RIFT-Core Header Generation (Pure RIFT Implementation)
# =============================================================================

generate_rift_core_headers() {
    rift_log_info "Generating RIFT-Core header definitions..."
    
    # Main RIFT-Core header
    local rift_core_header='#ifndef RIFT_CORE_H
#define RIFT_CORE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* RIFT-Core Version Information */
#define RIFT_CORE_VERSION_MAJOR 2
#define RIFT_CORE_VERSION_MINOR 1
#define RIFT_CORE_VERSION_PATCH 0
#define RIFT_CORE_VERSION_STRING "2.1.0-core"

/* RIFT Exception Classification System */
typedef enum {
    RIFT_SUCCESS = 0,
    RIFT_ERROR_BASIC = 1,      /* 0-4: Non-blocking warnings */
    RIFT_ERROR_MODERATE = 5,   /* 5-6: Recoverable pause states */
    RIFT_ERROR_HIGH = 7,       /* 7-8: Escalated governance check */
    RIFT_ERROR_CRITICAL = 9    /* 9-12: Execution halt + panic mode */
} rift_result_t;

/* RIFT Context Management */
typedef struct {
    uint64_t rift_id;
    char rift_uuid[37];
    char rift_hash[65];
    uint32_t rift_prng_seed;
    uint64_t rift_timestamp;
} rift_context_t;

/* RIFT NULL/nil Semantics (Pure RIFT Implementation) */
#define RIFT_NULL ((void*)0)
#define RIFT_NIL  ((void*)0)

/* RIFT Memory Safety Macros */
#define RIFT_SAFE_FREE(ptr) do { \
    if ((ptr) != RIFT_NIL) { \
        free(ptr); \
        (ptr) = RIFT_NIL; \
    } \
} while(0)

/* RIFT Yoda-Style Condition Macros */
#define RIFT_YODA_EQ(constant, variable) ((constant) == (variable))
#define RIFT_YODA_NE(constant, variable) ((constant) != (variable))
#define RIFT_YODA_LT(constant, variable) ((constant) < (variable))
#define RIFT_YODA_GT(constant, variable) ((constant) > (variable))

/* RIFT-Core Initialization and Cleanup */
rift_result_t rift_core_init(void);
void rift_core_cleanup(void);

/* RIFT Context Management */
rift_result_t rift_context_create(rift_context_t *ctx);
void rift_context_destroy(rift_context_t *ctx);

/* RIFT Color Governance (Native Implementation) */
void rift_color_log(const char* level, const char* message);
void rift_color_log_critical(const char* message);
void rift_color_log_high(const char* message);
void rift_color_log_moderate(const char* message);
void rift_color_log_info(const char* message);
void rift_color_log_success(const char* message);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_H */'
    
    if [[ "$DRY_RUN" == false ]]; then
        echo "$rift_core_header" > "$RIFT_CORE_DIR/include/rift-core/core.h"
    fi
    
    # RIFT Audit System Header
    local rift_audit_header='#ifndef RIFT_AUDIT_H
#define RIFT_AUDIT_H

#include "core.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* RIFT Audit Stream Types */
typedef enum {
    RIFT_AUDIT_STDIN = 0,   /* .audit-0 */
    RIFT_AUDIT_STDERR = 1,  /* .audit-1 */
    RIFT_AUDIT_STDOUT = 2   /* .audit-2 */
} rift_audit_stream_t;

/* RIFT Audit Context */
typedef struct {
    rift_audit_stream_t stream;
    char audit_filename[256];
    FILE *audit_file;
    char state_hash[65];
    rift_context_t context;
    uint64_t audit_sequence;
} rift_audit_t;

/* RIFT Audit Management Functions */
rift_result_t rift_audit_init(rift_audit_t *audit, rift_audit_stream_t stream, int stage);
rift_result_t rift_audit_write(rift_audit_t *audit, const char *data, size_t size);
rift_result_t rift_audit_write_colored(rift_audit_t *audit, const char *level, 
                                      const char *data, size_t size);
rift_result_t rift_audit_finalize(rift_audit_t *audit);
void rift_audit_cleanup(rift_audit_t *audit);

/* RIFT Audit Utilities */
rift_result_t rift_audit_generate_hash(const char *data, size_t size, char *hash_out);
rift_result_t rift_audit_verify_integrity(const rift_audit_t *audit);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_AUDIT_H */'
    
    if [[ "$DRY_RUN" == false ]]; then
        echo "$rift_audit_header" > "$RIFT_CORE_DIR/include/rift-core/audit.h"
    fi
    
    rift_log_success "RIFT-Core headers generated"
}

# =============================================================================
# RIFT-Core Source Implementation
# =============================================================================

generate_rift_core_sources() {
    rift_log_info "Generating RIFT-Core source implementations..."
    
    # Main RIFT-Core implementation
    local rift_core_source='#include "rift-core/core.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

static bool rift_core_initialized = false;

/* RIFT Color Codes (Native Implementation) */
static const char* RIFT_COLOR_CRITICAL = "\033[0;31m";
static const char* RIFT_COLOR_HIGH = "\033[0;33m";
static const char* RIFT_COLOR_MODERATE = "\033[1;33m";
static const char* RIFT_COLOR_INFO = "\033[0;34m";
static const char* RIFT_COLOR_SUCCESS = "\033[0;32m";
static const char* RIFT_COLOR_RESET = "\033[0m";

rift_result_t rift_core_init(void) {
    if (rift_core_initialized) {
        return RIFT_SUCCESS;
    }
    
    /* Initialize RIFT random number generator */
    srand((unsigned int)time(NULL));
    
    rift_core_initialized = true;
    
    /* Log initialization with RIFT color governance */
    rift_color_log_success("RIFT-Core initialized successfully");
    
    return RIFT_SUCCESS;
}

void rift_core_cleanup(void) {
    if (rift_core_initialized) {
        rift_color_log_info("RIFT-Core cleanup initiated");
        rift_core_initialized = false;
    }
}

rift_result_t rift_context_create(rift_context_t *ctx) {
    if (RIFT_YODA_EQ(RIFT_NIL, ctx)) {
        return RIFT_ERROR_BASIC;
    }
    
    /* Generate RIFT unique ID */
    ctx->rift_id = (uint64_t)time(NULL) * 1000 + (rand() % 1000);
    ctx->rift_timestamp = (uint64_t)time(NULL);
    
    /* Generate RIFT UUID */
    snprintf(ctx->rift_uuid, sizeof(ctx->rift_uuid), 
             "rift-%08x-%04x-%04x-%04x-%012x",
             rand(), rand() & 0xFFFF, rand() & 0xFFFF,
             rand() & 0xFFFF, rand());
    
    /* Generate RIFT hash */
    snprintf(ctx->rift_hash, sizeof(ctx->rift_hash),
             "%016lx%016lx%016lx%016lx", 
             (unsigned long)rand(), (unsigned long)rand(),
             (unsigned long)rand(), (unsigned long)rand());
    
    /* Initialize RIFT PRNG seed */
    ctx->rift_prng_seed = (uint32_t)time(NULL);
    
    return RIFT_SUCCESS;
}

void rift_context_destroy(rift_context_t *ctx) {
    if (RIFT_YODA_NE(RIFT_NIL, ctx)) {
        memset(ctx, 0, sizeof(rift_context_t));
    }
}

/* RIFT Color Logging Implementation (Native) */
void rift_color_log(const char* level, const char* message) {
    const char* color = RIFT_COLOR_INFO;
    
    if (strcmp(level, "CRITICAL") == 0) color = RIFT_COLOR_CRITICAL;
    else if (strcmp(level, "HIGH") == 0) color = RIFT_COLOR_HIGH;
    else if (strcmp(level, "MODERATE") == 0) color = RIFT_COLOR_MODERATE;
    else if (strcmp(level, "SUCCESS") == 0) color = RIFT_COLOR_SUCCESS;
    
    printf("%s[RIFT-%s]%s %s\n", color, level, RIFT_COLOR_RESET, message);
}

void rift_color_log_critical(const char* message) { rift_color_log("CRITICAL", message); }
void rift_color_log_high(const char* message) { rift_color_log("HIGH", message); }
void rift_color_log_moderate(const char* message) { rift_color_log("MODERATE", message); }
void rift_color_log_info(const char* message) { rift_color_log("INFO", message); }
void rift_color_log_success(const char* message) { rift_color_log("SUCCESS", message); }'
    
    if [[ "$DRY_RUN" == false ]]; then
        echo "$rift_core_source" > "$RIFT_CORE_DIR/src/rift-core/core.c"
    fi
    
    rift_log_success "RIFT-Core sources generated"
}

# =============================================================================
# RIFT Governance Schema Generation (Pure RIFT)
# =============================================================================

generate_rift_governance_schema() {
    rift_log_info "Generating RIFT governance schema..."
    
    local rift_governance_schema='{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "title": "RIFT-Core Governance Configuration Schema",
  "description": "Pure RIFT governance metadata for compiler pipeline stages with native color governance and audit stream management",
  "version": "'$RIFT_GOVERNANCE_SCHEMA'",
  "type": "object",
  "properties": {
    "rift_package_name": { 
      "type": "string",
      "description": "Name of the RIFT package being governed"
    },
    "rift_version": { 
      "type": "string",
      "pattern": "^\\d+\\.\\d+\\.\\d+(-[a-zA-Z0-9]+)?$",
      "description": "RIFT semantic version"
    },
    "rift_timestamp": { 
      "type": "string", 
      "format": "date-time",
      "description": "RIFT governance configuration timestamp"
    },
    "rift_stage": { 
      "type": "integer", 
      "minimum": 0,
      "maximum": 6,
      "description": "RIFT compilation stage number (0-6)"
    },
    "rift_stage_type": {
      "type": "string",
      "enum": ["legacy", "experimental", "stable", "rift-core"],
      "default": "rift-core",
      "description": "RIFT stage implementation type"
    },
    "rift_governance": {
      "type": "object",
      "description": "RIFT-Core governance configuration",
      "properties": {
        "null_nil_semantics": {
          "type": "object",
          "properties": {
            "auto_cast_null_to_nil": { "type": "boolean", "default": true },
            "audit_transformations": { "type": "boolean", "default": true },
            "prevent_double_free": { "type": "boolean", "default": true }
          }
        },
        "yoda_style_enforcement": {
          "type": "object",
          "properties": {
            "required": { "type": "boolean", "default": true },
            "check_assignments": { "type": "boolean", "default": true },
            "audit_conditions": { "type": "boolean", "default": true }
          }
        },
        "thread_safety": {
          "type": "object",
          "properties": {
            "max_workers_per_thread": { "type": "integer", "default": '$RIFT_MAX_WORKERS' },
            "max_thread_depth": { "type": "integer", "default": '$RIFT_MAX_THREAD_DEPTH' },
            "lifecycle_encoding": { "type": "string", "default": "bit_string" },
            "parity_elimination": { "type": "boolean", "default": true }
          }
        },
        "audit_streams": {
          "type": "object",
          "properties": {
            "audit_0_stdin": { "type": "boolean", "default": true },
            "audit_1_stderr": { "type": "boolean", "default": true },
            "audit_2_stdout": { "type": "boolean", "default": true },
            "state_hash_verification": { "type": "boolean", "default": true }
          }
        },
        "color_governance": {
          "type": "object",
          "properties": {
            "rift_native_colors": { "type": "boolean", "default": true },
            "critical_color": { "type": "string", "default": "red" },
            "high_color": { "type": "string", "default": "orange" },
            "moderate_color": { "type": "string", "default": "yellow" },
            "info_color": { "type": "string", "default": "blue" },
            "success_color": { "type": "string", "default": "green" }
          }
        },
        "exception_classification": {
          "type": "object",
          "properties": {
            "basic": {"range": [0, 4], "action": "warning"},
            "moderate": {"range": [5, 6], "action": "pause"},
            "high": {"range": [7, 8], "action": "escalate"},
            "critical": {"range": [9, 12], "action": "halt"}
          }
        }
      }
    },
    "rift_audit_configuration": {
      "type": "object",
      "properties": {
        "audit_0_path": { "type": "string", "default": "rift-audit/audit-0/" },
        "audit_1_path": { "type": "string", "default": "rift-audit/audit-1/" },
        "audit_2_path": { "type": "string", "default": "rift-audit/audit-2/" },
        "governance_audit_path": { "type": "string", "default": "rift-audit/governance/" },
        "audit_hash_algorithm": { "type": "string", "default": "sha256" },
        "audit_signature_required": { "type": "boolean", "default": true }
      }
    },
    "rift_telemetry": {
      "type": "object",
      "properties": {
        "rift_guid_required": { "type": "boolean", "default": true },
        "rift_uuid_required": { "type": "boolean", "default": true },
        "rift_crypto_hash_required": { "type": "boolean", "default": true },
        "rift_prng_identifier_required": { "type": "boolean", "default": true },
        "rift_stage_tracking": { "type": "boolean", "default": true }
      }
    }
  },
  "required": ["rift_package_name", "rift_version", "rift_stage", "rift_timestamp"],
  "additionalProperties": false
}'
    
    if [[ "$DRY_RUN" == false ]]; then
        echo "$rift_governance_schema" > "$RIFT_CORE_DIR/config/governance.json"
    fi
    
    # Generate stage-specific configurations
    for ((stage=0; stage<RIFT_STAGE_COUNT; stage++)); do
        local stage_config="# RIFT Stage $stage Governance Configuration
# Schema Version: $RIFT_GOVERNANCE_SCHEMA

[rift.general]
stage = $stage
version = $RIFT_VERSION
technical_lead = $AEGIS_PROJECT_LEAD

[rift.validation]
yoda_style = required
null_nil_semantics = enforce
thread_lifecycle = validate
parity_elimination = enable

[rift.audit]
audit_${stage}_stdin = true
audit_${stage}_stderr = true
audit_${stage}_stdout = true
state_hash = required

[rift.telemetry]
stage_${stage}_tracing = enable
rift_guid_generation = required
rift_uuid_generation = required
rift_crypto_hash = sha256
rift_prng_seed = time_based

[rift.color_governance]
native_colors = true
critical_halt = true
exception_escalation = true"
        
        if [[ "$DRY_RUN" == false ]]; then
            echo "$stage_config" > "$RIFT_CORE_DIR/config/gov.riftrc.$stage"
        fi
    done
    
    rift_log_success "RIFT governance schema and stage configurations generated"
}

# =============================================================================
# RIFT CMake Integration
# =============================================================================

generate_rift_cmake() {
    rift_log_info "Generating RIFT-Core CMake configuration..."
    
    # Root CMakeLists.txt
    local root_cmake='cmake_minimum_required(VERSION 3.14)
project(RIFT VERSION 2.1.0 LANGUAGES C)

# RIFT-Core Configuration
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# RIFT-Core compiler flags
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic -DDEBUG -DRIFT_CORE_ENABLED")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG -DRIFT_CORE_ENABLED")

# RIFT-Core definitions
add_compile_definitions(
    RIFT_VERSION="'$RIFT_VERSION'"
    RIFT_GOVERNANCE_SCHEMA_VERSION="'$RIFT_GOVERNANCE_SCHEMA'"
    RIFT_AUDIT_SCHEMA_VERSION="'$RIFT_AUDIT_SCHEMA'"
    RIFT_CORE_NATIVE=1
)

# Include RIFT-Core modules
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rift-core/include)

# Add RIFT-Core subdirectory
add_subdirectory(rift-core)

# Add RIFT stages in dependency order
add_subdirectory(rift-0)
add_subdirectory(rift-1)
add_subdirectory(rift-2)
add_subdirectory(rift-3)
add_subdirectory(rift-4)
add_subdirectory(rift-5)
add_subdirectory(rift-6)

# RIFT-Core testing
enable_testing()
add_subdirectory(tests)

# Install RIFT-Core headers
install(DIRECTORY rift-core/include/ DESTINATION include)'
    
    if [[ "$DRY_RUN" == false ]]; then
        echo "$root_cmake" > "$PROJECT_ROOT/CMakeLists.txt"
    fi
    
    # RIFT-Core CMakeLists.txt
    local core_cmake='cmake_minimum_required(VERSION 3.14)

# RIFT-Core library sources
set(RIFT_CORE_SOURCES
    src/rift-core/core.c
    src/rift-core/audit.c
    src/rift-core/thread.c
    src/rift-core/telemetry.c
    src/rift-core/governance.c
    src/rift-core/accessibility.c
)

# RIFT-Core library headers
set(RIFT_CORE_HEADERS
    include/rift-core/core.h
    include/rift-core/audit.h
    include/rift-core/thread.h
    include/rift-core/telemetry.h
    include/rift-core/governance.h
    include/rift-core/accessibility.h
)

# Create RIFT-Core static library
add_library(rift-core STATIC ${RIFT_CORE_SOURCES})

# Set RIFT-Core target properties
set_target_properties(rift-core PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "${RIFT_CORE_HEADERS}"
)

# RIFT-Core include directories
target_include_directories(rift-core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# RIFT-Core compile definitions
target_compile_definitions(rift-core PRIVATE
    RIFT_CORE_VERSION="'$RIFT_VERSION'"
    RIFT_CORE_INTERNAL=1
    RIFT_NATIVE_IMPLEMENTATION=1
)

# Thread support for RIFT-Core
find_package(Threads REQUIRED)
target_link_libraries(rift-core PRIVATE Threads::Threads)

# Install RIFT-Core targets
install(TARGETS rift-core
    EXPORT rift-core-targets
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/rift-core
)

# Export RIFT-Core configuration
install(EXPORT rift-core-targets
    FILE rift-core-config.cmake
    DESTINATION lib/cmake/rift-core
)'
    
    if [[ "$DRY_RUN" == false ]]; then
        echo "$core_cmake" > "$RIFT_CORE_DIR/CMakeLists.txt"
    fi
    
    rift_log_success "RIFT-Core CMake configuration generated"
}

# =============================================================================
# RIFT Project File Indexing
# =============================================================================

index_rift_project_files() {
    rift_log_info "Indexing RIFT project files with audit traceability..."
    
    local index_file="$PROJECT_ROOT/rift_project_index.txt"
    local telemetry_file="$RIFT_TELEMETRY_DIR/file_inventory.json"
    
    if [[ "$DRY_RUN" == true ]]; then
        rift_log_info "DRY-RUN: Would generate RIFT project file index"
        return 0
    fi
    
    # Generate RIFT project index
    {
        echo "# RIFT Project File Index"
        echo "# Generated: $(date)"
        echo "# Version: $RIFT_VERSION"
        echo "# Technical Lead: $AEGIS_PROJECT_LEAD"
        echo ""
        
        echo "## RIFT-Core Infrastructure Files"
        find "$RIFT_CORE_DIR" -type f \( -name "*.c" -o -name "*.h" \) 2>/dev/null | sort || true
        echo ""
        
        echo "## RIFT Stage Files"
        for ((stage=0; stage<RIFT_STAGE_COUNT; stage++)); do
            echo "### RIFT Stage $stage"
            find "$PROJECT_ROOT/rift-$stage" -type f \( -name "*.c" -o -name "*.h" \) 2>/dev/null | sort || true
        done
        echo ""
        
        echo "## RIFT Configuration Files"
        find "$RIFT_CORE_DIR/config" -type f 2>/dev/null | sort || true
        echo ""
        
        echo "## RIFT CMake Files"
        find "$PROJECT_ROOT" -name "CMakeLists.txt" -type f 2>/dev/null | sort || true
        
    } > "$index_file"
    
    # Generate RIFT telemetry inventory
    {
        echo "{"
        echo "  \"rift_project\": \"RIFT-Core\","
        echo "  \"rift_version\": \"$RIFT_VERSION\","
        echo "  \"generated\": \"$(date -Iseconds)\","
        echo "  \"rift_uuid\": \"rift-$(date +%s)-$(shuf -i 1000-9999 -n 1)\","
        echo "  \"rift_core_enabled\": true,"
        echo "  \"files\": {"
        
        echo "    \"rift_core_sources\": ["
        find "$RIFT_CORE_DIR/src" -name "*.c" -type f 2>/dev/null | sed 's/.*/"&"/' | tr '\n' ',' | sed 's/,$//' || echo ""
        echo ""
        echo "    ],"
        
        echo "    \"rift_core_headers\": ["
        find "$RIFT_CORE_DIR/include" -name "*.h" -type f 2>/dev/null | sed 's/.*/"&"/' | tr '\n' ',' | sed 's/,$//' || echo ""
        echo ""
        echo "    ]"
        
        echo "  },"
        echo "  \"rift_governance\": {"
        echo "    \"schema_version\": \"$RIFT_GOVERNANCE_SCHEMA\","
        echo "    \"audit_streams\": [\"audit-0\", \"audit-1\", \"audit-2\"],"
        echo "    \"color_governance\": \"native_rift\","
        echo "    \"yoda_style_enforced\": true,"
        echo "    \"null_nil_semantics\": true"
        echo "  }"
        echo "}"
    } > "$telemetry_file"
    
    rift_log_success "RIFT project files indexed: $index_file"
    rift_log_success "RIFT telemetry inventory: $telemetry_file"
}

# =============================================================================
# RIFT Validation Framework
# =============================================================================

validate_rift_setup() {
    rift_log_info "Validating RIFT-Core setup and governance compliance..."
    
    local validation_report="$RIFT_TELEMETRY_DIR/rift_validation_$(date +%Y%m%d_%H%M%S).json"
    local validation_errors=0
    
    if [[ "$DRY_RUN" == true ]]; then
        rift_log_info "DRY-RUN: Would generate RIFT validation report"
        return 0
    fi
    
    {
        echo "{"
        echo "  \"rift_validation_timestamp\": \"$(date -Iseconds)\","
        echo "  \"rift_version\": \"$RIFT_VERSION\","
        echo "  \"technical_lead\": \"$AEGIS_PROJECT_LEAD\","
        echo "  \"validation_results\": {"
        
        # Validate RIFT-Core directory structure
        echo "    \"directory_validation\": {"
        local required_dirs=(
            "rift-core"
            "rift-core/include/rift-core"
            "rift-core/src/rift-core"
            "rift-core/config"
            "rift-audit"
            "rift-telemetry"
        )
        
        for dir in "${required_dirs[@]}"; do
            if [[ -d "$PROJECT_ROOT/$dir" ]]; then
                echo "      \"$dir\": {\"status\": \"exists\"},"
                rift_log_success "Validated: $dir"
            else
                echo "      \"$dir\": {\"status\": \"missing\"},"
                rift_log_high "Missing: $dir"
                ((validation_errors++))
            fi
        done
        sed -i '$ s/,$//' "$validation_report" 2>/dev/null || true
        echo "    },"
        
        # Validate RIFT governance files
        echo "    \"governance_validation\": {"
        if [[ -f "$RIFT_CORE_DIR/config/governance.json" ]]; then
            echo "      \"main_governance\": {\"status\": \"exists\"},"
            rift_log_success "RIFT governance schema validated"
        else
            echo "      \"main_governance\": {\"status\": \"missing\"},"
            rift_log_high "Missing RIFT governance schema"
            ((validation_errors++))
        fi
        
        # Validate stage configurations
        for ((stage=0; stage<RIFT_STAGE_COUNT; stage++)); do
            local config_file="$RIFT_CORE_DIR/config/gov.riftrc.$stage"
            if [[ -f "$config_file" ]]; then
                echo "      \"stage_${stage}_config\": {\"status\": \"exists\"},"
                rift_log_success "Stage $stage configuration validated"
            else
                echo "      \"stage_${stage}_config\": {\"status\": \"missing\"},"
                rift_log_moderate "Stage $stage configuration missing (acceptable)"
            fi
        done
        sed -i '$ s/,$//' "$validation_report" 2>/dev/null || true
        echo "    },"
        
        # Summary
        echo "    \"summary\": {"
        echo "      \"validation_errors\": $validation_errors,"
        echo "      \"rift_core_status\": \"$([ $validation_errors -eq 0 ] && echo "VALIDATED" || echo "REQUIRES_ATTENTION")\","
        echo "      \"governance_compliant\": $([ $validation_errors -eq 0 ] && echo true || echo false)"
        echo "    }"
        echo "  }"
        echo "}"
    } > "$validation_report"
    
    if [[ $validation_errors -eq 0 ]]; then
        rift_log_success "RIFT-Core validation PASSED"
    else
        rift_log_high "RIFT-Core validation completed with $validation_errors issues"
    fi
    
    rift_log_info "Validation report: $validation_report"
    return $validation_errors
}

# =============================================================================
# Command Line Interface
# =============================================================================

show_usage() {
    cat << 'EOF'
Usage: setup-rift-core.sh [OPTIONS]

RIFT-Core Pure Infrastructure Setup Script
OBINexus RIFT-N Toolchain - Aegis Project Implementation

OPTIONS:
    --dry-run           Preview operations without executing
    --verbose           Enable verbose output
    --force-rebuild     Force rebuild of existing structures
    --help              Show this help message

RIFT-CORE FEATURES:
    ✅ Pure RIFT implementation (no Polycall dependencies)
    🎨 Native color governance with ANSI terminal support
    🧵 Thread safety with NULL/nil semantics and Yoda-style enforcement
    📊 Audit streams (.audit-0, .audit-1, .audit-2) with hash verification
    🔧 CMake integration with RIFT-Core library generation
    🔍 Comprehensive validation and governance compliance

EXAMPLES:
    ./setup-rift-core.sh --dry-run --verbose
    ./setup-rift-core.sh --force-rebuild
    ./setup-rift-core.sh

EOF
}

parse_arguments() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            --dry-run)
                DRY_RUN=true
                shift
                ;;
            --verbose)
                VERBOSE=true
                shift
                ;;
            --force-rebuild)
                FORCE_REBUILD=true
                shift
                ;;
            --help)
                show_usage
                exit 0
                ;;
            *)
                rift_log_high "Unknown option: $1"
                show_usage
                exit 1
                ;;
        esac
    done
}

# =============================================================================
# Main Execution Function
# =============================================================================

main() {
    parse_arguments "$@"
    
    # Display RIFT-Core banner
    rift_log_success "🚀 RIFT-Core Pure Infrastructure Setup"
    rift_log_info "📋 OBINexus RIFT-N Toolchain - Aegis Project"
    rift_log_info "👨‍💻 Technical Lead: $AEGIS_PROJECT_LEAD"
    rift_log_info "🔧 Version: $RIFT_VERSION"
    rift_log_info "🏠 Project Root: $PROJECT_ROOT"
    
    if [[ "$DRY_RUN" == true ]]; then
        rift_log_moderate "⚠️  DRY-RUN MODE: No files will be created or modified"
    fi
    
    if [[ "$FORCE_REBUILD" == true ]]; then
        rift_log_moderate "🔄 FORCE-REBUILD: Existing structures will be overwritten"
    fi
    
    echo ""
    
    # Execute RIFT-Core setup phases
    rift_log_info "🚀 Beginning RIFT-Core infrastructure setup..."
    
    create_rift_core_structure
    generate_rift_core_headers
    generate_rift_core_sources
    generate_rift_governance_schema
    generate_rift_cmake
    index_rift_project_files
    
    if validate_rift_setup; then
        rift_log_success "✅ RIFT-Core validation PASSED"
    else
        local validation_errors=$?
        rift_log_high "⚠️  RIFT-Core validation completed with $validation_errors issues"
    fi
    
    # Final summary
    echo ""
    rift_log_success "🎉 RIFT-Core Pure Infrastructure Setup Completed!"
    rift_log_info "🏗️  Build system ready for RIFT-N toolchain development"
    rift_log_info "🎨 Native color governance system integrated"
    rift_log_info "📊 Audit streams configured (.audit-0, .audit-1, .audit-2)"
    echo ""
    rift_log_info "📋 Next steps for development:"
    rift_log_info "   1. cd $PROJECT_ROOT && mkdir build && cd build"
    rift_log_info "   2. cmake .."
    rift_log_info "   3. make"
    rift_log_info "   4. make test"
    echo ""
    rift_log_info "📁 RIFT-Core artifacts:"
    rift_log_info "   • Core library: $RIFT_CORE_DIR"
    rift_log_info "   • Audit streams: $RIFT_AUDIT_DIR"
    rift_log_info "   • Telemetry: $RIFT_TELEMETRY_DIR"
    rift_log_info "   • Governance: $RIFT_CORE_DIR/config/"
}

# Execute main function
main "$@"
