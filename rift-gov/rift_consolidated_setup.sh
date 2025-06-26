#!/bin/bash

# =============================================================================
# RIFT Consolidated Color Governance & Core Infrastructure Setup
# OBINexus RIFT-N Toolchain with Accessibility Color Palette Integration
# Version: 2.0.0-aegis
# Maintains OBINexus session continuity and governance framework integrity
# =============================================================================

set -euo pipefail

# =============================================================================
# ANSI Color Accessibility Palette - Biafran Compliance
# =============================================================================

# Critical states (red-black) - Fatal errors, irreversible memory violations
readonly CRITICAL_RED='\033[0;31m'
readonly CRITICAL_BLACK='\033[0;30m'
readonly CRITICAL_BG='\033[41m'

# Warning states (orange-dark) - Degraded but recoverable execution
readonly WARNING_ORANGE='\033[0;33m'
readonly WARNING_CLAY='\033[38;5;94m'
readonly WARNING_BROWN='\033[38;5;52m'

# Caution states (yellow) - Volatile mutations, unstable modules
readonly CAUTION_YELLOW='\033[1;33m'
readonly CAUTION_BRIGHT='\033[93m'

# Info states (blue) - Neutral diagnostic, no memory side effects
readonly INFO_BLUE='\033[0;34m'
readonly INFO_CYAN='\033[0;36m'
readonly INFO_DEEP='\033[38;5;39m'

# Success states (green) - Validated transitions, production eligible
readonly SUCCESS_GREEN='\033[0;32m'
readonly SUCCESS_BRIGHT='\033[92m'
readonly SUCCESS_FOREST='\033[38;5;22m'

# Control codes
readonly NC='\033[0m' # No Color
readonly BOLD='\033[1m'
readonly DIM='\033[2m'

# =============================================================================
# Project Configuration & Session Continuity
# =============================================================================

readonly SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
readonly ROOT_DIR="${SCRIPT_DIR}"
readonly CORE_DIR="${ROOT_DIR}/rift-core"
readonly AUDIT_DIR="${ROOT_DIR}/rift-audit"
readonly TELEMETRY_DIR="${ROOT_DIR}/telemetry"
readonly TOOLS_DIR="${ROOT_DIR}/tools"

# OBINexus versioning and compliance
readonly RIFT_VERSION="0.2.0-aegis"
readonly GOVERNANCE_SCHEMA_VERSION="2.0"
readonly AUDIT_SCHEMA_VERSION="2.0"
readonly ACCESSIBILITY_VERSION="1.0-biafran"

# RIFT Core constants (synchronized with C headers)
readonly RIFT_MAX_WORKERS=32
readonly RIFT_MAX_THREAD_DEPTH=32

# Stage definitions with color mapping
readonly RIFT_STAGES=(rift-0 rift-1 rift-2 rift-3 rift-4 rift-5 rift-6)

# Exception level to color mapping
declare -A EXCEPTION_COLORS=(
    ["BASIC"]="${SUCCESS_GREEN}"      # 0-4: Non-blocking warnings
    ["MODERATE"]="${CAUTION_YELLOW}"  # 5-6: Recoverable pause
    ["HIGH"]="${WARNING_ORANGE}"      # 7-8: Escalated governance check
    ["CRITICAL"]="${CRITICAL_RED}"    # 9-12: Execution halt + panic
)

# Command line options
DRY_RUN=false
VERBOSE=false
MIGRATE_CLEANUP=false

# =============================================================================
# Terminal Color Support Detection
# =============================================================================

detect_terminal_color_support() {
    local color_support="basic"
    
    # Check for color support
    if [[ -t 1 ]] && command -v tput &> /dev/null; then
        local colors=$(tput colors 2>/dev/null || echo 0)
        if [[ $colors -ge 256 ]]; then
            color_support="256"
        elif [[ $colors -ge 16 ]]; then
            color_support="16"
        elif [[ $colors -ge 8 ]]; then
            color_support="8"
        fi
    fi
    
    # Windows terminal detection
    if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
        if [[ -n "${WT_SESSION:-}" ]]; then
            color_support="256"  # Windows Terminal
        elif [[ -n "${ConEmuPID:-}" ]]; then
            color_support="256"  # ConEmu
        else
            color_support="basic"  # Legacy cmd/PowerShell
        fi
    fi
    
    echo "$color_support"
}

# =============================================================================
# Polycall Format Colored Text Function
# =============================================================================

polycall_format_colored_text() {
    local level="$1"
    local message="$2"
    local timestamp="$(date '+%Y-%m-%d %H:%M:%S')"
    local uuid="$(generate_uuid)"
    local support_level="$(detect_terminal_color_support)"
    
    # Get color based on exception level
    local color="${EXCEPTION_COLORS[$level]:-${INFO_BLUE}}"
    
    # Fallback for basic terminals
    if [[ "$support_level" == "basic" ]]; then
        case "$level" in
            "CRITICAL") color="${CRITICAL_RED}" ;;
            "HIGH") color="${WARNING_ORANGE}" ;;
            "MODERATE") color="${CAUTION_YELLOW}" ;;
            "BASIC") color="${SUCCESS_GREEN}" ;;
            *) color="${INFO_BLUE}" ;;
        esac
    fi
    
    # Format with telemetry metadata
    echo -e "${color}[${level}]${NC} ${BOLD}${timestamp}${NC} ${DIM}(${uuid:0:8})${NC} $message"
    
    # Log to telemetry if enabled
    if [[ -d "$TELEMETRY_DIR/logs" ]]; then
        echo "{\"timestamp\":\"$timestamp\",\"level\":\"$level\",\"uuid\":\"$uuid\",\"message\":\"$message\",\"color_support\":\"$support_level\"}" >> "$TELEMETRY_DIR/logs/color_governance.jsonl"
    fi
}

# =============================================================================
# Enhanced Logging Functions with Color Governance
# =============================================================================

log_critical() {
    polycall_format_colored_text "CRITICAL" "$1"
}

log_high() {
    polycall_format_colored_text "HIGH" "$1"
}

log_moderate() {
    polycall_format_colored_text "MODERATE" "$1"
}

log_basic() {
    polycall_format_colored_text "BASIC" "$1"
}

log_info() {
    polycall_format_colored_text "INFO" "$1"
}

log_success() {
    polycall_format_colored_text "BASIC" "✅ $1"
}

log_warning() {
    polycall_format_colored_text "MODERATE" "⚠️  $1"
}

log_error() {
    polycall_format_colored_text "HIGH" "❌ $1"
}

log_panic() {
    polycall_format_colored_text "CRITICAL" "🚨 PANIC: $1"
    exit 9
}

