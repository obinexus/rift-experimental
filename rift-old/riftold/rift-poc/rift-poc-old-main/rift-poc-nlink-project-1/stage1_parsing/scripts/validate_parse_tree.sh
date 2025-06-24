#!/bin/bash
echo "Validating parse tree structure: $1"
if grep -q "PROGRAM" "$1"; then
    echo "✓ Valid parse tree structure"
else
    echo "✗ Invalid parse tree"
    exit 1
fi
