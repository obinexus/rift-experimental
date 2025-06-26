#!/bin/bash
#
# demo_pipeline.sh
# RIFT Compiler Pipeline Demonstration
# OBINexus Computing Framework - Technical Validation
#

set -euo pipefail

echo "🎯 RIFT Compiler Pipeline Demonstration"
echo "======================================="
echo "Input: let result = (x + y) * 42;"
echo ""

# Test input with RIFT syntax
TEST_INPUT='R"/let result = (x + y) * 42;/gmi[bt]"'

# Create output directory
mkdir -p demo_output

# Ensure binaries are built
if [ ! -f "bin/rift.exe" ]; then
    echo "Building RIFT compiler..."
    mkdir -p build
    cd build
    cmake ..
    make -j$(nproc)
    cd ..
fi

echo "📋 Stage-by-Stage Execution:"
echo "----------------------------"

# Stage 0: Tokenization
echo "🚀 RIFT-0: Tokenization"
echo "$TEST_INPUT" | ./bin/rift-0.exe --output demo_output/tokens.json
echo "   Output: demo_output/tokens.json"
echo ""

# Stage 1: Parsing
echo "🌳 RIFT-1: Parsing (Dual-Mode)"
./bin/rift-1.exe --input demo_output/tokens.json --output demo_output/ast.json --bottom-up --top-down
echo "   Output: demo_output/ast.json"
echo ""

# Stage 2: Semantic Analysis
echo "🔍 RIFT-2: Semantic Analysis"
./bin/rift-2.exe --input demo_output/ast.json --output demo_output/semantic_ast.json
echo "   Output: demo_output/semantic_ast.json"
echo ""

# Stage 3: Validation
echo "✅ RIFT-3: Validation"
./bin/rift-3.exe --input demo_output/semantic_ast.json --output demo_output/validated_ast.json
echo "   Output: demo_output/validated_ast.json"
echo ""

# Stage 4: Bytecode Generation
echo "⚙️  RIFT-4: Bytecode Generation (Trust Tagged)"
./bin/rift-4.exe --input demo_output/validated_ast.json --output demo_output/bytecode.rbc --architecture amd_ryzen
echo "   Output: demo_output/bytecode.rbc"
echo ""

# Stage 5: Verification
echo "🛡️  RIFT-5: Verification (AEGIS Compliance)"
./bin/rift-5.exe --input demo_output/bytecode.rbc --output demo_output/verified_bytecode.rbc
echo "   Output: demo_output/verified_bytecode.rbc"
echo ""

# Stage 6: Emission
echo "📦 RIFT-6: Emission"
./bin/rift-6.exe --input demo_output/verified_bytecode.rbc --output demo_output/result.rbc
echo "   Output: demo_output/result.rbc"
echo ""

echo "🎉 Pipeline Execution Complete!"
echo ""
echo "📊 Artifacts Generated:"
echo "----------------------"
ls -la demo_output/
echo ""
echo "🔍 Bytecode Analysis:"
echo "--------------------"
if [ -f "demo_output/bytecode.rbc" ]; then
    hexdump -C demo_output/bytecode.rbc | head -10
    echo "..."
fi
echo ""
echo "✅ RIFT Compiler Pipeline Demonstration Successful"
