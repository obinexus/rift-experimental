#!/bin/bash
# ===== scripts/rift-orchestrator.sh =====
# RIFT Scripts Orchestration System - Main Entry Point
# OBINexus Computing - Modular QA & TDD Component Architecture

set -euo pipefail

SCRIPTS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPTS_DIR")"

# Source component artifacts
source "$SCRIPTS_DIR/components/rift-artifacts.sh"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m'

log_info() { echo -e "${BLUE}ℹ️  $1${NC}"; }
log_success() { echo -e "${GREEN}✅ $1${NC}"; }
log_warning() { echo -e "${YELLOW}⚠️  $1${NC}"; }
log_error() { echo -e "${RED}❌ $1${NC}"; }
log_header() { echo -e "${PURPLE}$1${NC}"; }

# ===== Main Orchestration Function =====
main() {
    local command="${1:-help}"
    
    case "$command" in
        "bootstrap")
            run_bootstrap "${@:2}"
            ;;
        "qa")
            run_qa_suite "${@:2}"
            ;;
        "tdd")
            run_tdd_cycle "${@:2}"
            ;;
        "validate")
            run_validation "${@:2}"
            ;;
        "build")
            run_build_system "${@:2}"
            ;;
        "integrate")
            run_integration "${@:2}"
            ;;
        "status")
            show_project_status "${@:2}"
            ;;
        "help"|*)
            show_help
            ;;
    esac
}

# ===== Bootstrap Command =====
run_bootstrap() {
    local stage="$1"
    local mode="${2:-full}"
    
    if [[ -z "$stage" ]]; then
        log_error "Stage required for bootstrap"
        echo "Usage: $0 bootstrap <stage> [mode]"
        echo "Stages: rift0, rift1, rift3, rift4, rift5"
        exit 1
    fi
    
    log_header "🚀 RIFT $stage Bootstrap Orchestration"
    log_header "====================================="
    
    # Run the modular bootstrap
    "$SCRIPTS_DIR/bootstrap/rift-bootstrap.sh" "$stage" "$mode"
    
    log_success "Bootstrap orchestration complete for RIFT $stage"
}

# ===== QA Suite Command =====
run_qa_suite() {
    local stage="${1:-all}"
    
    log_header "🔍 RIFT QA Suite Orchestration"
    log_header "==============================="
    
    if [[ "$stage" == "all" ]]; then
        # Run QA for all available stages
        for stage_dir in "$PROJECT_ROOT"/rift*; do
            if [[ -d "$stage_dir" ]]; then
                local stage_name=$(basename "$stage_dir")
                run_stage_qa "$stage_name"
            fi
        done
    else
        run_stage_qa "$stage"
    fi
    
    log_success "QA suite orchestration complete"
}

run_stage_qa() {
    local stage="$1"
    local stage_dir="$PROJECT_ROOT/$stage"
    
    if [[ ! -d "$stage_dir" ]]; then
        log_warning "Stage $stage not found - skipping QA"
        return 0
    fi
    
    log_info "Running QA for $stage..."
    
    cd "$stage_dir"
    
    # Run QA validation if available
    if [[ -x "QA/validate_qa.sh" ]]; then
        ./QA/validate_qa.sh
    else
        log_warning "QA validation script not found for $stage"
    fi
    
    # Run build verification
    if [[ -f "Makefile" ]]; then
        make verify-architecture 2>/dev/null || log_warning "Architecture verification failed"
    fi
    
    cd "$PROJECT_ROOT"
}

