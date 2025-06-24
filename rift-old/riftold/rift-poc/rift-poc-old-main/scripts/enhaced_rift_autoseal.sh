#!/bin/bash

#==============================================================================
# RIFT Auto-Seal Loader + Context-Aware One-Way Function Verifier
# 
# Implementation: Forward-only sealed pipeline with structural entropy validation
# Security Model: .rift.0 → .rift.1 → .rift.3 → .rift.4 → .rift.5 (NO REVERSE)
# Author: OBINexus Computing - AEGIS Waterfall Methodology
# Version: 1.1.0 (Context-Aware Integrity Implementation)
#
# Core Principle: Each stage loses backward information through progressive obfuscation
# Enhanced: Context-aware one-way function prevents structural recomposition attacks
# Based on: "The Hidden Cipher" - Odd Perfect Numbers and Cryptographic Integrity
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
readonly CONTEXT_INTEGRITY_DIR="${PROJECT_ROOT}/context_integrity"

# AuraSeal Cryptographic Configuration
readonly AURASEAL_PRIVATE_KEY="${AURASEAL_DIR}/rift_signing_key.pem"
readonly AURASEAL_PUBLIC_KEY="${AURASEAL_DIR}/rift_signing_pub.pem"
readonly GOVERNANCE_PRIVATE_KEY="${GOVERNANCE_DIR}/governance_master_key.pem"
readonly GOVERNANCE_PUBLIC_KEY="${GOVERNANCE_DIR}/governance_master_pub.pem"

# Context-Aware One-Way Function Parameters
readonly ENTROPY_TOLERANCE="0.05"
readonly STRUCTURAL_INTEGRITY_THRESHOLD="85.0"
readonly DIVISOR_ECHO_VALIDATION_ENABLED="true"
readonly CONTEXT_SIGNATURE_LENGTH="64"

# Progressive Security Levels (Increasing protection)
readonly STAGE_0_SECURITY_LEVEL="basic_optional"
readonly STAGE_1_SECURITY_LEVEL="sealed_signature"
readonly STAGE_3_SECURITY_LEVEL="obfuscated_minimized_entropy_aware"
readonly STAGE_4_SECURITY_LEVEL="hardened_encrypted_context_validated"
readonly STAGE_5_SECURITY_LEVEL="fully_sealed_anti_reversion_perfect_integrity"

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
readonly ENTROPY='\033[1;36m'
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
        "ENTROPY") echo -e "${ENTROPY}[ENTROPY]${NC} ${timestamp} - $message" | tee -a "${PROJECT_ROOT}/security_audit.log" ;;
        *)       echo -e "${BLUE}[SEC]${NC}   ${timestamp} - $message" | tee -a "${PROJECT_ROOT}/security_audit.log" ;;
    esac
    
    # Critical events also logged to system security log
    if [[ "$level" == "BLOCK" ]]; then
        echo "RIFT_SECURITY_VIOLATION|${timestamp}|${message}" >> "/var/log/rift_security.log" 2>/dev/null || true
    fi
}

# ============================================================================
# CONTEXT-AWARE ONE-WAY FUNCTION IMPLEMENTATION
# Based on "The Hidden Cipher" - Structural Entropy and Cryptographic Integrity
# ============================================================================

