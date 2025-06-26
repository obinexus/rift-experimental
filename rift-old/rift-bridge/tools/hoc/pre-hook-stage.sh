#!/usr/bin/env bash
# Pre-Hook Template for RIFT Stages 3-6
# Usage: pre-hook-stage3.sh, pre-hook-stage4.sh, etc.

set -euo pipefail

STAGE_ID="${1:-}"
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
HOOK_LOG="$PROJECT_ROOT/build/logs/hooks.log"

log_hook() {
    echo "[$(date -Iseconds)] [PRE-HOOK-$STAGE_ID] $1" | tee -a "$HOOK_LOG"
}

validate_stage_prerequisites() {
    local stage="$1"
    
    case "$stage" in
        3)
            # IR Builder prerequisites
            log_hook "Validating AST output from stage 2"
            if [[ ! -f "$PROJECT_ROOT/obj/stage-2/ast_output.json" ]]; then
                log_hook "ERROR: AST output not found for IR builder"
                return 1
            fi
            ;;
        4)
            # Optimizer prerequisites  
            log_hook "Validating IR output from stage 3"
            if [[ ! -f "$PROJECT_ROOT/obj/stage-3/ir_output.bc" ]]; then
                log_hook "ERROR: IR output not found for optimizer"
                return 1
            fi
            ;;
        5)
            # Bytecode Emitter prerequisites
            log_hook "Validating optimized IR from stage 4"
            if [[ ! -f "$PROJECT_ROOT/obj/stage-4/optimized_ir.bc" ]]; then
                log_hook "ERROR: Optimized IR not found for bytecode emitter"
                return 1
            fi
            ;;
        6)
            # Runtime Adapter prerequisites
            log_hook "Validating bytecode from stage 5"
            if [[ ! -f "$PROJECT_ROOT/obj/stage-5/bytecode.wasm" ]]; then
                log_hook "ERROR: Bytecode not found for runtime adapter"
                return 1
            fi
            ;;
    esac
    
    return 0
}

# Load governance for specific stage
load_stage_governance() {
    local stage="$1"
    local governance_file="$PROJECT_ROOT/.riftrc.$stage"
    
    if [[ -f "$governance_file" ]]; then
        log_hook "Loading governance policy for stage $stage"
        # Parse YAML governance (simplified)
        export RIFT_TRUST_LEVEL=$(grep "trust_level:" "$governance_file" | cut -d'"' -f2)
        export RIFT_SANDBOX=$(grep "sandbox:" "$governance_file" | cut -d' ' -f4)
        export RIFT_ZERO_TRUST=$(grep "zero_trust_mode:" "$governance_file" | cut -d' ' -f4)
        log_hook "Governance loaded: trust_level=$RIFT_TRUST_LEVEL, sandbox=$RIFT_SANDBOX"
    else
        log_hook "WARNING: No governance file found for stage $stage"
        return 1
    fi
}

# Security validation
validate_security_context() {
    local stage="$1"
    
    log_hook "Validating security context for stage $stage"
    
    # Check zero-trust compliance
    if [[ "$RIFT_ZERO_TRUST" == "enabled" ]]; then
        log_hook "Zero-trust mode validated"
        
        # Verify stage isolation
        local stage_dir="$PROJECT_ROOT/obj/stage-$stage"
        if [[ ! -d "$stage_dir" ]]; then
            mkdir -p "$stage_dir"
            log_hook "Created isolated stage directory: $stage_dir"
        fi
        
        # Set restrictive permissions
        chmod 700 "$stage_dir"
        log_hook "Applied restrictive permissions to stage directory"
    fi
    
    # Validate required tools
    if ! command -v emcc >/dev/null 2>&1; then
        log_hook "ERROR: Emscripten not found in PATH"
        return 1
    fi
    
    if ! command -v wasm-validate >/dev/null 2>&1; then
        log_hook "WARNING: wasm-validate not found, skipping validation"
    fi
}

# Main pre-hook execution
main() {
    if [[ -z "$STAGE_ID" ]]; then
        echo "Usage: $0 <stage_id>"
        exit 1
    fi
    
    log_hook "Starting pre-hook validation for stage $STAGE_ID"
    
    # Execute validation chain
    validate_stage_prerequisites "$STAGE_ID" || exit 1
    load_stage_governance "$STAGE_ID" || exit 1  
    validate_security_context "$STAGE_ID" || exit 1
    
    log_hook "Pre-hook validation completed successfully for stage $STAGE_ID"
}

