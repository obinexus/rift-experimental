#!/usr/bin/env bash
# RIFT Governance Validation - Corrected Implementation
# OBINexus Computing Division - Technical Resolution
# Author: Systematic Problem Resolution (Collaborative with Nnamdi Okpala)

set -euo pipefail

readonly RIFT_MIN_STAGE=0
readonly RIFT_MAX_STAGE=6
readonly RIFT_GOV_DIR="$(pwd)"

validate_stage_corrected() {
    local stage="$1"
    local config_file="$RIFT_GOV_DIR/.riftrc.$stage"
    local governance_file="$RIFT_GOV_DIR/gov.riftrc.$stage.in"
    
    echo "[INFO] Validating RIFT Stage $stage governance files (corrected logic)"
    
    # Verify configuration file
    if [[ -f "$config_file" ]]; then
        echo "[INFO] ✅ Found stage configuration: $config_file"
        # Validate file content structure
        if grep -q "\[governance\]" "$config_file"; then
            echo "[INFO] ✅ Configuration structure validated"
        else
            echo "[WARN] ⚠️  Configuration missing [governance] section"
        fi
    else
        echo "[ERROR] ❌ Missing stage configuration: $config_file"
        return 1
    fi
    
    # Verify governance input file
    if [[ -f "$governance_file" ]]; then
        echo "[INFO] ✅ Found governance input: $governance_file"
        # Validate JSON structure if applicable
        if command -v jq &> /dev/null; then
            if jq empty "$governance_file" 2>/dev/null; then
                echo "[INFO] ✅ Governance input JSON structure validated"
            else
                echo "[WARN] ⚠️  Governance input JSON validation failed"
            fi
        fi
    else
        echo "[ERROR] ❌ Missing governance input: $governance_file"
        return 1
    fi
    
    echo "[SUCCESS] ✅ Stage $stage governance validation passed"
    return 0
}

# Main validation execution
main() {
    echo "🔍 RIFT Governance Validation (Corrected Implementation)"
    echo "📁 Directory: $RIFT_GOV_DIR"
    echo ""
    
    local validation_result=0
    
    for stage in $(seq $RIFT_MIN_STAGE $RIFT_MAX_STAGE); do
        if ! validate_stage_corrected "$stage"; then
            validation_result=1
        fi
        echo ""
    done
    
    if [[ $validation_result -eq 0 ]]; then
        echo "🎉 All RIFT governance stages validated successfully"
        echo "📋 Infrastructure ready for AEGIS waterfall progression"
    else
        echo "❌ RIFT governance validation failed for one or more stages"
        echo "🔧 Technical resolution required before proceeding"
    fi
    
    return $validation_result
}

main "$@"
