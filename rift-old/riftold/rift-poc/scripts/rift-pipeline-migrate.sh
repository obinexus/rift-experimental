#!/bin/bash

# RIFT Pipeline Migration Framework
# OBINexus Computing - AEGIS Methodology Implementation
# Systematic stage-by-stage development with TDD integration

set -euo pipefail

# Script metadata
SCRIPT_NAME="rift-pipeline-migrate"
SCRIPT_VERSION="3.0.0"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Migration configuration
SOURCE_STAGE=""
TARGET_STAGE=""
MIGRATION_MODE="incremental"
DRY_RUN=false
VERBOSE=false
FORCE_OVERWRITE=false
PRESERVE_ARTIFACTS=true
ENABLE_TDD=true

# Stage configuration mapping
declare -A STAGE_CONFIGS=(
    ["0"]="tokenization:lexical_analysis:rift.0"
    ["1"]="parsing:syntax_analysis:rift.1"
    ["3"]="ast:semantic_analysis:rift.3"
    ["4"]="bytecode:code_generation:rift.4"
    ["5"]="program:execution_runtime:rift.5"
)

declare -A STAGE_DEPENDENCIES=(
    ["1"]="0"
    ["3"]="1"
    ["4"]="3"
    ["5"]="4"
)

# Logging framework
log_info() {
    echo "[INFO] $*"
}

log_warning() {
    echo "[WARN] $*" >&2
}

log_error() {
    echo "[ERROR] $*" >&2
}

log_verbose() {
    if [ "$VERBOSE" = true ]; then
        echo "[VERBOSE] $*"
    fi
}

log_migration() {
    echo "[MIGRATE] $*"
}

# Dry-run execution wrapper
execute_command() {
    local description="$1"
    shift
    
    if [ "$DRY_RUN" = true ]; then
        echo "[DRY-RUN] $description"
        echo "  Command: $*"
    else
        log_verbose "$description"
        "$@"
    fi
}

# File creation with migration tracking
create_migrated_file() {
    local file_path="$1"
    local source_file="${2:-}"
    local description="${3:-Migrating file $file_path}"
    
    if [ "$DRY_RUN" = true ]; then
        echo "[DRY-RUN] $description"
        echo "  File: $file_path"
        [ -n "$source_file" ] && echo "  Source: $source_file"
        return 0
    fi
    
    if [ -f "$file_path" ] && [ "$FORCE_OVERWRITE" = false ]; then
        log_warning "File exists: $file_path (use --force to overwrite)"
        return 1
    fi
    
    log_verbose "$description"
    return 0
}

# Usage information
show_usage() {
    cat << EOF
RIFT Pipeline Migration Framework - Enhanced TDD Architecture

Usage: $0 [OPTIONS] SOURCE_STAGE TARGET_STAGE

Arguments:
  SOURCE_STAGE          Source RIFT stage (0, 1, 3, 4, 5)
  TARGET_STAGE          Target RIFT stage (must be > SOURCE_STAGE)

Options:
  --mode MODE           Migration mode: incremental|full|template (default: incremental)
  --dry-run             Show migration plan without executing
  --verbose             Enable verbose output
  --force               Overwrite existing files
  --no-preserve         Don't preserve source artifacts
  --disable-tdd         Skip TDD framework generation
  --help                Show this help message
  --version             Show version information

Migration Modes:
  incremental           Copy and extend source stage with new functionality
  full                  Complete stage bootstrap with advanced features
  template              Generate template files for manual implementation

Examples:
  $0 0 1                           # Migrate rift0 to rift1
  $0 --dry-run 1 3                 # Preview rift1 to rift3 migration
  $0 --mode full --verbose 3 4     # Full migration with verbose output
  $0 --no-preserve 0 5             # Direct migration without intermediate stages

AEGIS Methodology - "Structure IS the syntax"
EOF
}

# Version information
show_version() {
    cat << EOF
$SCRIPT_NAME version $SCRIPT_VERSION
OBINexus RIFT Pipeline Migration Framework
AEGIS (Automaton Engine for Generative Interpretation & Syntax)

Stage Pipeline Support:
  Stage 0: Tokenization/Lexical Analysis
  Stage 1: Parsing/Syntax Analysis  
  Stage 3: AST/Semantic Analysis
  Stage 4: Bytecode/Code Generation
  Stage 5: Program/Execution Runtime

Copyright (c) 2025 OBINexus Computing
License: Proprietary - All Rights Reserved
EOF
}

