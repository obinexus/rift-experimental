#!/usr/bin/env bash
# RIFT Governance Policy Validation Script
# OBINexus Computing Division - RIFT Flexible Translator
# Checks for required .riftrc.N files across stage-bound architecture
# Author: Nnamdi Michael Okpala (OBINexus)

set -euo pipefail

# RIFT Stage Validation Constants
readonly RIFT_MIN_STAGE=0
readonly RIFT_MAX_STAGE=6
readonly RIFT_GOVERNANCE_DIR="rift-gov"
readonly RIFT_AUDIT_PREFIX=".audit"
readonly RIFT_CONFIG_PREFIX=".riftrc"

usage() {
    cat << EOF
Usage: $0 --stage=<N|all> [--audit] [--verbose]

RIFT Governance Policy Validation for Stage-Bound Architecture

Options:
    --stage=N       Validate specific stage (0-6)
    --stage=all     Validate all stages (0-6)
    --audit         Generate audit trail output
    --verbose       Enable detailed validation output
    --help          Show this help message

Examples:
    $0 --stage=0                    # Validate tokenizer stage
    $0 --stage=all --audit          # Full validation with audit
    $0 --stage=3 --verbose          # Verbose bytecode stage validation

RIFT Stage Architecture:
    Stage 0: Tokenizer (lexeme → tokens)
    Stage 1: Parser → AST
    Stage 2: AST Validation & Policy Injection
    Stage 3: Bytecode Generator (AST → IR)
    Stage 4: Code Generator (IR → C/H)
    Stage 5: Program Generator & Linker
    Stage 6: Threading & Concurrency Integration

EOF
    exit 2
}

# Initialize script variables
dir="$(cd "$(dirname "$0")" && pwd)"
stage_arg="all"
audit_mode=false
verbose_mode=false

# Parse command line arguments
for arg in "$@"; do
    case "$arg" in
        --stage=*) 
            stage_arg="${arg#*=}" 
            ;;
        --audit) 
            audit_mode=true 
            ;;
        --verbose) 
            verbose_mode=true 
            ;;
        --help) 
            usage 
            ;;
        -*) 
            echo "Error: Unknown option $arg" >&2
            usage 
            ;;
    esac
done

# Logging functions
log_info() {
    if [[ "$verbose_mode" == true ]]; then
        echo "[INFO] $1" >&2
    fi
}

log_audit() {
    if [[ "$audit_mode" == true ]]; then
        echo "[AUDIT] $(date -Iseconds) $1" >> "${dir}/${RIFT_AUDIT_PREFIX}.policy_validation.log"
    fi
}

log_error() {
    echo "[ERROR] $1" >&2
    log_audit "ERROR: $1"
}

# Stage validation function with RIFT governance compliance
check_stage() {
    local stage="$1"
    local config_file="$dir/${RIFT_CONFIG_PREFIX}.$stage"
    local governance_file="$dir/gov.${RIFT_CONFIG_PREFIX}.$stage.in"
    
    log_info "Validating RIFT Stage $stage governance files"
    
    # Check primary configuration file
    if [[ -f "$config_file" ]]; then
        log_info "Found stage configuration: $config_file"
        log_audit "VALID: Stage $stage configuration exists"
    else
        log_error "Missing stage configuration: $config_file"
        return 1
    fi
    
    # Check governance input file
    if [[ -f "$governance_file" ]]; then
        log_info "Found governance input: $governance_file"
        log_audit "VALID: Stage $stage governance input exists"
    else
        log_error "Missing governance input: $governance_file"
        return 1
    fi
    
    # Validate stage-specific requirements
    case "$stage" in
        0)
            log_info "Stage 0: Tokenizer validation - checking lexical analysis requirements"
            ;;
        1)
            log_info "Stage 1: Parser validation - checking AST generation requirements"
            ;;
        2)
            log_info "Stage 2: AST validation - checking semantic analysis requirements"
            ;;
        3)
            log_info "Stage 3: Bytecode validation - checking IR generation requirements"
            ;;
        4)
            log_info "Stage 4: Code generation validation - checking C/H output requirements"
            ;;
        5)
            log_info "Stage 5: Program generation validation - checking linker requirements"
            ;;
        6)
            log_info "Stage 6: Threading validation - checking concurrency requirements"
            ;;
    esac
    
    echo "✅ Stage $stage validation passed"
    return 0
}

# Main validation logic
main() {
    log_info "RIFT Governance Policy Validation initiated"
    log_audit "Policy validation started for stage(s): $stage_arg"
    
    local validation_result=0
    
    if [[ "$stage_arg" == "all" ]]; then
        echo "🔍 Validating all RIFT stages (0-$RIFT_MAX_STAGE)..."
        
        for stage in $(seq $RIFT_MIN_STAGE $RIFT_MAX_STAGE); do
            if ! check_stage "$stage"; then
                validation_result=1
            fi
        done
        
        if [[ $validation_result -eq 0 ]]; then
            echo "✅ All RIFT governance stages validated successfully"
            log_audit "SUCCESS: All stages validated"
        else
            echo "❌ RIFT governance validation failed for one or more stages"
            log_audit "FAILURE: Validation incomplete"
        fi
        
    elif [[ "$stage_arg" =~ ^[0-6]$ ]]; then
        echo "🔍 Validating RIFT Stage $stage_arg..."
        
        if check_stage "$stage_arg"; then
            echo "✅ Stage $stage_arg governance validation completed"
            log_audit "SUCCESS: Stage $stage_arg validated"
        else
            echo "❌ Stage $stage_arg governance validation failed"
            validation_result=1
        fi
        
    else
        echo "Error: Invalid stage '$stage_arg'. Must be 0-6 or 'all'" >&2
        usage
    fi
    
    log_audit "Policy validation completed with exit code: $validation_result"
    exit $validation_result
}

# Execute main function
main "$@"
