#!/bin/bash
# Validate RIFT project structure compliance

echo "🔍 Validating RIFT project structure..."

# Check required directories
required_dirs=(
    "src/core" "src/cli" "include/project-root"
    "out/stage0" "out/artifacts/rift0" 
    "examples/basic" "examples/csv"
    "build" "lib" "config"
)

for dir in "${required_dirs[@]}"; do
    if [ -d "$dir" ]; then
        echo "✅ $dir"
    else
        echo "❌ Missing: $dir"
    fi
done

# Check configuration files
if [ -f ".riftrc" ]; then
    echo "✅ .riftrc configuration"
else
    echo "❌ Missing: .riftrc"
fi

# Check executables
if [ -f "bin/rift0" ]; then
    echo "✅ bin/rift0 executable"
else
    echo "❌ Missing: bin/rift0"
fi

echo "📊 Structure validation complete"