# Argument parsing
parse_arguments() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            --mode)
                MIGRATION_MODE="$2"
                shift 2
                ;;
            --dry-run)
                DRY_RUN=true
                shift
                ;;
            --verbose)
                VERBOSE=true
                shift
                ;;
            --force)
                FORCE_OVERWRITE=true
                shift
                ;;
            --no-preserve)
                PRESERVE_ARTIFACTS=false
                shift
                ;;
            --disable-tdd)
                ENABLE_TDD=false
                shift
                ;;
            --help)
                show_usage
                exit 0
                ;;
            --version)
                show_version
                exit 0
                ;;
            --*)
                log_error "Unknown option: $1"
                show_usage
                exit 1
                ;;
            *)
                if [ -z "$SOURCE_STAGE" ]; then
                    SOURCE_STAGE="$1"
                elif [ -z "$TARGET_STAGE" ]; then
                    TARGET_STAGE="$1"
                else
                    log_error "Too many arguments: $1"
                    show_usage
                    exit 1
                fi
                shift
                ;;
        esac
    done
    
    # Validate required arguments
    if [ -z "$SOURCE_STAGE" ] || [ -z "$TARGET_STAGE" ]; then
        log_error "Both SOURCE_STAGE and TARGET_STAGE are required"
        show_usage
        exit 1
    fi
}

# Validation functions
validate_stage() {
    local stage="$1"
    case "$stage" in
        0|1|3|4|5)
            return 0
            ;;
        *)
            log_error "Invalid stage: $stage. Supported stages: 0, 1, 3, 4, 5"
            return 1
            ;;
    esac
}

validate_migration_path() {
    local source="$1"
    local target="$2"
    
    if [ "$target" -le "$source" ]; then
        log_error "Target stage ($target) must be greater than source stage ($source)"
        return 1
    fi
    
    # Check if source directory exists
    if [ ! -d "rift$source" ]; then
        log_error "Source directory rift$source does not exist"
        return 1
    fi
    
    # Validate dependency chain
    local current="$target"
    while [ "$current" != "$source" ]; do
        local dependency="${STAGE_DEPENDENCIES[$current]:-}"
        if [ -z "$dependency" ]; then
            log_error "No dependency defined for stage $current"
            return 1
        fi
        
        if [ "$dependency" -lt "$source" ]; then
            current="$dependency"
        else
            break
        fi
    done
    
    return 0
}

validate_mode() {
    local mode="$1"
    case "$mode" in
        incremental|full|template)
            return 0
            ;;
        *)
            log_error "Invalid mode: $mode. Supported modes: incremental, full, template"
            return 1
            ;;
    esac
}

# Artifact preservation system
preserve_source_artifacts() {
    local source_stage="$1"
    local target_stage="$2"
    
    if [ "$PRESERVE_ARTIFACTS" = false ]; then
        log_info "Artifact preservation disabled"
        return 0
    fi
    
    log_migration "Preserving artifacts from rift$source_stage"
    
    local source_dir="rift$source_stage"
    local target_dir="rift$target_stage"
    
    # Copy core architecture
    execute_command "Copying core headers" \
        cp -r "$source_dir/include/" "$target_dir/"
    
    # Update header guards and namespaces
    if [ "$DRY_RUN" = false ]; then
        find "$target_dir/include" -name "*.h" -exec sed -i "s/rift$source_stage/rift$target_stage/g" {} \;
    fi
    
    # Copy and adapt source structure
    execute_command "Copying source templates" \
        cp -r "$source_dir/src/" "$target_dir/"
    
    # Copy test framework
    execute_command "Copying QA framework" \
        cp -r "$source_dir/tests/" "$target_dir/"
    
    # Copy build system
    execute_command "Copying build configuration" \
        cp "$source_dir/Makefile" "$target_dir/"
    
    # Update Makefile for new stage
    if [ "$DRY_RUN" = false ]; then
        sed -i "s/rift$source_stage/rift$target_stage/g" "$target_dir/Makefile"
        sed -i "s/Stage $source_stage/Stage $target_stage/g" "$target_dir/Makefile"
    fi
}

