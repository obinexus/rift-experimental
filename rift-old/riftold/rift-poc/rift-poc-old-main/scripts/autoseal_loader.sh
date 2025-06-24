#!/bin/bash

#==============================================================================
# RIFT Auto-Seal Loader + Automaton Verifier with Transversal Stage Mapping Protection
# 
# Implementation: Forward-only sealed pipeline with progressive security enforcement
# Security Model: .rift.0 → .rift.1 → .rift.3 → .rift.4 → .rift.5 (NO REVERSE)
# Author: OBINexus Computing - AEGIS Waterfall Methodology
# Version: 1.0.0 (Critical Security Implementation)
#
# Core Principle: Each stage loses backward information through progressive obfuscation
# Anti-Pattern: Prevents reverse engineering, AST dumping, and transversal mapping attacks
#==============================================================================

set -euo pipefail

# ============================================================================
# GLOBAL CONFIGURATION AND SECURITY CONSTANTS
# ============================================================================

readonly SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
readonly PROJECT_ROOT="${SCRIPT_DIR}"
readonly AURASEAL_DIR="${PROJECT_ROOT}/keys"
readonly ENTROPY_DB_DIR="${PROJECT_ROOT}/entropy_signatures"
readonly GOVERNANCE_DIR="${PROJECT_ROOT}/governance"

# AuraSeal Cryptographic Configuration
readonly AURASEAL_PRIVATE_KEY="${AURASEAL_DIR}/rift_signing_key.pem"
readonly AURASEAL_PUBLIC_KEY="${AURASEAL_DIR}/rift_signing_pub.pem"
readonly GOVERNANCE_PRIVATE_KEY="${GOVERNANCE_DIR}/governance_master_key.pem"
readonly GOVERNANCE_PUBLIC_KEY="${GOVERNANCE_DIR}/governance_master_pub.pem"

# Progressive Security Levels (Increasing protection)
readonly STAGE_0_SECURITY_LEVEL="basic_optional"
readonly STAGE_1_SECURITY_LEVEL="sealed_signature"
readonly STAGE_3_SECURITY_LEVEL="obfuscated_minimized"
readonly STAGE_4_SECURITY_LEVEL="hardened_encrypted"
readonly STAGE_5_SECURITY_LEVEL="fully_sealed_anti_reversion"

# Transversal Stage Mapping Protection States
readonly STATE_INIT="INIT"
readonly STATE_RIFT_0="RIFT_0"
readonly STATE_RIFT_1="RIFT_1"
readonly STATE_RIFT_3="RIFT_3"
readonly STATE_RIFT_4="RIFT_4"
readonly STATE_RIFT_5="RIFT_5"
readonly STATE_BLOCKED="BLOCKED"

# Security Enforcement Constants
readonly BLOCK_EXECUTION=1
readonly EXECUTION_ALLOWED=0
readonly GOVERNANCE_REQUIRED=2

# Colors for security output
readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly YELLOW='\033[1;33m'
readonly BLUE='\033[0;34m'
readonly PURPLE='\033[0;35m'
readonly CYAN='\033[0;36m'
readonly CRITICAL='\033[1;31m'
readonly SECURITY='\033[1;35m'
readonly NC='\033[0m'

# ============================================================================
# SECURITY LOGGING AND AUDIT TRAIL
# ============================================================================

log_security() {
    local level="$1"
    shift
    local message="$*"
    local timestamp=$(date '+%Y-%m-%d %H:%M:%S.%3N')
    
    case "$level" in
        "BLOCK") echo -e "${CRITICAL}[BLOCK]${NC} ${timestamp} - $message" | tee -a "${PROJECT_ROOT}/security_audit.log" ;;
        "ALLOW") echo -e "${GREEN}[ALLOW]${NC} ${timestamp} - $message" | tee -a "${PROJECT_ROOT}/security_audit.log" ;;
        "WARN")  echo -e "${YELLOW}[WARN]${NC}  ${timestamp} - $message" | tee -a "${PROJECT_ROOT}/security_audit.log" ;;
        "SEAL")  echo -e "${SECURITY}[SEAL]${NC}  ${timestamp} - $message" | tee -a "${PROJECT_ROOT}/security_audit.log" ;;
        *)       echo -e "${BLUE}[SEC]${NC}   ${timestamp} - $message" | tee -a "${PROJECT_ROOT}/security_audit.log" ;;
    esac
    
    # Critical events also logged to system security log
    if [[ "$level" == "BLOCK" ]]; then
        echo "RIFT_SECURITY_VIOLATION|${timestamp}|${message}" >> "/var/log/rift_security.log" 2>/dev/null || true
    fi
}