# ===== TDD Cycle Command =====
run_tdd_cycle() {
    local stage="$1"
    local phase="${2:-full}"
    
    if [[ -z "$stage" ]]; then
        log_error "Stage required for TDD cycle"
        echo "Usage: $0 tdd <stage> [phase]"
        echo "Phases: red, green, refactor, full"
        exit 1
    fi
    
    log_header "🔄 RIFT $stage TDD Cycle Orchestration"
    log_header "======================================"
    
    local stage_dir="$PROJECT_ROOT/$stage"
    
    if [[ ! -d "$stage_dir" ]]; then
        log_error "Stage $stage not found"
        exit 1
    fi
    
    cd "$stage_dir"
    
    case "$phase" in
        "red")
            log_info "🔴 TDD RED Phase - Writing failing tests"
            make test 2>/dev/null || log_info "Tests failing as expected in RED phase"
            ;;
        "green")
            log_info "🟢 TDD GREEN Phase - Making tests pass"
            make test
            ;;
        "refactor")
            log_info "🔵 TDD REFACTOR Phase - Improving code quality"
            make test
            make verify-architecture
            ;;
        "full"|*)
            log_info "🔄 Full TDD Cycle"
            make tdd-cycle 2>/dev/null || {
                log_info "Running manual TDD cycle..."
                make clean
                make test 2>/dev/null || log_info "RED phase - tests failing"
                make all
                make test
                make verify-architecture
            }
            ;;
    esac
    
    cd "$PROJECT_ROOT"
    log_success "TDD cycle complete for $stage"
}

# ===== Validation Command =====
run_validation() {
    local scope="${1:-architecture}"
    
    log_header "🔍 RIFT Validation Orchestration"
    log_header "================================"
    
    case "$scope" in
        "architecture")
            validate_architecture
            ;;
        "obinexus")
            validate_obinexus_compliance
            ;;
        "pipeline")
            validate_pipeline_integrity
            ;;
        "all")
            validate_architecture
            validate_obinexus_compliance
            validate_pipeline_integrity
            ;;
        *)
            log_error "Unknown validation scope: $scope"
            echo "Scopes: architecture, obinexus, pipeline, all"
            exit 1
            ;;
    esac
    
    log_success "Validation orchestration complete"
}

