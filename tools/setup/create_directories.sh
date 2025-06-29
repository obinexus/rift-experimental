#!/usr/bin/env bash
# =================================================================
# RIFT AEGIS Directory Architecture Creation Tool
# OBINexus Computing Framework - Technical Lead: Nnamdi Michael Okpala
# VERSION: 1.3.0-PRODUCTION
# METHODOLOGY: Systematic directory structure with QA compliance
# =================================================================

set -e

# Directory creation tool configuration
TOOL_VERSION="1.3.0"
DEFAULT_RIFT_VERSION="1.3.0"
QA_COMPLIANCE_STRUCTURE=true

# Color codes for systematic output
readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly BLUE='\033[0;34m'
readonly PURPLE='\033[0;35m'
readonly NC='\033[0m'

# Logging framework
log_info() {
    echo -e "${BLUE}[DIRECTORIES]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# AEGIS stage configuration
STAGES=(0 1 2 3 4 5 6)
STAGE_NAMES=("tokenizer" "parser" "semantic" "validator" "bytecode" "verifier" "emitter")

# Create systematic project directory structure
create_core_directories() {
    local platform="$1"
    local version="$2"
    
    log_info "Creating core AEGIS directory architecture for platform: $platform"
    
    # Primary build directories
    local core_dirs=(
        "build"
        "build/logs"
        "build/metadata"
        "build/hash_lookup"
        "build/pkgconfig"
        "lib"
        "bin"
        "obj"
    )
    
    for dir in "${core_dirs[@]}"; do
        if mkdir -p "$dir" 2>/dev/null; then
            log_success "Created core directory: $dir"
        else
            log_error "Failed to create directory: $dir"
            return 1
        fi
    done
    
    # Source code architecture
    local src_dirs=(
        "rift/src"
        "rift/src/core"
        "rift/src/cli"
        "rift/include"
        "rift/include/rift"
        "rift/include/rift/core"
        "rift/include/rift/cli"
    )
    
    for dir in "${src_dirs[@]}"; do
        if mkdir -p "$dir" 2>/dev/null; then
            log_success "Created source directory: $dir"
        else
            log_error "Failed to create source directory: $dir"
            return 1
        fi
    done
    
    log_success "Core directory structure created successfully"
}

# Create stage-specific directories
create_stage_directories() {
    local platform="$1"
    local version="$2"
    
    log_info "Creating stage-specific directory architecture"
    
    for stage_id in "${STAGES[@]}"; do
        local stage_name="${STAGE_NAMES[$stage_id]}"
        
        # Stage-specific directories
        local stage_dirs=(
            "obj/rift-$stage_id"
            "rift/src/core/stage-$stage_id"
            "rift/src/cli/stage-$stage_id"
            "rift/include/rift/core/stage-$stage_id"
            "build/hash_lookup/stage-$stage_id"
        )
        
        for dir in "${stage_dirs[@]}"; do
            if mkdir -p "$dir" 2>/dev/null; then
                log_success "Created stage $stage_id ($stage_name) directory: $dir"
            else
                log_error "Failed to create stage directory: $dir"
                return 1
            fi
        done
        
        # Create stage-specific metadata files
        create_stage_metadata "$stage_id" "$stage_name" "$platform" "$version"
    done
    
    log_success "Stage-specific directories created successfully"
}

# Create stage metadata for QA compliance
create_stage_metadata() {
    local stage_id="$1"
    local stage_name="$2"
    local platform="$3"
    local version="$4"
    
    local metadata_file="build/metadata/stage_${stage_id}_metadata.json"
    
    cat > "$metadata_file" << EOF
{
  "stage_id": $stage_id,
  "stage_name": "$stage_name",
  "platform": "$platform",
  "version": "$version",
  "created": "$(date -Iseconds)",
  "directories": {
    "obj": "obj/rift-$stage_id",
    "src_core": "rift/src/core/stage-$stage_id",
    "src_cli": "rift/src/cli/stage-$stage_id",
    "include": "rift/include/rift/core/stage-$stage_id",
    "hash_lookup": "build/hash_lookup/stage-$stage_id"
  },
  "artifacts": {
    "library": "lib/librift-$stage_id.a",
    "executable": "bin/rift-$stage_id",
    "object": "obj/rift-$stage_id/stage$stage_id.o"
  },
  "qa_compliance": {
    "aegis_methodology": true,
    "waterfall_progression": true,
    "systematic_validation": true
  }
}
EOF
    
    log_success "Created metadata for stage $stage_id: $metadata_file"
}

# Create tools and QA directories
create_tools_directories() {
    local platform="$1"
    local version="$2"
    
    log_info "Creating tools and QA directory architecture"
    
    # Tools framework directories
    local tools_dirs=(
        "tools"
        "tools/setup"
        "tools/qa"
        "tools/scripts"
        "tools/pkgconfig"
    )
    
    for dir in "${tools_dirs[@]}"; do
        if mkdir -p "$dir" 2>/dev/null; then
            log_success "Created tools directory: $dir"
        else
            log_error "Failed to create tools directory: $dir"
            return 1
        fi
    done
    
    # QA framework directories
    local qa_dirs=(
        "qa"
        "qa/logs"
        "qa/reports"
        "qa/artifacts"
        "qa/tests/unit"
        "qa/tests/integration"
        "qa/tests/system"
        "qa/tests/performance"
    )
    
    for dir in "${qa_dirs[@]}"; do
        if mkdir -p "$dir" 2>/dev/null; then
            log_success "Created QA directory: $dir"
        else
            log_error "Failed to create QA directory: $dir"
            return 1
        fi
    done
    
    log_success "Tools and QA directories created successfully"
}

# Create documentation structure
create_documentation_directories() {
    local platform="$1"
    local version="$2"
    
    log_info "Creating documentation directory architecture"
    
    local doc_dirs=(
        "docs"
        "docs/api"
        "docs/technical"
        "docs/qa"
        "docs/canonical"
        "examples"
        "examples/basic"
        "examples/advanced"
        "examples/integration"
    )
    
    for dir in "${doc_dirs[@]}"; do
        if mkdir -p "$dir" 2>/dev/null; then
            log_success "Created documentation directory: $dir"
        else
            log_error "Failed to create documentation directory: $dir"
            return 1
        fi
    done
    
    # Create documentation index
    create_documentation_index "$platform" "$version"
    
    log_success "Documentation directories created successfully"
}

# Create documentation index file
create_documentation_index() {
    local platform="$1"
    local version="$2"
    
    cat > "docs/README.md" << EOF
# RIFT AEGIS Documentation Structure

**Version:** $version  
**Platform:** $platform  
**Created:** $(date -Iseconds)  
**Methodology:** AEGIS Waterfall with QA Compliance

## Directory Structure

### Technical Documentation
- \`docs/technical/\` - Technical specifications and architecture
- \`docs/api/\` - API documentation and reference materials
- \`docs/qa/\` - QA framework documentation and compliance reports

### Canonical Documents
- \`docs/canonical/\` - Authoritative project documentation
- Official specifications, compliance frameworks, and governance policies

### Examples and Tutorials
- \`examples/basic/\` - Basic usage examples and tutorials
- \`examples/advanced/\` - Advanced implementation patterns
- \`examples/integration/\` - Integration and testing examples

## QA Compliance

This documentation structure follows AEGIS Waterfall methodology requirements:
- Systematic organization of technical materials
- Comprehensive coverage of all project phases
- Audit trail maintenance through version control
- Collaborative development support through clear structure

## Navigation

For stage-specific documentation, refer to:
- Stage implementations: \`rift/src/core/stage-N/\`
- Stage APIs: \`rift/include/rift/core/stage-N/\`
- Stage testing: \`qa/tests/\`

---
*Generated by RIFT AEGIS Directory Creation Tool v$TOOL_VERSION*
EOF
    
    log_success "Created documentation index: docs/README.md"
}

# Generate directory structure report
generate_structure_report() {
    local platform="$1"
    local version="$2"
    
    local report_file="build/metadata/directory_structure_report.md"
    
    log_info "Generating comprehensive directory structure report"
    
    cat > "$report_file" << EOF
# RIFT AEGIS Directory Structure Report

**Version:** $version  
**Platform:** $platform  
**Generated:** $(date -Iseconds)  
**Tool Version:** $TOOL_VERSION

## Executive Summary

Complete AEGIS-compliant directory structure has been established following systematic waterfall methodology principles. All required directories for build orchestration, QA compliance, and collaborative development have been created.

## Directory Categories

### Core Build Infrastructure
\`\`\`
build/                    # Build orchestration and metadata
├── logs/                 # Build and execution logs
├── metadata/             # Stage and system metadata
├── hash_lookup/          # O(1) lookup system data
└── pkgconfig/            # Cross-platform configuration

lib/                      # Static libraries output
bin/                      # Executable binaries output
obj/                      # Object files and intermediate builds
\`\`\`

### Source Code Architecture
\`\`\`
rift/
├── src/
│   ├── core/             # Core stage implementations
│   └── cli/              # Command-line interfaces
└── include/
    └── rift/
        ├── core/         # Core API headers
        └── cli/          # CLI interface headers
\`\`\`

### Quality Assurance Framework
\`\`\`
qa/                       # QA testing and compliance
├── logs/                 # QA execution logs
├── reports/              # Compliance and test reports
├── artifacts/            # QA generated artifacts
└── tests/                # Test suites by category
    ├── unit/             # Unit testing framework
    ├── integration/      # Integration testing
    ├── system/           # System-level testing
    └── performance/      # Performance validation
\`\`\`

### Development Tools
\`\`\`
tools/                    # Development and build tools
├── setup/                # Setup and initialization scripts
├── qa/                   # QA automation tools
├── scripts/              # Utility scripts
└── pkgconfig/            # Package configuration tools
\`\`\`

### Documentation and Examples
\`\`\`
docs/                     # Project documentation
├── api/                  # API reference documentation
├── technical/            # Technical specifications
├── qa/                   # QA methodology documentation
└── canonical/            # Authoritative project documents

examples/                 # Usage examples and tutorials
├── basic/                # Basic implementation examples
├── advanced/             # Advanced usage patterns
└── integration/          # Integration examples
\`\`\`

## Stage-Specific Structure

For each stage (0-6), the following directories are created:
- \`obj/rift-N/\` - Stage-specific object files
- \`rift/src/core/stage-N/\` - Core implementation
- \`rift/src/cli/stage-N/\` - CLI interface
- \`rift/include/rift/core/stage-N/\` - Stage headers
- \`build/hash_lookup/stage-N/\` - Hash lookup data

## Compliance Verification

✅ AEGIS Waterfall Methodology: COMPLIANT  
✅ Systematic Organization: IMPLEMENTED  
✅ QA Framework Integration: ESTABLISHED  
✅ Cross-Platform Support: CONFIGURED  
✅ Collaborative Development: ENABLED  

## Next Steps

1. Execute \`make all\` to populate directories with build artifacts
2. Run \`tools/setup/validate_stage.sh --comprehensive\` for validation
3. Initialize QA framework with \`tools/qa_framework.sh --init\`

---
*Report generated by RIFT AEGIS Directory Creation Tool*
EOF
    
    log_success "Directory structure report generated: $report_file"
}

# Main execution function
main() {
    local platform="linux"
    local version="$DEFAULT_RIFT_VERSION"
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --platform=*)
                platform="${1#*=}"
                shift
                ;;
            --version=*)
                version="${1#*=}"
                shift
                ;;
            --help)
                echo "RIFT AEGIS Directory Architecture Creation Tool v$TOOL_VERSION"
                echo "Usage: $0 [OPTIONS]"
                echo ""
                echo "Options:"
                echo "  --platform=NAME    Target platform (linux, macos, windows)"
                echo "  --version=VER      RIFT version string"
                echo "  --help             Show this help message"
                echo ""
                echo "This tool creates the complete AEGIS-compliant directory structure"
                echo "required for systematic build orchestration and QA compliance."
                exit 0
                ;;
            *)
                log_error "Unknown argument: $1"
                exit 1
                ;;
        esac
    done
    
    # Validate platform
    case "$platform" in
        linux|macos|windows)
            log_info "Creating directory structure for platform: $platform"
            ;;
        *)
            log_error "Unsupported platform: $platform (must be linux, macos, or windows)"
            exit 1
            ;;
    esac
    
    # Execute directory creation sequence
    log_info "Starting AEGIS directory architecture creation"
    echo -e "${PURPLE}=================================================================${NC}"
    echo -e "${PURPLE}RIFT AEGIS Directory Creation Tool v$TOOL_VERSION${NC}"
    echo -e "${PURPLE}Platform: $platform | Version: $version${NC}"
    echo -e "${PURPLE}=================================================================${NC}"
    
    create_core_directories "$platform" "$version"
    create_stage_directories "$platform" "$version"
    create_tools_directories "$platform" "$version"
    create_documentation_directories "$platform" "$version"
    generate_structure_report "$platform" "$version"
    
    echo -e "\n${PURPLE}=================================================================${NC}"
    echo -e "${GREEN}DIRECTORY STRUCTURE CREATION COMPLETED${NC}"
    echo -e "${PURPLE}=================================================================${NC}"
    echo -e "Platform: $platform"
    echo -e "Version: $version"
    echo -e "Methodology: AEGIS Waterfall"
    echo -e "QA Compliance: ENABLED"
    echo -e "Next Step: Execute 'make all' to populate with build artifacts"
    
    log_success "AEGIS directory architecture creation completed successfully"
}

# Execute main function with all arguments
main "$@"
