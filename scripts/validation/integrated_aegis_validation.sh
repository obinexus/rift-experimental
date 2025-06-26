#!/bin/bash
#
# integrated_aegis_validation.sh
# RIFT Comprehensive AEGIS Methodology Validation
# OBINexus Computing Framework - Waterfall Methodology Compliance
# Technical Lead: Nnamdi Okpala
#
# Integrates existing validation requirements with RIFT-4 implementation
#

set -euo pipefail

# Color codes for professional output formatting
readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly YELLOW='\033[1;33m'
readonly BLUE='\033[0;34m'
readonly PURPLE='\033[0;35m'
readonly NC='\033[0m' # No Color

# Validation state tracking
VALIDATION_ERRORS=0
VALIDATION_WARNINGS=0
TESTS_EXECUTED=0

# Logging functions for structured output
log_test() {
    ((TESTS_EXECUTED++))
    printf "${BLUE}[TEST %03d]${NC} %s\n" "$TESTS_EXECUTED" "$1"
}

log_pass() {
    printf "${GREEN}[PASS]${NC} %s\n" "$1"
}

log_fail() {
    ((VALIDATION_ERRORS++))
    printf "${RED}[FAIL]${NC} %s\n" "$1"
}

log_warn() {
    ((VALIDATION_WARNINGS++))
    printf "${YELLOW}[WARN]${NC} %s\n" "$1"
}

log_info() {
    printf "${BLUE}[INFO]${NC} %s\n" "$1"
}

banner() {
    echo -e "${PURPLE}"
    echo "================================================================="
    echo "  RIFT AEGIS Methodology Validation Framework                   "
    echo "  OBINexus Computing - Waterfall Methodology Compliance         "
    echo "  Technical Integration: Existing + RIFT-4 Requirements         "
    echo "================================================================="
    echo -e "${NC}"
}

#
# AEGIS Core Architecture Validation
#

validate_token_type_value_separation() {
    log_test "Token Type/Value Separation Architecture"
    
    local separation_validated=true
    
    # Check for proper separation in headers
    if [ -f "include/rift/core/rift_types.h" ]; then
        if grep -q "char\* type;" include/rift/core/rift_types.h && \
           grep -q "char\* value;" include/rift/core/rift_types.h; then
            log_pass "Token type/value separation preserved in rift_types.h"
        else
            separation_validated=false
            log_fail "Token type/value separation violation in rift_types.h"
        fi
    else
        # Check alternative header locations for existing implementation
        local header_found=false
        for header_path in include/rift*/core/rift_types.h include/rift/token_type/token_type.h include/rift/token_value/token_value.h; do
            if [ -f "$header_path" ]; then
                header_found=true
                if grep -q "char\* type;\|rift_token_type" "$header_path" && \
                   grep -q "char\* value;\|rift_token_value" "$header_path"; then
                    log_pass "Token separation verified in $header_path"
                else
                    separation_validated=false
                    log_fail "Token separation violation in $header_path"
                fi
            fi
        done
        
        if [ "$header_found" = false ]; then
            separation_validated=false
            log_fail "No token type/value headers found"
        fi
    fi
    
    # Check for architectural violations in source code
    if find src/ -name "*.c" -type f 2>/dev/null | xargs grep -l "token.*type.*value\|token.*value.*type" 2>/dev/null | grep -v "separate\|preserve"; then
        log_warn "Potential type/value merging detected in source files"
        log_info "Review above files for AEGIS compliance"
    else
        log_pass "No type/value merging violations detected"
    fi
    
    return $([ "$separation_validated" = true ] && echo 0 || echo 1)
}