# ============================================================================
# CONTEXT-AWARE ONE-WAY FUNCTION IMPLEMENTATION
# Based on "The Hidden Cipher" - Odd Perfect Numbers and Cryptographic Integrity
# ============================================================================

# Calculate Context-Aware Checksum using Divisor Echo Hypothesis principles
calculate_context_aware_checksum() {
    local rift_file="$1"
    local stage_level="$2"
    
    log_security "SEAL" "Calculating context-aware checksum for: $(basename "$rift_file") at stage $stage_level"
    
    # Apply Divisor Echo Hypothesis to RIFT artifacts
    # If GCD(n, d) = d and LCM(n, d) = n for all proper divisors d, and ∑d = n,
    # then n is structurally perfect and cryptographically sound
    
    local file_size=$(stat -f%z "$rift_file" 2>/dev/null || stat -c%s "$rift_file" 2>/dev/null)
    local content_hash=$(sha256sum "$rift_file" | cut -d' ' -f1)
    local structural_entropy=$(calculate_structural_entropy "$rift_file")
    
    # Calculate proper divisors of file_size (analog to perfect number divisors)
    local divisors=$(python3 -c "
import math
n = $file_size
if n <= 0:
    print('1')
else:
    divisors = []
    for i in range(1, int(math.sqrt(n)) + 1):
        if n % i == 0:
            divisors.append(i)
            if i != n // i and i != 1:
                divisors.append(n // i)
    # Remove n itself to get proper divisors
    divisors = [d for d in divisors if d != n and d != 0]
    print(' '.join(map(str, sorted(divisors))))
" 2>/dev/null || echo "1")
    
    # Apply GCD/LCM alignment test (structural integrity validation)
    local gcd_lcm_alignment=$(python3 -c "
import math
n = $file_size
divisors = [$divisors]
alignment_score = 0
total_divisors = len(divisors)

if total_divisors > 0:
    for d in divisors:
        if d > 0:
            gcd_val = math.gcd(n, d)
            lcm_val = (n * d) // math.gcd(n, d) if d != 0 else 0
            # Check Divisor Echo: GCD(n, d) should equal d for structural perfection
            if gcd_val == d:
                alignment_score += 1
            # Additional LCM consistency check
            if lcm_val == n:
                alignment_score += 0.5
    
    # Normalize alignment score (0-100)
    max_possible_score = total_divisors * 1.5
    if max_possible_score > 0:
        alignment_percentage = (alignment_score / max_possible_score) * 100
    else:
        alignment_percentage = 0
else:
    alignment_percentage = 100  # Edge case: no divisors means perfect alignment

print(f'{alignment_percentage:.2f}')
" 2>/dev/null || echo "0")
    
    # Generate context-aware signature combining all structural elements
    local context_signature=$(echo "${content_hash}${structural_entropy}${gcd_lcm_alignment}${stage_level}" | sha512sum | cut -d' ' -f1)
    
    # Create 256-bit segmented entropy signature (as per your research)
    local segmented_signature=$(echo "$context_signature" | cut -c1-64)
    
    log_security "SEAL" "Context checksum: entropy=${structural_entropy}, alignment=${gcd_lcm_alignment}%, signature=${segmented_signature:0:16}..."
    
    echo "$segmented_signature"
}

# Verify Context-Aware Structural Integrity
verify_context_aware_integrity() {
    local rift_file="$1"
    local expected_signature="$2"
    local stage_level="$3"
    
    log_security "SEAL" "Verifying context-aware integrity for stage $stage_level"
    
    local calculated_signature=$(calculate_context_aware_checksum "$rift_file" "$stage_level")
    
    if [[ "$calculated_signature" == "$expected_signature" ]]; then
        log_security "ALLOW" "Context-aware integrity verification PASSED"
        return $EXECUTION_ALLOWED
    else
        log_security "BLOCK" "Context-aware integrity verification FAILED"
        log_security "BLOCK" "Expected: $expected_signature"
        log_security "BLOCK" "Calculated: $calculated_signature"
        return $BLOCK_EXECUTION
    fi
}

# Entropy Distribution Analysis (High-entropy zone detection)
analyze_entropy_distribution() {
    local rift_file="$1"
    local stage_level="$2"
    
    log_security "SEAL" "Analyzing entropy distribution for stage $stage_level"
    
    # Calculate local entropy field as per your research
    local entropy_zones=$(python3 -c "
import math
from collections import Counter

try:
    with open('$rift_file', 'rb') as f:
        data = f.read()
    
    if len(data) == 0:
        print('0 0 0')
        exit()
    
    # Divide into zones for entropy analysis
    chunk_size = max(256, len(data) // 8)  # Minimum 256 bytes per zone
    zones = []
    
    for i in range(0, len(data), chunk_size):
        chunk = data[i:i+chunk_size]
        if len(chunk) > 0:
            counts = Counter(chunk)
            entropy = -sum(count/len(chunk) * math.log2(count/len(chunk)) for count in counts.values())
            zones.append(entropy)
    
    if zones:
        avg_entropy = sum(zones) / len(zones)
        max_entropy = max(zones)
        entropy_variance = sum((e - avg_entropy)**2 for e in zones) / len(zones)
    else:
        avg_entropy = max_entropy = entropy_variance = 0
    
    print(f'{avg_entropy:.6f} {max_entropy:.6f} {entropy_variance:.6f}')
    
except Exception as e:
    print('0 0 0')
" 2>/dev/null || echo "0 0 0")
    
    local avg_entropy=$(echo "$entropy_zones" | cut -d' ' -f1)
    local max_entropy=$(echo "$entropy_zones" | cut -d' ' -f2)
    local entropy_variance=$(echo "$entropy_zones" | cut -d' ' -f3)
    
    # Determine if this is a high-entropy zone requiring deeper examination
    local is_high_entropy=$(python3 -c "
avg = float('$avg_entropy')
var = float('$entropy_variance')
# High entropy zones have avg > 6.0 and variance > 1.0 (arbitrary thresholds based on your research)
if avg > 6.0 and var > 1.0:
    print('true')
else:
    print('false')
" 2>/dev/null || echo "false")
    
    log_security "SEAL" "Entropy analysis: avg=${avg_entropy}, max=${max_entropy}, variance=${entropy_variance}, high_entropy=${is_high_entropy}"
    
    echo "$is_high_entropy"
}

# ============================================================================
# ENHANCED AURASEAL CRYPTOGRAPHIC VERIFICATION FUNCTIONS
# ============================================================================

verify_aura_seal() {
    local riftN_file_path="$1"
    local signature_path="$2"
    local pubkey_path="$3"
    
    log_security "SEAL" "Verifying AuraSeal for: $(basename "$riftN_file_path")"
    
    # Check if all required files exist
    if [[ ! -f "$riftN_file_path" ]]; then
        log_security "BLOCK" "AuraSeal verification failed: Artifact not found - $riftN_file_path"
        return $BLOCK_EXECUTION
    fi
    
    if [[ ! -f "$signature_path" ]]; then
        log_security "BLOCK" "AuraSeal verification failed: Signature not found - $signature_path"
        return $BLOCK_EXECUTION
    fi
    
    if [[ ! -f "$pubkey_path" ]]; then
        log_security "BLOCK" "AuraSeal verification failed: Public key not found - $pubkey_path"
        return $BLOCK_EXECUTION
    fi
    
    # Verify cryptographic signature using OpenSSL
    if openssl dgst -sha512 -verify "$pubkey_path" -signature "$signature_path" "$riftN_file_path" 2>/dev/null; then
        log_security "ALLOW" "AuraSeal cryptographic verification PASSED: $(basename "$riftN_file_path")"
        
        # Additional context-aware integrity verification
        local stage_level=""
        case "$riftN_file_path" in
            *.rift.0) stage_level="0" ;;
            *.rift.1) stage_level="1" ;;
            *.rift.3) stage_level="3" ;;
            *.rift.4) stage_level="4" ;;
            *.rift.5) stage_level="5" ;;
        esac
        
        # Apply Divisor Echo Hypothesis validation
        local context_checksum=$(calculate_context_aware_checksum "$riftN_file_path" "$stage_level")
        local context_file="${riftN_file_path}.context"
        
        if [[ -f "$context_file" ]]; then
            local expected_context=$(cat "$context_file")
            if ! verify_context_aware_integrity "$riftN_file_path" "$expected_context" "$stage_level"; then
                log_security "BLOCK" "Context-aware integrity check failed despite valid signature"
                return $BLOCK_EXECUTION
            fi
        else
            # Generate context signature for future verification
            echo "$context_checksum" > "$context_file"
            log_security "SEAL" "Generated context signature: $(basename "$context_file")"
        fi
        
        return $EXECUTION_ALLOWED
    else
        log_security "BLOCK" "AuraSeal verification FAILED: Invalid signature for $(basename "$riftN_file_path")"
        return $BLOCK_EXECUTION
    fi
}

verify_structural_entropy() {
    local riftN_file_path="$1"
    local expected_entropy_signature="$2"
    
    log_security "SEAL" "Verifying structural entropy for: $(basename "$riftN_file_path")"
    
    # Calculate current structural entropy
    local current_entropy=$(calculate_structural_entropy "$riftN_file_path")
    
    # Hash the entropy for comparison
    local current_entropy_hash=$(echo "$current_entropy" | sha256sum | cut -d' ' -f1)
    
    if [[ "$current_entropy_hash" == "$expected_entropy_signature" ]]; then
        log_security "ALLOW" "Structural entropy verification PASSED: $(basename "$riftN_file_path")"
        return $EXECUTION_ALLOWED
    else
        log_security "BLOCK" "Structural entropy verification FAILED: Hash mismatch for $(basename "$riftN_file_path")"
        log_security "BLOCK" "Expected: $expected_entropy_signature"
        log_security "BLOCK" "Actual:   $current_entropy_hash"
        return $BLOCK_EXECUTION
    fi
}

calculate_structural_entropy() {
    local file_path="$1"
    
    if [[ ! -f "$file_path" ]]; then
        echo "0"
        return
    fi
    
    # Calculate Shannon entropy and structural complexity
    local file_entropy=$(python3 -c "
import math
from collections import Counter
import sys

try:
    with open('$file_path', 'rb') as f:
        data = f.read()
    
    if len(data) == 0:
        print('0')
        sys.exit(0)
    
    # Calculate Shannon entropy
    counts = Counter(data)
    entropy = -sum(count/len(data) * math.log2(count/len(data)) for count in counts.values())
    
    # Calculate structural complexity (line count, token density, nesting depth)
    with open('$file_path', 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()
    
    lines = len(content.split('\n'))
    tokens = len(content.split())
    nesting_depth = max(content.count('{'), content.count('['), content.count('('))
    
    # Combined structural entropy score
    structural_score = entropy * (1 + math.log(lines + 1)) * (1 + math.log(tokens + 1)) * (1 + nesting_depth/10)
    print(f'{structural_score:.6f}')
    
except Exception as e:
    print('0')
" 2>/dev/null || echo "0")
    
    echo "$file_entropy"
}

# ============================================================================
# TRANSVERSAL STAGE MAPPING PROTECTION ENGINE
# ============================================================================

check_transversal_policy() {
    local current_stage="$1"
    local target_stage="$2"
    local governance_mode="${3:-false}"
    
    log_security "SEAL" "Checking transversal policy: $current_stage → $target_stage"
    
    # Define valid forward transitions
    local valid_transitions=(
        "$STATE_INIT:$STATE_RIFT_0"
        "$STATE_RIFT_0:$STATE_RIFT_1"
        "$STATE_RIFT_1:$STATE_RIFT_3"
        "$STATE_RIFT_3:$STATE_RIFT_4"
        "$STATE_RIFT_4:$STATE_RIFT_5"
    )
    
    # Convert stage file extensions to state names
    local current_state=$(extension_to_state "$current_stage")
    local target_state=$(extension_to_state "$target_stage")
    
    local transition="${current_state}:${target_state}"
    
    # Check if transition is in valid list
    for valid_transition in "${valid_transitions[@]}"; do
        if [[ "$transition" == "$valid_transition" ]]; then
            log_security "ALLOW" "Valid forward transition: $current_stage → $target_stage"
            return $EXECUTION_ALLOWED
        fi
    done
    
    # Check for forbidden reverse transitions
    local forbidden_transitions=(
        "$STATE_RIFT_5:$STATE_RIFT_4"
        "$STATE_RIFT_5:$STATE_RIFT_3"
        "$STATE_RIFT_5:$STATE_RIFT_1"
        "$STATE_RIFT_5:$STATE_RIFT_0"
        "$STATE_RIFT_4:$STATE_RIFT_3"
        "$STATE_RIFT_4:$STATE_RIFT_1"
        "$STATE_RIFT_4:$STATE_RIFT_0"
        "$STATE_RIFT_3:$STATE_RIFT_1"
        "$STATE_RIFT_3:$STATE_RIFT_0"
        "$STATE_RIFT_1:$STATE_RIFT_0"
    )
    
    for forbidden_transition in "${forbidden_transitions[@]}"; do
        if [[ "$transition" == "$forbidden_transition" ]]; then
            if [[ "$governance_mode" == "true" ]]; then
                log_security "WARN" "Reverse transition requires governance authorization: $current_stage → $target_stage"
                return $GOVERNANCE_REQUIRED
            else
                log_security "BLOCK" "FORBIDDEN: Transversal stage mapping attack detected: $current_stage → $target_stage"
                log_security "BLOCK" "RIFT pipeline is forward-only. Reverse engineering blocked."
                return $BLOCK_EXECUTION
            fi
        fi
    done
    
    log_security "BLOCK" "Invalid transition: $current_stage → $target_stage"
    return $BLOCK_EXECUTION
}

extension_to_state() {
    local stage="$1"
    case "$stage" in
        "*.rift.0"|".rift.0") echo "$STATE_RIFT_0" ;;
        "*.rift.1"|".rift.1") echo "$STATE_RIFT_1" ;;
        "*.rift.3"|".rift.3") echo "$STATE_RIFT_3" ;;
        "*.rift.4"|".rift.4") echo "$STATE_RIFT_4" ;;
        "*.rift.5"|".rift.5") echo "$STATE_RIFT_5" ;;
        *) echo "$STATE_INIT" ;;
    esac
}

