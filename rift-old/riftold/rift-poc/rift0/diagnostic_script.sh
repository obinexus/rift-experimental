#!/bin/bash

# RIFT Stage 0 Diagnostic and Recovery Script
# OBINexus Computing - AEGIS Methodology Compliance Validation
# Systematic diagnosis of build pipeline and architectural integrity

set -euo pipefail

# Configuration
SCRIPT_VERSION="1.0.0"
DIAGNOSTIC_LOG="logs/diagnostic_$(date +%Y%m%d_%H%M%S).log"
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Ensure logs directory
mkdir -p logs

# Enhanced logging
log_diagnostic() {
    local level="$1"
    local message="$2"
    local timestamp=$(date '+%Y-%m-%d %H:%M:%S')
    echo "[$timestamp] [$level] $message" | tee -a "$DIAGNOSTIC_LOG"
}

echo "🔬 RIFT Stage 0 Diagnostic Framework"
echo "====================================="
echo "OBINexus AEGIS Methodology Compliance Validator"
echo "Version: $SCRIPT_VERSION"
echo ""

# Diagnostic Categories
diagnostic_categories=(
    "environment"
    "structure" 
    "makefile"
    "sources"
    "dependencies"
    "governance"
)

# Environment Diagnostics
diagnose_environment() {
    log_diagnostic "INFO" "Starting environment diagnostics"
    echo "🔍 Environment Diagnostics"
    echo "========================="
    
    # System information
    echo "System Information:"
    echo "  OS: $(uname -s)"
    echo "  Architecture: $(uname -m)"
    echo "  Kernel: $(uname -r)"
    
    # Compiler diagnostics
    echo ""
    echo "Compiler Environment:"
    if command -v gcc >/dev/null 2>&1; then
        echo "  ✅ GCC: $(gcc --version | head -1)"
        echo "  ✅ GCC Path: $(which gcc)"
    else
        echo "  ❌ GCC not found"
        return 1
    fi
    
    if command -v make >/dev/null 2>&1; then
        echo "  ✅ Make: $(make --version | head -1)"
    else
        echo "  ❌ Make not found"
        return 1
    fi
    
    # Build tools
    echo ""
    echo "Build Tools:"
    local tools=("ar" "ld" "nm" "objdump" "strip")
    for tool in "${tools[@]}"; do
        if command -v "$tool" >/dev/null 2>&1; then
            echo "  ✅ $tool: $(which $tool)"
        else
            echo "  ⚠️  $tool not found"
        fi
    done
    
    return 0
}

