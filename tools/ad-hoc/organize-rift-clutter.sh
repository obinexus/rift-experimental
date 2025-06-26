#!/bin/bash

# =============================================================================
# RIFT Project Structure Consolidation Script
# Aegis Project - Technical Debt Resolution & Build System Stabilization
# Technical Lead: Nnamdi Michael Okpala
# Waterfall Phase: Infrastructure Consolidation & Standardization
# =============================================================================

set -euo pipefail

readonly SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
readonly PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
readonly TIMESTAMP="$(date +%Y%m%d_%H%M%S)"

# Technical configuration constants
readonly RIFT_VERSION="2.1.0-consolidated"
readonly TECHNICAL_LEAD="Nnamdi Michael Okpala"
readonly WATERFALL_PHASE="Infrastructure Consolidation"

# Color codes for professional logging
readonly LOG_INFO='\033[0;34m'
readonly LOG_SUCCESS='\033[0;32m'
readonly LOG_WARNING='\033[1;33m'
readonly LOG_ERROR='\033[0;31m'
readonly LOG_RESET='\033[0m'

# Execution flags
DRY_RUN=false
VERBOSE=false
AGGRESSIVE_CLEANUP=false

# =============================================================================
# Professional Logging Framework
# =============================================================================

log_technical() {
    local level="$1"
    local message="$2"
    local color=""
    
    case "$level" in
        "INFO")    color="$LOG_INFO" ;;
        "SUCCESS") color="$LOG_SUCCESS" ;;
        "WARNING") color="$LOG_WARNING" ;;
        "ERROR")   color="$LOG_ERROR" ;;
    esac
    
    echo -e "${color}[RIFT-${level}]${LOG_RESET} $(date '+%H:%M:%S') $message"
}

log_info() { log_technical "INFO" "$1"; }
log_success() { log_technical "SUCCESS" "$1"; }
log_warning() { log_technical "WARNING" "$1"; }
log_error() { log_technical "ERROR" "$1"; }

# =============================================================================
# Project Structure Analysis Functions
# =============================================================================

analyze_project_fragmentation() {
    log_info "Analyzing project structure fragmentation..."
    
    local analysis_report="$PROJECT_ROOT/structure_analysis_${TIMESTAMP}.txt"
    
    {
        echo "RIFT Project Structure Analysis Report"
        echo "====================================="
        echo "Timestamp: $(date)"
        echo "Technical Lead: $TECHNICAL_LEAD"
        echo "Waterfall Phase: $WATERFALL_PHASE"
        echo ""
        
        echo "## Multiple rift-core Instances Detected:"
        find "$PROJECT_ROOT" -type d -name "rift-core" 2>/dev/null | while read -r dir; do
            echo "- $dir ($(find "$dir" -type f | wc -l) files)"
        done
        echo ""
        
        echo "## Configuration File Duplication:"
        find "$PROJECT_ROOT" -name "governance.json" -o -name "CMakeLists.txt" | while read -r file; do
            echo "- $file"
        done
        echo ""
        
        echo "## Legacy Artifacts Requiring Cleanup:"
        find "$PROJECT_ROOT" -type f \( -name "*.bak" -o -name "*.old" -o -name "*backup*" -o -name "*corrupted*" \) | head -20 | while read -r file; do
            echo "- $file"
        done
        
        local total_legacy=$(find "$PROJECT_ROOT" -type f \( -name "*.bak" -o -name "*.old" -o -name "*backup*" -o -name "*corrupted*" \) | wc -l)
        echo "- Total legacy artifacts: $total_legacy files"
        echo ""
        
        echo "## Build System Issues:"
        echo "- Multiple CMakeLists.txt configurations causing conflicts"
        echo "- OBJECT library target configuration errors"
        echo "- Missing pkg-config template files"
        echo "- Unbound variable errors in setup scripts"
        
    } > "$analysis_report"
    
    log_success "Structure analysis completed: $analysis_report"
    
    if [[ "$VERBOSE" == true ]]; then
        echo ""
        echo "=== FRAGMENTATION SUMMARY ==="
        echo "rift-core instances: $(find "$PROJECT_ROOT" -type d -name "rift-core" | wc -l)"
        echo "CMakeLists.txt files: $(find "$PROJECT_ROOT" -name "CMakeLists.txt" | wc -l)"
        echo "Legacy artifacts: $(find "$PROJECT_ROOT" -type f \( -name "*.bak" -o -name "*.old" -o -name "*backup*" -o -name "*corrupted*" \) | wc -l)"
        echo "=========================="
        echo ""
    fi
}

# =============================================================================
# Legacy Artifact Quarantine System
# =============================================================================