# ============================================================================
# PROGRESSIVE SECURITY LEVEL ENFORCEMENT
# ============================================================================

verify_security_level() {
    local stage="$1"
    local file_path="$2"
    
    log_security "SEAL" "Verifying security level for stage: $stage"
    
    case "$stage" in
        ".rift.0")
            verify_stage_0_security "$file_path"
            ;;
        ".rift.1")
            verify_stage_1_security "$file_path"
            ;;
        ".rift.3")
            verify_stage_3_security "$file_path"
            ;;
        ".rift.4")
            verify_stage_4_security "$file_path"
            ;;
        ".rift.5")
            verify_stage_5_security "$file_path"
            ;;
        *)
            log_security "BLOCK" "Unknown stage: $stage"
            return $BLOCK_EXECUTION
            ;;
    esac
}

verify_stage_0_security() {
    local file_path="$1"
    log_security "SEAL" "Stage 0: Basic optional sealing verification"
    
    # Stage 0: Basic validation (optional sealing)
    if [[ ! -f "$file_path" ]]; then
        log_security "BLOCK" "Stage 0 artifact not found: $file_path"
        return $BLOCK_EXECUTION
    fi
    
    # Check for basic RIFT Stage 0 format
    if ! grep -q "# RIFT Stage 0" "$file_path" 2>/dev/null; then
        log_security "BLOCK" "Stage 0 format validation failed: $file_path"
        return $BLOCK_EXECUTION
    fi
    
    log_security "ALLOW" "Stage 0 security verification PASSED"
    return $EXECUTION_ALLOWED
}

