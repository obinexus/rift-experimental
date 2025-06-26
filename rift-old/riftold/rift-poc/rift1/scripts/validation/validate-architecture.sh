#!/bin/bash
# RIFT Architecture Validation

echo "🏗️  RIFT Architecture Validation"
echo "================================"

validate_token_separation() {
    echo "Validating token type/value separation..."
    
    local violations=0
    for stage_dir in rift*; do
        if [[ -d "$stage_dir" ]]; then
            # Check headers
            if ! grep -q "char\* type;" "$stage_dir"/include/*/core/*.h 2>/dev/null; then
                echo "❌ Missing type field in $stage_dir"
                ((violations++))
            fi
            
            if ! grep -q "char\* value;" "$stage_dir"/include/*/core/*.h 2>/dev/null; then
                echo "❌ Missing value field in $stage_dir"
                ((violations++))
            fi
            
            # Check for merging violations
            if grep -r "type.*value\|value.*type" "$stage_dir/src" 2>/dev/null | grep -v "separate\|preserve"; then
                echo "⚠️  Potential type/value merging in $stage_dir"
            fi
        fi
    done
    
    if [[ $violations -eq 0 ]]; then
        echo "✅ Token separation validation passed"
    else
        echo "❌ Token separation validation failed ($violations violations)"
        return 1
    fi
}

validate_matched_state_preservation() {
    echo "Validating matched_state preservation..."
    
    local found=0
    for stage_dir in rift*; do
        if [[ -d "$stage_dir" ]]; then
            if grep -r "matched_state" "$stage_dir" >/dev/null 2>&1; then
                echo "✅ matched_state found in $stage_dir"
                ((found++))
            fi
        fi
    done
    
    if [[ $found -gt 0 ]]; then
        echo "✅ AST optimization state preservation verified"
    else
        echo "⚠️  AST optimization state preservation not detected"
    fi
}

validate_compiler_compliance() {
    echo "Validating compiler compliance..."
    
    for stage_dir in rift*; do
        if [[ -d "$stage_dir" && -f "$stage_dir/Makefile" ]]; then
            if grep -q "\-Werror.*\-Wall.*\-Wextra" "$stage_dir/Makefile"; then
                echo "✅ Strict flags in $stage_dir"
            else
                echo "❌ Missing strict compiler flags in $stage_dir"
                return 1
            fi
        fi
    done
    
    echo "✅ Compiler compliance validated"
}

validate_token_separation
validate_matched_state_preservation
validate_compiler_compliance

echo "🏗️  Architecture validation complete"