validate_architecture() {
    log_info "Validating RIFT architecture compliance..."
    
    # Check for token type/value separation across all stages
    local violations=0
    
    for stage_dir in "$PROJECT_ROOT"/rift*; do
        if [[ -d "$stage_dir" ]]; then
            log_info "Checking $(basename "$stage_dir")..."
            
            # Check headers maintain separation
            if ! grep -q "char\* type;" "$stage_dir"/include/*/core/*.h 2>/dev/null; then
                log_warning "Missing type field in $(basename "$stage_dir") headers"
                ((violations++))
            fi
            
            if ! grep -q "char\* value;" "$stage_dir"/include/*/core/*.h 2>/dev/null; then
                log_warning "Missing value field in $(basename "$stage_dir") headers"
                ((violations++))
            fi
        fi
    done
    
    if [[ $violations -eq 0 ]]; then
        log_success "Architecture validation passed"
    else
        log_warning "Architecture validation found $violations issues"
    fi
}

validate_obinexus_compliance() {
    log_info "Validating OBINexus compliance..."
    
    # Check for Zero Trust markers
    local compliance_score=0
    local total_checks=4
    
    if grep -r "Zero Trust\|nlink\|polybuild" "$PROJECT_ROOT" >/dev/null 2>&1; then
        log_success "Zero Trust governance markers found"
        ((compliance_score++))
    else
        log_warning "Zero Trust governance markers missing"
    fi
    
    if grep -r "riftlang.*\.so\.a.*rift\.exe.*gosilang" "$PROJECT_ROOT" >/dev/null 2>&1; then
        log_success "Toolchain integration documented"
        ((compliance_score++))
    else
        log_warning "Toolchain integration missing"
    fi
    
    if grep -r "AEGIS\|Automaton Engine" "$PROJECT_ROOT" >/dev/null 2>&1; then
        log_success "AEGIS architecture references found"
        ((compliance_score++))
    else
        log_warning "AEGIS architecture references missing"
    fi
    
    if find "$PROJECT_ROOT" -name "*.sh" -path "*/scripts/*" | head -1 >/dev/null; then
        log_success "Modular scripts structure present"
        ((compliance_score++))
    else
        log_warning "Modular scripts structure missing"
    fi
    
    log_info "OBINexus compliance score: $compliance_score/$total_checks"
}

validate_pipeline_integrity() {
    log_info "Validating pipeline integrity..."
    
    # Check stage dependencies and file formats
    local stages=("rift0" "rift1" "rift3" "rift4" "rift5")
    local pipeline_intact=true
    
    for stage in "${stages[@]}"; do
        if [[ -d "$PROJECT_ROOT/$stage" ]]; then
            log_success "Stage $stage present"
        else
            log_warning "Stage $stage missing"
            pipeline_intact=false
        fi
    done
    
    if $pipeline_intact; then
        log_success "Pipeline integrity validated"
    else
        log_warning "Pipeline integrity compromised"
    fi
}

# ===== Build System Command =====
run_build_system() {
    local stage="${1:-all}"
    local target="${2:-all}"
    
    log_header "🔨 RIFT Build System Orchestration"
    log_header "=================================="
    
    if [[ "$stage" == "all" ]]; then
        # Build all stages in dependency order
        local build_order=("rift0" "rift1" "rift3" "rift4" "rift5")
        
        for build_stage in "${build_order[@]}"; do
            build_single_stage "$build_stage" "$target"
        done
    else
        build_single_stage "$stage" "$target"
    fi
    
    log_success "Build orchestration complete"
}

build_single_stage() {
    local stage="$1"
    local target="$2"
    local stage_dir="$PROJECT_ROOT/$stage"
    
    if [[ ! -d "$stage_dir" ]]; then
        log_warning "Stage $stage not found - skipping build"
        return 0
    fi
    
    log_info "Building $stage with target: $target"
    
    cd "$stage_dir"
    
    if [[ -f "Makefile" ]]; then
        make "$target"
    else
        log_warning "No Makefile found for $stage"
    fi
    
    cd "$PROJECT_ROOT"
}

# ===== Integration Command =====
run_integration() {
    local mode="${1:-pipeline}"
    
    log_header "🔗 RIFT Integration Orchestration"
    log_header "================================="
    
    case "$mode" in
        "pipeline")
            integrate_pipeline
            ;;
        "obinexus")
            integrate_obinexus
            ;;
        "toolchain")
            integrate_toolchain
            ;;
        *)
            log_error "Unknown integration mode: $mode"
            echo "Modes: pipeline, obinexus, toolchain"
            exit 1
            ;;
    esac
    
    log_success "Integration orchestration complete"
}

integrate_pipeline() {
    log_info "Integrating RIFT pipeline..."
    
    # TODO: Implement pipeline integration
    # - Verify stage outputs match next stage inputs
    # - Test end-to-end compilation
    # - Validate IR format compatibility
    
    log_info "Pipeline integration placeholder - TODO"
}

integrate_obinexus() {
    log_info "Integrating with OBINexus ecosystem..."
    
    # TODO: Implement OBINexus integration
    # - Setup Zero Trust cryptographic keys
    # - Configure nlink build orchestration
    # - Setup polybuild pipeline management
    
    log_info "OBINexus integration placeholder - TODO"
}

integrate_toolchain() {
    log_info "Integrating toolchain components..."
    
    # TODO: Implement toolchain integration
    # - riftlang.exe setup
    # - .so.a library generation
    # - rift.exe compilation
    # - gosilang integration
    
    log_info "Toolchain integration placeholder - TODO"
}

# ===== Status Command =====
show_project_status() {
    local detail="${1:-summary}"
    
    log_header "📊 RIFT Project Status"
    log_header "======================"
    
    # Count available stages
    local stage_count=0
    local built_stages=0
    local qa_enabled=0
    
    for stage_dir in "$PROJECT_ROOT"/rift*; do
        if [[ -d "$stage_dir" ]]; then
            ((stage_count++))
            local stage_name=$(basename "$stage_dir")
            
            echo ""
            log_info "Stage: $stage_name"
            
            # Check build status
            if [[ -f "$stage_dir/bin/${stage_name}.exe" ]]; then
                echo "  ✅ Built executable"
                ((built_stages++))
            else
                echo "  ⚠️  No executable found"
            fi
            
            # Check QA status
            if [[ -x "$stage_dir/QA/validate_qa.sh" ]]; then
                echo "  ✅ QA framework present"
                ((qa_enabled++))
            else
                echo "  ⚠️  QA framework missing"
            fi
            
            # Check for library
            if [[ -f "$stage_dir/lib/lib${stage_name}.a" ]]; then
                echo "  ✅ Static library built"
            else
                echo "  ⚠️  Static library missing"
            fi
            
            # Check TDD support
            if [[ -f "$stage_dir/tests/unit/test_tdd_cycle.c" ]]; then
                echo "  ✅ TDD suite present"
            else
                echo "  ⚠️  TDD suite missing"
            fi
            
            if [[ "$detail" == "verbose" ]]; then
                # Show file counts
                local c_files=$(find "$stage_dir/src" -name "*.c" 2>/dev/null | wc -l)
                local h_files=$(find "$stage_dir/include" -name "*.h" 2>/dev/null | wc -l)
                echo "  📁 Files: $c_files C sources, $h_files headers"
            fi
        fi
    done
    
    echo ""
    log_header "📈 Summary"
    echo "Stages found: $stage_count"
    echo "Built stages: $built_stages/$stage_count"
    echo "QA enabled: $qa_enabled/$stage_count"
    
    # Architecture compliance check
    echo ""
    log_header "🏗️  Architecture Compliance"
    if grep -r "char\* type.*char\* value" "$PROJECT_ROOT"/rift*/include 2>/dev/null | head -1 >/dev/null; then
        echo "✅ Token type/value separation maintained"
    else
        echo "⚠️  Token type/value separation needs verification"
    fi
    
    if grep -r "matched_state" "$PROJECT_ROOT"/rift*/include 2>/dev/null | head -1 >/dev/null; then
        echo "✅ AST optimization state preservation found"
    else
        echo "⚠️  AST optimization state preservation needs verification"
    fi
}

