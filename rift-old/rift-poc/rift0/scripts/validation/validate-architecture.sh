#!/bin/bash

# RIFT Architecture Validation Script
# Ensures AEGIS methodology compliance

echo "🔍 RIFT Architecture Validation"
echo "================================"

# Check token type/value separation in headers
echo "Checking header architecture..."
if grep -q "char\* type;" include/*/rift.h && grep -q "char\* value;" include/*/rift.h; then
    echo "✅ Token type/value separation preserved in headers"
else
    echo "❌ Token type/value separation violation in headers"
    exit 1
fi

# Check for type/value merging in source
echo "Checking source code for architecture violations..."
if grep -r "token->type.*token->value\|token->value.*token->type" src/ 2>/dev/null; then
    echo "❌ Potential type/value merging detected in source"
    echo "   Review the above findings for AEGIS compliance"
    exit 1
else
    echo "✅ No type/value merging detected in source"
fi

# Check matched_state preservation
echo "Checking matched_state preservation..."
if grep -q "matched_state" include/*/rift.h && grep -q "matched_state" src/core/*.c; then
    echo "✅ matched_state preserved for AST minimization"
else
    echo "❌ matched_state not properly preserved"
    exit 1
fi

# Check compiler compliance
echo "Checking compiler compliance..."
if grep -q "\-Werror" Makefile && grep -q "\-Wall" Makefile && grep -q "\-Wextra" Makefile; then
    echo "✅ Strict compiler flags enforced"
else
    echo "❌ Missing strict compiler flags"
    exit 1
fi

echo ""
echo "🏗️  RIFT Architecture Validation Complete"
echo "✅ All AEGIS methodology requirements satisfied"