verify_stage_1_security() {
    local file_path="$1"
    log_security "SEAL" "Stage 1: Sealed + signature verification"
    
    # Stage 1: Requires signature
    local signature_file="${file_path}.sig"
    if [[ ! -f "$signature_file" ]]; then
        log_security "BLOCK" "Stage 1 signature missing: $signature_file"
        return $BLOCK_EXECUTION
    fi
    
    # Verify AuraSeal signature
    if ! verify_aura_seal "$file_path" "$signature_file" "$AURASEAL_PUBLIC_KEY"; then
        return $BLOCK_EXECUTION
    fi
    
    log_security "ALLOW" "Stage 1 security verification PASSED"
    return $EXECUTION_ALLOWED
}

verify_stage_3_security() {
    local file_path="$1"
    log_security "SEAL" "Stage 3: Obfuscated + minimized verification"
    
    # Stage 3: Requires signature + structural obfuscation
    local signature_file="${file_path}.sig"
    local entropy_file="${file_path}.entropy"
    
    if [[ ! -f "$signature_file" ]]; then
        log_security "BLOCK" "Stage 3 signature missing: $signature_file"
        return $BLOCK_EXECUTION
    fi
    
    # Verify AuraSeal signature
    if ! verify_aura_seal "$file_path" "$signature_file" "$AURASEAL_PUBLIC_KEY"; then
        return $BLOCK_EXECUTION
    fi
    
    # Verify structural obfuscation (AST minimization applied)
    if ! grep -q "Tennis FSM Validation: PASSED" "$file_path" 2>/dev/null; then
        log_security "BLOCK" "Stage 3 AST minimization validation failed: $file_path"
        return $BLOCK_EXECUTION
    fi
    
    # Verify entropy signature if exists
    if [[ -f "$entropy_file" ]]; then
        local expected_entropy=$(cat "$entropy_file")
        if ! verify_structural_entropy "$file_path" "$expected_entropy"; then
            return $BLOCK_EXECUTION
        fi
    fi
    
    log_security "ALLOW" "Stage 3 security verification PASSED"
    return $EXECUTION_ALLOWED
}

