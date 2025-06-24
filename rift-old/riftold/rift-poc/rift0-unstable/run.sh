#!/bin/bash

echo "🚀 RIFT Stage 0 - Running Tokenization Demo"
echo "==========================================="

# Build the project
echo "🔨 Building RIFT Stage 0..."
make clean && make

if [ $? -ne 0 ]; then
    echo "❌ Build failed!"
    exit 1
fi

echo ""
echo "🎯 Executing RIFT Stage 0 demo..."
echo "-----------------------------------"

# Run the main program
./build/rift0

echo ""
echo "🧪 Running QA tests..."
echo "----------------------"

# Run tests
make test

echo ""
echo "📋 RIFT Stage 0 Summary:"
echo "  ✅ Tokenization/Lexical Analysis"
echo "  ✅ AEGIS Regex Automaton Engine"  
echo "  ✅ Pattern parsing (R\"pattern/flags[mode]\")"
echo "  ✅ Top-down/bottom-up matching modes"
echo "  ✅ QA framework with edge case testing"
echo ""
echo "🎉 Ready for Stage 1 (Parsing/Grammar Analysis)!"
