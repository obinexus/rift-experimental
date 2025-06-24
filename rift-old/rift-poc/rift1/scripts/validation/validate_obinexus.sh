#!/bin/bash
# OBINexus Compliance Validation
# Ensures compliance with OBINexus Legal Policy architecture

echo "🔍 OBINexus Compliance Validation"
echo "================================="

# Check for Zero Trust governance markers
echo "Checking Zero Trust governance..."
if grep -q "Zero Trust\|nlink\|polybuild" Makefile README.md 2>/dev/null; then
    echo "✅ Zero Trust governance markers present"
else
    echo "❌ Zero Trust governance markers missing"
    exit 1
fi

# Check toolchain integration
echo "Checking toolchain integration..."
if grep -q "riftlang.exe.*\.so\.a.*rift\.exe.*gosilang" Makefile README.md 2>/dev/null; then
    echo "✅ Toolchain integration documented"
else
    echo "❌ Toolchain integration missing"
    exit 1
fi

echo ""
echo "🏗️  OBINexus Compliance Validation Complete"
echo "✅ All governance requirements satisfied"
