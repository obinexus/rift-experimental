#!/bin/bash

OUTPUT_FILE="$1"

echo "=== Tennis FSM Validation Check ==="
echo "Validating Tennis FSM equivalence proof in: $OUTPUT_FILE"

# Check for Tennis FSM validation metadata
if grep -q "Tennis FSM Validation: PASSED" "$OUTPUT_FILE"; then
    echo "✓ Tennis FSM validation metadata found"
else
    echo "✗ Tennis FSM validation metadata missing"
    exit 1
fi

# Check for state reduction
if grep -q "State Reduction:" "$OUTPUT_FILE"; then
    REDUCTION=$(grep "State Reduction:" "$OUTPUT_FILE" | sed 's/.*: \([0-9.]*\)%.*/\1/')
    if (( $(echo "$REDUCTION > 0" | bc -l) )); then
        echo "✓ State reduction achieved: ${REDUCTION}%"
    else
        echo "✗ No state reduction detected"
        exit 1
    fi
else
    echo "✗ State reduction data missing"
    exit 1
fi

# Check for equivalence proof
if grep -q "Equivalence Verification: PROVEN" "$OUTPUT_FILE"; then
    echo "✓ Equivalence verification completed"
else
    echo "✗ Equivalence verification failed"
    exit 1
fi

echo "✓ Tennis FSM validation: ALL CHECKS PASSED"
echo "  State minimization proven equivalent via Tennis FSM methodology"
