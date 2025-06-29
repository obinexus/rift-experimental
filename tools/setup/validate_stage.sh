#!/usr/bin/env bash
# =================================================================
# RIFT AEGIS Stage Validation Framework
# OBINexus Computing Framework - Technical Lead: Nnamdi Michael Okpala
# VERSION: 1.3.0-PRODUCTION
# METHODOLOGY: Systematic validation with QA compliance verification
# =================================================================

set -e

# Validation framework configuration
TOOL_VERSION="1.3.0"
QA_COMPLIANCE_REQUIRED=true
AEGIS_METHODOLOGY_VALIDATION=true

# Cross-platform detection
PLATFORM=$(uname -s | tr '[:upper:]' '[:lower:]')
case "$PLATFORM" in
    linux*)     PLATFORM="linux" ;;
    darwin*)    PLATFORM="macos" ;;
    mingw*|msys*|cygwin*) PLATFORM="windows" ;;
    *)          PLATFORM="unknown" ;;
esac

# Project structure paths
PROJECT_ROOT="$(pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
LIB_DIR="$PROJECT_ROOT/lib"
BIN_DIR="$PROJECT_ROOT/bin"
TOOLS_DIR="$PROJECT_ROOT/tools"
SETUP_DIR="$TOOLS_DIR/setup"

# Validation results tracking
VALIDATION_ERRORS=0
VALIDATION_WARNINGS=0
VALIDATION_CHECKS=0

# Color codes for systematic output
readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly YELLOW='\033[1;33m'
readonly BLUE='\033[0;34m'
readonly PURPLE='\033[0;35m'
readonly NC='\033[0m'

# Logging framework with audit trail
log_validation() {
    local level="$1"
    local message="$2"
    echo -e "${BLUE}[VALIDATE-$level]${NC} $message"
    ((VALIDATION_CHECKS++))
}

log_success() {
    local message="$1"
    echo -e "${GREEN}[✓ PASS]${NC} $message"
}

log_warning() {
    local message="$1"
    echo -e "${YELLOW}[⚠ WARN]${NC} $message"
    ((VALIDATION_WARNINGS++))
}

log_error() {
    local message="$1"
    echo -e "${RED}[✗ FAIL]${NC} $message"
    ((VALIDATION_ERRORS++))
}

# AEGIS Stage configuration
STAGES=(0 1 2 3 4 5 6)
STAGE_NAMES=("tokenizer" "parser" "semantic" "validator" "bytecode" "verifier" "emitter")

get_stage_name() {
    local stage_id="$1"
    if [[ "$stage_id" =~ ^[0-6]$ ]]; then
        echo "${STAGE_NAMES[$stage_id]}"
    else
        echo "unknown"
    fi
}

# Tools validation for setup framework
validate_setup_tools() {
    log_validation "TOOLS" "Validating setup tool framework"
    
    local required_tools=(
        "$SETUP_DIR/generate_source.sh"
        "$SETUP_DIR/create_directories.sh"
        "$SETUP_DIR/generate_hash_table.sh"
    )
    
    for tool in "${required_tools[@]}"; do
        if [[ -f "$tool" && -x "$tool" ]]; then
            log_success "Setup tool available: $(basename "$tool")"
        else
            log_error "Setup tool missing or not executable: $tool"
        fi
    done
    
    # Validate tools directory structure
    if [[ -d "$TOOLS_DIR" ]]; then
        log_success "Tools directory structure validated"
    else
        log_error "Tools directory missing: $TOOLS_DIR"
    fi
}

