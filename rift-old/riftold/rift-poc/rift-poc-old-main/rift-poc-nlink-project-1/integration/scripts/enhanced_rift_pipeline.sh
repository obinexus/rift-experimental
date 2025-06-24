#!/bin/bash

echo "=== OBINexus RIFT-POC Enhanced Pipeline ==="

STAGES=("stage0_tokenization" "stage1_parsing" "stage3_ast" "stage4_bytecode" "stage5_program")

for STAGE in "${STAGES[@]}"; do
    echo ">>> Processing ${STAGE}"
    cd "../${STAGE}"
    
    if ! make validate-and-build; then
        echo "CRITICAL: ${STAGE} validation failed"
        exit 1
    fi
    
    if ! make auraseal-verify; then
        echo "CRITICAL: ${STAGE} AuraSeal verification failed"
        exit 1
    fi
    
    cd "../integration"
done

echo "=== All stages completed with Zero Trust validation ==="
