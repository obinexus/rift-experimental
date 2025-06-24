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