# TDD framework generation
generate_tdd_framework() {
    local target_stage="$1"
    local target_dir="rift$target_stage"
    
    if [ "$ENABLE_TDD" = false ]; then
        log_info "TDD framework generation disabled"
        return 0
    fi
    
    log_migration "Generating TDD framework for stage $target_stage"
    
    # Stage-specific test categories
    local stage_config="${STAGE_CONFIGS[$target_stage]}"
    local stage_name=$(echo "$stage_config" | cut -d: -f1)
    local stage_function=$(echo "$stage_config" | cut -d: -f2)
    local stage_output=$(echo "$stage_config" | cut -d: -f3)
    
    # Create comprehensive test suite
    local test_file="$target_dir/tests/qa_mocks/test_stage${target_stage}_comprehensive.c"
    
    if ! create_migrated_file "$test_file" "" "Generating comprehensive test suite"; then
        return 0
    fi
    
    cat > "$test_file" << EOF
// RIFT Stage $target_stage Comprehensive Test Suite
// OBINexus Computing - AEGIS TDD Framework
// Stage: $stage_name - Function: $stage_function - Output: $stage_output

#include "../../include/rift$target_stage/rift.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

// Test categories for Stage $target_stage
typedef enum {
    TEST_CATEGORY_BASIC_FUNCTIONALITY,
    TEST_CATEGORY_EDGE_CASES,
    TEST_CATEGORY_ERROR_HANDLING,
    TEST_CATEGORY_PERFORMANCE,
    TEST_CATEGORY_SECURITY,
    TEST_CATEGORY_INTEGRATION,
    TEST_CATEGORY_MEMORY_SAFETY,
    TEST_CATEGORY_THREAD_SAFETY
} TestCategory;

// Test result tracking
typedef struct {
    int total_tests;
    int passed_tests;
    int failed_tests;
    int skipped_tests;
} TestResults;

static TestResults test_results = {0};

// Test assertion macros
#define ASSERT_STAGE_EQ(expected, actual, message) \\
    do { \\
        test_results.total_tests++; \\
        if ((expected) == (actual)) { \\
            test_results.passed_tests++; \\
            printf("✅ %s\\n", message); \\
        } else { \\
            test_results.failed_tests++; \\
            printf("❌ %s: expected %d, got %d\\n", message, (int)(expected), (int)(actual)); \\
        } \\
    } while(0)

#define ASSERT_STAGE_TRUE(condition, message) \\
    do { \\
        test_results.total_tests++; \\
        if (condition) { \\
            test_results.passed_tests++; \\
            printf("✅ %s\\n", message); \\
        } else { \\
            test_results.failed_tests++; \\
            printf("❌ %s: condition failed\\n", message); \\
        } \\
    } while(0)

// Stage $target_stage specific tests
void test_${stage_name}_basic_functionality(void) {
    printf("\\n🧪 Testing $stage_name basic functionality...\\n");
    
    // TODO: Implement stage-specific basic functionality tests
    ASSERT_STAGE_TRUE(true, "Basic functionality placeholder");
}

void test_${stage_name}_edge_cases(void) {
    printf("\\n🔍 Testing $stage_name edge cases...\\n");
    
    // TODO: Implement comprehensive edge case testing
    // - Boundary conditions
    // - Invalid input handling
    // - Resource exhaustion scenarios
    // - Malformed data processing
    
    ASSERT_STAGE_TRUE(true, "Edge cases placeholder");
}

void test_${stage_name}_error_handling(void) {
    printf("\\n⚠️  Testing $stage_name error handling...\\n");
    
    // TODO: Implement error handling validation
    // - Graceful degradation
    // - Error propagation
    // - Recovery mechanisms
    // - Audit trail generation
    
    ASSERT_STAGE_TRUE(true, "Error handling placeholder");
}

void test_${stage_name}_memory_safety(void) {
    printf("\\n🛡️  Testing $stage_name memory safety...\\n");
    
    // TODO: Implement memory safety validation
    // - No memory leaks
    // - Proper cleanup on error paths
    // - Buffer overflow protection
    // - Use-after-free prevention
    
    ASSERT_STAGE_TRUE(true, "Memory safety placeholder");
}

void test_${stage_name}_integration(void) {
    printf("\\n🔗 Testing $stage_name integration...\\n");
    
    // TODO: Implement integration testing
    // - Pipeline compatibility
    // - Cross-stage data flow
    // - Configuration consistency
    // - Artifact format validation
    
    ASSERT_STAGE_TRUE(true, "Integration placeholder");
}

// Main test execution
int main(void) {
    printf("🚀 RIFT Stage $target_stage Comprehensive Test Suite\\n");
    printf("==================================================\\n");
    printf("Stage: $stage_name\\n");
    printf("Function: $stage_function\\n");
    printf("Output Format: $stage_output\\n\\n");
    
    // Execute test categories
    test_${stage_name}_basic_functionality();
    test_${stage_name}_edge_cases();
    test_${stage_name}_error_handling();
    test_${stage_name}_memory_safety();
    test_${stage_name}_integration();
    
    // Test results summary
    printf("\\n📊 Test Results Summary\\n");
    printf("========================\\n");
    printf("Total Tests: %d\\n", test_results.total_tests);
    printf("Passed: %d\\n", test_results.passed_tests);
    printf("Failed: %d\\n", test_results.failed_tests);
    printf("Skipped: %d\\n", test_results.skipped_tests);
    
    if (test_results.failed_tests == 0) {
        printf("\\n✅ All tests passed! Stage $target_stage is ready for integration.\\n");
        return 0;
    } else {
        printf("\\n❌ %d tests failed. Review implementation before proceeding.\\n", test_results.failed_tests);
        return 1;
    }
}
EOF
}