# Cross-platform environment validation
validate_platform_environment() {
    log_validation "PLATFORM" "Validating platform environment: $PLATFORM"
    
    # Compiler validation
    case "$PLATFORM" in
        linux|macos)
            if command -v gcc >/dev/null 2>&1; then
                local gcc_version=$(gcc --version | head -1)
                log_success "GCC compiler available: $gcc_version"
            elif command -v clang >/dev/null 2>&1; then
                local clang_version=$(clang --version | head -1)
                log_success "Clang compiler available: $clang_version"
            else
                log_error "No suitable C compiler found (gcc/clang)"
            fi
            ;;
        windows)
            if command -v gcc >/dev/null 2>&1; then
                local gcc_version=$(gcc --version | head -1)
                log_success "MinGW GCC available: $gcc_version"
            else
                log_error "MinGW GCC not found"
            fi
            ;;
    esac
    
    # Build tools validation
    local build_tools=("make" "ar" "ranlib")
    for tool in "${build_tools[@]}"; do
        if command -v "$tool" >/dev/null 2>&1; then
            log_success "Build tool available: $tool"
        else
            log_error "Build tool missing: $tool"
        fi
    done
    
    # PKG-Config validation
    if command -v pkg-config >/dev/null 2>&1; then
        local pkg_version=$(pkg-config --version)
        log_success "pkg-config available: version $pkg_version"
        
        # Test critical dependencies
        local deps=("openssl" "zlib")
        for dep in "${deps[@]}"; do
            if pkg-config --exists "$dep"; then
                local dep_version=$(pkg-config --modversion "$dep")
                log_success "Dependency $dep: version $dep_version"
            else
                log_warning "Optional dependency $dep not found"
            fi
        done
    else
        log_error "pkg-config not available for dependency management"
    fi
}

# Build artifact validation
validate_build_artifacts() {
    log_validation "ARTIFACTS" "Validating build artifacts for QA compliance"
    
    # Directory structure validation
    local required_dirs=("$BUILD_DIR" "$LIB_DIR" "$BIN_DIR")
    for dir in "${required_dirs[@]}"; do
        if [[ -d "$dir" ]]; then
            log_success "Directory exists: $dir"
        else
            log_warning "Directory missing (will be created): $dir"
        fi
    done
    
    # Stage-specific artifact validation
    for stage_id in "${STAGES[@]}"; do
        local stage_name=$(get_stage_name "$stage_id")
        local lib_file="$LIB_DIR/librift-$stage_id.a"
        local bin_file="$BIN_DIR/rift-$stage_id"
        
        # Library validation
        if [[ -f "$lib_file" ]]; then
            if ar t "$lib_file" >/dev/null 2>&1; then
                log_success "Stage $stage_id library valid: $lib_file"
            else
                log_error "Stage $stage_id library corrupted: $lib_file"
            fi
        else
            log_warning "Stage $stage_id library not built: $lib_file"
        fi
        
        # Executable validation
        if [[ -f "$bin_file" ]]; then
            if [[ -x "$bin_file" ]]; then
                log_success "Stage $stage_id executable valid: $bin_file"
                
                # Basic functionality test
                if "$bin_file" --version >/dev/null 2>&1; then
                    log_success "Stage $stage_id version check passed"
                else
                    log_warning "Stage $stage_id version check failed"
                fi
            else
                log_error "Stage $stage_id executable not executable: $bin_file"
            fi
        else
            log_warning "Stage $stage_id executable not built: $bin_file"
        fi
    done
}