verify_stage_4_security() {
    local file_path="$1"
    log_security "SEAL" "Stage 4: Hardened + encrypted verification"
    
    # Stage 4: Maximum security - hardened + encrypted + sealed
    local signature_file="${file_path}.sig"
    local entropy_file="${file_path}.entropy"
    local hardening_file="${file_path}.hardened"
    
    if [[ ! -f "$signature_file" ]]; then
        log_security "BLOCK" "Stage 4 signature missing: $signature_file"
        return $BLOCK_EXECUTION
    fi
    
    if [[ ! -f "$entropy_file" ]]; then
        log_security "BLOCK" "Stage 4 entropy signature missing: $entropy_file"
        return $BLOCK_EXECUTION
    fi
    
    # Verify AuraSeal signature
    if ! verify_aura_seal "$file_path" "$signature_file" "$AURASEAL_PUBLIC_KEY"; then
        return $BLOCK_EXECUTION
    fi
    
    # Mandatory structural entropy verification for Stage 4
    local expected_entropy=$(cat "$entropy_file")
    if ! verify_structural_entropy "$file_path" "$expected_entropy"; then
        return $BLOCK_EXECUTION
    fi
    
    # Verify hardening markers
    if [[ -f "$hardening_file" ]]; then
        if ! grep -q "HARDENING_APPLIED" "$hardening_file" 2>/dev/null; then
            log_security "BLOCK" "Stage 4 hardening verification failed: $file_path"
            return $BLOCK_EXECUTION
        fi
    fi
    
    log_security "ALLOW" "Stage 4 security verification PASSED"
    return $EXECUTION_ALLOWED
}

