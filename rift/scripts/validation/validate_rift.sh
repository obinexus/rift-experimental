#!/bin/bash
#
# validate_rift.sh
# RIFT Compiler Validation Suite
# OBINexus Computing Framework
#

set -euo pipefail

echo "🔍 RIFT Compiler Validation Suite"
echo "================================="

# Function to run validation test
run_test() {
    local test_name="$1"
    local test_command="$2"
    
    echo -n "Testing $test_name... "
    if eval "$test_command" > /dev/null 2>&1; then
        echo "✅ PASS"
        return 0
    else
        echo "❌ FAIL"
        return 1
    fi
}

# Validation tests
TESTS_PASSED=0
TESTS_TOTAL=0

# Test 1: Directory structure
((TESTS_TOTAL++))
if run_test "Directory Structure" "[ -d 'src/core' ] && [ -d 'include/rift' ] && [ -d 'tests' ]"; then
    ((TESTS_PASSED++))
fi

# Test 2: Configuration file
((TESTS_TOTAL++))
if run_test "Configuration File" "[ -f '.riftrc' ]"; then
    ((TESTS_PASSED++))
fi

# Test 3: CMake configuration
((TESTS_TOTAL++))
if run_test "CMake Configuration" "[ -f 'CMakeLists.txt' ] && [ -f 'cmake/common/compiler_pipeline.cmake' ]"; then
    ((TESTS_PASSED++))
fi

# Test 4: Core headers
((TESTS_TOTAL++))
if run_test "Core Headers" "[ -f 'include/rift/rift.h' ] && [ -f 'include/rift/cli/cli.h' ]"; then
    ((TESTS_PASSED++))
fi

# Test 5: Core sources
((TESTS_TOTAL++))
if run_test "Core Sources" "[ -f 'src/core/rift.c' ] && [ -f 'src/cli/cli.c' ]"; then
    ((TESTS_PASSED++))
fi

# Test 6: Stage directories
((TESTS_TOTAL++))
STAGE_CHECK=true
for stage in rift-{0..6}; do
    if [ ! -d "$stage" ]; then
        STAGE_CHECK=false
        break
    fi
done
if run_test "Pipeline Stages" "$STAGE_CHECK"; then
    ((TESTS_PASSED++))
fi

# Test 7: Build system
((TESTS_TOTAL++))
if run_test "Build System" "mkdir -p build && cd build && cmake .. > /dev/null 2>&1"; then
    ((TESTS_PASSED++))
fi

echo ""
echo "📊 Validation Results:"
echo "====================="
echo "Tests Passed: $TESTS_PASSED/$TESTS_TOTAL"

if [ $TESTS_PASSED -eq $TESTS_TOTAL ]; then
    echo "🎉 All validation tests passed!"
    exit 0
else
    echo "❌ Some validation tests failed"
    exit 1
fi