validate_aegis_automaton_implementation() {
    log_test "AEGIS Automaton Implementation"
    
    local automaton_found=false
    
    # Check for RiftAutomaton structure definition
    for header_path in include/rift/core/rift_automaton.h include/rift*/core/rift_automaton.h include/rift*/core/*.h; do
        if [ -f "$header_path" ] && grep -q "RiftAutomaton\|rift_automaton" "$header_path"; then
            automaton_found=true
            log_pass "AEGIS automaton structure found in $header_path"
            
            # Validate 5-tuple automaton components (Q, Σ, δ, q0, F)
            if grep -q "states\|state_count" "$header_path" && \
               grep -q "alphabet\|input_symbols" "$header_path" && \
               grep -q "transitions\|transition_function" "$header_path" && \
               grep -q "initial_state\|q0" "$header_path" && \
               grep -q "accepting_states\|final_states\|F" "$header_path"; then
                log_pass "5-tuple automaton structure validated"
            else
                log_warn "Incomplete 5-tuple automaton structure"
            fi
            break
        fi
    done
    
    if [ "$automaton_found" = false ]; then
        log_fail "AEGIS automaton structure missing"
        return 1
    fi
    
    return 0
}

validate_dual_parsing_strategies() {
    log_test "Dual Parsing Strategies Implementation"
    
    local strategies_found=false
    
    # Check for dual parsing strategy definitions
    for header_path in include/rift/core/rift_parser.h include/rift*/core/rift_parser.h include/rift*/core/*.h; do
        if [ -f "$header_path" ]; then
            if grep -q "RIFT_PARSE_BOTTOM_UP\|RIFT_PARSE_TOP_DOWN\|bottom_up\|top_down" "$header_path"; then
                strategies_found=true
                log_pass "Dual parsing strategies found in $header_path"
                
                # Validate both strategies are present
                if grep -q "RIFT_PARSE_BOTTOM_UP\|bottom_up" "$header_path" && \
                   grep -q "RIFT_PARSE_TOP_DOWN\|top_down" "$header_path"; then
                    log_pass "Both bottom-up and top-down strategies implemented"
                else
                    log_warn "Incomplete dual parsing strategy implementation"
                fi
                break
            fi
        fi
    done
    
    if [ "$strategies_found" = false ]; then
        log_fail "Dual parsing strategies missing"
        return 1
    fi
    
    return 0
}

validate_matched_state_preservation() {
    log_test "matched_state Preservation for AST Optimization"
    
    local state_preservation_found=false
    
    # Check for matched_state preservation in headers and source
    for file_path in include/rift*/core/*.h src/core/*.c src/core/*/*.c; do
        if [ -f "$file_path" ] && grep -q "matched_state" "$file_path"; then
            state_preservation_found=true
            log_pass "matched_state preservation found in $file_path"
        fi
    done
    
    if [ "$state_preservation_found" = true ]; then
        log_pass "matched_state preserved for AST minimization"
        
        # Check if preservation is properly integrated with AST nodes
        if find include/ src/ -name "*.h" -o -name "*.c" 2>/dev/null | xargs grep -l "ast.*matched_state\|matched_state.*ast" 2>/dev/null; then
            log_pass "matched_state integrated with AST structures"
        else
            log_warn "matched_state may not be properly integrated with AST"
        fi
    else
        log_warn "matched_state preservation not detected"
        log_info "This is required for state machine minimization optimization"
    fi
    
    return 0
}

#
# OBINexus Governance Validation
#

validate_zero_trust_governance() {
    log_test "Zero Trust Governance Markers"
    
    local governance_found=false
    
    # Check for Zero Trust governance markers in project files
    for file_path in Makefile README.md .riftrc CMakeLists.txt; do
        if [ -f "$file_path" ]; then
            if grep -q "Zero Trust\|nlink\|polybuild\|AEGIS\|governance" "$file_path" 2>/dev/null; then
                governance_found=true
                log_pass "Zero Trust governance markers found in $file_path"
            fi
        fi
    done
    
    if [ "$governance_found" = false ]; then
        log_warn "Zero Trust governance markers not prominently displayed"
        log_info "Consider adding governance documentation to README.md"
    fi
    
    return 0
}