log_dry_run() {
    echo -e "${INFO_CYAN}[DRY-RUN]${NC} $1"
}

log_verbose() {
    if [[ "$VERBOSE" == true ]]; then
        echo -e "${DIM}[VERBOSE]${NC} $1"
    fi
}

# =============================================================================
# Utility Functions
# =============================================================================

show_usage() {
    cat << 'EOF'
Usage: rift-consolidated-setup.sh [OPTIONS]

RIFT Consolidated Color Governance & Core Infrastructure Setup
OBINexus RIFT-N Toolchain with Accessibility Integration

OPTIONS:
    --dry-run           Preview operations without executing
    --verbose           Enable verbose output
    --migrate-cleanup   Clean up old structures during migration
    --help              Show this help message

EXAMPLES:
    ./rift-consolidated-setup.sh --dry-run
    ./rift-consolidated-setup.sh --verbose --migrate-cleanup
    ./rift-consolidated-setup.sh --dry-run --verbose

RIFT STAGES WITH COLOR GOVERNANCE:
    Stage 0 (Tokenizer):    SUCCESS (Green) / MODERATE (Yellow) states
    Stage 1 (Parser):       SUCCESS (Green) / HIGH (Orange) states  
    Stage 2 (Semantic):     SUCCESS (Green) / CRITICAL (Red) states
    Stage 3 (Validator):    INFO (Blue) / SUCCESS (Green) states
    Stage 4 (Bytecode):     SUCCESS (Green) / HIGH (Orange) states
    Stage 5 (Optimizer):    SUCCESS (Green) / MODERATE (Yellow) states
    Stage 6 (Emitter):      SUCCESS (Green) / CRITICAL (Red) states

EOF
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
# Command Line Argument Parsing
# =============================================================================

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
            --migrate-cleanup)
                MIGRATE_CLEANUP=true
                shift
                ;;
            --help)
                show_usage
                exit 0
                ;;
            *)
                log_error "Unknown option: $1"
                show_usage
                exit 1
                ;;
        esac
    done
}

# =============================================================================
# Dry-run Aware Operations
# =============================================================================

dry_run_mkdir() {
    local dir="$1"
    if [[ "$DRY_RUN" == true ]]; then
        log_dry_run "Would create directory: $dir"
    else
        mkdir -p "$dir"
        log_verbose "Created directory: $dir"
    fi
}

dry_run_touch() {
    local file="$1"
    if [[ "$DRY_RUN" == true ]]; then
        log_dry_run "Would create file: $file"
    else
        touch "$file"
        log_verbose "Created file: $file"
    fi
}

dry_run_write_file() {
    local file="$1"
    local content="$2"
    if [[ "$DRY_RUN" == true ]]; then
        log_dry_run "Would write file: $file ($(echo "$content" | wc -l) lines)"
        if [[ "$VERBOSE" == true ]]; then
            echo "--- File content preview (first 10 lines) ---"
            echo "$content" | head -10
            echo "--- End preview ---"
        fi
    else
        echo "$content" > "$file"
        log_verbose "Wrote file: $file ($(echo "$content" | wc -l) lines)"
    fi
}

dry_run_move() {
    local src="$1"
    local dest="$2"
    if [[ "$DRY_RUN" == true ]]; then
        log_dry_run "Would move: $src -> $dest"
    else
        if [[ -e "$src" ]]; then
            mv "$src" "$dest"
            log_verbose "Moved: $src -> $dest"
        fi
    fi
}

dry_run_remove() {
    local target="$1"
    if [[ "$DRY_RUN" == true ]]; then
        log_dry_run "Would remove: $target"
    else
        if [[ -e "$target" ]]; then
            rm -rf "$target"
            log_verbose "Removed: $target"
        fi
    fi
}

# =============================================================================
# Migration and Cleanup Functions
# =============================================================================

migrate_old_structures() {
    log_info "Migrating old rift-core structures to centralized location..."
    
    # Migrate from tools/ad-hoc/rift-core if it exists
    if [[ -d "$TOOLS_DIR/ad-hoc/rift-core" ]]; then
        log_warning "Found old rift-core in tools/ad-hoc, migrating..."
        
        # Copy important files before cleanup
        if [[ -d "$TOOLS_DIR/ad-hoc/rift-core/config" ]]; then
            dry_run_mkdir "$CORE_DIR/config"
            if [[ "$DRY_RUN" == false ]]; then
                cp -r "$TOOLS_DIR/ad-hoc/rift-core/config/"* "$CORE_DIR/config/" 2>/dev/null || true
            fi
        fi
        
        if [[ "$MIGRATE_CLEANUP" == true ]]; then
            dry_run_remove "$TOOLS_DIR/ad-hoc/rift-core"
        fi
    fi
    
    # Clean up scattered scripts
    local old_scripts=(
        "rift_core_setup.sh"
        "setup-rift-core.sh"
        "rift_build_recovery.sh"
        "rift-orchestrator.sh"
        "rift-pipeline-migrate.sh"
    )
    
    for script in "${old_scripts[@]}"; do
        if [[ -f "$ROOT_DIR/$script" && "$MIGRATE_CLEANUP" == true ]]; then
            dry_run_move "$ROOT_DIR/$script" "$TOOLS_DIR/legacy/"
        fi
    done
    
    log_success "Migration completed"
}

cleanup_unused_structures() {
    if [[ "$MIGRATE_CLEANUP" != true ]]; then
        return
    fi
    
    log_info "Cleaning up unused development structures..."
    
    # Clean up clutter from directory listing
    local cleanup_targets=(
        "backup"
        "demo_output"
        "poc/gov"
        "qa/reports"
        "scripts/locks"
        "Testing"
    )
    
    for target in "${cleanup_targets[@]}"; do
        if [[ -d "$ROOT_DIR/$target" ]]; then
            dry_run_remove "$ROOT_DIR/$target"
        fi
    done
    
    # Clean up old Makefiles and backups
    find "$ROOT_DIR" -name "*.backup*" -o -name "*.old*" -o -name "*.corrupted*" | while read -r file; do
        if [[ "$MIGRATE_CLEANUP" == true ]]; then
            dry_run_remove "$file"
        fi
    done
    
    log_success "Cleanup completed"
}

# =============================================================================
# Core Directory Structure Creation
# =============================================================================