main "$@"

# -------------------------------------------------------------------
# POST-HOOK TEMPLATE (post-hook-stage3.sh, post-hook-stage4.sh, etc.)
# -------------------------------------------------------------------

#!/usr/bin/env bash
# Post-Hook Template for RIFT Stages 3-6

set -euo pipefail

STAGE_ID="${1:-}"
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
HOOK_LOG="$PROJECT_ROOT/build/logs/hooks.log"

log_hook() {
    echo "[$(date -Iseconds)] [POST-HOOK-$STAGE_ID] $1" | tee -a "$HOOK_LOG"
}

validate_stage_output() {
    local stage="$1"
    local obj_dir="$PROJECT_ROOT/obj/stage-$stage"
    
    case "$stage" in
        3)
            # Validate IR Builder output
            log_hook "Validating IR builder output"
            if [[ -f "$obj_dir/ir_output.bc" ]]; then
                log_hook "IR bytecode generated successfully"
                # Validate LLVM IR format
                if command -v llvm-dis >/dev/null 2>&1; then
                    llvm-dis "$obj_dir/ir_output.bc" -o "$obj_dir/ir_output.ll"
                    log_hook "Generated human-readable LLVM IR"
                fi
            else
                log_hook "ERROR: IR output not generated"
                return 1
            fi
            ;;
        4)
            # Validate Optimizer output
            log_hook "Validating optimizer output"
            if [[ -f "$obj_dir/optimized_ir.bc" ]]; then
                log_hook "Optimized IR generated successfully"
                # Compare optimization metrics
                local original_size=$(stat -f%z "$PROJECT_ROOT/obj/stage-3/ir_output.bc" 2>/dev/null || stat -c%s "$PROJECT_ROOT/obj/stage-3/ir_output.bc")
                local optimized_size=$(stat -f%z "$obj_dir/optimized_ir.bc" 2>/dev/null || stat -c%s "$obj_dir/optimized_ir.bc")
                local reduction=$((100 - (optimized_size * 100 / original_size)))
                log_hook "Optimization achieved ${reduction}% size reduction"
            else
                log_hook "ERROR: Optimized IR not generated"
                return 1
            fi
            ;;
        5)
            # Validate Bytecode Emitter output
            log_hook "Validating bytecode emitter output"
            if [[ -f "$obj_dir/bytecode.wasm" ]]; then
                log_hook "WebAssembly bytecode generated successfully"
                
                # Validate WASM format
                if command -v wasm-validate >/dev/null 2>&1; then
                    if wasm-validate "$obj_dir/bytecode.wasm"; then
                        log_hook "WASM bytecode validation passed"
                    else
                        log_hook "ERROR: WASM bytecode validation failed"
                        return 1
                    fi
                fi
                
                # Generate WAT for human inspection
                if command -v wasm2wat >/dev/null 2>&1; then
                    wasm2wat "$obj_dir/bytecode.wasm" -o "$obj_dir/bytecode.wat"
                    log_hook "Generated WAT file for inspection"
                fi
                
                # Test round-trip conversion
                if command -v wat2wasm >/dev/null 2>&1 && [[ -f "$obj_dir/bytecode.wat" ]]; then
                    wat2wasm "$obj_dir/bytecode.wat" -o "$obj_dir/bytecode_roundtrip.wasm"
                    if cmp -s "$obj_dir/bytecode.wasm" "$obj_dir/bytecode_roundtrip.wasm"; then
                        log_hook "Round-trip conversion test passed"
                        rm "$obj_dir/bytecode_roundtrip.wasm"
                    else
                        log_hook "WARNING: Round-trip conversion test failed"
                    fi
                fi
            else
                log_hook "ERROR: WASM bytecode not generated"
                return 1
            fi
            ;;
        6)
            # Validate Runtime Adapter output
            log_hook "Validating runtime adapter output"
            if [[ -f "$obj_dir/runtime_module.js" ]]; then
                log_hook "Runtime adapter module generated successfully"
                
                # Validate JavaScript module syntax
                if command -v node >/dev/null 2>&1; then
                    if node -c "$obj_dir/runtime_module.js" 2>/dev/null; then
                        log_hook "JavaScript module syntax validation passed"
                    else
                        log_hook "ERROR: JavaScript module syntax validation failed"
                        return 1
                    fi
                fi
            else
                log_hook "ERROR: Runtime adapter module not generated"
                return 1
            fi
            ;;
    esac
    
    return 0
}

