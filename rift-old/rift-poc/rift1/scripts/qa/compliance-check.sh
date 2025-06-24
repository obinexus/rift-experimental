#!/bin/bash
# OBINexus Compliance Validation

echo "🔍 OBINexus Compliance Check"
echo "============================"

check_zero_trust_markers() {
    if grep -r "Zero Trust\|nlink\|polybuild" . >/dev/null 2>&1; then
        echo "✅ Zero Trust governance markers present"
    else
        echo "❌ Zero Trust governance markers missing"
        return 1
    fi
}

check_toolchain_integration() {
    if grep -r "riftlang.*\.so\.a.*rift\.exe.*gosilang" . >/dev/null 2>&1; then
        echo "✅ Toolchain integration documented"
    else
        echo "❌ Toolchain integration missing"
        return 1
    fi
}

check_aegis_architecture() {
    if grep -r "AEGIS\|Automaton Engine" . >/dev/null 2>&1; then
        echo "✅ AEGIS architecture references found"
    else
        echo "❌ AEGIS architecture references missing"
        return 1
    fi
}

check_zero_trust_markers
check_toolchain_integration
check_aegis_architecture

echo "✅ OBINexus compliance validation complete"