validate_toolchain_integration() {
    log_test "OBINexus Toolchain Integration"
    
    local toolchain_documented=false
    
    # Check for toolchain integration documentation
    for file_path in Makefile README.md CMakeLists.txt docs/*.md; do
        if [ -f "$file_path" ]; then
            if grep -q "riftlang\.exe\|\.so\.a\|rift\.exe\|gosilang\|toolchain" "$file_path" 2>/dev/null; then
                toolchain_documented=true
                log_pass "Toolchain integration documented in $file_path"
            fi
        fi
    done
    
    if [ "$toolchain_documented" = false ]; then
        log_warn "Toolchain integration documentation incomplete"
        log_info "Document: riftlang.exe → .so.a → rift.exe → gosilang"
    fi
    
    return 0
}

#
# Compiler and Build System Validation
#

validate_compiler_compliance() {
    log_test "Strict Compiler Compliance"
    
    local compliance_enforced=false
    
    # Check for strict compiler flags in build systems
    for build_file in Makefile CMakeLists.txt */CMakeLists.txt; do
        if [ -f "$build_file" ]; then
            if grep -q "\-Werror.*\-Wall.*\-Wextra\|\-Wall.*\-Wextra.*\-Werror\|\-Wextra.*\-Wall.*\-Werror" "$build_file" || \
               grep -q "CFLAGS.*-Wall" "$build_file" && grep -q "CFLAGS.*-Wextra" "$build_file" && grep -q "CFLAGS.*-Werror" "$build_file"; then
                compliance_enforced=true
                log_pass "Strict compiler flags enforced in $build_file"
            fi
        fi
    done
    
    if [ "$compliance_enforced" = false ]; then
        log_fail "Missing strict compiler flags (-Wall -Wextra -Werror)"
        log_info "Required for AEGIS methodology compliance"
        return 1
    fi
    
    return 0
}

validate_build_system_integration() {
    log_test "Build System Architecture"
    
    # Check for proper build system structure
    if [ -f "CMakeLists.txt" ]; then
        log_pass "CMake build system detected"
        
        # Check for stage isolation
        if ls rift-*/CMakeLists.txt 2>/dev/null | wc -l | grep -q "[1-9]"; then
            log_pass "Pipeline stage isolation maintained"
        else
            log_warn "Pipeline stage isolation may be incomplete"
        fi
        
        # Check for common build configuration
        if [ -f "cmake/common/compiler_pipeline.cmake" ]; then
            log_pass "Common build orchestration found"
        else
            log_warn "Common build orchestration missing"
        fi
    elif [ -f "Makefile" ]; then
        log_pass "Make build system detected"
    else
        log_fail "No build system configuration found"
        return 1
    fi
    
    return 0
}

#
# RIFT-4 Specific Validation
#

validate_rift4_bytecode_integration() {
    log_test "RIFT-4 Bytecode Generation Integration"
    
    local rift4_found=false
    
    # Check for RIFT-4 implementation
    for rift4_path in rift-4/ src/core/bytecode/ include/rift/core/bytecode/; do
        if [ -d "$rift4_path" ]; then
            rift4_found=true
            log_pass "RIFT-4 bytecode generation structure found: $rift4_path"
            
            # Check for trust tagging implementation
            if find "$rift4_path" -name "*.h" -o -name "*.c" 2>/dev/null | xargs grep -l "trust.*tag\|SHA.*256\|AEGIS" 2>/dev/null; then
                log_pass "Trust tagging implementation detected"
            else
                log_warn "Trust tagging implementation may be incomplete"
            fi
            
            # Check for architecture targeting
            if find "$rift4_path" -name "*.h" -o -name "*.c" 2>/dev/null | xargs grep -l "amd_ryzen\|architecture\|target_arch" 2>/dev/null; then
                log_pass "Architecture targeting support detected"
            else
                log_warn "Architecture targeting may be incomplete"
            fi
            break
        fi
    done
    
    if [ "$rift4_found" = false ]; then
        log_fail "RIFT-4 bytecode generation implementation not found"
        return 1
    fi
    
    return 0
}

#
# Comprehensive Test Execution
#