# ===== Help Command =====
show_help() {
    cat << 'EOF'
🏗️  RIFT Scripts Orchestration System
=====================================

Usage: ./scripts/rift-orchestrator.sh <command> [options]

Commands:
  bootstrap <stage> [mode]     Bootstrap a RIFT stage
                              Stages: rift0, rift1, rift3, rift4, rift5
                              Modes: full, headers, impl, qa

  qa [stage]                  Run QA validation suite
                              stage: specific stage or 'all'

  tdd <stage> [phase]         Run TDD development cycle
                              Phases: red, green, refactor, full

  validate [scope]            Run validation checks
                              Scopes: architecture, obinexus, pipeline, all

  build [stage] [target]      Build system orchestration
                              stage: specific stage or 'all'
                              target: all, clean, test, etc.

  integrate [mode]            Integration orchestration
                              Modes: pipeline, obinexus, toolchain

  status [detail]             Show project status
                              detail: summary, verbose

  help                        Show this help message

Examples:
  ./scripts/rift-orchestrator.sh bootstrap rift0 full
  ./scripts/rift-orchestrator.sh qa all
  ./scripts/rift-orchestrator.sh tdd rift1 full
  ./scripts/rift-orchestrator.sh validate architecture
  ./scripts/rift-orchestrator.sh build all
  ./scripts/rift-orchestrator.sh status verbose

Architecture:
  🏗️  OBINexus AEGIS - Modular component system
  🔗 Zero Trust governance with milestone investment
  📋 TDD & QA driven development with reusable artifacts
  🎯 Token type/value separation + matched_state preservation

Directory Structure:
  scripts/
  ├── rift-orchestrator.sh      # Main orchestration (this file)
  ├── bootstrap/
  │   └── rift-bootstrap.sh      # Modular bootstrap system
  ├── components/
  │   └── rift-artifacts.sh      # Reusable component templates
  ├── qa/
  │   ├── validate-pipeline.sh   # Pipeline validation
  │   └── compliance-check.sh    # OBINexus compliance
  └── build/
      ├── deploy.sh              # Deployment orchestration
      └── package.sh             # Packaging system

For detailed help on specific commands:
  ./scripts/rift-orchestrator.sh <command> --help
EOF
}

# ===== Main Execution =====
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi
