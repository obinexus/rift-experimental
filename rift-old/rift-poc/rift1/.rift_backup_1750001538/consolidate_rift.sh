#!/bin/bash

# RIFT Architecture Consolidation Script
# OBINexus Computing - AEGIS Framework
# Consolidates include/rift0 → include/rift1 duplication and organizes src structure

set -euo pipefail

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="${PROJECT_ROOT:-$(pwd)}"
STAGE="${RIFT_STAGE:-1}"
DRY_RUN="${DRY_RUN:-false}"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

log_info() {
    echo -e "${BLUE}ℹ️  $1${NC}"
}

log_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

log_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

log_error() {
    echo -e "${RED}❌ $1${NC}"
}

show_help() {
    cat << EOF
🔧 RIFT Architecture Consolidation Script
========================================

Consolidates duplicated include structures and organizes source hierarchy
according to OBINexus AEGIS methodology.

Usage: $0 [OPTIONS]

Options:
    -s, --stage STAGE       Target RIFT stage (default: 1)
    -d, --dry-run          Show what would be done without executing
    -h, --help             Show this help message
    -v, --verbose          Enable verbose output
    --project-root PATH    Set project root directory

Environment Variables:
    RIFT_STAGE             Target stage (default: 1)
    PROJECT_ROOT           Project root path (default: current directory)
    DRY_RUN               Enable dry-run mode (default: false)

Examples:
    $0                     # Consolidate current directory for stage 1
    $0 --stage 0 --dry-run # Preview consolidation for stage 0
    $0 --project-root ../rift1 --verbose

🏗️  AEGIS Architecture: Structure IS the syntax!
📚 Part of OBINexus governance-first compilation toolchain
EOF
}

# Validation functions
validate_rift_project() {
    local project_dir="$1"
    
    if [[ ! -d "$project_dir" ]]; then
        log_error "Project directory does not exist: $project_dir"
        return 1
    fi
    
    if [[ ! -f "$project_dir/Makefile" ]] && [[ ! -f "$project_dir/makefile" ]]; then
        log_warning "No Makefile found in project directory"
    fi
    
    # Check for RIFT-specific structure
    if [[ ! -d "$project_dir/include" ]] && [[ ! -d "$project_dir/src" ]]; then
        log_error "Invalid RIFT project structure: missing include/ or src/ directories"
        return 1
    fi
    
    log_success "RIFT project structure validated: $project_dir"
    return 0
}

# Core consolidation functions
consolidate_include_headers() {
    local project_dir="$1"
    local target_stage="$2"
    
    log_info "Consolidating include headers for stage $target_stage"
    
    local include_dir="$project_dir/include"
    local target_include="$include_dir/rift$target_stage"
    
    # Create target include directory if it doesn't exist
    if [[ "$DRY_RUN" == "false" ]]; then
        mkdir -p "$target_include"
    else
        log_info "[DRY-RUN] Would create: $target_include"
    fi
    
    # Consolidate headers from different stages
    for stage_dir in "$include_dir"/rift*; do
        if [[ -d "$stage_dir" ]] && [[ "$stage_dir" != "$target_include" ]]; then
            local stage_name=$(basename "$stage_dir")
            log_info "Processing headers from $stage_name → rift$target_stage"
            
            # Copy and consolidate headers
            for header in "$stage_dir"/*.h; do
                if [[ -f "$header" ]]; then
                    local header_name=$(basename "$header")
                    local target_header="$target_include/$header_name"
                    
                    if [[ "$DRY_RUN" == "false" ]]; then
                        # Merge headers intelligently, avoiding duplicates
                        if [[ -f "$target_header" ]]; then
                            log_warning "Header conflict: $header_name (merging)"
                            # Create backup and merge
                            cp "$target_header" "$target_header.bak"
                            merge_header_files "$header" "$target_header"
                        else
                            cp "$header" "$target_header"
                            log_success "Copied: $header_name"
                        fi
                    else
                        log_info "[DRY-RUN] Would consolidate: $header_name"
                    fi
                fi
            done
            
            # Mark old directory for removal
            if [[ "$DRY_RUN" == "false" ]]; then
                log_warning "Marking for cleanup: $stage_dir"
                mv "$stage_dir" "$stage_dir.deprecated.$(date +%s)"
            else
                log_info "[DRY-RUN] Would mark for cleanup: $stage_dir"
            fi
        fi
    done
}

merge_header_files() {
    local source_header="$1"
    local target_header="$2"
    
    # Simple merge strategy: append non-duplicate includes and definitions
    {
        echo "/* Merged from $(basename "$source_header") */"
        cat "$source_header" | grep -v '^#ifndef\|^#define.*_H\|^#endif.*_H'
        echo "/* End merge from $(basename "$source_header") */"
    } >> "$target_header"
}

organize_source_structure() {
    local project_dir="$1"
    local target_stage="$2"
    
    log_info "Organizing source structure for stage $target_stage"
    
    local src_dir="$project_dir/src"
    local core_dir="$src_dir/core"
    local cli_dir="$src_dir/cli"
    
    # Ensure proper directory structure
    if [[ "$DRY_RUN" == "false" ]]; then
        mkdir -p "$core_dir" "$cli_dir"
    else
        log_info "[DRY-RUN] Would create directories: $core_dir, $cli_dir"
    fi
    
    # Move main files to appropriate locations
    organize_main_files "$project_dir" "$target_stage"
    
    # Organize core functionality
    organize_core_files "$project_dir" "$target_stage"
    
    # Organize CLI files
    organize_cli_files "$project_dir" "$target_stage"
}

organize_main_files() {
    local project_dir="$1"
    local target_stage="$2"
    
    log_info "Organizing main entry points"
    
    local src_dir="$project_dir/src"
    local cli_dir="$src_dir/cli"
    
    # Find scattered main files
    find "$src_dir" -name "main.c" -o -name "main_*.c" | while read -r main_file; do
        local relative_path="${main_file#$src_dir/}"
        
        # Skip if already in CLI directory
        if [[ "$relative_path" == cli/* ]]; then
            continue
        fi
        
        local new_name="main_$(basename "$(dirname "$main_file")").c"
        local target_file="$cli_dir/$new_name"
        
        if [[ "$DRY_RUN" == "false" ]]; then
            # Move and rename main files to CLI directory
            mv "$main_file" "$target_file"
            log_success "Moved: $relative_path → cli/$new_name"
            
            # Create symbolic link for backward compatibility
            local link_path="$(dirname "$main_file")/main.c"
            if [[ ! -e "$link_path" ]]; then
                ln -s "../../cli/$new_name" "$link_path"
                log_info "Created compatibility link: $link_path"
            fi
        else
            log_info "[DRY-RUN] Would move: $relative_path → cli/$new_name"
        fi
    done
}

organize_core_files() {
    local project_dir="$1"
    local target_stage="$2"
    
    log_info "Organizing core functionality files"
    
    local src_dir="$project_dir/src"
    local core_dir="$src_dir/core"
    
    # Move core implementation files
    find "$src_dir" -name "*.c" -not -path "*/cli/*" -not -name "main*.c" | while read -r core_file; do
        local relative_path="${core_file#$src_dir/}"
        
        # Skip if already in core directory
        if [[ "$relative_path" == core/* ]]; then
            continue
        fi
        
        local target_file="$core_dir/$(basename "$core_file")"
        
        if [[ "$DRY_RUN" == "false" ]]; then
            mv "$core_file" "$target_file"
            log_success "Moved core file: $relative_path → core/$(basename "$core_file")"
        else
            log_info "[DRY-RUN] Would move core: $relative_path → core/$(basename "$core_file")"
        fi
    done
}

organize_cli_files() {
    local project_dir="$1"
    local target_stage="$2"
    
    log_info "Organizing CLI-specific files"
    
    local src_dir="$project_dir/src"
    local cli_dir="$src_dir/cli"
    
    # Ensure we have a primary CLI main file
    if [[ ! -f "$cli_dir/main.c" ]]; then
        local template_main="$cli_dir/main.c"
        
        if [[ "$DRY_RUN" == "false" ]]; then
            generate_unified_cli_main "$template_main" "$target_stage"
            log_success "Generated unified CLI main: $template_main"
        else
            log_info "[DRY-RUN] Would generate unified CLI main: $template_main"
        fi
    fi
}

generate_unified_cli_main() {
    local output_file="$1"
    local stage="$2"
    
    cat > "$output_file" << 'EOF'
// RIFT Unified CLI Entry Point
// OBINexus Computing - AEGIS Framework
// Consolidated from multiple main implementations

#include "../../include/riftSTAGE/rift.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <libgen.h>
#include <errno.h>

static void print_usage(const char* program_name) {
    printf("🔤 RIFT Stage STAGE - Unified CLI\n");
    printf("===================================\n");
    printf("Usage: %s [options] <input_file>\n\n", program_name);
    printf("Options:\n");
    printf("  -h, --help              Show this help message\n");
    printf("  -v, --version           Show version information\n");
    printf("  -V, --verbose           Enable verbose output\n");
    printf("  -o, --output <file>     Specify output file\n");
    printf("  -c, --config <file>     Use custom configuration file\n");
    printf("  -d, --output-dir <dir>  Set output directory\n");
    printf("  --debug                 Enable debug mode\n");
    printf("  --stage <name>          Set stage name\n");
    printf("\n");
    printf("🏗️  RIFT Architecture: Structure IS the syntax!\n");
    printf("📚 Part of OBINexus governance-first compilation toolchain\n");
}

static void print_version(void) {
    printf("🔤 RIFT Stage STAGE Unified CLI v1.0.0\n");
    printf("AEGIS Automaton Engine for Generative Interpretation & Syntax\n");
    printf("OBINexus Computing - Governance-First Compilation Architecture\n");
    printf("Built: %s %s\n", __DATE__, __TIME__);
    printf("\n");
    printf("🏗️  Architecture: Memory-first token triplets (memory→type→value)\n");
    printf("🔗 Toolchain: riftlang.exe → .so.a → rift.exe → gosilang\n");
    printf("📋 Stage: STAGE (Unified processing)\n");
}

int main(int argc, char* argv[]) {
    // Configuration variables
    const char* input_file = NULL;
    const char* output_file = NULL;
    const char* config_file = ".riftrc";
    const char* output_dir = NULL;
    const char* stage_name = NULL;
    bool verbose = false;
    bool debug = false;
    bool show_help = false;
    bool show_version = false;

    // Long option definitions
    static struct option long_options[] = {
        {"help",       no_argument,       0, 'h'},
        {"version",    no_argument,       0, 'v'},
        {"verbose",    no_argument,       0, 'V'},
        {"output",     required_argument, 0, 'o'},
        {"config",     required_argument, 0, 'c'},
        {"output-dir", required_argument, 0, 'd'},
        {"debug",      no_argument,       0, 1000},
        {"stage",      required_argument, 0, 1001},
        {0, 0, 0, 0}
    };

    // Parse command line arguments
    int option_index = 0;
    int c;
    
    while ((c = getopt_long(argc, argv, "hvVo:c:d:", long_options, &option_index)) != -1) {
        switch (c) {
            case 'h':
                show_help = true;
                break;
            case 'v':
                show_version = true;
                break;
            case 'V':
                verbose = true;
                break;
            case 'o':
                output_file = optarg;
                break;
            case 'c':
                config_file = optarg;
                break;
            case 'd':
                output_dir = optarg;
                break;
            case 1000: // --debug
                debug = true;
                break;
            case 1001: // --stage
                stage_name = optarg;
                break;
            case '?':
                fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
                return 1;
            default:
                fprintf(stderr, "❌ Unknown option: %c\n", c);
                return 1;
        }
    }

    // Handle help and version flags
    if (show_help) {
        print_usage(argv[0]);
        return 0;
    }

    if (show_version) {
        print_version();
        return 0;
    }

    // Validate input file
    if (optind >= argc) {
        fprintf(stderr, "❌ Error: No input file specified\n");
        fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
        return 1;
    }

    input_file = argv[optind];

    // Initialize RIFT processing
    RiftConfig* config = rift_config_create();
    if (!config) {
        fprintf(stderr, "❌ Failed to create configuration\n");
        return 1;
    }

    // Load configuration and set options
    if (access(config_file, F_OK) == 0) {
        RiftResult result = rift_config_load(config, config_file);
        if (result != RIFT_SUCCESS && verbose) {
            printf("⚠️  Warning: Could not load config file %s\n", config_file);
        }
    }

    // Override with command line options
    if (verbose) config->verbose = true;
    if (debug) config->debug_mode = true;
    if (output_dir) {
        free(config->output_dir);
        config->output_dir = strdup(output_dir);
    }

    // Generate output filename if not specified
    char generated_output[1024];
    if (!output_file) {
        snprintf(generated_output, sizeof(generated_output), "%s.STAGE", 
                 input_file);
        output_file = generated_output;
    }

    // Process the file
    if (verbose) {
        printf("🚀 Processing: %s → %s\n", input_file, output_file);
    }

    RiftResult result = rift_process_file(input_file, output_file, config);

    if (result == RIFT_SUCCESS) {
        printf("✅ Stage STAGE processing complete\n");
        if (verbose) {
            printf("📄 Output saved: %s\n", output_file);
        }
    } else {
        printf("❌ Processing failed: %s\n", rift_result_string(result));
    }

    rift_config_destroy(config);
    return (result == RIFT_SUCCESS) ? 0 : 1;
}
EOF

    # Replace STAGE placeholders with actual stage number
    sed -i "s/STAGE/$stage/g" "$output_file"
}

update_makefile() {
    local project_dir="$1"
    local target_stage="$2"
    
    log_info "Updating Makefile for consolidated structure"
    
    local makefile="$project_dir/Makefile"
    
    if [[ ! -f "$makefile" ]]; then
        log_warning "No Makefile found, skipping update"
        return 0
    fi
    
    if [[ "$DRY_RUN" == "false" ]]; then
        # Backup original Makefile
        cp "$makefile" "$makefile.bak.$(date +%s)"
        
        # Update include paths to use consolidated structure
        sed -i "s|include/rift[0-9]|include/rift$target_stage|g" "$makefile"
        
        log_success "Updated Makefile include paths"
    else
        log_info "[DRY-RUN] Would update Makefile include paths"
    fi
}

create_cleanup_report() {
    local project_dir="$1"
    local target_stage="$2"
    
    local report_file="$project_dir/consolidation_report.md"
    
    if [[ "$DRY_RUN" == "false" ]]; then
        cat > "$report_file" << EOF
# RIFT Architecture Consolidation Report

**Date:** $(date)
**Target Stage:** $target_stage
**Project Directory:** $project_dir

## Changes Made

### Include Directory Consolidation
- Consolidated all stage headers into \`include/rift$target_stage/\`
- Deprecated old stage directories (marked with timestamp)

### Source Structure Organization
- Moved core implementation files to \`src/core/\`
- Moved CLI files to \`src/cli/\`
- Created unified CLI main entry point

### Build System Updates
- Updated Makefile include paths
- Maintained backward compatibility where possible

## Cleanup Actions

### Files to Review
- Check \`*.bak\` files in include directory
- Review \`*.deprecated.*\` directories
- Verify unified CLI functionality

### Testing Required
- Build verification: \`make clean && make all\`
- CLI functionality: \`./bin/rift$target_stage.exe --help\`
- Core functionality tests

## Next Steps

1. Test the consolidated build system
2. Update documentation references
3. Clean up deprecated directories after verification
4. Update CI/CD pipeline if applicable

---
Generated by RIFT Architecture Consolidation Script
OBINexus Computing - AEGIS Framework
EOF
        log_success "Created consolidation report: $report_file"
    else
        log_info "[DRY-RUN] Would create consolidation report: $report_file"
    fi
}

# Main execution function
main() {
    local stage="$STAGE"
    local verbose=false
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -s|--stage)
                stage="$2"
                shift 2
                ;;
            -d|--dry-run)
                DRY_RUN=true
                shift
                ;;
            -h|--help)
                show_help
                exit 0
                ;;
            -v|--verbose)
                verbose=true
                shift
                ;;
            --project-root)
                PROJECT_ROOT="$2"
                shift 2
                ;;
            *)
                log_error "Unknown option: $1"
                show_help
                exit 1
                ;;
        esac
    done
    
    # Display operation summary
    echo "🔧 RIFT Architecture Consolidation"
    echo "=================================="
    echo "Project Root: $PROJECT_ROOT"
    echo "Target Stage: $stage"
    echo "Dry Run: $DRY_RUN"
    echo ""
    
    # Validate project structure
    if ! validate_rift_project "$PROJECT_ROOT"; then
        exit 1
    fi
    
    # Execute consolidation steps
    if [[ "$DRY_RUN" == "true" ]]; then
        log_warning "DRY RUN MODE - No files will be modified"
        echo ""
    fi
    
    consolidate_include_headers "$PROJECT_ROOT" "$stage"
    organize_source_structure "$PROJECT_ROOT" "$stage"
    update_makefile "$PROJECT_ROOT" "$stage"
    create_cleanup_report "$PROJECT_ROOT" "$stage"
    
    echo ""
    log_success "RIFT architecture consolidation completed for stage $stage"
    
    if [[ "$DRY_RUN" == "false" ]]; then
        echo ""
        echo "Next steps:"
        echo "1. Review consolidation_report.md"
        echo "2. Test build: make clean && make all"
        echo "3. Verify CLI: ./bin/rift$stage.exe --help"
        echo "4. Clean up deprecated directories after verification"
    fi
}

# Execute main function
main "$@"
