#!/bin/bash
# RIFT Pipeline Validation Script

echo "🔍 RIFT Pipeline Validation"
echo "=========================="

# Validate stage dependencies
validate_stage_dependencies() {
    local stages=("rift0" "rift1" "rift3" "rift4" "rift5")
    local dependencies=("" "rift0" "rift0 rift1" "rift0 rift1 rift3" "rift0 rift1 rift3 rift4")
    
    for i in "${!stages[@]}"; do
        local stage="${stages[$i]}"
        local deps="${dependencies[$i]}"
        
        if [[ -d "$stage" ]]; then
            echo "✅ Stage $stage present"
            
            # Check dependencies
            if [[ -n "$deps" ]]; then
                for dep in $deps; do
                    if [[ ! -d "$dep" ]]; then
                        echo "❌ Dependency $dep missing for $stage"
                        return 1
                    fi
                done
            fi
        else
            echo "⚠️  Stage $stage not found"
        fi
    done
    
    echo "✅ Pipeline validation complete"
}

validate_stage_dependencies
