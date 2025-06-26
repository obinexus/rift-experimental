#!/bin/bash
echo "Validating USCN compliance for: $1"
if grep -q "USCN Normalization: ENABLED" "$1"; then
    echo "✓ USCN normalization confirmed"
    return 0
else
    echo "✗ USCN normalization missing"
    return 1
fi