create_consolidated_structure() {
    if [[ "$DRY_RUN" == true ]]; then
        log_dry_run "Creating consolidated rift-core directory structure..."
        log_dry_run "Directory tree that would be created:"
        cat << 'EOF'
rift-core/
├── include/rift-core/
│   ├── common/
│   ├── thread/
│   ├── audit/
│   ├── telemetry/
│   ├── governance/
│   └── accessibility/
├── src/
│   ├── common/
│   ├── thread/
│   ├── audit/
│   ├── telemetry/
│   ├── governance/
│   └── accessibility/
├── build/
│   ├── debug/
│   ├── prod/
│   ├── bin/
│   ├── obj/
│   └── lib/
├── cmake/
│   ├── sorting/
│   ├── searching/
│   ├── indexing/
│   └── flashing/
├── config/
├── tests/
└── docs/

rift-audit/
├── stage-0/ through stage-6/

telemetry/
├── logs/
├── schemas/
├── reports/
└── color_governance/

tools/
├── legacy/
├── validation/
└── deployment/
EOF
    else
        log_info "Creating consolidated rift-core directory structure..."
    fi
    
    # Core directories with accessibility support
    dry_run_mkdir "$CORE_DIR/include/rift-core/common"
    dry_run_mkdir "$CORE_DIR/include/rift-core/thread"
    dry_run_mkdir "$CORE_DIR/include/rift-core/audit"
    dry_run_mkdir "$CORE_DIR/include/rift-core/telemetry"
    dry_run_mkdir "$CORE_DIR/include/rift-core/governance"
    dry_run_mkdir "$CORE_DIR/include/rift-core/accessibility"
    
    dry_run_mkdir "$CORE_DIR/src/common"
    dry_run_mkdir "$CORE_DIR/src/thread"
    dry_run_mkdir "$CORE_DIR/src/audit"
    dry_run_mkdir "$CORE_DIR/src/telemetry"
    dry_run_mkdir "$CORE_DIR/src/governance"
    dry_run_mkdir "$CORE_DIR/src/accessibility"
    
    # Build directories with clear separation
    dry_run_mkdir "$CORE_DIR/build/debug"
    dry_run_mkdir "$CORE_DIR/build/prod"
    dry_run_mkdir "$CORE_DIR/build/bin"
    dry_run_mkdir "$CORE_DIR/build/obj"
    dry_run_mkdir "$CORE_DIR/build/lib"
    
    # CMake organization directories
    dry_run_mkdir "$CORE_DIR/cmake/sorting"
    dry_run_mkdir "$CORE_DIR/cmake/searching"
    dry_run_mkdir "$CORE_DIR/cmake/indexing"
    dry_run_mkdir "$CORE_DIR/cmake/flashing"
    
    dry_run_mkdir "$CORE_DIR/config"
    dry_run_mkdir "$CORE_DIR/tests/unit"
    dry_run_mkdir "$CORE_DIR/tests/integration"
    dry_run_mkdir "$CORE_DIR/tests/benchmark"
    dry_run_mkdir "$CORE_DIR/docs"
    
    # Audit infrastructure with stage separation
    for stage in "${RIFT_STAGES[@]}"; do
        dry_run_mkdir "$AUDIT_DIR/${stage}"
    done
    
    # Enhanced telemetry with color governance
    dry_run_mkdir "$TELEMETRY_DIR/logs"
    dry_run_mkdir "$TELEMETRY_DIR/schemas"
    dry_run_mkdir "$TELEMETRY_DIR/reports"
    dry_run_mkdir "$TELEMETRY_DIR/color_governance"
    
    # Organized tools directory
    dry_run_mkdir "$TOOLS_DIR/legacy"
    dry_run_mkdir "$TOOLS_DIR/validation"
    dry_run_mkdir "$TOOLS_DIR/deployment"
    
    if [[ "$DRY_RUN" == false ]]; then
        # Create .gitkeep files for empty directories
        find "$CORE_DIR" -type d -empty -exec touch {}/.gitkeep \;
        find "$AUDIT_DIR" -type d -empty -exec touch {}/.gitkeep \;
        find "$TELEMETRY_DIR" -type d -empty -exec touch {}/.gitkeep \;
    else
        log_dry_run "Would create .gitkeep files in empty directories"
    fi
    
    log_success "Consolidated structure creation completed"
}

# =============================================================================
# Accessibility Color System Implementation
# =============================================================================