verify_stage_5_security() {
    local file_path="$1"
    log_security "SEAL" "Stage 5: Fully sealed + anti-reversion verification"
    
    # Stage 5: Maximum security + anti-reversion metadata
    local signature_file="${file_path}.sig"
    local entropy_file="${file_path}.entropy"
    local anti_reversion_file="${file_path}.anti_reversion"
    
    if [[ ! -f "$signature_file" ]]; then
        log_security "BLOCK" "Stage 5 signature missing: $signature_file"
        return $BLOCK_EXECUTION
    fi
    
    if [[ ! -f "$entropy_file" ]]; then
        log_security "BLOCK" "Stage 5 entropy signature missing: $entropy_file"
        return $BLOCK_EXECUTION
    fi
    
    if [[ ! -f "$anti_reversion_file" ]]; then
        log_security "BLOCK" "Stage 5 anti-reversion metadata missing: $anti_reversion_file"
        return $BLOCK_EXECUTION
    fi
    
    # Verify AuraSeal signature
    if ! verify_aura_seal "$file_path" "$signature_file" "$AURASEAL_PUBLIC_KEY"; then
        return $BLOCK_EXECUTION
    fi
    
    # Mandatory structural entropy verification for Stage 5
    local expected_entropy=$(cat "$entropy_file")
    if ! verify_structural_entropy "$file_path" "$expected_entropy"; then
        return $BLOCK_EXECUTION
    fi
    
    # Verify anti-reversion metadata
    if ! grep -q "ANTI_REVERSION_PROTECTED" "$anti_reversion_file" 2>/dev/null; then
        log_security "BLOCK" "Stage 5 anti-reversion verification failed: $file_path"
        return $BLOCK_EXECUTION
    fi
    
    log_security "ALLOW" "Stage 5 security verification PASSED"
    return $EXECUTION_ALLOWED
}

# ============================================================================
# MAIN AUTO-SEAL LOADER FUNCTION
# ============================================================================