# Structure Diagnostics
diagnose_structure() {
    log_diagnostic "INFO" "Starting structure diagnostics"
    echo ""
    echo "🏗️  Project Structure Diagnostics"
    echo "================================"
    
    # Required directories
    local required_dirs=(
        "src/core"
        "src/cli" 
        "include"
        "examples"
        "tests/qa_mocks"
        "QA"
    )
    
    echo "Required Directories:"
    local missing_dirs=()
    for dir in "${required_dirs[@]}"; do
        if [ -d "$dir" ]; then
            echo "  ✅ $dir/"
        else
            echo "  ❌ $dir/ (MISSING)"
            missing_dirs+=("$dir")
        fi
    done
    
    # Critical files
    echo ""
    echo "Critical Files:"
    local critical_files=(
        "Makefile"
        "src/core/rift_core.c"
        "src/cli/main.c"
        "include/rift.h"
        "run.sh"
    )
    
    local missing_files=()
    for file in "${critical_files[@]}"; do
        if [ -f "$file" ]; then
            echo "  ✅ $file"
        else
            echo "  ❌ $file (MISSING)"
            missing_files+=("$file")
        fi
    done
    
    # R Extensions structure
    echo ""
    echo "R Extensions Structure:"
    local r_ext_files=(
        "src/core/r_extensions/r_extensions_core.c"
        "src/core/r_extensions/governance/r_governance_validation.c"
        "src/core/r_extensions/uml/r_uml_patterns.c"
        "include/rift0/core/r_extension/r_extensions.h"
    )
    
    for file in "${r_ext_files[@]}"; do
        if [ -f "$file" ]; then
            echo "  ✅ $file"
        else
            echo "  ⚠️  $file (missing R extension)"
        fi
    done
    
    # Report missing components
    if [ ${#missing_dirs[@]} -gt 0 ] || [ ${#missing_files[@]} -gt 0 ]; then
        echo ""
        echo "🔧 Recovery Commands:"
        echo "==================="
        for dir in "${missing_dirs[@]}"; do
            echo "mkdir -p $dir"
        done
        return 1
    fi
    
    return 0
}

# Makefile Diagnostics
diagnose_makefile() {
    log_diagnostic "INFO" "Starting Makefile diagnostics"
    echo ""
    echo "📋 Makefile Diagnostics"
    echo "======================"
    
    if [ ! -f "Makefile" ]; then
        echo "  ❌ Makefile not found"
        return 1
    fi
    
    # Check for common issues
    echo "Makefile Analysis:"
    
    # Duplicate targets
    local duplicate_targets=$(grep -n "^[a-zA-Z_-]*:" Makefile | cut -d: -f2 | sort | uniq -d)
    if [ -n "$duplicate_targets" ]; then
        echo "  ⚠️  Duplicate targets detected:"
        echo "$duplicate_targets" | sed 's/^/      /'
    else
        echo "  ✅ No duplicate targets"
    fi
    
    # Required targets
    local required_targets=("all" "clean" "lib" "test" "directories")
    echo ""
    echo "Required Targets:"
    for target in "${required_targets[@]}"; do
        if grep -q "^$target:" Makefile; then
            echo "  ✅ $target"
        else
            echo "  ❌ $target (MISSING)"
        fi
    done
    
    # Include file validation
    echo ""
    echo "Include Files:"
    if grep -q "include.*\.riftrc\.make" Makefile; then
        echo "  ✅ .riftrc.make inclusion found"
        if [ -f ".riftrc.make" ]; then
            echo "  ✅ .riftrc.make exists"
        else
            echo "  ⚠️  .riftrc.make missing (will create default)"
        fi
    else
        echo "  ⚠️  .riftrc.make inclusion not found"
    fi
    
    return 0
}

# Source Code Diagnostics
diagnose_sources() {
    log_diagnostic "INFO" "Starting source code diagnostics"
    echo ""
    echo "📝 Source Code Diagnostics"
    echo "========================="
    
    # Core source validation
    echo "Core Sources:"
    if [ -f "src/core/rift_core.c" ]; then
        local line_count=$(wc -l < "src/core/rift_core.c")
        echo "  ✅ rift_core.c ($line_count lines)"
        
        # Check for key functions
        local key_functions=("rift_config_create" "rift_engine_create" "rift_pattern_compile")
        for func in "${key_functions[@]}"; do
            if grep -q "$func" "src/core/rift_core.c"; then
                echo "    ✅ $func() implemented"
            else
                echo "    ⚠️  $func() not found"
            fi
        done
    else
        echo "  ❌ rift_core.c missing"
        return 1
    fi
    
    # CLI source validation
    echo ""
    echo "CLI Sources:"
    if [ -f "src/cli/main.c" ]; then
        local line_count=$(wc -l < "src/cli/main.c")
        echo "  ✅ main.c ($line_count lines)"
        
        # Check for main function
        if grep -q "int main" "src/cli/main.c"; then
            echo "    ✅ main() function found"
        else
            echo "    ⚠️  main() function not found"
        fi
    else
        echo "  ❌ main.c missing"
        return 1
    fi
    
    # Header validation
    echo ""
    echo "Header Files:"
    if [ -f "include/rift.h" ]; then
        local line_count=$(wc -l < "include/rift.h")
        echo "  ✅ rift.h ($line_count lines)"
        
        # Check for key structures
        local key_structs=("RiftAutomaton" "RiftEngine" "RiftPattern")
        for struct in "${key_structs[@]}"; do
            if grep -q "typedef struct.*$struct" "include/rift.h"; then
                echo "    ✅ $struct defined"
            else
                echo "    ⚠️  $struct not found"
            fi
        done
    else
        echo "  ❌ rift.h missing"
        return 1
    fi
    
    return 0
}

# Dependencies Diagnostics  
diagnose_dependencies() {
    log_diagnostic "INFO" "Starting dependencies diagnostics"
    echo ""
    echo "🔗 Dependencies Diagnostics"
    echo "=========================="
    
    # System libraries
    echo "System Libraries:"
    local sys_libs=("libc" "libm" "libpthread")
    for lib in "${sys_libs[@]}"; do
        if ldconfig -p | grep -q "$lib"; then
            echo "  ✅ $lib available"
        else
            echo "  ⚠️  $lib not found in ldconfig"
        fi
    done
    
    # Development headers
    echo ""
    echo "Development Headers:"
    local dev_headers=("/usr/include/stdio.h" "/usr/include/stdlib.h" "/usr/include/regex.h")
    for header in "${dev_headers[@]}"; do
        if [ -f "$header" ]; then
            echo "  ✅ $(basename $header)"
        else
            echo "  ⚠️  $(basename $header) not found"
        fi
    done
    
    return 0
}

# Governance Diagnostics
diagnose_governance() {
    log_diagnostic "INFO" "Starting governance diagnostics"
    echo ""
    echo "🔒 Governance Diagnostics"
    echo "========================"
    
    # QA structure validation
    echo "QA Framework:"
    if [ -d "tests/qa_mocks" ]; then
        echo "  ✅ QA mocks directory exists"
        local qa_files=$(find tests/qa_mocks -name "*.c" 2>/dev/null | wc -l)
        echo "  📊 QA test files: $qa_files"
    else
        echo "  ❌ QA mocks directory missing"
    fi
    
    if [ -f "QA/edge_case_registry.md" ]; then
        echo "  ✅ Edge case registry exists"
    else
        echo "  ⚠️  Edge case registry missing"
    fi
    
    # R Extensions governance
    echo ""
    echo "R Extensions Governance:"
    if [ -f "src/core/r_extensions/governance/r_governance_validation.c" ]; then
        echo "  ✅ Governance validation implementation"
        
        # Check for governance functions
        local gov_functions=("validate_governance_triangle" "calculate_governance_norm")
        for func in "${gov_functions[@]}"; do
            if grep -q "$func" "src/core/r_extensions/governance/r_governance_validation.c"; then
                echo "    ✅ $func() implemented"
            else
                echo "    ⚠️  $func() not found"
            fi
        done
    else
        echo "  ❌ Governance validation missing"
    fi
    
    return 0
}

# Recovery Recommendations
generate_recovery_plan() {
    log_diagnostic "INFO" "Generating recovery plan"
    echo ""
    echo "🔧 Recovery Plan"
    echo "==============="
    
    # Immediate fixes
    echo "Immediate Fixes:"
    echo "1. Fix permissions:"
    echo "   chmod +x run.sh"
    echo "   chmod +x scripts/validation/*.sh 2>/dev/null || true"
    echo ""
    echo "2. Create missing directories:"
    echo "   mkdir -p {build,bin,lib,out,logs}"
    echo "   mkdir -p build/core/r_extensions/{governance,uml}"
    echo ""
    echo "3. Clean and rebuild:"
    echo "   make clean"
    echo "   make bootstrap"
    echo ""
    
    # Advanced recovery
    echo "Advanced Recovery (if needed):"
    echo "1. Reset project state:"
    echo "   git clean -fdx  # WARNING: removes all untracked files"
    echo "   git reset --hard HEAD"
    echo ""
    echo "2. Rebuild configuration:"
    echo "   cp Makefile.backup.* Makefile  # if available"
    echo "   ./scripts/build/process_artifacts.sh  # if available"
    echo ""
    
    # Validation steps
    echo "Validation Steps:"
    echo "1. ./run.sh  # Should complete without errors"
    echo "2. bin/rift0 --help  # Should display help"
    echo "3. bin/rift0_demo  # Should run demo"
    echo "4. make test  # Should pass all tests"
}

# Main diagnostic execution
main() {
    log_diagnostic "INFO" "Starting RIFT Stage 0 comprehensive diagnostics"
    
    local failed_diagnostics=()
    
    # Execute all diagnostic categories
    for category in "${diagnostic_categories[@]}"; do
        if "diagnose_$category"; then
            log_diagnostic "PASS" "$category diagnostics passed"
        else
            log_diagnostic "FAIL" "$category diagnostics failed"
            failed_diagnostics+=("$category")
        fi
    done
    
    # Generate summary
    echo ""
    echo "📊 Diagnostic Summary"
    echo "===================="
    
    if [ ${#failed_diagnostics[@]} -eq 0 ]; then
        echo "✅ All diagnostics passed"
        echo "🎉 Project appears healthy - minor warnings may be acceptable"
        log_diagnostic "SUCCESS" "All diagnostics passed"
    else
        echo "⚠️  Diagnostics failed for: ${failed_diagnostics[*]}"
        echo "🔧 Recovery plan generated below"
        log_diagnostic "WARNING" "Some diagnostics failed: ${failed_diagnostics[*]}"
        generate_recovery_plan
    fi
    
    echo ""
    echo "📋 Full diagnostic log: $DIAGNOSTIC_LOG"
    log_diagnostic "INFO" "Diagnostic session completed"
}

# Execute diagnostics
main "$@"