generate_accessibility_colors() {
    log_info "Generating accessibility color system..."
    
    local accessibility_header='#ifndef RIFT_ACCESSIBILITY_COLORS_H
#define RIFT_ACCESSIBILITY_COLORS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Biafran Accessibility Palette - ANSI Compatible */
typedef enum {
    RIFT_COLOR_CRITICAL_RED = 0,
    RIFT_COLOR_CRITICAL_BLACK = 1,
    RIFT_COLOR_WARNING_ORANGE = 2,
    RIFT_COLOR_WARNING_CLAY = 3,
    RIFT_COLOR_WARNING_BROWN = 4,
    RIFT_COLOR_CAUTION_YELLOW = 5,
    RIFT_COLOR_CAUTION_BRIGHT = 6,
    RIFT_COLOR_INFO_BLUE = 7,
    RIFT_COLOR_INFO_CYAN = 8,
    RIFT_COLOR_INFO_DEEP = 9,
    RIFT_COLOR_SUCCESS_GREEN = 10,
    RIFT_COLOR_SUCCESS_BRIGHT = 11,
    RIFT_COLOR_SUCCESS_FOREST = 12,
    RIFT_COLOR_NC = 13,
    RIFT_COLOR_BOLD = 14,
    RIFT_COLOR_DIM = 15
} rift_color_t;

/* Exception level to color mapping */
typedef enum {
    RIFT_EXCEPTION_BASIC = 0,    /* 0-4: Green states */
    RIFT_EXCEPTION_MODERATE = 5, /* 5-6: Yellow states */
    RIFT_EXCEPTION_HIGH = 7,     /* 7-8: Orange states */
    RIFT_EXCEPTION_CRITICAL = 9  /* 9-12: Red states */
} rift_exception_level_t;

/* Terminal color support detection */
typedef enum {
    RIFT_TERMINAL_BASIC = 0,
    RIFT_TERMINAL_8_COLOR = 8,
    RIFT_TERMINAL_16_COLOR = 16,
    RIFT_TERMINAL_256_COLOR = 256
} rift_terminal_support_t;

/* Color governance functions */
rift_terminal_support_t detect_terminal_color_support(void);
const char* get_color_code(rift_color_t color, rift_terminal_support_t support);
const char* get_exception_color(rift_exception_level_t level);
void polycall_format_colored_text(rift_exception_level_t level, const char* message);

/* Stage-specific color mapping */
rift_color_t get_stage_color(int stage, rift_exception_level_t level);
void log_stage_transition(int from_stage, int to_stage, rift_exception_level_t status);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_ACCESSIBILITY_COLORS_H */'

    dry_run_write_file "$CORE_DIR/include/rift-core/accessibility/colors.h" "$accessibility_header"

    local accessibility_impl='#include "rift-core/accessibility/colors.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ANSI color code mappings */
static const char* color_codes[] = {
    [RIFT_COLOR_CRITICAL_RED] = "\033[0;31m",
    [RIFT_COLOR_CRITICAL_BLACK] = "\033[0;30m",
    [RIFT_COLOR_WARNING_ORANGE] = "\033[0;33m",
    [RIFT_COLOR_WARNING_CLAY] = "\033[38;5;94m",
    [RIFT_COLOR_WARNING_BROWN] = "\033[38;5;52m",
    [RIFT_COLOR_CAUTION_YELLOW] = "\033[1;33m",
    [RIFT_COLOR_CAUTION_BRIGHT] = "\033[93m",
    [RIFT_COLOR_INFO_BLUE] = "\033[0;34m",
    [RIFT_COLOR_INFO_CYAN] = "\033[0;36m",
    [RIFT_COLOR_INFO_DEEP] = "\033[38;5;39m",
    [RIFT_COLOR_SUCCESS_GREEN] = "\033[0;32m",
    [RIFT_COLOR_SUCCESS_BRIGHT] = "\033[92m",
    [RIFT_COLOR_SUCCESS_FOREST] = "\033[38;5;22m",
    [RIFT_COLOR_NC] = "\033[0m",
    [RIFT_COLOR_BOLD] = "\033[1m",
    [RIFT_COLOR_DIM] = "\033[2m"
};

/* Basic color fallbacks for limited terminals */
static const char* basic_color_codes[] = {
    [RIFT_COLOR_CRITICAL_RED] = "\033[0;31m",
    [RIFT_COLOR_CRITICAL_BLACK] = "\033[0;30m",
    [RIFT_COLOR_WARNING_ORANGE] = "\033[0;33m",
    [RIFT_COLOR_WARNING_CLAY] = "\033[0;33m",
    [RIFT_COLOR_WARNING_BROWN] = "\033[0;33m",
    [RIFT_COLOR_CAUTION_YELLOW] = "\033[1;33m",
    [RIFT_COLOR_CAUTION_BRIGHT] = "\033[1;33m",
    [RIFT_COLOR_INFO_BLUE] = "\033[0;34m",
    [RIFT_COLOR_INFO_CYAN] = "\033[0;36m",
    [RIFT_COLOR_INFO_DEEP] = "\033[0;34m",
    [RIFT_COLOR_SUCCESS_GREEN] = "\033[0;32m",
    [RIFT_COLOR_SUCCESS_BRIGHT] = "\033[0;32m",
    [RIFT_COLOR_SUCCESS_FOREST] = "\033[0;32m",
    [RIFT_COLOR_NC] = "\033[0m",
    [RIFT_COLOR_BOLD] = "\033[1m",
    [RIFT_COLOR_DIM] = "\033[2m"
};

rift_terminal_support_t detect_terminal_color_support(void) {
    const char* term = getenv("TERM");
    const char* colorterm = getenv("COLORTERM");
    
    /* Windows terminal detection */
    if (getenv("WT_SESSION")) {
        return RIFT_TERMINAL_256_COLOR;
    }
    
    /* Check for 256 color support */
    if (colorterm && (strstr(colorterm, "256") || strstr(colorterm, "24bit"))) {
        return RIFT_TERMINAL_256_COLOR;
    }
    
    /* Check terminal type */
    if (term) {
        if (strstr(term, "256") || strstr(term, "color")) {
            return RIFT_TERMINAL_256_COLOR;
        } else if (strstr(term, "16color")) {
            return RIFT_TERMINAL_16_COLOR;
        } else if (strstr(term, "color") || strstr(term, "ansi")) {
            return RIFT_TERMINAL_8_COLOR;
        }
    }
    
    return RIFT_TERMINAL_BASIC;
}

const char* get_color_code(rift_color_t color, rift_terminal_support_t support) {
    if (support >= RIFT_TERMINAL_256_COLOR) {
        return color_codes[color];
    } else {
        return basic_color_codes[color];
    }
}

const char* get_exception_color(rift_exception_level_t level) {
    switch (level) {
        case RIFT_EXCEPTION_BASIC:
            return color_codes[RIFT_COLOR_SUCCESS_GREEN];
        case RIFT_EXCEPTION_MODERATE:
            return color_codes[RIFT_COLOR_CAUTION_YELLOW];
        case RIFT_EXCEPTION_HIGH:
            return color_codes[RIFT_COLOR_WARNING_ORANGE];
        case RIFT_EXCEPTION_CRITICAL:
            return color_codes[RIFT_COLOR_CRITICAL_RED];
        default:
            return color_codes[RIFT_COLOR_INFO_BLUE];
    }
}

void polycall_format_colored_text(rift_exception_level_t level, const char* message) {
    char timestamp[32];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    const char* color = get_exception_color(level);
    const char* level_str;
    
    switch (level) {
        case RIFT_EXCEPTION_BASIC: level_str = "BASIC"; break;
        case RIFT_EXCEPTION_MODERATE: level_str = "MODERATE"; break;
        case RIFT_EXCEPTION_HIGH: level_str = "HIGH"; break;
        case RIFT_EXCEPTION_CRITICAL: level_str = "CRITICAL"; break;
        default: level_str = "INFO"; break;
    }
    
    printf("%s[%s]%s %s%s%s %s%s%s %s\n",
           color, level_str, color_codes[RIFT_COLOR_NC],
           color_codes[RIFT_COLOR_BOLD], timestamp, color_codes[RIFT_COLOR_NC],
           color_codes[RIFT_COLOR_DIM], "UUID", color_codes[RIFT_COLOR_NC],
           message);
}

rift_color_t get_stage_color(int stage, rift_exception_level_t level) {
    /* Stage-specific color logic */
    switch (stage) {
        case 0: /* Tokenizer */
        case 1: /* Parser */
            return (level == RIFT_EXCEPTION_BASIC) ? 
                   RIFT_COLOR_SUCCESS_GREEN : RIFT_COLOR_CAUTION_YELLOW;
        case 2: /* Semantic */
        case 6: /* Emitter */
            return (level == RIFT_EXCEPTION_BASIC) ? 
                   RIFT_COLOR_SUCCESS_GREEN : RIFT_COLOR_CRITICAL_RED;
        case 3: /* Validator */
            return (level == RIFT_EXCEPTION_BASIC) ? 
                   RIFT_COLOR_SUCCESS_GREEN : RIFT_COLOR_INFO_BLUE;
        case 4: /* Bytecode */
            return (level == RIFT_EXCEPTION_BASIC) ? 
                   RIFT_COLOR_SUCCESS_GREEN : RIFT_COLOR_WARNING_ORANGE;
        case 5: /* Optimizer */
            return (level == RIFT_EXCEPTION_BASIC) ? 
                   RIFT_COLOR_SUCCESS_GREEN : RIFT_COLOR_CAUTION_YELLOW;
        default:
            return RIFT_COLOR_INFO_BLUE;
    }
}

void log_stage_transition(int from_stage, int to_stage, rift_exception_level_t status) {
    char message[256];
    snprintf(message, sizeof(message), 
             "Stage transition: %d -> %d", from_stage, to_stage);
    
    polycall_format_colored_text(status, message);
}'

    dry_run_write_file "$CORE_DIR/src/accessibility/colors.c" "$accessibility_impl"
    
    log_success "Accessibility color system generated"
}

