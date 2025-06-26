#!/bin/bash
# RIFT pkg-config Validation Script
# AEGIS Compliant Testing Framework

set -euo pipefail

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m'

echo "🔍 RIFT pkg-config Validation Suite"
echo "===================================="

# Check pkg-config availability
if ! command -v pkg-config >/dev/null 2>&1; then
    echo -e "${RED}✗ pkg-config not found${NC}"
    exit 1
fi

echo -e "${GREEN}✓ pkg-config available: $(pkg-config --version)${NC}"

# Check each RIFT stage
failed_stages=()
for stage in {0..6}; do
    echo -n "Testing rift-${stage}.pc: "
    if pkg-config --exists "rift-${stage}" 2>/dev/null; then
        version=$(pkg-config --modversion "rift-${stage}")
        cflags=$(pkg-config --cflags "rift-${stage}")
        libs=$(pkg-config --libs "rift-${stage}")
        echo -e "${GREEN}✓ Found (v${version})${NC}"
        echo "  CFLAGS: ${cflags}"
        echo "  LIBS: ${libs}"
    else
        echo -e "${RED}✗ Missing${NC}"
        failed_stages+=("${stage}")
    fi
    echo
done

# Test unified linking
echo "Testing unified linking:"
if pkg-config --exists $(printf "rift-%d " {0..6}) 2>/dev/null; then
    unified_cflags=$(pkg-config --cflags $(printf "rift-%d " {0..6}))
    unified_libs=$(pkg-config --libs $(printf "rift-%d " {0..6}))
    echo -e "${GREEN}✓ Unified linking available${NC}"
    echo "  Combined CFLAGS: ${unified_cflags}"
    echo "  Combined LIBS: ${unified_libs}"
else
    echo -e "${YELLOW}⚠ Unified linking not available${NC}"
fi

# Summary
if [ ${#failed_stages[@]} -eq 0 ]; then
    echo -e "\n${GREEN}✓ All RIFT pkg-config files validated successfully${NC}"
    exit 0
else
    echo -e "\n${RED}✗ Failed stages: ${failed_stages[*]}${NC}"
    exit 1
fi