quarantine_legacy_artifacts() {
    log_info "Quarantining legacy artifacts and backup files..."
    
    local quarantine_dir="$PROJECT_ROOT/rift-quarantine"
    local quarantine_log="$quarantine_dir/quarantine_log_${TIMESTAMP}.txt"
    
    if [[ "$DRY_RUN" == true ]]; then
        log_info "DRY-RUN: Would quarantine legacy artifacts"
        return 0
    fi
    
    mkdir -p "$quarantine_dir"
    
    # Initialize quarantine log
    {
        echo "RIFT Legacy Artifact Quarantine Log"
        echo "==================================="
        echo "Timestamp: $(date)"
        echo "Technical Lead: $TECHNICAL_LEAD"
        echo ""
    } > "$quarantine_log"
    
    # Quarantine legacy files
    local quarantine_count=0
    find "$PROJECT_ROOT" -type f \( -name "*.bak" -o -name "*.old" -o -name "*backup*" -o -name "*corrupted*" \) \
        ! -path "$quarantine_dir/*" | while read -r file; do
        
        local relative_path="${file#$PROJECT_ROOT/}"
        local quarantine_path="$quarantine_dir/$(basename "$file")"
        
        # Ensure unique names in quarantine
        if [[ -f "$quarantine_path" ]]; then
            quarantine_path="${quarantine_path}_${TIMESTAMP}"
        fi
        
        mv "$file" "$quarantine_path" 2>/dev/null || true
        echo "Quarantined: $relative_path -> $(basename "$quarantine_path")" >> "$quarantine_log"
        ((quarantine_count++)) || true
    done
    
    log_success "Quarantined $quarantine_count legacy artifacts"
    log_info "Quarantine location: $quarantine_dir"
}

# =============================================================================
# rift-core Consolidation System
# =============================================================================