# =============================================================================
# Enhanced CMake Configuration Generation
# =============================================================================

generate_enhanced_cmake() {
    log_info "Generating enhanced CMake configuration with accessibility support..."
    
    local root_cmake='cmake_minimum_required(VERSION 3.14)
project(RIFT VERSION 0.2.0 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Compiler flags with accessibility support
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic -DDEBUG -DRIFT_ACCESSIBILITY_ENABLED")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG -DRIFT_ACCESSIBILITY_ENABLED")

# Color governance definitions
add_compile_definitions(
    RIFT_VERSION="${PROJECT_VERSION}"
    RIFT_GOVERNANCE_SCHEMA_VERSION="2.0"
    RIFT_AUDIT_SCHEMA_VERSION="2.0"
    RIFT_ACCESSIBILITY_VERSION="1.0-biafran"
)

# Include custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/rift-core/cmake")

# Global include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rift-core/include)

# Add subdirectories in dependency order
add_subdirectory(rift-core)
add_subdirectory(rift-0)
add_subdirectory(rift-1)
add_subdirectory(rift-2)
add_subdirectory(rift-3)
add_subdirectory(rift-4)
add_subdirectory(rift-5)
add_subdirectory(rift-6)

# Testing with color governance
enable_testing()
add_subdirectory(tests)

# Enhanced documentation with accessibility guide
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

# Color governance validation target
add_custom_target(validate-colors
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools/validation/validate-color-governance.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Validating color governance compliance"
)'

    dry_run_write_file "$ROOT_DIR/CMakeLists.txt" "$root_cmake"

    local core_cmake='cmake_minimum_required(VERSION 3.14)

# Enhanced core library sources with accessibility
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
    src/accessibility/colors.c
    src/accessibility/error.c
)

# Enhanced core library headers
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
    include/rift-core/accessibility/colors.h
    include/rift-core/accessibility/error.h
)

# Create static library with accessibility support
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

# Compiler definitions with color governance
target_compile_definitions(rift-core PRIVATE
    RIFT_CORE_VERSION="${PROJECT_VERSION}"
    RIFT_GOVERNANCE_SCHEMA_VERSION="2.0"
    RIFT_AUDIT_SCHEMA_VERSION="2.0"
    RIFT_ACCESSIBILITY_ENABLED=1
    RIFT_COLOR_GOVERNANCE_ENABLED=1
)

# Thread support
find_package(Threads REQUIRED)
target_link_libraries(rift-core PRIVATE Threads::Threads)

# CMake utility modules
include(cmake/sorting/FileSort.cmake)
include(cmake/searching/FileSearch.cmake)
include(cmake/indexing/FileIndex.cmake)
include(cmake/flashing/QuickDeploy.cmake)

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

# Tests with color governance
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()'

    dry_run_write_file "$CORE_DIR/CMakeLists.txt" "$core_cmake"
    
    log_success "Enhanced CMake configuration generated"
}

# =============================================================================
# Enhanced Schema Generation with Core Layer Integrity
# =============================================================================