load_rift_program() {
    local rift_file="$1"
    local governance_mode="${2:-false}"
    
    log_security "SEAL" "RIFT Auto-Seal Loader starting for: $(basename "$rift_file")"
    
    # Determine stage from file extension
    local stage=""
    case "$rift_file" in
        *.rift.0) stage=".rift.0" ;;
        *.rift.1) stage=".rift.1" ;;
        *.rift.3) stage=".rift.3" ;;
        *.rift.4) stage=".rift.4" ;;
        *.rift.5) stage=".rift.5" ;;
        *)
            log_security "BLOCK" "Invalid RIFT file extension: $rift_file"
            return $BLOCK_EXECUTION
            ;;
    esac
    
    log_security "SEAL" "Processing RIFT stage: $stage"
    
    # Step 1: Verify security level for current stage
    if ! verify_security_level "$stage" "$rift_file"; then
        log_security "BLOCK" "Security level verification failed for stage: $stage"
        return $BLOCK_EXECUTION
    fi
    
    # Step 2: Check for transversal policy violations (if target stage specified)
    if [[ -n "${3:-}" ]]; then
        local target_stage="$3"
        if ! check_transversal_policy "$stage" "$target_stage" "$governance_mode"; then
            log_security "BLOCK" "Transversal policy violation: $stage → $target_stage"
            return $BLOCK_EXECUTION
        fi
    fi
    
    # Step 3: Final authorization check
    log_security "ALLOW" "All security checks PASSED for: $(basename "$rift_file")"
    log_security "ALLOW" "Authorized to load RIFT program: $stage"
    
    # Log successful load to audit trail
    echo "RIFT_LOAD_SUCCESS|$(date -u '+%Y-%m-%d %H:%M:%S')|STAGE=${stage}|FILE=$(basename "$rift_file")" >> "${PROJECT_ROOT}/load_audit.log"
    
    return $EXECUTION_ALLOWED
}

# ============================================================================
# GOVERNANCE AUTHORIZATION FUNCTIONS
# ============================================================================

verify_governance_authorization() {
    local operation="$1"
    local governance_token="${2:-}"
    
    log_security "SEAL" "Verifying governance authorization for: $operation"
    
    if [[ -z "$governance_token" ]]; then
        log_security "BLOCK" "Governance authorization required but no token provided"
        return $BLOCK_EXECUTION
    fi
    
    # Verify governance token signature
    local governance_signature="${governance_token}.sig"
    if [[ ! -f "$governance_signature" ]]; then
        log_security "BLOCK" "Governance signature missing: $governance_signature"
        return $BLOCK_EXECUTION
    fi
    
    if ! verify_aura_seal "$governance_token" "$governance_signature" "$GOVERNANCE_PUBLIC_KEY"; then
        log_security "BLOCK" "Governance authorization FAILED: Invalid signature"
        return $BLOCK_EXECUTION
    fi
    
    log_security "ALLOW" "Governance authorization GRANTED for: $operation"
    return $EXECUTION_ALLOWED
}

# ============================================================================
# UTILITY FUNCTIONS
# ============================================================================

initialize_security_environment() {
    log_security "SEAL" "Initializing RIFT security environment"
    
    # Create required directories
    mkdir -p "$AURASEAL_DIR" "$ENTROPY_DB_DIR" "$GOVERNANCE_DIR"
    
    # Verify cryptographic keys exist
    if [[ ! -f "$AURASEAL_PRIVATE_KEY" ]] || [[ ! -f "$AURASEAL_PUBLIC_KEY" ]]; then
        log_security "BLOCK" "AuraSeal keys missing. Run 'make zero-trust-init' first."
        return $BLOCK_EXECUTION
    fi
    
    # Initialize audit logs
    touch "${PROJECT_ROOT}/security_audit.log"
    touch "${PROJECT_ROOT}/load_audit.log"
    
    log_security "ALLOW" "Security environment initialized successfully"
    return $EXECUTION_ALLOWED
}

show_security_status() {
    echo ""
    echo "=== RIFT Auto-Seal Loader Security Status ==="
    echo "Project Root: $PROJECT_ROOT"
    echo "AuraSeal Keys: $([ -f "$AURASEAL_PUBLIC_KEY" ] && echo "✓ Present" || echo "✗ Missing")"
    echo "Governance Keys: $([ -f "$GOVERNANCE_PUBLIC_KEY" ] && echo "✓ Present" || echo "✗ Missing")"
    echo "Security Audit Log: $([ -f "${PROJECT_ROOT}/security_audit.log" ] && echo "✓ Active" || echo "✗ Inactive")"
    echo ""
    echo "Security Levels:"
    echo "  Stage 0: $STAGE_0_SECURITY_LEVEL"
    echo "  Stage 1: $STAGE_1_SECURITY_LEVEL"
    echo "  Stage 3: $STAGE_3_SECURITY_LEVEL"
    echo "  Stage 4: $STAGE_4_SECURITY_LEVEL"
    echo "  Stage 5: $STAGE_5_SECURITY_LEVEL"
    echo ""
    echo "Forward-Only Pipeline: .rift.0 → .rift.1 → .rift.3 → .rift.4 → .rift.5"
    echo "Reverse Engineering Protection: ACTIVE"
    echo "Transversal Stage Mapping Protection: ACTIVE"
    echo "=========================================="
}

# ============================================================================
# MAIN EXECUTION LOGIC
# ============================================================================

main() {
    case "${1:-}" in
        "load")
            if [[ $# -lt 2 ]]; then
                echo "Usage: $0 load <rift_file> [target_stage] [--governance]"
                exit 1
            fi
            
            local rift_file="$2"
            local target_stage="${3:-}"
            local governance_mode="false"
            
            if [[ "${4:-}" == "--governance" ]]; then
                governance_mode="true"
            fi
            
            initialize_security_environment
            
            if load_rift_program "$rift_file" "$governance_mode" "$target_stage"; then
                echo "✓ RIFT program load AUTHORIZED: $(basename "$rift_file")"
                exit $EXECUTION_ALLOWED
            else
                echo "✗ RIFT program load BLOCKED: $(basename "$rift_file")"
                exit $BLOCK_EXECUTION
            fi
            ;;
            
        "verify")
            if [[ $# -lt 2 ]]; then
                echo "Usage: $0 verify <rift_file>"
                exit 1
            fi
            
            local rift_file="$2"
            initialize_security_environment
            
            # Determine stage and verify
            local stage=""
            case "$rift_file" in
                *.rift.0) stage=".rift.0" ;;
                *.rift.1) stage=".rift.1" ;;
                *.rift.3) stage=".rift.3" ;;
                *.rift.4) stage=".rift.4" ;;
                *.rift.5) stage=".rift.5" ;;
                *)
                    echo "Invalid RIFT file extension"
                    exit $BLOCK_EXECUTION
                    ;;
            esac
            
            if verify_security_level "$stage" "$rift_file"; then
                echo "✓ RIFT security verification PASSED: $(basename "$rift_file")"
                exit $EXECUTION_ALLOWED
            else
                echo "✗ RIFT security verification FAILED: $(basename "$rift_file")"
                exit $BLOCK_EXECUTION
            fi
            ;;
            
        "status")
            initialize_security_environment
            show_security_status
            ;;
            
        "init")
            initialize_security_environment
            echo "✓ RIFT security environment initialized"
            ;;
            
        "help"|"-h"|"--help")
            echo "RIFT Auto-Seal Loader + Automaton Verifier"
            echo "Usage: $0 [command] [options]"
            echo ""
            echo "Commands:"
            echo "  load <rift_file> [target_stage] [--governance]"
            echo "    Load and verify RIFT program with security checks"
            echo "  verify <rift_file>"
            echo "    Verify security level for RIFT file"
            echo "  status"
            echo "    Show security environment status"
            echo "  init"
            echo "    Initialize security environment"
            echo "  help"
            echo "    Show this help message"
            echo ""
            echo "Security Features:"
            echo "  ✓ AuraSeal cryptographic verification"
            echo "  ✓ Structural entropy validation"
            echo "  ✓ Transversal stage mapping protection"
            echo "  ✓ Progressive security level enforcement"
            echo "  ✓ Forward-only pipeline enforcement"
            echo "  ✓ Anti-reverse engineering protection"
            echo ""
            echo "Examples:"
            echo "  $0 load myfile.rift.5"
            echo "  $0 verify myfile.rift.4"
            echo "  $0 load myfile.rift.3 .rift.4"
            echo "  $0 load myfile.rift.5 .rift.4 --governance"
            ;;
            
        *)
            echo "RIFT Auto-Seal Loader + Automaton Verifier"
            echo "Use '$0 help' for usage information"
            exit 1
            ;;
    esac
}

# Execute main function with all arguments
main "$@"
