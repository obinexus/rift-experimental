#!/bin/bash
# TDD Cycle Runner for RIFT Stages

run_tdd_cycle() {
    local stage="$1"
    
    if [[ ! -d "$stage" ]]; then
        echo "❌ Stage $stage not found"
        return 1
    fi
    
    echo "🔄 Running TDD cycle for $stage"
    cd "$stage"
    
    # RED: Tests should fail
    echo "🔴 RED Phase - Writing failing tests..."
    make test 2>/dev/null || echo "Tests failing as expected"
    
    # GREEN: Make tests pass
    echo "🟢 GREEN Phase - Making tests pass..."
    make all
    make test
    
    # REFACTOR: Improve code quality
    echo "🔵 REFACTOR Phase - Improving quality..."
    make verify-architecture
    
    cd ..
    echo "✅ TDD cycle complete for $stage"
}

if [[ $# -gt 0 ]]; then
    run_tdd_cycle "$1"
else
    echo "Usage: $0 <stage>"
    echo "Available stages: rift0, rift1, rift3, rift4, rift5"
fi
