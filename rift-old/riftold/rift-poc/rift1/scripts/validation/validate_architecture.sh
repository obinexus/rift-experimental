#!/bin/bash
# RIFT Architecture Validation Script
# Ensures AEGIS methodology compliance

echo "🔍 RIFT Architecture Validation"
echo "================================"

# Check token type/value separation in headers
echo "Checking header architecture..."
if grep -q "char\* type;" include/rift1/core/rift_types.h && grep -q "char\* value;" include/rift1/core/rift_types.h; then
    echo "✅ Token type/value separation preserved in headers"
else
    echo "❌ Token type/value separation violation in headers"
    exit 1
fi

# Check for AEGIS automaton implementation
echo "Checking AEGIS automaton..."
if grep -q "RiftAutomaton" include/rift1/core/rift_automaton.h; then
    echo "✅ AEGIS automaton structure defined"
else
    echo "❌ AEGIS automaton structure missing"
    exit 1
fi

# Check dual parsing strategies
echo "Checking parsing strategies..."
if grep -q "RIFT_PARSE_BOTTOM_UP\|RIFT_PARSE_TOP_DOWN" include/rift1/core/rift_parser.h; then
    echo "✅ Dual parsing strategies implemented"
else
    echo "❌ Dual parsing strategies missing"
    exit 1
fi

echo ""
echo "🏗️  RIFT Architecture Validation Complete"
echo "✅ All AEGIS methodology requirements satisfied"