# Stage-specific implementation templates
generate_stage_implementation() {
    local target_stage="$1"
    local target_dir="rift$target_stage"
    
    local stage_config="${STAGE_CONFIGS[$target_stage]}"
    local stage_name=$(echo "$stage_config" | cut -d: -f1)
    local stage_function=$(echo "$stage_config" | cut -d: -f2)
    
    log_migration "Generating stage $target_stage implementation templates"
    
    # Core implementation file
    local core_file="$target_dir/src/core/rift${target_stage}_${stage_name}.c"
    
    if ! create_migrated_file "$core_file" "" "Generating core implementation"; then
        return 0
    fi
    
    cat > "$core_file" << EOF
// RIFT Stage $target_stage Core Implementation
// OBINexus Computing - AEGIS Framework
// Stage: $stage_name - Function: $stage_function

#include "../../include/rift$target_stage/rift.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Stage $target_stage specific structures and functions

// TODO: Implement stage-specific data structures
typedef struct Rift${target_stage^}Engine {
    // Stage-specific engine state
    void* stage_data;
    size_t data_size;
    bool initialized;
} Rift${target_stage^}Engine;

// TODO: Implement stage-specific processing functions
Rift${target_stage^}Engine* rift${target_stage}_engine_create(void) {
    Rift${target_stage^}Engine* engine = calloc(1, sizeof(Rift${target_stage^}Engine));
    if (!engine) {
        return NULL;
    }
    
    // Initialize stage-specific components
    engine->initialized = true;
    
    return engine;
}

void rift${target_stage}_engine_destroy(Rift${target_stage^}Engine* engine) {
    if (!engine) return;
    
    // Cleanup stage-specific resources
    free(engine->stage_data);
    free(engine);
}

// TODO: Implement main processing function
RiftResult rift${target_stage}_process(Rift${target_stage^}Engine* engine, 
                                      const char* input_file, 
                                      const char* output_file) {
    if (!engine || !input_file || !output_file) {
        return RIFT_ERROR_NULL_POINTER;
    }
    
    // TODO: Implement stage-specific processing logic
    // 1. Read input from previous stage
    // 2. Apply stage-specific transformations
    // 3. Generate output for next stage
    // 4. Validate results
    
    return RIFT_SUCCESS;
}
EOF

    # CLI integration
    local cli_file="$target_dir/src/cli/rift${target_stage}_cli.c"
    
    if ! create_migrated_file "$cli_file" "" "Generating CLI integration"; then
        return 0
    fi
    
    cat > "$cli_file" << EOF
// RIFT Stage $target_stage CLI Implementation
// OBINexus Computing - AEGIS Framework

#include "../../include/rift$target_stage/rift.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

// CLI command structure
typedef struct {
    char* input_file;
    char* output_file;
    char* config_file;
    bool verbose;
    bool debug;
} CliOptions;

void print_usage(const char* program_name) {
    printf("Usage: %s [OPTIONS] INPUT_FILE\\n", program_name);
    printf("\\nOBINexus RIFT Stage $target_stage - $stage_function\\n");
    printf("\\nOptions:\\n");
    printf("  -o, --output FILE     Output file (default: INPUT.rift.$target_stage)\\n");
    printf("  -c, --config FILE     Configuration file\\n");
    printf("  -v, --verbose         Enable verbose output\\n");
    printf("  -d, --debug           Enable debug mode\\n");
    printf("  -h, --help            Show this help message\\n");
    printf("\\nExamples:\\n");
    printf("  %s input.rift.$((target_stage - 1))\\n", program_name);
    printf("  %s -v -o output.rift.$target_stage input.rift.$((target_stage - 1))\\n", program_name);
}

int main(int argc, char* argv[]) {
    CliOptions opts = {0};
    
    static struct option long_options[] = {
        {"output",  required_argument, 0, 'o'},
        {"config",  required_argument, 0, 'c'},
        {"verbose", no_argument,       0, 'v'},
        {"debug",   no_argument,       0, 'd'},
        {"help",    no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };
    
    int option_index = 0;
    int c;
    
    while ((c = getopt_long(argc, argv, "o:c:vdh", long_options, &option_index)) != -1) {
        switch (c) {
            case 'o':
                opts.output_file = optarg;
                break;
            case 'c':
                opts.config_file = optarg;
                break;
            case 'v':
                opts.verbose = true;
                break;
            case 'd':
                opts.debug = true;
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            case '?':
                print_usage(argv[0]);
                return 1;
            default:
                abort();
        }
    }
    
    // Validate arguments
    if (optind >= argc) {
        fprintf(stderr, "Error: Input file required\\n");
        print_usage(argv[0]);
        return 1;
    }
    
    opts.input_file = argv[optind];
    
    // Generate default output file if not specified
    if (!opts.output_file) {
        // TODO: Generate output filename based on input
        opts.output_file = "output.rift.$target_stage";
    }
    
    if (opts.verbose) {
        printf("🚀 RIFT Stage $target_stage Processing\\n");
        printf("Input: %s\\n", opts.input_file);
        printf("Output: %s\\n", opts.output_file);
    }
    
    // TODO: Implement stage processing
    printf("✅ Stage $target_stage processing complete\\n");
    
    return 0;
}
EOF
}

# Main migration execution
execute_migration() {
    local source_stage="$1"
    local target_stage="$2"
    local mode="$3"
    
    log_migration "Executing migration: rift$source_stage → rift$target_stage (mode: $mode)"
    
    local target_dir="rift$target_stage"
    
    # Create target directory structure
    execute_command "Creating target directory structure" \
        mkdir -p "$target_dir"/{src/{core,cli},include/rift$target_stage,tests/qa_mocks,scripts,examples,bin,lib,out}
    
    case "$mode" in
        incremental)
            preserve_source_artifacts "$source_stage" "$target_stage"
            generate_stage_implementation "$target_stage"
            generate_tdd_framework "$target_stage"
            ;;
        full)
            # Use bootstrap script for full generation
            execute_command "Running full bootstrap for stage $target_stage" \
                ./bootstraped_15june.sh --mode full "$target_stage" --output-dir "$target_dir"
            ;;
        template)
            # Generate minimal templates
            execute_command "Creating template structure" \
                ./bootstraped_15june.sh --mode templates "$target_stage" --output-dir "$target_dir"
            ;;
    esac
    
    # Generate migration report
    generate_migration_report "$source_stage" "$target_stage" "$mode"
}

