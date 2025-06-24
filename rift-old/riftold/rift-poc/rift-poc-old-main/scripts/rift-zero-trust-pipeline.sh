#!/bin/bash

#==============================================================================
# Rift Zero Trust Compilation Pipeline
# 
# Enforces cryptographic verification at every stage transition
# Implements Residual Trust Principle with fail-fast governance
# Stages: .rift → .rift.0 → .rift.1 → .rift.3 → .rift.4 → .rift.5
#==============================================================================

set -euo pipefail

# Configuration
readonly SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
readonly PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
readonly KEYS_DIR="${PROJECT_ROOT}/keys"
readonly PRIVATE_KEY="${KEYS_DIR}/rift_signing_key.pem"
readonly PUBLIC_KEY="${KEYS_DIR}/rift_signing_pub.pem"
readonly BUILD_DIR="${PROJECT_ROOT}/build"

# Colors
readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly YELLOW='\033[1;33m'
readonly BLUE='\033[0;34m'
readonly ORANGE='\033[0;33m'
readonly NC='\033[0m'

# Logging
log_trust() {
    echo -e "${ORANGE}[TRUST]${NC} $(date '+%Y-%m-%d %H:%M:%S') - $*"
}

log_stage() {
    echo -e "${BLUE}[STAGE]${NC} $(date '+%Y-%m-%d %H:%M:%S') - $*"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $(date '+%Y-%m-%d %H:%M:%S') - $*"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $(date '+%Y-%m-%d %H:%M:%S') - $*"
}

# Cryptographic functions
sign_artifact() {
    local artifact="$1"
    log_trust "Signing artifact: $(basename "$artifact")"
    if ! openssl dgst -sha256 -sign "$PRIVATE_KEY" -out "$artifact.sig" "$artifact" 2>/dev/null; then
        log_error "Failed to sign artifact: $artifact"
        exit 1
    fi
    log_trust "Signed: $(basename "$artifact").sig"
}

verify_artifact() {
    local artifact="$1"
    log_trust "Verifying artifact: $(basename "$artifact")"
    
    if [[ ! -f "$artifact" ]]; then
        log_error "Artifact not found: $artifact"
        exit 1
    fi
    
    if [[ ! -f "$artifact.sig" ]]; then
        log_error "Signature not found for: $artifact"
        exit 1
    fi
    
    if openssl dgst -sha256 -verify "$PUBLIC_KEY" -signature "$artifact.sig" "$artifact" 2>/dev/null; then
        log_trust "Verification PASSED: $(basename "$artifact")"
    else
        log_error "Verification FAILED: $(basename "$artifact")"
        log_error "ZERO TRUST VIOLATION - Aborting pipeline"
        exit 1
    fi
}

# Residual Trust enforcement
check_residual_trust() {
    local stage_file="$1"
    local stage_name="$2"
    
    log_trust "Residual Trust check for: $stage_name"
    
    if [[ ! -f "$stage_file.sig" ]]; then
        log_error "Missing signature for $stage_name. Refusing to proceed."
        exit 1
    fi
    
    verify_artifact "$stage_file"
    log_trust "Residual Trust PASSED: $stage_name"
}

# Main pipeline execution
main() {
    if [[ $# -ne 1 ]]; then
        echo "Usage: $0 <input.rift>"
        exit 1
    fi
    
    local RIFT_INPUT="$1"
    local BASENAME=$(basename "$RIFT_INPUT" .rift)
    
    # Verify input file exists
    if [[ ! -f "$RIFT_INPUT" ]]; then
        log_error "Input file not found: $RIFT_INPUT"
        exit 1
    fi
    
    # Verify keys exist
    if [[ ! -f "$PRIVATE_KEY" ]] || [[ ! -f "$PUBLIC_KEY" ]]; then
        log_error "Cryptographic keys not found. Run orchestration script first."
        exit 1
    fi
    
    log_stage "Starting Zero Trust Rift Pipeline for: $RIFT_INPUT"
    
    # Create build directory
    mkdir -p "$BUILD_DIR"
    
    # Stage 0: Token + Type Analysis
    log_stage ">>> Stage 0: Token + Type Analysis"
    if ! "${BUILD_DIR}/rift.exe" "$RIFT_INPUT" --stage 0 --output "${BUILD_DIR}/${BASENAME}.rift.0"; then
        log_error "Stage 0 compilation failed"
        exit 1
    fi
    sign_artifact "${BUILD_DIR}/${BASENAME}.rift.0"
    
    # Stage 1: Parser
    check_residual_trust "${BUILD_DIR}/${BASENAME}.rift.0" "Stage 0"
    log_stage ">>> Stage 1: Parser Stage"
    if ! "${BUILD_DIR}/rift.exe" "$RIFT_INPUT" --stage 1 --output "${BUILD_DIR}/${BASENAME}.rift.1"; then
        log_error "Stage 1 compilation failed"
        exit 1
    fi
    sign_artifact "${BUILD_DIR}/${BASENAME}.rift.1"
    
    # Stage 3: AST Generation
    check_residual_trust "${BUILD_DIR}/${BASENAME}.rift.1" "Stage 1"
    log_stage ">>> Stage 3: AST Generation"
    if ! "${BUILD_DIR}/rift.exe" "$RIFT_INPUT" --stage 3 --output "${BUILD_DIR}/${BASENAME}.rift.3"; then
        log_error "Stage 3 compilation failed"
        exit 1
    fi
    sign_artifact "${BUILD_DIR}/${BASENAME}.rift.3"
    
    # Stage 4: Bytecode Generation (HIGH SECURITY)
    check_residual_trust "${BUILD_DIR}/${BASENAME}.rift.3" "Stage 3"
    log_stage ">>> Stage 4: Bytecode Generation (HIGH SECURITY)"
    if ! "${BUILD_DIR}/rift.exe" "$RIFT_INPUT" --stage 4 --output "${BUILD_DIR}/${BASENAME}.rift.4"; then
        log_error "Stage 4 compilation failed"
        exit 1
    fi
    sign_artifact "${BUILD_DIR}/${BASENAME}.rift.4"
    
    # Stage 5: Program Generation (HIGHEST SECURITY)
    check_residual_trust "${BUILD_DIR}/${BASENAME}.rift.4" "Stage 4"
    log_stage ">>> Stage 5: Program Generation (HIGHEST SECURITY)"
    if ! "${BUILD_DIR}/rift.exe" "$RIFT_INPUT" --stage 5 --output "${BUILD_DIR}/${BASENAME}.rift.5"; then
        log_error "Stage 5 compilation failed"
        exit 1
    fi
    sign_artifact "${BUILD_DIR}/${BASENAME}.rift.5"
    
    # Final verification
    check_residual_trust "${BUILD_DIR}/${BASENAME}.rift.5" "Stage 5"
    
    log_success "Zero Trust Rift Pipeline completed successfully for: $RIFT_INPUT"
    log_success "All artifacts cryptographically signed and verified"
    log_success "Output: ${BUILD_DIR}/${BASENAME}.rift.5"
}

main "$@"