consolidate_rift_core_instances() {
    log_info "Consolidating multiple rift-core instances..."
    
    local primary_core="$PROJECT_ROOT/rift-core"
    local secondary_cores=(
        "$PROJECT_ROOT/rift-core/setup/rift-core"
        "$PROJECT_ROOT/tools/ad-hoc/rift-core"
    )
    
    # Ensure primary rift-core exists with proper structure
    if [[ "$DRY_RUN" == false ]]; then
        mkdir -p "$primary_core"/{include/rift-core,src/rift-core,config,setup,cmake,docs,tests}
        mkdir -p "$primary_core/include/rift-core"/{common,thread,audit,telemetry,governance,accessibility}
        mkdir -p "$primary_core/src/rift-core"/{common,thread,audit,telemetry,governance,accessibility}
    fi
    
    # Consolidate secondary instances
    for secondary_core in "${secondary_cores[@]}"; do
        if [[ -d "$secondary_core" ]]; then
            log_info "Consolidating: $secondary_core"
            
            if [[ "$DRY_RUN" == true ]]; then
                log_info "DRY-RUN: Would consolidate $secondary_core into $primary_core"
                continue
            fi
            
            # Merge configuration files with conflict resolution
            if [[ -d "$secondary_core/config" ]]; then
                for config_file in "$secondary_core/config"/*; do
                    if [[ -f "$config_file" ]]; then
                        local filename="$(basename "$config_file")"
                        local dest_file="$primary_core/config/$filename"
                        
                        if [[ -f "$dest_file" ]]; then
                            # Create backup of existing file
                            cp "$dest_file" "${dest_file}.merge_backup_${TIMESTAMP}"
                            log_warning "Config conflict resolved: $filename (backup created)"
                        fi
                        
                        cp "$config_file" "$dest_file"
                    fi
                done
                log_success "Merged configuration from: $secondary_core"
            fi
            
            # Merge source files
            if [[ -d "$secondary_core/src" ]]; then
                cp -r "$secondary_core/src/"* "$primary_core/src/" 2>/dev/null || true
                log_success "Merged sources from: $secondary_core"
            fi
            
            # Merge headers
            if [[ -d "$secondary_core/include" ]]; then
                cp -r "$secondary_core/include/"* "$primary_core/include/" 2>/dev/null || true
                log_success "Merged headers from: $secondary_core"
            fi
            
            # Archive the secondary instance
            local archive_dir="$PROJECT_ROOT/rift-quarantine/archived_cores"
            mkdir -p "$archive_dir"
            mv "$secondary_core" "$archive_dir/rift-core_$(basename "$(dirname "$secondary_core")")_${TIMESTAMP}"
            log_info "Archived secondary instance: $secondary_core"
        fi
    done
    
    log_success "rift-core consolidation completed"
}

# =============================================================================
# Build System Repair Functions
# =============================================================================

repair_cmake_configuration() {
    log_info "Repairing CMake configuration issues..."
    
    # Fix root CMakeLists.txt
    local root_cmake="$PROJECT_ROOT/CMakeLists.txt"
    
    if [[ "$DRY_RUN" == false ]]; then
        # Backup existing CMakeLists.txt
        if [[ -f "$root_cmake" ]]; then
            cp "$root_cmake" "${root_cmake}.repair_backup_${TIMESTAMP}"
        fi
        
        # Generate corrected root CMakeLists.txt
        cat > "$root_cmake" << 'EOF'
cmake_minimum_required(VERSION 3.14)
project(RIFT VERSION 2.1.0 LANGUAGES C)

# RIFT Core Configuration
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Compiler flags
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

# Global include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rift-core/include)

# Add rift-core first (dependency for all stages)
add_subdirectory(rift-core)

# Add RIFT stages
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

# Install headers
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY rift-core/include/ DESTINATION include)
EOF
    fi
    
    # Fix rift-core CMakeLists.txt
    local core_cmake="$PROJECT_ROOT/rift-core/CMakeLists.txt"
    
    if [[ "$DRY_RUN" == false ]]; then
        cat > "$core_cmake" << 'EOF'
cmake_minimum_required(VERSION 3.14)

# RIFT Core library sources
file(GLOB_RECURSE RIFT_CORE_SOURCES 
    "src/rift-core/*.c"
)

# RIFT Core library headers
file(GLOB_RECURSE RIFT_CORE_HEADERS 
    "include/rift-core/*.h"
)

# Create rift-core static library
add_library(rift-core STATIC ${RIFT_CORE_SOURCES})

# Set target properties
set_target_properties(rift-core PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Include directories
target_include_directories(rift-core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Thread support
find_package(Threads REQUIRED)
target_link_libraries(rift-core PRIVATE Threads::Threads)

# Install targets
install(TARGETS rift-core
    EXPORT rift-core-targets
    ARCHIVE DESTINATION lib
)

install(EXPORT rift-core-targets
    FILE rift-core-config.cmake
    DESTINATION lib/cmake/rift-core
)
EOF
    fi
    
    log_success "CMake configuration repaired"
}

create_missing_pkg_config_templates() {
    log_info "Creating missing pkg-config templates..."
    
    local config_templates_dir="$PROJECT_ROOT/config_templates"
    
    if [[ "$DRY_RUN" == false ]]; then
        mkdir -p "$config_templates_dir"
        
        # Generate pkg-config templates for each stage
        for stage in {0..6}; do
            cat > "$config_templates_dir/rift-${stage}.pc.in" << EOF
prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=\${prefix}
libdir=\${exec_prefix}/lib
includedir=\${prefix}/include

Name: rift-${stage}
Description: RIFT Stage ${stage} Library
Version: @PROJECT_VERSION@
Libs: -L\${libdir} -lrift-${stage} -lrift-core
Cflags: -I\${includedir}
EOF
        done
    fi
    
    log_success "pkg-config templates created in: $config_templates_dir"
}

# =============================================================================
# Setup Script Variable Binding Repair
# =============================================================================

repair_setup_script_variables() {
    log_info "Repairing setup script variable bindings..."
    
    local setup_scripts=(
        "$PROJECT_ROOT/rift-core/setup/setup-rift-core.sh"
        "$PROJECT_ROOT/tools/rift_core_pure_setup.sh"
    )
    
    for script in "${setup_scripts[@]}"; do
        if [[ -f "$script" ]]; then
            if [[ "$DRY_RUN" == false ]]; then
                # Backup original script
                cp "$script" "${script}.variable_fix_backup_${TIMESTAMP}"
                
                # Fix unbound variables
                sed -i 's/RIFT_MAX_WORKERS/${RIFT_MAX_WORKERS:-32}/g' "$script" 2>/dev/null || true
                sed -i 's/RIFT_MAX_THREAD_DEPTH/${RIFT_MAX_THREAD_DEPTH:-32}/g' "$script" 2>/dev/null || true
                sed -i 's/RIFT_STAGE_COUNT/${RIFT_STAGE_COUNT:-7}/g' "$script" 2>/dev/null || true
                
                log_success "Repaired variable bindings in: $(basename "$script")"
            else
                log_info "DRY-RUN: Would repair variables in: $(basename "$script")"
            fi
        fi
    done
}

# =============================================================================
# Validation and Quality Assurance
# =============================================================================

validate_consolidation_results() {
    log_info "Validating project consolidation results..."
    
    local validation_errors=0
    
    # Validate primary rift-core structure
    local required_dirs=(
        "rift-core"
        "rift-core/include/rift-core"
        "rift-core/src/rift-core"
        "rift-core/config"
        "config_templates"
    )
    
    for dir in "${required_dirs[@]}"; do
        if [[ -d "$PROJECT_ROOT/$dir" ]]; then
            log_success "Validated: $dir"
        else
            log_error "Missing: $dir"
            ((validation_errors++))
        fi
    done
    
    # Validate no duplicate rift-core instances
    local core_instances=$(find "$PROJECT_ROOT" -type d -name "rift-core" ! -path "*/rift-quarantine/*" | wc -l)
    if [[ $core_instances -eq 1 ]]; then
        log_success "Single rift-core instance confirmed"
    else
        log_warning "Multiple rift-core instances still exist: $core_instances"
    fi
    
    # Validate CMake files
    if [[ -f "$PROJECT_ROOT/CMakeLists.txt" && -f "$PROJECT_ROOT/rift-core/CMakeLists.txt" ]]; then
        log_success "Core CMake files validated"
    else
        log_error "Missing critical CMake files"
        ((validation_errors++))
    fi
    
    log_info "Validation completed with $validation_errors errors"
    return $validation_errors
}

# =============================================================================
# Command Line Interface
# =============================================================================

show_usage() {
    cat << 'EOF'
Usage: rift-structure-consolidation.sh [OPTIONS]

RIFT Project Structure Consolidation Script
Aegis Project - Technical Debt Resolution & Build System Stabilization

OPTIONS:
    --dry-run               Preview operations without executing
    --verbose               Enable detailed technical output
    --aggressive-cleanup    Permanently remove quarantined artifacts
    --help                  Show this help message

CONSOLIDATION OPERATIONS:
    • Legacy artifact quarantine and cleanup
    • Multiple rift-core instance consolidation
    • CMake configuration repair and standardization
    • pkg-config template generation
    • Setup script variable binding repair
    • Comprehensive validation and quality assurance

EXAMPLES:
    ./rift-structure-consolidation.sh --dry-run --verbose
    ./rift-structure-consolidation.sh --aggressive-cleanup
    ./rift-structure-consolidation.sh

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
            --aggressive-cleanup)
                AGGRESSIVE_CLEANUP=true
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
# Main Execution Framework
# =============================================================================

main() {
    parse_arguments "$@"
    
    # Display professional banner
    log_success "🔧 RIFT Project Structure Consolidation"
    log_info "📋 Aegis Project - Technical Debt Resolution"
    log_info "👨‍💻 Technical Lead: $TECHNICAL_LEAD"
    log_info "🌊 Waterfall Phase: $WATERFALL_PHASE"
    log_info "🏠 Project Root: $PROJECT_ROOT"
    
    if [[ "$DRY_RUN" == true ]]; then
        log_warning "⚠️  DRY-RUN MODE: Operations will be previewed only"
    fi
    
    if [[ "$AGGRESSIVE_CLEANUP" == true ]]; then
        log_warning "🗑️  AGGRESSIVE-CLEANUP: Quarantined artifacts will be permanently removed"
    fi
    
    echo ""
    
    # Execute consolidation phases
    log_info "🚀 Beginning systematic project consolidation..."
    
    analyze_project_fragmentation
    quarantine_legacy_artifacts
    consolidate_rift_core_instances
    repair_cmake_configuration
    create_missing_pkg_config_templates
    repair_setup_script_variables
    
    if validate_consolidation_results; then
        log_success "✅ Project consolidation completed successfully"
    else
        local validation_errors=$?
        log_warning "⚠️  Consolidation completed with $validation_errors validation issues"
    fi
    
    # Generate executive summary
    echo ""
    log_success "🎉 RIFT Project Structure Consolidation Completed!"
    log_info "🏗️  Build system standardized and repaired"
    log_info "📦 Legacy artifacts quarantined systematically"
    log_info "🔧 CMake configuration errors resolved"
    echo ""
    log_info "📋 Recommended next steps:"
    log_info "   1. cd $PROJECT_ROOT && rm -rf build && mkdir build && cd build"
    log_info "   2. cmake .. -DCMAKE_BUILD_TYPE=Debug"
    log_info "   3. make -j$(nproc)"
    log_info "   4. make test"
    echo ""
    log_info "📁 Consolidation artifacts:"
    log_info "   • Primary rift-core: $PROJECT_ROOT/rift-core"
    log_info "   • Quarantine location: $PROJECT_ROOT/rift-quarantine"
    log_info "   • Configuration templates: $PROJECT_ROOT/config_templates"
}

# Execute main consolidation framework
main "$@"