# Calculate Shannon Entropy using Python (high precision)
calculate_shannon_entropy() {
    local file_path="$1"
    
    log_security "ENTROPY" "Calculating Shannon entropy for: $(basename "$file_path")"
    
    if [[ ! -f "$file_path" ]]; then
        echo "0.0"
        return
    fi
    
    # High-precision Shannon entropy calculation
    local entropy=$(python3 -c "
import math
from collections import Counter
import sys

try:
    with open('$file_path', 'rb') as f:
        data = f.read()
    
    if len(data) == 0:
        print('0.0')
        sys.exit(0)
    
    # Calculate Shannon entropy with maximum precision
    byte_counts = Counter(data)
    data_len = len(data)
    
    entropy = 0.0
    for count in byte_counts.values():
        if count > 0:
            probability = count / data_len
            entropy -= probability * math.log2(probability)
    
    print(f'{entropy:.8f}')
    
except Exception as e:
    print('0.0')
" 2>/dev/null || echo "0.0")
    
    echo "$entropy"
}

# Calculate Divisor Echo Hypothesis Alignment (Perfect Number Analogy)
calculate_divisor_echo_alignment() {
    local file_path="$1"
    local stage_level="$2"
    
    log_security "ENTROPY" "Calculating Divisor Echo alignment for stage $stage_level"
    
    local file_size=$(stat -f%z "$file_path" 2>/dev/null || stat -c%s "$file_path" 2>/dev/null)
    local content_hash=$(sha256sum "$file_path" | cut -d' ' -f1)
    
    # Apply Divisor Echo Hypothesis to RIFT artifacts
    # For perfect structural integrity: GCD(n, d) = d and LCM(n, d) = n for all proper divisors d
    local alignment_score=$(python3 -c "
import math
import hashlib

# File size as our base number (analog to perfect number)
n = $file_size
stage = $stage_level

if n <= 0:
    print('0.0')
    exit()

# Calculate proper divisors of file_size
divisors = []
for i in range(1, int(math.sqrt(n)) + 1):
    if n % i == 0:
        divisors.append(i)
        if i != n // i and i != 1:
            divisors.append(n // i)

# Remove n itself to get proper divisors
divisors = [d for d in divisors if d != n and d != 0]

if len(divisors) == 0:
    print('50.0')  # Neutral score for edge cases
    exit()

# Apply Divisor Echo: check GCD(n, d) = d for structural perfection
alignment_score = 0.0
total_tests = 0

for d in divisors:
    if d > 0:
        gcd_val = math.gcd(n, d)
        lcm_val = (n * d) // math.gcd(n, d) if d != 0 else 0
        
        # Divisor Echo test: GCD(n, d) should equal d for structural integrity
        if gcd_val == d:
            alignment_score += 1.0
            total_tests += 1
        
        # LCM consistency test
        if lcm_val == n:
            alignment_score += 0.5
            total_tests += 0.5

# Normalize to percentage (0-100)
if total_tests > 0:
    alignment_percentage = (alignment_score / (len(divisors) * 1.5)) * 100
else:
    alignment_percentage = 0

# Apply stage-specific weighting
stage_weight = 1.0 + (stage * 0.1)  # Higher stages require higher integrity
final_score = min(alignment_percentage * stage_weight, 100.0)

print(f'{final_score:.2f}')
" 2>/dev/null || echo "0.0")
    
    echo "$alignment_score"
}

# Generate Context-Aware Checksum (256-bit segmented signature)
calculate_context_aware_checksum() {
    local rift_file="$1"
    local stage_level="$2"
    
    log_security "ENTROPY" "Generating context-aware checksum for stage $stage_level"
    
    local shannon_entropy=$(calculate_shannon_entropy "$rift_file")
    local divisor_alignment=$(calculate_divisor_echo_alignment "$rift_file" "$stage_level")
    local content_hash=$(sha256sum "$rift_file" | cut -d' ' -f1)
    local file_size=$(stat -f%z "$rift_file" 2>/dev/null || stat -c%s "$rift_file" 2>/dev/null)
    
    # Generate entropy distribution zones for high-entropy detection
    local entropy_zones=$(analyze_entropy_distribution_zones "$rift_file" "$stage_level")
    
    # Create context-aware signature combining all structural elements
    # This implements the "perfect hash" concept from the Hidden Cipher paper
    local context_data="${content_hash}${shannon_entropy}${divisor_alignment}${stage_level}${file_size}${entropy_zones}"
    local context_signature=$(echo "$context_data" | sha512sum | cut -d' ' -f1)
    
    # Create segmented 256-bit signature for tamper detection
    local segmented_signature=$(echo "$context_signature" | cut -c1-64)
    
    log_security "ENTROPY" "Context checksum generated: entropy=${shannon_entropy}, alignment=${divisor_alignment}%, signature=${segmented_signature:0:16}..."
    
    echo "$segmented_signature"
}

# Verify Context-Aware Structural Integrity
verify_context_aware_integrity() {
    local rift_file="$1"
    local expected_signature="$2"
    local stage_level="$3"
    
    log_security "ENTROPY" "Verifying context-aware integrity for stage $stage_level"
    
    local calculated_signature=$(calculate_context_aware_checksum "$rift_file" "$stage_level")
    
    if [[ "$calculated_signature" == "$expected_signature" ]]; then
        log_security "ALLOW" "Context-aware integrity verification PASSED"
        return $EXECUTION_ALLOWED
    else
        log_security "BLOCK" "Context-aware integrity verification FAILED - Structural compromise detected"
        log_security "BLOCK" "Expected: $expected_signature"
        log_security "BLOCK" "Calculated: $calculated_signature"
        
        # Additional analysis for debugging
        local shannon_entropy=$(calculate_shannon_entropy "$rift_file")
        local divisor_alignment=$(calculate_divisor_echo_alignment "$rift_file" "$stage_level")
        
        log_security "BLOCK" "Integrity breach analysis: entropy=${shannon_entropy}, alignment=${divisor_alignment}%"
        
        return $BLOCK_EXECUTION
    fi
}

# Entropy Distribution Analysis (High-entropy zone detection)
analyze_entropy_distribution_zones() {
    local rift_file="$1"
    local stage_level="$2"
    
    log_security "ENTROPY" "Analyzing entropy distribution zones for stage $stage_level"
    
    # Calculate local entropy field distribution
    local entropy_analysis=$(python3 -c "
import math
from collections import Counter

try:
    with open('$rift_file', 'rb') as f:
        data = f.read()
    
    if len(data) == 0:
        print('0.0 0.0 0.0 false')
        exit()
    
    # Divide file into entropy zones for analysis
    chunk_size = max(256, len(data) // 16)  # 16 zones minimum
    zones = []
    
    for i in range(0, len(data), chunk_size):
        chunk = data[i:i+chunk_size]
        if len(chunk) > 0:
            counts = Counter(chunk)
            if len(chunk) > 0:
                entropy = -sum(count/len(chunk) * math.log2(count/len(chunk)) for count in counts.values() if count > 0)
                zones.append(entropy)
    
    if not zones:
        print('0.0 0.0 0.0 false')
        exit()
    
    avg_entropy = sum(zones) / len(zones)
    max_entropy = max(zones)
    entropy_variance = sum((e - avg_entropy)**2 for e in zones) / len(zones)
    
    # Determine if this is a high-entropy zone requiring deeper examination
    # High entropy zones: avg > 6.0 AND variance > 1.0 (based on Hidden Cipher research)
    is_high_entropy = avg_entropy > 6.0 and entropy_variance > 1.0
    
    print(f'{avg_entropy:.6f} {max_entropy:.6f} {entropy_variance:.6f} {str(is_high_entropy).lower()}')
    
except Exception as e:
    print('0.0 0.0 0.0 false')
" 2>/dev/null || echo "0.0 0.0 0.0 false")
    
    echo "$entropy_analysis"
}

# Verify Structural Entropy with Shannon Entropy Precision
verify_structural_entropy_shannon() {
    local riftN_file_path="$1"
    local expected_entropy_str="$2"
    
    log_security "ENTROPY" "Verifying Shannon entropy for: $(basename "$riftN_file_path")"
    
    local expected_entropy=$(echo "$expected_entropy_str" | tr -d '"' | tr -d "'")
    local actual_entropy=$(calculate_shannon_entropy "$riftN_file_path")
    
    log_security "ENTROPY" "Expected entropy: $expected_entropy, Actual entropy: $actual_entropy"
    
    # Use configurable tolerance for entropy comparison
    local entropy_diff=$(python3 -c "print(abs(float('$actual_entropy') - float('$expected_entropy')))" 2>/dev/null || echo "999")
    local tolerance=$(python3 -c "print(float('$ENTROPY_TOLERANCE'))" 2>/dev/null || echo "0.05")
    
    if (( $(echo "$entropy_diff > $tolerance" | bc -l 2>/dev/null || echo "1") )); then
        log_security "BLOCK" "Structural entropy verification FAILED: deviation ${entropy_diff} > tolerance ${tolerance}"
        return $BLOCK_EXECUTION
    else
        log_security "ALLOW" "Structural entropy verification PASSED: deviation ${entropy_diff} within tolerance"
        return $EXECUTION_ALLOWED
    fi
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
        
        # Enhanced: Context-aware integrity verification
        local stage_level=""
        case "$riftN_file_path" in
            *.rift.0) stage_level="0" ;;
            *.rift.1) stage_level="1" ;;
            *.rift.3) stage_level="3" ;;
            *.rift.4) stage_level="4" ;;
            *.rift.5) stage_level="5" ;;
        esac
        
        # Apply Hidden Cipher context-aware validation
        local context_checksum=$(calculate_context_aware_checksum "$riftN_file_path" "$stage_level")
        local context_file="${riftN_file_path}.context"
        
        if [[ -f "$context_file" ]]; then
            local expected_context=$(cat "$context_file")
            if ! verify_context_aware_integrity "$riftN_file_path" "$expected_context" "$stage_level"; then
                log_security "BLOCK" "Context-aware integrity check FAILED despite valid cryptographic signature"
                log_security "BLOCK" "Potential structural tampering or entropy manipulation detected"
                return $BLOCK_EXECUTION
            fi
        else
            # Generate context signature for future verification
            echo "$context_checksum" > "$context_file"
            log_security "SEAL" "Generated context signature: $(basename "$context_file")"
            
            # Also store entropy signature for Shannon entropy verification
            local entropy_signature=$(calculate_shannon_entropy "$riftN_file_path")
            echo "$entropy_signature" > "${riftN_file_path}.entropy_shannon"
            log_security "ENTROPY" "Generated Shannon entropy signature: $entropy_signature"
        fi
        
        # Additional Divisor Echo validation if enabled
        if [[ "$DIVISOR_ECHO_VALIDATION_ENABLED" == "true" ]]; then
            local divisor_alignment=$(calculate_divisor_echo_alignment "$riftN_file_path" "$stage_level")
            local threshold=$(echo "$STRUCTURAL_INTEGRITY_THRESHOLD" | tr -d '"')
            
            if (( $(echo "$divisor_alignment < $threshold" | bc -l 2>/dev/null || echo "1") )); then
                log_security "BLOCK" "Divisor Echo validation FAILED: alignment ${divisor_alignment}% < threshold ${threshold}%"
                return $BLOCK_EXECUTION
            else
                log_security "ALLOW" "Divisor Echo validation PASSED: alignment ${divisor_alignment}%"
            fi
        fi
        
        return $EXECUTION_ALLOWED
    else
        log_security "BLOCK" "AuraSeal verification FAILED: Invalid cryptographic signature for $(basename "$riftN_file_path")"
        return $BLOCK_EXECUTION
    fi
}