execute_comprehensive_validation() {
    log_info "Executing comprehensive AEGIS methodology validation..."
    
    # Core AEGIS architecture validation
    validate_token_type_value_separation
    validate_aegis_automaton_implementation
    validate_dual_parsing_strategies
    validate_matched_state_preservation
    
    # OBINexus governance validation
    validate_zero_trust_governance
    validate_toolchain_integration
    
    # Compiler and build system validation
    validate_compiler_compliance
    validate_build_system_integration
    
    # RIFT-4 specific validation
    validate_rift4_bytecode_integration
}

#
# Integration Test Execution
#

execute_integration_tests() {
    log_test "Pipeline Integration Verification"
    
    # Test build system if available
    if [ -f "CMakeLists.txt" ]; then
        log_info "Testing CMake configuration..."
        if mkdir -p build_test && cd build_test && cmake .. >/dev/null 2>&1; then
            log_pass "CMake configuration valid"
            cd ..
            rm -rf build_test
        else
            log_fail "CMake configuration invalid"
            cd .. 2>/dev/null || true
            rm -rf build_test 2>/dev/null || true
            return 1
        fi
    fi
    
    # Test existing validation scripts integration
    local existing_scripts=0
    for script in scripts/validation/*.sh; do
        if [ -f "$script" ] && [ -x "$script" ]; then
            ((existing_scripts++))
            log_pass "Existing validation script: $(basename "$script")"
        fi
    done
    
    if [ $existing_scripts -gt 0 ]; then
        log_pass "Integration with existing validation framework verified"
    else
        log_warn "No existing validation scripts found for integration"
    fi
    
    return 0
}

#
# Main Execution and Reporting
#

generate_validation_report() {
    echo ""
    echo -e "${PURPLE}=================================================================${NC}"
    echo -e "${PURPLE}  AEGIS Methodology Validation Report${NC}"
    echo -e "${PURPLE}=================================================================${NC}"
    echo ""
    
    printf "Tests Executed: %d\n" "$TESTS_EXECUTED"
    printf "Validation Errors: %d\n" "$VALIDATION_ERRORS"
    printf "Warnings: %d\n" "$VALIDATION_WARNINGS"
    echo ""
    
    if [ $VALIDATION_ERRORS -eq 0 ]; then
        echo -e "${GREEN}🎉 AEGIS Methodology Validation: PASSED${NC}"
        echo -e "${GREEN}✅ All critical AEGIS requirements satisfied${NC}"
        echo ""
        echo "Status: Ready for production deployment"
        echo "Next Steps:"
        echo "  1. Execute build system: make all / cmake build"
        echo "  2. Run unit tests: make test"
        echo "  3. Execute integration tests"
        echo "  4. Deploy to staging environment"
        return 0
    else
        echo -e "${RED}❌ AEGIS Methodology Validation: FAILED${NC}"
        echo -e "${RED}Critical requirements not met: $VALIDATION_ERRORS errors${NC}"
        echo ""
        echo "Status: Requires architectural remediation"
        echo "Required Actions:"
        if grep -q "Token type/value separation violation" <<< "$(echo "$VALIDATION_ERRORS")"; then
            echo "  1. Implement proper token type/value separation"
        fi
        if grep -q "AEGIS automaton structure missing" <<< "$(echo "$VALIDATION_ERRORS")"; then
            echo "  2. Implement AEGIS automaton structure (5-tuple)"
        fi
        if grep -q "Dual parsing strategies missing" <<< "$(echo "$VALIDATION_ERRORS")"; then
            echo "  3. Implement dual parsing strategies (bottom-up/top-down)"
        fi
        if grep -q "Missing strict compiler flags" <<< "$(echo "$VALIDATION_ERRORS")"; then
            echo "  4. Enforce strict compiler flags (-Wall -Wextra -Werror)"
        fi
        echo "  5. Re-run validation after remediation"
        return 1
    fi
}

main() {
    banner
    
    log_info "AEGIS Methodology Validation Framework"
    log_info "Integration with existing validation infrastructure"
    log_info "Technical Lead: Nnamdi Okpala"
    echo ""
    
    # Execute validation phases
    execute_comprehensive_validation
    execute_integration_tests
    
    # Generate final report
    generate_validation_report
}

# Execute main validation
main "$@"