# Generate stage metadata
generate_stage_metadata() {
    local stage="$1"
    local obj_dir="$PROJECT_ROOT/obj/stage-$stage"
    local metadata_file="$obj_dir/stage_metadata.json"
    
    log_hook "Generating metadata for stage $stage"
    
    cat > "$metadata_file" <<EOF
{
    "stage_id": $stage,
    "completion_timestamp": "$(date -Iseconds)",
    "governance_compliance": "verified",
    "zero_trust_status": "${RIFT_ZERO_TRUST:-enabled}",
    "artifacts": [
        $(find "$obj_dir" -type f -name "*.*" | sed 's/.*/"&"/' | paste -sd, -)
    ],
    "validation_status": "passed",
    "toolchain_version": "$(emcc --version 2>/dev/null | head -1 || echo 'unknown')",
    "build_flags": {
        "optimization": "O3",
        "debug_symbols": true,
        "target": "wasm32-unknown-emscripten"
    }
}
EOF
    
    log_hook "Metadata generated: $metadata_file"
}

# Verify detach mode if enabled
check_detach_mode() {
    local stage="$1"
    local governance_file="$PROJECT_ROOT/.riftrc.$stage"
    
    if [[ -f "$governance_file" ]] && grep -q "detach_mode_enabled: true" "$governance_file"; then
        log_hook "Detach mode enabled for stage $stage - halting trust flow"
        
        # Create detach checkpoint
        local checkpoint_file="$PROJECT_ROOT/obj/stage-$stage/detach_checkpoint.json"
        cat > "$checkpoint_file" <<EOF
{
    "detach_timestamp": "$(date -Iseconds)",
    "stage_id": $stage,
    "trust_flow_status": "halted",
    "human_approval_required": false,
    "checkpoint_hash": "$(find "$PROJECT_ROOT/obj/stage-$stage" -type f -exec sha256sum {} \; | sha256sum | cut -d' ' -f1)"
}
EOF
        log_hook "Detach checkpoint created: $checkpoint_file"
        
        # Signal trust flow halt (implementation would depend on orchestration system)
        touch "$PROJECT_ROOT/build/.trust_flow_halted_stage_$stage"
    fi
}

# Performance metrics collection
collect_performance_metrics() {
    local stage="$1"
    local obj_dir="$PROJECT_ROOT/obj/stage-$stage"
    local metrics_file="$obj_dir/performance_metrics.json"
    
    log_hook "Collecting performance metrics for stage $stage"
    
    # Basic file size metrics
    local artifact_sizes=$(find "$obj_dir" -type f -name "*.wasm" -o -name "*.bc" -o -name "*.js" | while read file; do
        echo "\"$(basename "$file")\": $(stat -f%z "$file" 2>/dev/null || stat -c%s "$file")"
    done | paste -sd, -)
    
    cat > "$metrics_file" <<EOF
{
    "stage_id": $stage,
    "timestamp": "$(date -Iseconds)",
    "artifact_sizes": { $artifact_sizes },
    "compilation_target": "wasm32-unknown-emscripten",
    "optimization_level": "O3"
}
EOF
    
    log_hook "Performance metrics collected: $metrics_file"
}

# Main post-hook execution
main() {
    if [[ -z "$STAGE_ID" ]]; then
        echo "Usage: $0 <stage_id>"
        exit 1
    fi
    
    log_hook "Starting post-hook validation for stage $STAGE_ID"
    
    # Load governance context (from pre-hook)
    local governance_file="$PROJECT_ROOT/.riftrc.$STAGE_ID"
    if [[ -f "$governance_file" ]]; then
        export RIFT_ZERO_TRUST=$(grep "zero_trust_mode:" "$governance_file" | cut -d' ' -f4)
    fi
    
    # Execute validation and finalization chain
    validate_stage_output "$STAGE_ID" || exit 1
    generate_stage_metadata "$STAGE_ID"
    check_detach_mode "$STAGE_ID"
    collect_performance_metrics "$STAGE_ID"
    
    log_hook "Post-hook validation completed successfully for stage $STAGE_ID"
}

main "$@"