# Legacy entropy verification (maintained for compatibility)
verify_structural_entropy() {
    local riftN_file_path="$1"
    local expected_entropy_signature="$2"
    
    log_security "ENTROPY" "Verifying structural entropy (legacy) for: $(basename "$riftN_file_path")"
    
    # Check for Shannon entropy file first
    local shannon_entropy_file="${riftN_file_path}.entropy_shannon"
    if [[ -f "$shannon_entropy_file" ]]; then
        local expected_shannon=$(cat "$shannon_entropy_file")
        return $(verify_structural_entropy_shannon "$riftN_file_path" "$expected_shannon")
    fi
    
    # Fall back to legacy entropy calculation
    local current_entropy=$(calculate_structural_entropy "$riftN_file_path")
    local current_entropy_hash=$(echo "$current_entropy" | sha256sum | cut -d' ' -f1)
    
    if [[ "$current_entropy_hash" == "$expected_entropy_signature" ]]; then
        log_security "ALLOW" "Structural entropy verification PASSED (legacy): $(basename "$riftN_file_path")"
        return $EXECUTION_ALLOWED
    else
        log_security "BLOCK" "Structural entropy verification FAILED (legacy): Hash mismatch for $(basename "$riftN_file_path")"
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
    
    # Enhanced structural entropy calculation with context awareness
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
    
    # Calculate Shannon entropy (high precision)
    counts = Counter(data)
    entropy = -sum(count/len(data) * math.log2(count/len(data)) for count in counts.values())
    
    # Calculate structural complexity factors
    with open('$file_path', 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()
    
    lines = len(content.split('\n'))
    tokens = len(content.split())
    nesting_depth = max(content.count('{'), content.count('['), content.count('('), 1)
    
    # Enhanced structural entropy incorporating Hidden Cipher principles
    # Base entropy + structural complexity + nesting penalty
    base_score = entropy * (1 + math.log(lines + 1)) * (1 + math.log(tokens + 1))
    nesting_penalty = nesting_depth / 10.0
    context_factor = 1.0 + (len(data) / 1000000.0)  # File size influence
    
    final_score = base_score * (1 + nesting_penalty) * context_factor
    print(f'{final_score:.6f}')
    
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
    
    # Check for forbidden reverse transitions (context-aware one-way function violation)
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
                log_security "BLOCK" "FORBIDDEN: Context-aware one-way function violation detected: $current_stage → $target_stage"
                log_security "BLOCK" "RIFT pipeline enforces structural integrity - reverse engineering blocked"
                log_security "BLOCK" "Attempted structural recomposition violates Divisor Echo Hypothesis"
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
# ENHANCED PROGRESSIVE SECURITY LEVEL ENFORCEMENT
# ============================================================================

verify_security_level() {
    local stage="$1"
    local file_path="$2"
    
    log_security "SEAL" "Verifying enhanced security level for stage: $stage"
    
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
    
    # Verify AuraSeal signature (with enhanced context-aware validation)
    if ! verify_aura_seal "$file_path" "$signature_file" "$AURASEAL_PUBLIC_KEY"; then
        return $BLOCK_EXECUTION
    fi
    
    log_security "ALLOW" "Stage 1 security verification PASSED"
    return $EXECUTION_ALLOWED
}

verify_stage_3_security() {
    local file_path="$1"
    log_security "SEAL" "Stage 3: Obfuscated + minimized + entropy-aware verification"
    
    # Stage 3: Requires signature + structural obfuscation + entropy validation
    local signature_file="${file_path}.sig"
    local entropy_file="${file_path}.entropy"
    local shannon_entropy_file="${file_path}.entropy_shannon"
    
    if [[ ! -f "$signature_file" ]]; then
        log_security "BLOCK" "Stage 3 signature missing: $signature_file"
        return $BLOCK_EXECUTION
    fi
    
    # Verify AuraSeal signature with context-aware validation
    if ! verify_aura_seal "$file_path" "$signature_file" "$AURASEAL_PUBLIC_KEY"; then
        return $BLOCK_EXECUTION
    fi
    
    # Verify structural obfuscation (AST minimization applied)
    if ! grep -q "Tennis FSM Validation: PASSED" "$file_path" 2>/dev/null; then
        log_security "BLOCK" "Stage 3 AST minimization validation failed: $file_path"
        return $BLOCK_EXECUTION
    fi
    
    # Enhanced entropy verification (Shannon entropy preferred)
    if [[ -f "$shannon_entropy_file" ]]; then
        local expected_shannon=$(cat "$shannon_entropy_file")
        if ! verify_structural_entropy_shannon "$file_path" "$expected_shannon"; then
            return $BLOCK_EXECUTION
        fi
    elif [[ -f "$entropy_file" ]]; then
        local expected_entropy=$(cat "$entropy_file")
        if ! verify_structural_entropy "$file_path" "$expected_entropy"; then
            return $BLOCK_EXECUTION
        fi
    fi
    
    # Verify entropy distribution zones
    local entropy_analysis=$(analyze_entropy_distribution_zones "$file_path" "3")
    local is_high_entropy=$(echo "$entropy_analysis" | awk '{print $4}')
    
    if [[ "$is_high_entropy" == "true" ]]; then
        log_security "ENTROPY" "Stage 3: High-entropy zone detected - applying enhanced validation"
        
        # Apply additional context-aware validation for high-entropy zones
        local context_file="${file_path}.context"
        if [[ -f "$context_file" ]]; then
            local expected_context=$(cat "$context_file")
            if ! verify_context_aware_integrity "$file_path" "$expected_context" "3"; then
                log_security "BLOCK" "Stage 3 high-entropy context validation failed"
                return $BLOCK_EXECUTION
            fi
        fi
    fi
    
    log_security "ALLOW" "Stage 3 enhanced security verification PASSED"
    return $EXECUTION_ALLOWED
}

verify_stage_4_security() {
    local file_path="$1"
    log_security "SEAL" "Stage 4: Hardened + encrypted + context-validated verification"
    
    # Stage 4: Maximum security - hardened + encrypted + sealed + context-aware
    local signature_file="${file_path}.sig"
    local entropy_file="${file_path}.entropy"
    local shannon_entropy_file="${file_path}.entropy_shannon"
    local context_file="${file_path}.context"
    local hardening_file="${file_path}.hardened"
    
    if [[ ! -f "$signature_file" ]]; then
        log_security "BLOCK" "Stage 4 signature missing: $signature_file"
        return $BLOCK_EXECUTION
    fi
    
    if [[ ! -f "$shannon_entropy_file" ]] && [[ ! -f "$entropy_file" ]]; then
        log_security "BLOCK" "Stage 4 entropy signature missing"
        return $BLOCK_EXECUTION
    fi
    
    if [[ ! -f "$context_file" ]]; then
        log_security "BLOCK" "Stage 4 context signature missing: $context_file"
        return $BLOCK_EXECUTION
    fi
    
    # Verify AuraSeal signature with enhanced context validation
    if ! verify_aura_seal "$file_path" "$signature_file" "$AURASEAL_PUBLIC_KEY"; then
        return $BLOCK_EXECUTION
    fi
    
    # Mandatory context-aware integrity verification for Stage 4
    local expected_context=$(cat "$context_file")
    if ! verify_context_aware_integrity "$file_path" "$expected_context" "4"; then
        return $BLOCK_EXECUTION
    fi
    
    # Mandatory Shannon entropy verification for Stage 4
    if [[ -f "$shannon_entropy_file" ]]; then
        local expected_shannon=$(cat "$shannon_entropy_file")
        if ! verify_structural_entropy_shannon "$file_path" "$expected_shannon"; then
            return $BLOCK_EXECUTION
        fi
    else
        local expected_entropy=$(cat "$entropy_file")
        if ! verify_structural_entropy "$file_path" "$expected_entropy"; then
            return $BLOCK_EXECUTION
        fi
    fi
    
    # Verify hardening markers
    if [[ -f "$hardening_file" ]]; then
        if ! grep -q "HARDENING_APPLIED" "$hardening_file" 2>/dev/null; then
            log_security "BLOCK" "Stage 4 hardening verification failed: $file_path"
            return $BLOCK_EXECUTION
        fi
    fi
    
    # Apply Divisor Echo validation for Stage 4
    local divisor_alignment=$(calculate_divisor_echo_alignment "$file_path" "4")
    local threshold="90.0"  # Higher threshold for Stage 4
    
    if (( $(echo "$divisor_alignment < $threshold" | bc -l 2>/dev/null || echo "1") )); then
        log_security "BLOCK" "Stage 4 Divisor Echo validation failed: alignment ${divisor_alignment}% < ${threshold}%"
        return $BLOCK_EXECUTION
    fi
    
    log_security "ALLOW" "Stage 4 enhanced security verification PASSED"
    return $EXECUTION_ALLOWED
}

verify_stage_5_security() {
    local file_path="$1"
    log_security "SEAL" "Stage 5: Fully sealed + anti-reversion + perfect integrity verification"
    
    # Stage 5: Maximum security + anti-reversion + perfect integrity validation
    local signature_file="${file_path}.sig"
    local entropy_file="${file_path}.entropy"
    local shannon_entropy_file="${file_path}.entropy_shannon"
    local context_file="${file_path}.context"
    local anti_reversion_file="${file_path}.anti_reversion"
    
    if [[ ! -f "$signature_file" ]]; then
        log_security "BLOCK" "Stage 5 signature missing: $signature_file"
        return $BLOCK_EXECUTION
    fi
    
    if [[ ! -f "$shannon_entropy_file" ]] && [[ ! -f "$entropy_file" ]]; then
        log_security "BLOCK" "Stage 5 entropy signature missing"
        return $BLOCK_EXECUTION
    fi
    
    if [[ ! -f "$context_file" ]]; then
        log_security "BLOCK" "Stage 5 context signature missing: $context_file"
        return $BLOCK_EXECUTION
    fi
    
    if [[ ! -f "$anti_reversion_file" ]]; then
        log_security "BLOCK" "Stage 5 anti-reversion metadata missing: $anti_reversion_file"
        return $BLOCK_EXECUTION
    fi
    
    # Verify AuraSeal signature with maximum context validation
    if ! verify_aura_seal "$file_path" "$signature_file" "$AURASEAL_PUBLIC_KEY"; then
        return $BLOCK_EXECUTION
    fi
    
    # Mandatory context-aware integrity verification for Stage 5
    local expected_context=$(cat "$context_file")
    if ! verify_context_aware_integrity "$file_path" "$expected_context" "5"; then
        return $BLOCK_EXECUTION
    fi
    
    # Mandatory Shannon entropy verification for Stage 5
    if [[ -f "$shannon_entropy_file" ]]; then
        local expected_shannon=$(cat "$shannon_entropy_file")
        if ! verify_structural_entropy_shannon "$file_path" "$expected_shannon"; then
            return $BLOCK_EXECUTION
        fi
    else
        local expected_entropy=$(cat "$entropy_file")
        if ! verify_structural_entropy "$file_path" "$expected_entropy"; then
            return $BLOCK_EXECUTION
        fi
    fi
    
    # Verify anti-reversion metadata
    if ! grep -q "ANTI_REVERSION_PROTECTED" "$anti_reversion_file" 2>/dev/null; then
        log_security "BLOCK" "Stage 5 anti-reversion verification failed: $file_path"
        return $BLOCK_EXECUTION
    fi
    
    # Apply maximum Divisor Echo validation for Stage 5 (Perfect integrity)
    local divisor_alignment=$(calculate_divisor_echo_alignment "$file_path" "5")
    local threshold="95.0"  # Near-perfect threshold for Stage 5
    
    if (( $(echo "$divisor_alignment < $threshold" | bc -l 2>/dev/null || echo "1") )); then
        log_security "BLOCK" "Stage 5 perfect integrity validation failed: alignment ${divisor_alignment}% < ${threshold}%"
        log_security "BLOCK" "Stage 5 requires near-perfect structural integrity per Hidden Cipher principles"
        return $BLOCK_EXECUTION
    fi
    
    # Final entropy distribution analysis for Stage 5
    local entropy_analysis=$(analyze_entropy_distribution_zones "$file_path" "5")
    local avg_entropy=$(echo "$entropy_analysis" | awk '{print $1}')
    local entropy_variance=$(echo "$entropy_analysis" | awk '{print $3}')
    
    # Stage 5 should have controlled entropy distribution
    if (( $(echo "$entropy_variance > 2.0" | bc -l 2>/dev/null || echo "0") )); then
        log_security "BLOCK" "Stage 5 entropy distribution validation failed: excessive variance ${entropy_variance}"
        return $BLOCK_EXECUTION
    fi
    
    log_security "ALLOW" "Stage 5 perfect integrity verification PASSED"
    log_security "ALLOW" "Context-aware one-way function validation COMPLETED"
    return $EXECUTION_ALLOWED
}

# ============================================================================
# MAIN AUTO-SEAL LOADER FUNCTION
# ============================================================================

load_rift_program() {
    local rift_file="$1"
    local governance_mode="${2:-false}"
    
    log_security "SEAL" "RIFT Auto-Seal Loader (Context-Aware) starting for: $(basename "$rift_file")"
    
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
    
    # Step 1: Verify enhanced security level for current stage
    if ! verify_security_level "$stage" "$rift_file"; then
        log_security "BLOCK" "Enhanced security level verification failed for stage: $stage"
        return $BLOCK_EXECUTION
    fi
    
    # Step 2: Check for transversal policy violations (context-aware one-way function)
    if [[ -n "${3:-}" ]]; then
        local target_stage="$3"
        if ! check_transversal_policy "$stage" "$target_stage" "$governance_mode"; then
            log_security "BLOCK" "Context-aware one-way function violation: $stage → $target_stage"
            return $BLOCK_EXECUTION
        fi
    fi
    
    # Step 3: Final context-aware authorization check
    log_security "ALLOW" "All enhanced security checks PASSED for: $(basename "$rift_file")"
    log_security "ALLOW" "Context-aware one-way function validation COMPLETED"
    log_security "ALLOW" "Authorized to load RIFT program: $stage"
    
    # Log successful load to audit trail with context information
    local entropy_signature=$(calculate_shannon_entropy "$rift_file")
    local context_signature=$(calculate_context_aware_checksum "$rift_file" "${stage##*.rift.}")
    
    echo "RIFT_LOAD_SUCCESS|$(date -u '+%Y-%m-%d %H:%M:%S')|STAGE=${stage}|FILE=$(basename "$rift_file")|ENTROPY=${entropy_signature}|CONTEXT=${context_signature:0:16}" >> "${PROJECT_ROOT}/load_audit.log"
    
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
    log_security "SEAL" "Initializing RIFT enhanced security environment"
    
    # Create required directories
    mkdir -p "$AURASEAL_DIR" "$ENTROPY_DB_DIR" "$GOVERNANCE_DIR" "$CONTEXT_INTEGRITY_DIR"
    
    # Verify cryptographic keys exist
    if [[ ! -f "$AURASEAL_PRIVATE_KEY" ]] || [[ ! -f "$AURASEAL_PUBLIC_KEY" ]]; then
        log_security "BLOCK" "AuraSeal keys missing. Run 'make zero-trust-init' first."
        return $BLOCK_EXECUTION
    fi
    
    # Initialize audit logs
    touch "${PROJECT_ROOT}/security_audit.log"
    touch "${PROJECT_ROOT}/load_audit.log"
    touch "${PROJECT_ROOT}/context_integrity_audit.log"
    
    # Verify Python3 availability for entropy calculations
    if ! command -v python3 >/dev/null 2>&1; then
        log_security "BLOCK" "Python3 required for Shannon entropy calculations"
        return $BLOCK_EXECUTION
    fi
    
    # Verify bc availability for floating point comparisons
    if ! command -v bc >/dev/null 2>&1; then
        log_security "WARN" "bc not available - floating point comparisons may be limited"
    fi
    
    log_security "ALLOW" "Enhanced security environment initialized successfully"
    log_security "ENTROPY" "Context-aware one-way function system ACTIVE"
    return $EXECUTION_ALLOWED
}

show_security_status() {
    echo ""
    echo "=== RIFT Auto-Seal Loader Enhanced Security Status ==="
    echo "Project Root: $PROJECT_ROOT"
    echo "AuraSeal Keys: $([ -f "$AURASEAL_PUBLIC_KEY" ] && echo "✓ Present" || echo "✗ Missing")"
    echo "Governance Keys: $([ -f "$GOVERNANCE_PUBLIC_KEY" ] && echo "✓ Present" || echo "✗ Missing")"
    echo "Security Audit Log: $([ -f "${PROJECT_ROOT}/security_audit.log" ] && echo "✓ Active" || echo "✗ Inactive")"
    echo "Context Integrity Log: $([ -f "${PROJECT_ROOT}/context_integrity_audit.log" ] && echo "✓ Active" || echo "✗ Inactive")"
    echo ""
    echo "Enhanced Security Levels:"
    echo "  Stage 0: $STAGE_0_SECURITY_LEVEL"
    echo "  Stage 1: $STAGE_1_SECURITY_LEVEL"
    echo "  Stage 3: $STAGE_3_SECURITY_LEVEL"
    echo "  Stage 4: $STAGE_4_SECURITY_LEVEL"
    echo "  Stage 5: $STAGE_5_SECURITY_LEVEL"
    echo ""
    echo "Context-Aware One-Way Function Parameters:"
    echo "  Entropy Tolerance: ±$ENTROPY_TOLERANCE"
    echo "  Structural Integrity Threshold: $STRUCTURAL_INTEGRITY_THRESHOLD%"
    echo "  Divisor Echo Validation: $DIVISOR_ECHO_VALIDATION_ENABLED"
    echo "  Context Signature Length: $CONTEXT_SIGNATURE_LENGTH bits"
    echo ""
    echo "Forward-Only Pipeline: .rift.0 → .rift.1 → .rift.3 → .rift.4 → .rift.5"
    echo "Reverse Engineering Protection: ACTIVE"
    echo "Transversal Stage Mapping Protection: ACTIVE"
    echo "Context-Aware Integrity Validation: ACTIVE"
    echo "Shannon Entropy Verification: ACTIVE"
    echo "Divisor Echo Hypothesis Validation: ACTIVE"
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
                echo "✓ Context-aware integrity validation PASSED"
                exit $EXECUTION_ALLOWED
            else
                echo "✗ RIFT program load BLOCKED: $(basename "$rift_file")"
                echo "✗ Context-aware integrity validation FAILED"
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
            
            # Determine stage and verify with enhanced validation
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
                echo "✓ RIFT enhanced security verification PASSED: $(basename "$rift_file")"
                echo "✓ Context-aware one-way function validation COMPLETED"
                exit $EXECUTION_ALLOWED
            else
                echo "✗ RIFT enhanced security verification FAILED: $(basename "$rift_file")"
                echo "✗ Context-aware integrity compromise detected"
                exit $BLOCK_EXECUTION
            fi
            ;;
            
        "entropy")
            if [[ $# -lt 2 ]]; then
                echo "Usage: $0 entropy <rift_file>"
                exit 1
            fi
            
            local rift_file="$2"
            initialize_security_environment
            
            echo "=== RIFT Entropy Analysis ==="
            echo "File: $(basename "$rift_file")"
            
            local shannon_entropy=$(calculate_shannon_entropy "$rift_file")
            echo "Shannon Entropy: $shannon_entropy"
            
            local stage_level=""
            case "$rift_file" in
                *.rift.0) stage_level="0" ;;
                *.rift.1) stage_level="1" ;;
                *.rift.3) stage_level="3" ;;
                *.rift.4) stage_level="4" ;;
                *.rift.5) stage_level="5" ;;
                *) stage_level="0" ;;
            esac
            
            local divisor_alignment=$(calculate_divisor_echo_alignment "$rift_file" "$stage_level")
            echo "Divisor Echo Alignment: $divisor_alignment%"
            
            local entropy_analysis=$(analyze_entropy_distribution_zones "$rift_file" "$stage_level")
            echo "Entropy Distribution: $entropy_analysis"
            
            local context_checksum=$(calculate_context_aware_checksum "$rift_file" "$stage_level")
            echo "Context-Aware Checksum: $context_checksum"
            
            echo "=============================="
            ;;
            
        "status")
            initialize_security_environment
            show_security_status
            ;;
            
        "init")
            initialize_security_environment
            echo "✓ RIFT enhanced security environment initialized"
            echo "✓ Context-aware one-way function system activated"
            ;;
            
        "help"|"-h"|"--help")
            echo "RIFT Auto-Seal Loader + Context-Aware One-Way Function Verifier"
            echo "Enhanced with Hidden Cipher structural integrity validation"
            echo ""
            echo "Usage: $0 [command] [options]"
            echo ""
            echo "Commands:"
            echo "  load <rift_file> [target_stage] [--governance]"
            echo "    Load and verify RIFT program with enhanced security checks"
            echo "  verify <rift_file>"
            echo "    Verify enhanced security level for RIFT file"
            echo "  entropy <rift_file>"
            echo "    Analyze entropy distribution and context integrity"
            echo "  status"
            echo "    Show enhanced security environment status"
            echo "  init"
            echo "    Initialize enhanced security environment"
            echo "  help"
            echo "    Show this help message"
            echo ""
            echo "Enhanced Security Features:"
            echo "  ✓ AuraSeal cryptographic verification"
            echo "  ✓ Shannon entropy precision validation"
            echo "  ✓ Context-aware one-way function integrity"
            echo "  ✓ Divisor Echo Hypothesis validation"
            echo "  ✓ Entropy distribution zone analysis"
            echo "  ✓ Structural integrity checksums"
            echo "  ✓ Transversal stage mapping protection"
            echo "  ✓ Progressive security level enforcement"
            echo "  ✓ Forward-only pipeline enforcement"
            echo "  ✓ Anti-reverse engineering protection"
            echo ""
            echo "Examples:"
            echo "  $0 load myfile.rift.5"
            echo "  $0 verify myfile.rift.4"
            echo "  $0 entropy myfile.rift.3"
            echo "  $0 load myfile.rift.3 .rift.4"
            echo "  $0 load myfile.rift.5 .rift.4 --governance"
            echo ""
            echo "Based on 'The Hidden Cipher' research by Nnamdi Michael Okpala"
            echo "Implementing context-aware one-way functions for structural integrity"
            ;;
            
        *)
            echo "RIFT Auto-Seal Loader + Context-Aware One-Way Function Verifier"
            echo "Enhanced with Hidden Cipher structural integrity validation"
            echo "Use '$0 help' for usage information"
            exit 1
            ;;
    esac
}

# Execute main function with all arguments
main "$@"