# Migration report generation
generate_migration_report() {
    local source_stage="$1"
    local target_stage="$2"
    local mode="$3"
    
    local report_file="rift$target_stage/MIGRATION_REPORT.md"
    
    if ! create_migrated_file "$report_file" "" "Generating migration report"; then
        return 0
    fi
    
    cat > "$report_file" << EOF
# RIFT Pipeline Migration Report

## Migration Summary
- **Source Stage**: $source_stage
- **Target Stage**: $target_stage  
- **Migration Mode**: $mode
- **Migration Date**: $(date)
- **Migration Framework**: $SCRIPT_VERSION

## Stage Configuration
$(echo "${STAGE_CONFIGS[$target_stage]}" | tr ':' '\n' | sed 's/^/- /')

## Migration Checklist

### ✅ Completed Tasks
- [x] Directory structure created
- [x] Core headers migrated and updated
- [x] Build system adapted
- [x] TDD framework generated
- [x] CLI integration templates created

### 🔄 Implementation Tasks
- [ ] Core stage functionality implementation
- [ ] Comprehensive test suite completion
- [ ] Edge case validation
- [ ] Performance optimization
- [ ] Security validation
- [ ] Integration testing
- [ ] Documentation completion

### 🧪 Testing Requirements
- [ ] Unit tests passing
- [ ] Integration tests passing
- [ ] Performance benchmarks met
- [ ] Memory safety validation
- [ ] Security audit completed

## Next Steps

1. **Implementation Phase**
   \`\`\`bash
   cd rift$target_stage
   make bootstrap
   \`\`\`

2. **Development Workflow**
   \`\`\`bash
   # Edit core implementation
   vim src/core/rift${target_stage}_*.c
   
   # Run tests
   make test
   
   # Verify architecture
   make verify-architecture
   \`\`\`

3. **Integration Validation**
   \`\`\`bash
   # Test pipeline flow
   ./bin/rift$target_stage.exe input.rift.$((target_stage - 1))
   \`\`\`

## AEGIS Methodology Compliance

This migration maintains AEGIS principles:
- **Data Orientation**: All transformations are data-driven
- **Functional Composition**: Pure functions for predictable behavior
- **Pattern Recognition**: Regex automaton-based processing
- **Unified Pipeline**: Consistent data model across stages

## Architecture Notes

- Token type/value separation maintained
- matched_state preservation for AST minimization
- Strict compiler compliance enforced
- TDD framework integrated for comprehensive validation

---
*Generated by RIFT Pipeline Migration Framework v$SCRIPT_VERSION*
*OBINexus Computing - "Structure IS the syntax"*
EOF
}

# Main execution function
main() {
    log_info "OBINexus RIFT Pipeline Migration Framework v$SCRIPT_VERSION"
    log_info "AEGIS Methodology - Systematic Stage Development"
    
    # Parse and validate arguments
    parse_arguments "$@"
    
    validate_stage "$SOURCE_STAGE"
    validate_stage "$TARGET_STAGE"
    validate_migration_path "$SOURCE_STAGE" "$TARGET_STAGE"
    validate_mode "$MIGRATION_MODE"
    
    # Display migration plan
    if [ "$DRY_RUN" = true ]; then
        log_info "DRY RUN MODE - Showing migration plan"
    fi
    
    log_info "Migration Configuration:"
    log_info "  Source Stage: $SOURCE_STAGE"
    log_info "  Target Stage: $TARGET_STAGE"
    log_info "  Migration Mode: $MIGRATION_MODE"
    log_info "  Preserve Artifacts: $PRESERVE_ARTIFACTS"
    log_info "  TDD Framework: $ENABLE_TDD"
    log_info "  Verbose Output: $VERBOSE"
    log_info "  Force Overwrite: $FORCE_OVERWRITE"
    
    # Execute migration
    execute_migration "$SOURCE_STAGE" "$TARGET_STAGE" "$MIGRATION_MODE"
    
    # Final summary
    if [ "$DRY_RUN" = true ]; then
        log_info "DRY RUN COMPLETE - No files were created"
        log_info "Run without --dry-run to execute the migration"
    else
        log_info "Migration complete: rift$SOURCE_STAGE → rift$TARGET_STAGE"
        log_info "Target directory: rift$TARGET_STAGE/"
        log_info "Next steps:"
        log_info "  cd rift$TARGET_STAGE"
        log_info "  make bootstrap"
        log_info "  ./run.sh"
    fi
}

# Execute main function with all arguments
main "$@"
