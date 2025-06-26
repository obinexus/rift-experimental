#!/bin/bash
# RIFT Build Orchestration Script

echo "🔨 RIFT Build Orchestration"
echo "=========================="

build_stages_in_order() {
    local stages=("rift0" "rift1" "rift3" "rift4" "rift5")
    
    for stage in "${stages[@]}"; do
        if [[ -d "$stage" ]]; then
            echo "Building $stage..."
            cd "$stage"
            make bootstrap 2>/dev/null || make all || echo "Build failed for $stage"
            cd ..
        fi
    done
}

build_stages_in_order
echo "✅ Build orchestration complete"

echo "🔍 Running policy validation"
script_dir="$(cd "$(dirname "$0")" && pwd)"
root_dir="$(cd "$script_dir/../.." && pwd)"
policy_script="$root_dir/rift-gov/policy_validation.sh"
if [[ -x "$policy_script" ]]; then
    "$policy_script" --stage=all || {
        echo "Policy validation failed" >&2
        exit 1
    }
else
    echo "Policy validation script not found: $policy_script" >&2
    exit 1
fi
