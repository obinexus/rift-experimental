#!/bin/bash
echo "Validating Stage 0 output for Stage 1 input: $1"
if [ ! -f "$1" ]; then
    echo "✗ Stage 0 output file not found"
    exit 1
fi
if grep -q "TOKEN_" "$1"; then
    echo "✓ Valid tokenized input detected"
else
    echo "✗ Invalid input format"
    exit 1
fi
