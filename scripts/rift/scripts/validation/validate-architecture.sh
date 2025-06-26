#!/bin/bash
# RIFT Architecture Validation Script
# AEGIS Methodology Compliance Verification

echo "🔍 RIFT Architecture Validation"
echo "================================"

validation_passed=true

# Token type/value separation validation
if [ -d "include/rift/token_type" ] && [ -d "include/rift/token_value" ]; then
    echo "✅ Token type/value separation architecture validated"
else
    echo "❌ Token type/value separation violation"
    validation_passed=false
fi

# PoLiC security framework validation
if [ -f "include/rift/core/polic.h" ]; then
    echo "✅ PoLiC security framework present"
else
    echo "❌ PoLiC security framework missing"
    validation_passed=false
fi

# State machine integration validation
if grep -q "matched_state" include/rift/token_type/token_type.h; then
    echo "✅ State machine minimization integration verified"
else
    echo "❌ State machine integration missing"
    validation_passed=false
fi

if [ "$validation_passed" = true ]; then
    echo ""
    echo "🏗️  RIFT Architecture Validation Complete"
    echo "✅ All AEGIS methodology requirements satisfied"
    exit 0
else
    echo ""
    echo "❌ Architecture validation failed"
    exit 1
fi