# Hash table validation for O(1) lookup
validate_hash_table() {
    log_validation "HASHTABLE" "Validating O(1) hash table lookup system"
    
    local hash_table="$BUILD_DIR/stage_lookup.json"
    
    if [[ -f "$hash_table" ]]; then
        # Validate JSON structure
        if python3 -c "import json; json.load(open('$hash_table'))" 2>/dev/null; then
            log_success "Hash table JSON structure valid"
            
            # Validate stage entries
            for stage_id in "${STAGES[@]}"; do
                local stage_exists=$(python3 -c "
import json
with open('$hash_table', 'r') as f:
    data = json.load(f)
print('yes' if '$stage_id' in data.get('stages', {}) else 'no')
" 2>/dev/null)
                
                if [[ "$stage_exists" == "yes" ]]; then
                    log_success "Hash table entry exists for stage $stage_id"
                else
                    log_error "Hash table missing entry for stage $stage_id"
                fi
            done
        else
            log_error "Hash table JSON structure invalid: $hash_table"
        fi
    else
        log_warning "Hash table not generated: $hash_table"
    fi
}

# AEGIS methodology compliance validation
validate_aegis_compliance() {
    log_validation "AEGIS" "Validating AEGIS waterfall methodology compliance"
    
    # Check for systematic progression markers
    local compliance_markers=(
        "RIFT_AEGIS_COMPLIANCE"
        "AEGIS Waterfall"
        "QA Compliance"
        "Systematic"
    )
    
    local source_files=($(find . -name "*.c" -o -name "*.h" -o -name "*.sh" 2>/dev/null | head -10))
    
    if [[ ${#source_files[@]} -gt 0 ]]; then
        local compliant_files=0
        for file in "${source_files[@]}"; do
            local markers_found=0
            for marker in "${compliance_markers[@]}"; do
                if grep -q "$marker" "$file" 2>/dev/null; then
                    ((markers_found++))
                fi
            done
            
            if [[ $markers_found -ge 2 ]]; then
                ((compliant_files++))
            fi
        done
        
        local compliance_ratio=$((compliant_files * 100 / ${#source_files[@]}))
        if [[ $compliance_ratio -ge 80 ]]; then
            log_success "AEGIS compliance ratio: $compliance_ratio% ($compliant_files/${#source_files[@]} files)"
        else
            log_warning "AEGIS compliance ratio below threshold: $compliance_ratio%"
        fi
    else
        log_warning "No source files found for AEGIS compliance validation"
    fi
    
    # Validate systematic documentation
    local doc_files=("README.md" "docs/RIFT_SPECIFICATION.md" "docs/QA_FRAMEWORK.md")
    for doc in "${doc_files[@]}"; do
        if [[ -f "$doc" ]]; then
            log_success "Documentation present: $doc"
        else
            log_warning "Documentation missing: $doc"
        fi
    done
}

# Comprehensive validation execution
run_comprehensive_validation() {
    log_validation "COMPREHENSIVE" "Starting comprehensive AEGIS validation"
    echo -e "${PURPLE}=================================================================${NC}"
    echo -e "${PURPLE}RIFT AEGIS Comprehensive Validation Framework v$TOOL_VERSION${NC}"
    echo -e "${PURPLE}Platform: $PLATFORM${NC}"
    echo -e "${PURPLE}=================================================================${NC}"
    
    validate_setup_tools
    validate_platform_environment
    validate_build_artifacts
    validate_hash_table
    validate_aegis_compliance
    
    # Generate validation summary
    echo -e "\n${PURPLE}=================================================================${NC}"
    echo -e "${PURPLE}VALIDATION SUMMARY${NC}"
    echo -e "${PURPLE}=================================================================${NC}"
    echo -e "Total Checks: $VALIDATION_CHECKS"
    echo -e "Warnings: ${YELLOW}$VALIDATION_WARNINGS${NC}"
    echo -e "Errors: ${RED}$VALIDATION_ERRORS${NC}"
    
    if [[ $VALIDATION_ERRORS -eq 0 ]]; then
        echo -e "${GREEN}Overall Status: PASS${NC}"
        if [[ $VALIDATION_WARNINGS -gt 0 ]]; then
            echo -e "${YELLOW}Note: $VALIDATION_WARNINGS warnings detected${NC}"
        fi
        return 0
    else
        echo -e "${RED}Overall Status: FAIL${NC}"
        echo -e "${RED}Critical errors must be resolved before proceeding${NC}"
        return 1
    fi
}

# Main execution function
main() {
    case "${1:-}" in
        --comprehensive)
            run_comprehensive_validation
            ;;
        --tools-check)
            validate_setup_tools
            ;;
        --platform=*)
            PLATFORM="${1#*=}"
            validate_platform_environment
            ;;
        --artifacts)
            validate_build_artifacts
            ;;
        --hash-table)
            validate_hash_table
            ;;
        --aegis)
            validate_aegis_compliance
            ;;
        --help)
            echo "RIFT AEGIS Stage Validation Framework v$TOOL_VERSION"
            echo "Usage: $0 [option]"
            echo ""
            echo "Options:"
            echo "  --comprehensive    Run complete validation suite"
            echo "  --tools-check      Validate setup tools only"
            echo "  --platform=NAME    Validate platform environment"
            echo "  --artifacts        Validate build artifacts"
            echo "  --hash-table       Validate hash table system"
            echo "  --aegis            Validate AEGIS compliance"
            echo "  --help             Show this help message"
            echo ""
            echo "Platform Support: linux, macos, windows"
            echo "QA Compliance: AEGIS Waterfall Methodology"
            ;;
        *)
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
}

# Execute main function with provided arguments
main "$@"