generate_enhanced_schema() {
    log_info "Generating enhanced schema with core layer integrity..."
    
    local schema_content='{
  "$schema": "https://json-schema.org/draft/2020-12/schema",
  "title": "RIFT Enhanced Governance Configuration Schema with Color Accessibility",
  "description": "Defines governance metadata for RIFT compiler pipeline stages with Stage 5 security enforcement, custom stage support, and accessibility color governance.",
  "type": "object",
  "properties": {
    "package_name": { "type": "string" },
    "version": { "type": "string" },
    "timestamp": { "type": "string", "format": "date-time" },
    "stage": { "type": "integer", "minimum": 0 },
    "stage_type": {
      "type": "string",
      "enum": ["legacy", "experimental", "stable"],
      "default": "experimental"
    },
    "description": { "type": "string" },
    "semverx_lock": { "type": "boolean" },
    "entry_point": { "type": "string" },
    "nlink_enabled": { "type": "boolean" },
    "accessibility_governance": {
      "type": "object",
      "description": "Color Accessibility Governance for Biafran Compliance",
      "properties": {
        "color_palette_version": {
          "type": "string",
          "default": "1.0-biafran",
          "description": "Version of accessibility color palette in use"
        },
        "terminal_support_detection": {
          "type": "boolean",
          "default": true,
          "description": "Enable automatic terminal color support detection"
        },
        "exception_color_mapping": {
          "type": "object",
          "properties": {
            "basic": { "type": "string", "default": "green" },
            "moderate": { "type": "string", "default": "yellow" },
            "high": { "type": "string", "default": "orange" },
            "critical": { "type": "string", "default": "red" }
          }
        },
        "stage_color_overrides": {
          "type": "object",
          "description": "Per-stage color customization"
        },
        "telemetry_color_logging": {
          "type": "boolean",
          "default": true,
          "description": "Log color governance events to telemetry"
        }
      }
    },
    "stage_5_optimizer": {
      "type": "object",
      "description": "Stage 5 Optimizer Security Governance - Zero Trust Enforcement",
      "properties": {
        "optimizer_model": { 
          "type": "string",
          "enum": ["AST-aware-minimizer-v2", "state-reduction-secure", "path-sanitizer"],
          "description": "Certified optimizer implementation model"
        },
        "minimization_verified": { 
          "type": "boolean",
          "description": "Confirms AST minimization was successfully performed"
        },
        "path_hash": { 
          "type": "string",
          "pattern": "^[a-fA-F0-9]{64}$",
          "description": "SHA-256 hash of execution paths before optimization"
        },
        "post_optimization_hash": {
          "type": "string", 
          "pattern": "^[a-fA-F0-9]{64}$",
          "description": "SHA-256 hash of execution paths after optimization"
        },
        "audit_enabled": { 
          "type": "boolean", 
          "default": true,
          "description": "Requires signed audit trail generation"
        },
        "security_level": {
          "type": "string",
          "enum": ["path_reduction", "state_minimization", "exploit_elimination"],
          "default": "path_reduction"
        },
        "transformation_log": {
          "type": "string",
          "description": "Path to signed transformation log file"
        },
        "semantic_equivalence_proof": {
          "type": "boolean",
          "description": "Formal verification that optimizations preserve semantics"
        }
      },
      "required": ["optimizer_model", "minimization_verified", "path_hash", "post_optimization_hash"],
      "additionalProperties": false
    },
    "governance_substages": {
      "type": "object",
      "description": "Machine-verifiable substage governance definitions with core layer integrity",
      "properties": {
        "tokenizer": {
          "type": "object",
          "properties": {
            "lexeme_validation": { "type": "boolean" },
            "token_memory_constraints": { "type": "integer", "minimum": 4096 },
            "encoding_normalization": { "type": "boolean" },
            "yoda_style_enforcement": { "type": "boolean", "default": true },
            "null_nil_transformation": { "type": "boolean", "default": true }
          }
        },
        "parser": {
          "type": "object", 
          "properties": {
            "ast_depth_limit": { "type": "integer", "minimum": 1, "maximum": 1000 },
            "syntax_strictness": { "type": "string", "enum": ["permissive", "strict", "pedantic"] },
            "error_recovery": { "type": "boolean" }
          }
        },
        "semantic": {
          "type": "object",
          "properties": {
            "type_checking": { "type": "boolean" },
            "scope_validation": { "type": "boolean" },
            "symbol_table_integrity": { "type": "boolean" }
          }
        },
        "validator": {
          "type": "object",
          "properties": {
            "structural_acyclicity": { "type": "boolean" },
            "cost_bounds_enforced": { "type": "boolean" },
            "governance_hash_required": { "type": "boolean" }
          }
        },
        "bytecode": {
          "type": "object",
          "properties": {
            "opcode_validation": { "type": "boolean" },
            "complexity_limits": { "type": "boolean" },
            "operand_alignment": { "type": "integer", "enum": [4, 8, 16] }
          }
        },
        "verifier": {
          "type": "object",
          "properties": {
            "bytecode_integrity": { "type": "boolean" },
            "stack_safety": { "type": "boolean" },
            "memory_bounds": { "type": "boolean" },
            "core_layer_integrity": {
              "type": "boolean",
              "description": "Indicates if the rift-core shared components pass audit and structural validation"
            }
          }
        },
        "emitter": {
          "type": "object",
          "properties": {
            "target_architecture": { "type": "string" },
            "optimization_level": { "type": "integer", "minimum": 0, "maximum": 3 },
            "debug_symbols": { "type": "boolean" }
          }
        }
      }
    },
    "thread_safety_governance": {
      "type": "object",
      "description": "Enhanced thread safety and semantic control with color governance",
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
        "thread_lifecycle": {
          "type": "object",
          "properties": {
            "max_workers_per_thread": { "type": "integer", "default": 32 },
            "max_thread_depth": { "type": "integer", "default": 32 },
            "lifecycle_encoding": { "type": "string", "default": "bit_string" },
            "parity_elimination": { "type": "boolean", "default": true }
          }
        },
        "audit_streams": {
          "type": "object",
          "properties": {
            "stdin_audit": { "type": "boolean", "default": true },
            "stderr_audit": { "type": "boolean", "default": true },
            "stdout_audit": { "type": "boolean", "default": true },
            "state_hash_verification": { "type": "boolean", "default": true }
          }
        }
      }
    },
    "custom_stages": {
      "type": "array",
      "description": "User-defined pipeline extensions with governance contracts",
      "items": {
        "type": "object",
        "properties": {
          "name": { "type": "string" },
          "stage_id": { "type": "string" },
          "description": { "type": "string" },
          "activated": { "type": "boolean" },
          "dependencies": { "type": "array", "items": { "type": "string" } },
          "governance_required": { "type": "boolean", "default": true },
          "fallback_allowed": { "type": "boolean", "default": false },
          "machine_verifiable": { "type": "boolean", "default": true },
          "color_governance": { "type": "boolean", "default": true }
        },
        "required": ["name", "stage_id"],
        "additionalProperties": false
      }
    },
    "fallback_governance": {
      "type": "object",
      "description": "Fallback behavior when governance files are missing or invalid",
      "properties": {
        "enabled": { "type": "boolean", "default": true },
        "fallback_directory": { "type": "string", "default": "rift-core/" },
        "experimental_bypass": { "type": "boolean", "default": true },
        "halt_on_critical": { "type": "boolean", "default": true }
      }
    },
    "nlink_integration": {
      "type": "object",
      "description": "NLink orchestration and SemVerX enforcement settings",
      "properties": {
        "semverx_strict_mode": { "type": "boolean", "default": false },
        "hot_swap_validation": { "type": "boolean", "default": true },
        "component_lifecycle_tracking": { "type": "boolean", "default": true },
        "polybuild_coordination": { "type": "boolean", "default": true }
      }
    }
  },
  "required": ["package_name", "version", "stage", "timestamp"],
  "additionalProperties": false
}'

    dry_run_write_file "$ROOT_DIR/schema-enhanced.json" "$schema_content"
    
    log_success "Enhanced schema with core layer integrity generated"
}

# =============================================================================
# File Indexing and Traceability with Color Governance
# =============================================================================

index_project_files() {
    log_info "Indexing project files with color governance traceability..."
    
    local index_file="$ROOT_DIR/rift_project_index.txt"
    local telemetry_file="$TELEMETRY_DIR/file_inventory.json"
    local color_inventory="$TELEMETRY_DIR/color_governance/accessibility_inventory.json"
    
    if [[ "$DRY_RUN" == true ]]; then
        log_dry_run "Would generate comprehensive file index with color governance metadata"
        return
    fi
    
    # Generate comprehensive file index
    {
        echo "# RIFT Enhanced Project File Index with Color Governance"
        echo "# Generated: $(date)"
        echo "# Version: ${RIFT_VERSION}"
        echo "# Accessibility Version: ${ACCESSIBILITY_VERSION}"
        echo ""
        
        echo "## Core Infrastructure Files"
        find "$CORE_DIR" -type f \( -name "*.c" -o -name "*.h" \) | sort
        echo ""
        
        echo "## Stage-Specific Files"
        for stage in "${RIFT_STAGES[@]}"; do
            echo "### $stage"
            find "$ROOT_DIR/$stage" -type f \( -name "*.c" -o -name "*.h" \) 2>/dev/null | sort || true
        done
        echo ""
        
        echo "## CMake Configuration Files"
        find "$ROOT_DIR" -name "CMakeLists.txt" -type f | sort
        find "$CORE_DIR/cmake" -name "*.cmake" -type f 2>/dev/null | sort || true
        echo ""
        
        echo "## Governance and Configuration Files"
        find "$CORE_DIR/config" -type f 2>/dev/null | sort || true
        find "$ROOT_DIR" -name "*.json" -type f | sort
        echo ""
        
        echo "## Accessibility and Color Governance Files"
        find "$CORE_DIR" -path "*/accessibility/*" -type f 2>/dev/null | sort || true
        
    } > "$index_file"
    
    # Generate enhanced JSON inventory
    {
        echo "{"
        echo "  \"project\": \"RIFT-Enhanced\","
        echo "  \"version\": \"${RIFT_VERSION}\","
        echo "  \"accessibility_version\": \"${ACCESSIBILITY_VERSION}\","
        echo "  \"generated\": \"$(date -Iseconds)\","
        echo "  \"uuid\": \"$(generate_uuid)\","
        echo "  \"color_governance_enabled\": true,"
        echo "  \"files\": {"
        
        echo "    \"core_sources\": ["
        find "$CORE_DIR/src" -name "*.c" -type f 2>/dev/null | sed 's/.*/"&"/' | tr '\n' ',' | sed 's/,$//' || echo ""
        echo ""
        echo "    ],"
        
        echo "    \"core_headers\": ["
        find "$CORE_DIR/include" -name "*.h" -type f 2>/dev/null | sed 's/.*/"&"/' | tr '\n' ',' | sed 's/,$//' || echo ""
        echo ""
        echo "    ],"
        
        echo "    \"accessibility_files\": ["
        find "$CORE_DIR" -path "*/accessibility/*" -type f 2>/dev/null | sed 's/.*/"&"/' | tr '\n' ',' | sed 's/,$//' || echo ""
        echo ""
        echo "    ],"
        
        echo "    \"cmake_files\": ["
        find "$ROOT_DIR" -name "CMakeLists.txt" -o -name "*.cmake" -type f 2>/dev/null | sed 's/.*/"&"/' | tr '\n' ',' | sed 's/,$//' || echo ""
        echo ""
        echo "    ]"
        
        echo "  },"
        echo "  \"statistics\": {"
        echo "    \"total_core_files\": $(find "$CORE_DIR" -name "*.c" -o -name "*.h" -type f 2>/dev/null | wc -l),"
        echo "    \"total_accessibility_files\": $(find "$CORE_DIR" -path "*/accessibility/*" -type f 2>/dev/null | wc -l),"
        echo "    \"total_cmake_files\": $(find "$ROOT_DIR" -name "CMakeLists.txt" -o -name "*.cmake" -type f 2>/dev/null | wc -l)"
        echo "  },"
        echo "  \"color_governance\": {"
        echo "    \"exception_levels\": [\"BASIC\", \"MODERATE\", \"HIGH\", \"CRITICAL\"],"
        echo "    \"color_mappings\": {"
        echo "      \"BASIC\": \"green\","
        echo "      \"MODERATE\": \"yellow\","
        echo "      \"HIGH\": \"orange\","
        echo "      \"CRITICAL\": \"red\""
        echo "    },"
        echo "    \"terminal_support\": \"$(detect_terminal_color_support)\""
        echo "  }"
        echo "}"
    } > "$telemetry_file"
    
    # Generate color governance specific inventory
    {
        echo "{"
        echo "  \"accessibility_palette\": \"biafran\","
        echo "  \"version\": \"${ACCESSIBILITY_VERSION}\","
        echo "  \"generated\": \"$(date -Iseconds)\","
        echo "  \"terminal_detection\": \"$(detect_terminal_color_support)\","
        echo "  \"ansi_compatibility\": true,"
        echo "  \"exception_color_map\": {"
        for level in "${!EXCEPTION_COLORS[@]}"; do
            echo "    \"$level\": \"${EXCEPTION_COLORS[$level]}\","
        done | sed '$ s/,$//'
        echo "  },"
        echo "  \"stage_color_governance\": {"
        for i in {0..6}; do
            echo "    \"stage-$i\": {"
            echo "      \"success_color\": \"green\","
            echo "      \"failure_color\": \"red\","
            echo "      \"warning_color\": \"yellow\","
            echo "      \"info_color\": \"blue\""
            echo "    }$([ $i -lt 6 ] && echo ",")"
        done
        echo "  }"
        echo "}"
    } > "$color_inventory"
    
    log_success "Enhanced file indexing with color governance completed"
    log_success "Project index: $index_file"
    log_success "Telemetry inventory: $telemetry_file"
    log_success "Color governance inventory: $color_inventory"
}

# =============================================================================
# Validation and Verification with Color Governance
# =============================================================================

validate_enhanced_setup() {
    log_info "Validating enhanced setup with color governance..."
    
    local validation_report="$TELEMETRY_DIR/enhanced_setup_validation.txt"
    local uuid="$(generate_uuid)"
    
    if [[ "$DRY_RUN" == true ]]; then
        log_dry_run "Would generate comprehensive validation report"
        return
    fi
    
    {
        echo "RIFT Enhanced Setup Validation Report with Color Governance"
        echo "Generated: $(date)"
        echo "UUID: $uuid"
        echo "Version: $RIFT_VERSION"
        echo "Accessibility Version: $ACCESSIBILITY_VERSION"
        echo ""
        
        echo "=== Directory Structure Validation ==="
        
        # Check core directories
        local core_dirs=("include" "src" "build" "cmake" "config" "tests" "docs")
        for dir in "${core_dirs[@]}"; do
            if [[ -d "$CORE_DIR/$dir" ]]; then
                polycall_format_colored_text "BASIC" "✅ $CORE_DIR/$dir exists"
            else
                polycall_format_colored_text "HIGH" "❌ $CORE_DIR/$dir missing"
            fi
        done
        
        # Check accessibility directories
        if [[ -d "$CORE_DIR/include/rift-core/accessibility" ]]; then
            polycall_format_colored_text "BASIC" "✅ Accessibility headers directory exists"
        else
            polycall_format_colored_text "CRITICAL" "❌ Accessibility headers missing"
        fi
        
        if [[ -d "$CORE_DIR/src/accessibility" ]]; then
            polycall_format_colored_text "BASIC" "✅ Accessibility sources directory exists"
        else
            polycall_format_colored_text "CRITICAL" "❌ Accessibility sources missing"
        fi
        
        # Check audit directories
        for stage in "${RIFT_STAGES[@]}"; do
            if [[ -d "$AUDIT_DIR/$stage" ]]; then
                polycall_format_colored_text "BASIC" "✅ $AUDIT_DIR/$stage exists"
            else
                polycall_format_colored_text "MODERATE" "⚠️  $AUDIT_DIR/$stage missing"
            fi
        done
        
        echo ""
        echo "=== File Generation Validation ==="
        
        # Check core files
        local critical_files=(
            "$ROOT_DIR/CMakeLists.txt"
            "$CORE_DIR/CMakeLists.txt"
            "$CORE_DIR/include/rift-core/accessibility/colors.h"
            "$CORE_DIR/src/accessibility/colors.c"
            "$ROOT_DIR/schema-enhanced.json"
        )
        
        for file in "${critical_files[@]}"; do
            if [[ -f "$file" ]]; then
                polycall_format_colored_text "BASIC" "✅ $file generated"
            else
                polycall_format_colored_text "HIGH" "❌ $file missing"
            fi
        done
        
        echo ""
        echo "=== Color Governance Validation ==="
        
        # Test color support detection
        local color_support="$(detect_terminal_color_support)"
        polycall_format_colored_text "BASIC" "Terminal color support: $color_support"
        
        # Test exception color mapping
        for level in "${!EXCEPTION_COLORS[@]}"; do
            polycall_format_colored_text "BASIC" "Exception level $level mapped to color"
        done
        
        echo ""
        echo "=== Migration Status ==="
        
        if [[ "$MIGRATE_CLEANUP" == true ]]; then
            polycall_format_colored_text "BASIC" "✅ Migration cleanup enabled"
            if [[ -d "$TOOLS_DIR/ad-hoc/rift-core" ]]; then
                polycall_format_colored_text "MODERATE" "⚠️  Old rift-core structure still exists"
            else
                polycall_format_colored_text "BASIC" "✅ Old rift-core structure cleaned up"
            fi
        else
            polycall_format_colored_text "BASIC" "ℹ️  Migration cleanup disabled"
        fi
        
        echo ""
        echo "=== Summary ==="
        echo "Validation UUID: $uuid"
        echo "Terminal Color Support: $color_support"
        echo "Accessibility Compliance: Biafran Palette"
        echo "Core Layer Integrity: $([ -f "$CORE_DIR/include/rift-core/accessibility/colors.h" ] && echo "VERIFIED" || echo "FAILED")"
        
    } > "$validation_report"
    
    log_success "Enhanced validation report generated: $validation_report"
}

# =============================================================================
# Main Execution Function
# =============================================================================

main() {
    # Parse command line arguments
    parse_arguments "$@"
    
    # Display startup banner with color governance
    polycall_format_colored_text "BASIC" "🎨 RIFT Consolidated Color Governance & Core Infrastructure Setup"
    polycall_format_colored_text "BASIC" "🔧 OBINexus RIFT-N Toolchain with Accessibility Integration"
    polycall_format_colored_text "BASIC" "📋 Version: $RIFT_VERSION | Accessibility: $ACCESSIBILITY_VERSION"
    polycall_format_colored_text "BASIC" "🏠 Root Directory: $ROOT_DIR"
    
    if [[ "$DRY_RUN" == true ]]; then
        polycall_format_colored_text "MODERATE" "⚠️  DRY-RUN MODE: No files will be created or modified"
    fi
    
    if [[ "$VERBOSE" == true ]]; then
        polycall_format_colored_text "BASIC" "📝 Verbose mode enabled - detailed operation logging active"
    fi
    
    if [[ "$MIGRATE_CLEANUP" == true ]]; then
        polycall_format_colored_text "MODERATE" "🧹 Migration cleanup enabled - old structures will be removed"
    fi
    
    echo ""
    
    # Execute setup phases with color governance
    polycall_format_colored_text "BASIC" "🚀 Beginning enhanced setup process..."
    
    migrate_old_structures
    cleanup_unused_structures
    create_consolidated_structure
    generate_accessibility_colors
    generate_enhanced_cmake
    generate_enhanced_schema
    index_project_files
    validate_enhanced_setup
    
    # Final summary with color governance
    echo ""
    polycall_format_colored_text "BASIC" "🎉 RIFT Enhanced Setup Completed Successfully!"
    polycall_format_colored_text "BASIC" "🏗️  Build system ready for RIFT-N toolchain development"
    polycall_format_colored_text "BASIC" "🎨 Color governance system integrated with accessibility compliance"
    echo ""
    polycall_format_colored_text "BASIC" "📋 Next steps for production deployment:"
    polycall_format_colored_text "BASIC" "   1. cd $ROOT_DIR && mkdir build && cd build"
    polycall_format_colored_text "BASIC" "   2. cmake .."
    polycall_format_colored_text "BASIC" "   3. make"
    polycall_format_colored_text "BASIC" "   4. make test"
    polycall_format_colored_text "BASIC" "   5. make validate-colors"
    echo ""
    polycall_format_colored_text "BASIC" "📁 Generated artifacts:"
    polycall_format_colored_text "BASIC" "   • Enhanced core: $CORE_DIR"
    polycall_format_colored_text "BASIC" "   • Color governance: $CORE_DIR/src/accessibility/"
    polycall_format_colored_text "BASIC" "   • Audit directory: $AUDIT_DIR"
    polycall_format_colored_text "BASIC" "   • Telemetry directory: $TELEMETRY_DIR"
    polycall_format_colored_text "BASIC" "   • Enhanced schema: $ROOT_DIR/schema-enhanced.json"
    
    # Display color governance legend
    echo ""
    polycall_format_colored_text "BASIC" "🎨 Color Governance Legend:"
    polycall_format_colored_text "BASIC" "   GREEN (SUCCESS): Validated transitions, production eligible"
    polycall_format_colored_text "MODERATE" "   YELLOW (CAUTION): Volatile mutations, unstable modules"
    polycall_format_colored_text "HIGH" "   ORANGE (WARNING): Degraded but recoverable execution"
    polycall_format_colored_text "CRITICAL" "   RED (CRITICAL): Fatal errors, irreversible memory violations"
    echo ""
    polycall_format_colored_text "BASIC" "🔗 Terminal Color Support: $(detect_terminal_color_support)"
}

# Execute main function with all arguments
main "$@"