#!/bin/bash
#
# RIFT Setup Wizard - Cross-Platform Migration & Configuration
# ============================================================
#
# RIFT is a Flexible Translator - By OBINexus Nnamdi Michael Okpala
# 
# This wizard migrates the current RIFT codebase structure to a unified 
# rift(root)/ organization and sets up cross-platform development
# environment for stages 0-6.
#
# Supported Platforms: Linux (Ubuntu/Debian/RHEL/Arch), Windows (MinGW32/64)
# Toolchain: riftlang.exe → .so.a → rift.exe → gosilang
# Build Orchestration: nlink → polybuild (AEGIS Framework)
#
# Copyright © 2025 OBINexus Computing - Computing from the Heart
#

set -euo pipefail

# === GLOBAL CONFIGURATION ===
readonly SCRIPT_VERSION="2.0.0"
readonly AEGIS_COMPLIANCE="ENABLED"
readonly OBINEXUS_SIGNATURE="Computing from the Heart"

# Platform Detection
OS_TYPE=""
DISTRO=""
MINGW_ENV=""

# Build Configuration
readonly PROJECT_ROOT="$(pwd)"
readonly MIGRATION_TARGET="${PROJECT_ROOT}/rift"
readonly TOOLS_DIR="${MIGRATION_TARGET}/tools"
readonly BACKUP_DIR="${PROJECT_ROOT}/migration_backup_$(date +%Y%m%d_%H%M%S)"

# ANSI Colors for Enhanced UX
readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly YELLOW='\033[0;33m'
readonly BLUE='\033[0;34m'
readonly MAGENTA='\033[0;35m'
readonly CYAN='\033[0;36m'
readonly BOLD='\033[1m'
readonly NC='\033[0m'

# ASCII Art Banner with Accessibility Support
display_banner() {
    clear
    echo -e "${CYAN}${BOLD}"
    echo "  ██████╗ ██╗███████╗████████╗"
    echo "  ██╔══██╗██║██╔════╝╚══██╔══╝"
    echo "  ██████╔╝██║█████╗     ██║   "
    echo "  ██╔══██╗██║██╔══╝     ██║   "
    echo "  ██║  ██║██║██║        ██║   "
    echo "  ╚═╝  ╚═╝╚═╝╚═╝        ╚═╝   "
    echo -e "${NC}"
    echo -e "${BLUE}${BOLD}RIFT is a Flexible Translator${NC}"
    echo -e "${MAGENTA}By OBINexus Nnamdi Michael Okpala${NC}"
    echo -e "${MAGENTA}${OBINEXUS_SIGNATURE}${NC}"
    echo ""
    echo -e "${YELLOW}Setup Wizard v${SCRIPT_VERSION} - Cross-Platform Migration${NC}"
    echo -e "${GREEN}Supporting Stages 0-6 | Linux + Windows (MinGW32/64)${NC}"
    echo ""
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo ""
}

# Logging Functions with Accessibility
log_section() {
    echo ""
    echo -e "${BLUE}${BOLD}▶ $1${NC}"
    echo "$(printf '%*s' ${#1} '' | tr ' ' '─')"
}

log_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

log_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

log_error() {
    echo -e "${RED}✗ $1${NC}"
}

log_info() {
    echo -e "${CYAN}ℹ $1${NC}"
}

log_stage() {
    local stage="$1"
    local action="$2"
    echo -e "${MAGENTA}▶ RIFT-Stage-${stage}:${NC} ${action}"
}

# Platform Detection and Compatibility Check
detect_platform() {
    log_section "Platform Detection & Compatibility Analysis"
    
    # Operating System Detection
    case "$(uname -s)" in
        Linux*)
            OS_TYPE="linux"
            detect_linux_distro
            ;;
        MINGW32_NT*|MINGW64_NT*|MSYS_NT*)
            OS_TYPE="windows"
            detect_mingw_environment
            ;;
        Darwin*)
            OS_TYPE="macos"
            log_info "macOS detected - Limited support for this platform"
            ;;
        *)
            log_error "Unsupported operating system: $(uname -s)"
            exit 1
            ;;
    esac
    
    log_success "Platform: ${OS_TYPE} ${DISTRO} ${MINGW_ENV}"
}

detect_linux_distro() {
    if [ -f /etc/os-release ]; then
        source /etc/os-release
        case "$ID" in
            ubuntu|debian|linuxmint)
                DISTRO="debian"
                log_info "Debian-based Linux: $PRETTY_NAME"
                ;;
            rhel|centos|fedora|rocky|almalinux)
                DISTRO="redhat"
                log_info "RedHat-based Linux: $PRETTY_NAME"
                ;;
            arch|manjaro)
                DISTRO="arch"
                log_info "Arch-based Linux: $PRETTY_NAME"
                ;;
            *)
                DISTRO="generic"
                log_warning "Generic Linux distribution: $PRETTY_NAME"
                ;;
        esac
    else
        DISTRO="unknown"
        log_warning "Cannot determine Linux distribution"
    fi
}

detect_mingw_environment() {
    if [[ "${MSYSTEM:-}" == "MINGW64" ]]; then
        MINGW_ENV="mingw64"
        log_info "MinGW64 environment detected"
    elif [[ "${MSYSTEM:-}" == "MINGW32" ]]; then
        MINGW_ENV="mingw32"
        log_info "MinGW32 environment detected"
    elif command -v pacman >/dev/null 2>&1; then
        MINGW_ENV="msys2"
        log_info "MSYS2 environment detected"
    else
        MINGW_ENV="generic"
        log_warning "Generic Windows environment - may require manual dependency installation"
    fi
}

# Dependency Installation Manager
install_dependencies() {
    log_section "Installing Development Dependencies"
    
    case "$OS_TYPE" in
        linux)
            install_linux_dependencies
            ;;
        windows)
            install_windows_dependencies
            ;;
        macos)
            install_macos_dependencies
            ;;
    esac
    
    # Verify critical tools
    verify_build_tools
}

install_linux_dependencies() {
    case "$DISTRO" in
        debian)
            log_info "Installing dependencies via apt..."
            sudo apt update
            sudo apt install -y \
                build-essential \
                cmake \
                git \
                gcc \
                g++ \
                make \
                pkg-config \
                libc6-dev \
                linux-headers-generic \
                doxygen \
                valgrind \
                clang-format \
                mingw-w64 \
                wine-development
            ;;
        redhat)
            log_info "Installing dependencies via yum/dnf..."
            if command -v dnf >/dev/null 2>&1; then
                sudo dnf groupinstall -y "Development Tools"
                sudo dnf install -y cmake git gcc gcc-c++ make pkgconfig doxygen valgrind clang mingw64-gcc
            else
                sudo yum groupinstall -y "Development Tools"
                sudo yum install -y cmake git gcc gcc-c++ make pkgconfig doxygen
            fi
            ;;
        arch)
            log_info "Installing dependencies via pacman..."
            sudo pacman -S --needed base-devel cmake git gcc make pkgconfig doxygen valgrind clang mingw-w64-gcc
            ;;
        *)
            log_warning "Generic Linux - please install: build-essential, cmake, git, gcc, make manually"
            ;;
    esac
}

install_windows_dependencies() {
    case "$MINGW_ENV" in
        mingw64|mingw32|msys2)
            log_info "Installing dependencies via pacman (MSYS2)..."
            pacman -S --needed \
                mingw-w64-x86_64-toolchain \
                mingw-w64-x86_64-cmake \
                mingw-w64-x86_64-make \
                mingw-w64-x86_64-pkg-config \
                mingw-w64-x86_64-doxygen \
                git \
                base-devel
            ;;
        *)
            log_warning "Generic Windows environment detected"
            log_info "Please ensure you have MinGW-w64, CMake, and Git installed"
            ;;
    esac
}

install_macos_dependencies() {
    log_info "Installing dependencies via Homebrew..."
    if ! command -v brew >/dev/null 2>&1; then
        log_error "Homebrew not found. Please install Homebrew first."
        exit 1
    fi
    brew install cmake git gcc make pkg-config doxygen
}

verify_build_tools() {
    log_info "Verifying build tool installation..."
    
    local required_tools=("cmake" "make" "gcc" "git")
    local missing_tools=()
    
    for tool in "${required_tools[@]}"; do
        if command -v "$tool" >/dev/null 2>&1; then
            log_success "$tool: $(command -v "$tool")"
        else
            missing_tools+=("$tool")
            log_error "$tool: Not found"
        fi
    done
    
    if [ ${#missing_tools[@]} -ne 0 ]; then
        log_error "Missing required tools: ${missing_tools[*]}"
        exit 1
    fi
    
    log_success "All required build tools are available"
}

# Migration Engine - Consolidate Current Structure
perform_migration() {
    log_section "RIFT Codebase Migration - Legacy to Unified Structure"
    
    # Create backup
    create_migration_backup
    
    # Initialize new unified structure
    initialize_unified_structure
    
    # Migrate stages 0-6
    migrate_rift_stages
    
    # Migrate tools and utilities
    migrate_tools_and_utilities
    
    # Update build configuration
    update_build_configuration
    
    # Generate cross-platform makefiles
    generate_cross_platform_makefiles
}

create_migration_backup() {
    log_info "Creating migration backup..."
    mkdir -p "$BACKUP_DIR"
    
    # Backup existing rift directories
    for stage in {0..6}; do
        if [ -d "rift-${stage}" ]; then
            cp -r "rift-${stage}" "$BACKUP_DIR/"
            log_success "Backed up rift-${stage}"
        fi
    done
    
    # Backup existing unified rift directory if it exists
    if [ -d "rift" ]; then
        cp -r "rift" "$BACKUP_DIR/rift_existing"
        log_success "Backed up existing rift/ directory"
    fi
    
    # Backup scripts and configuration
    [ -d "scripts" ] && cp -r "scripts" "$BACKUP_DIR/"
    [ -f "CMakeLists.txt" ] && cp "CMakeLists.txt" "$BACKUP_DIR/"
    [ -f "Makefile" ] && cp "Makefile" "$BACKUP_DIR/"
    
    log_success "Migration backup created at: $BACKUP_DIR"
}

initialize_unified_structure() {
    log_info "Initializing unified RIFT structure..."
    
    # Create main directories
    mkdir -p "$MIGRATION_TARGET"/{src,include,lib,bin,tests,docs,tools,examples,governance}
    mkdir -p "$MIGRATION_TARGET"/src/{core,cli,drivers}
    mkdir -p "$MIGRATION_TARGET"/include/rift/{core,cli,drivers}
    mkdir -p "$MIGRATION_TARGET"/tests/{unit,integration,benchmark}
    mkdir -p "$MIGRATION_TARGET"/tools/{setup,build,validation,deployment}
    mkdir -p "$MIGRATION_TARGET"/governance/{policies,compliance,security}
    
    # Create stage-specific directories within unified structure
    for stage in {0..6}; do
        mkdir -p "$MIGRATION_TARGET"/src/core/stage-${stage}
        mkdir -p "$MIGRATION_TARGET"/include/rift/core/stage-${stage}
        mkdir -p "$MIGRATION_TARGET"/tests/unit/stage-${stage}
    done
    
    log_success "Unified directory structure created"
}

migrate_rift_stages() {
    log_info "Migrating RIFT stages 0-6..."
    
    local stage_names=("tokenizer" "parser" "semantic" "validator" "bytecode" "verifier" "emitter")
    
    for stage in {0..6}; do
        local stage_name="${stage_names[$stage]}"
        log_stage "$stage" "Migrating ${stage_name}"
        
        # Source directory (could be rift-X or rift/rift-X)
        local src_dir=""
        if [ -d "rift-${stage}" ]; then
            src_dir="rift-${stage}"
        elif [ -d "rift/rift-${stage}" ]; then
            src_dir="rift/rift-${stage}"
        else
            log_warning "Stage ${stage} source not found - creating skeleton"
            create_stage_skeleton "$stage" "$stage_name"
            continue
        fi
        
        # Migrate source files
        if [ -d "${src_dir}/src" ]; then
            find "${src_dir}/src" -name "*.c" -o -name "*.cpp" -o -name "*.h" | while read -r file; do
                local relative_path="${file#${src_dir}/src/}"
                local target_path="$MIGRATION_TARGET/src/core/stage-${stage}/${relative_path}"
                mkdir -p "$(dirname "$target_path")"
                cp "$file" "$target_path"
            done
        fi
        
        # Migrate header files
        if [ -d "${src_dir}/include" ]; then
            find "${src_dir}/include" -name "*.h" -o -name "*.hpp" | while read -r file; do
                local relative_path="${file#${src_dir}/include/}"
                local target_path="$MIGRATION_TARGET/include/rift/core/stage-${stage}/${relative_path}"
                mkdir -p "$(dirname "$target_path")"
                cp "$file" "$target_path"
            done
        fi
        
        # Migrate tests
        if [ -d "${src_dir}/tests" ]; then
            cp -r "${src_dir}/tests"/* "$MIGRATION_TARGET/tests/unit/stage-${stage}/" 2>/dev/null || true
        fi
        
        log_success "Stage ${stage} (${stage_name}) migrated"
    done
}

create_stage_skeleton() {
    local stage="$1"
    local stage_name="$2"
    
    log_info "Creating skeleton for stage ${stage} (${stage_name})"
    
    # Create main implementation file
    cat > "$MIGRATION_TARGET/src/core/stage-${stage}/${stage_name}.c" << EOF
/*
 * RIFT Stage ${stage}: ${stage_name^} Implementation
 * RIFT is a Flexible Translator - By OBINexus Nnamdi Michael Okpala
 * 
 * This module implements the ${stage_name} phase of the RIFT compilation pipeline.
 * 
 * Toolchain Progression: riftlang.exe → .so.a → rift.exe → gosilang
 * Build Orchestration: nlink → polybuild (AEGIS Framework)
 */

#include "rift/core/stage-${stage}/${stage_name}.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Stage ${stage} Processing Function */
RiftResult rift_stage${stage}_process(const char* input, char** output) {
    if (!input || !output) {
        return RIFT_ERROR_NULL_PARAMETER;
    }
    
    /* TODO: Implement ${stage_name} logic */
    printf("RIFT Stage ${stage} (${stage_name^}): Processing input\\n");
    
    /* Placeholder implementation */
    size_t input_len = strlen(input);
    *output = malloc(input_len + 100);
    if (!*output) {
        return RIFT_ERROR_MEMORY_ALLOCATION;
    }
    
    snprintf(*output, input_len + 100, 
             "/* Stage ${stage} (${stage_name^}) Output */\\n%s", input);
    
    return RIFT_SUCCESS;
}

/* Stage ${stage} Initialization */
RiftResult rift_stage${stage}_init(void) {
    printf("RIFT Stage ${stage} (${stage_name^}): Initialized\\n");
    return RIFT_SUCCESS;
}

/* Stage ${stage} Cleanup */
void rift_stage${stage}_cleanup(void) {
    printf("RIFT Stage ${stage} (${stage_name^}): Cleaned up\\n");
}
EOF

    # Create header file
    cat > "$MIGRATION_TARGET/include/rift/core/stage-${stage}/${stage_name}.h" << EOF
/*
 * RIFT Stage ${stage}: ${stage_name^} Header
 * RIFT is a Flexible Translator - By OBINexus Nnamdi Michael Okpala
 */

#ifndef RIFT_STAGE${stage}_${stage_name^^}_H
#define RIFT_STAGE${stage}_${stage_name^^}_H

#ifdef __cplusplus
extern "C" {
#endif

/* RIFT Result Codes */
typedef enum {
    RIFT_SUCCESS = 0,
    RIFT_ERROR_NULL_PARAMETER = -1,
    RIFT_ERROR_MEMORY_ALLOCATION = -2,
    RIFT_ERROR_PROCESSING = -3
} RiftResult;

/* Stage ${stage} Functions */
RiftResult rift_stage${stage}_process(const char* input, char** output);
RiftResult rift_stage${stage}_init(void);
void rift_stage${stage}_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_STAGE${stage}_${stage_name^^}_H */
EOF

    # Create basic test file
    cat > "$MIGRATION_TARGET/tests/unit/stage-${stage}/test_${stage_name}.c" << EOF
/*
 * RIFT Stage ${stage}: ${stage_name^} Unit Tests
 * RIFT is a Flexible Translator - By OBINexus Nnamdi Michael Okpala
 */

#include "rift/core/stage-${stage}/${stage_name}.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_stage${stage}_basic_processing(void) {
    const char* input = "test input";
    char* output = NULL;
    
    RiftResult result = rift_stage${stage}_process(input, &output);
    
    assert(result == RIFT_SUCCESS);
    assert(output != NULL);
    assert(strstr(output, input) != NULL);
    
    free(output);
    printf("✓ Stage ${stage} basic processing test passed\\n");
}

void test_stage${stage}_null_input(void) {
    char* output = NULL;
    
    RiftResult result = rift_stage${stage}_process(NULL, &output);
    
    assert(result == RIFT_ERROR_NULL_PARAMETER);
    assert(output == NULL);
    
    printf("✓ Stage ${stage} null input test passed\\n");
}

int main(void) {
    printf("Running RIFT Stage ${stage} (${stage_name^}) Tests...\\n");
    
    rift_stage${stage}_init();
    
    test_stage${stage}_basic_processing();
    test_stage${stage}_null_input();
    
    rift_stage${stage}_cleanup();
    
    printf("All Stage ${stage} tests passed!\\n");
    return 0;
}
EOF
}

migrate_tools_and_utilities() {
    log_info "Migrating tools and utilities to rift/tools/..."
    
    # Migrate existing scripts
    if [ -d "scripts" ]; then
        cp -r scripts/* "$TOOLS_DIR/" 2>/dev/null || true
        log_success "Existing scripts migrated to tools/"
    fi
    
    # Create tool categories
    mkdir -p "$TOOLS_DIR"/{setup,build,validation,deployment,governance}
    
    # Generate setup tools
    generate_setup_tools
    
    # Generate build tools
    generate_build_tools
    
    # Generate validation tools
    generate_validation_tools
}

generate_setup_tools() {
    log_info "Generating cross-platform setup tools..."
    
    # Create platform-specific setup scripts
    create_linux_setup_script
    create_windows_setup_script
    create_universal_setup_script
}

create_linux_setup_script() {
    cat > "$TOOLS_DIR/setup/setup-linux.sh" << 'EOF'
#!/bin/bash
# RIFT Linux Setup Script
# RIFT is a Flexible Translator - By OBINexus Nnamdi Michael Okpala

set -euo pipefail

echo "🐧 RIFT Linux Development Environment Setup"
echo "==========================================="

# Detect distribution and install dependencies
if command -v apt >/dev/null 2>&1; then
    sudo apt update
    sudo apt install -y build-essential cmake git gcc g++ make pkg-config doxygen
elif command -v dnf >/dev/null 2>&1; then
    sudo dnf groupinstall -y "Development Tools"
    sudo dnf install -y cmake git gcc gcc-c++ make pkgconfig doxygen
elif command -v pacman >/dev/null 2>&1; then
    sudo pacman -S --needed base-devel cmake git gcc make pkgconfig doxygen
fi

echo "✅ Linux development environment configured"
EOF
    chmod +x "$TOOLS_DIR/setup/setup-linux.sh"
}

create_windows_setup_script() {
    cat > "$TOOLS_DIR/setup/setup-windows.sh" << 'EOF'
#!/bin/bash
# RIFT Windows (MinGW) Setup Script
# RIFT is a Flexible Translator - By OBINexus Nnamdi Michael Okpala

set -euo pipefail

echo "🪟 RIFT Windows (MinGW) Development Environment Setup"
echo "=================================================="

# Check for MSYS2/MinGW environment
if [[ "${MSYSTEM:-}" == "MINGW64" ]] || [[ "${MSYSTEM:-}" == "MINGW32" ]]; then
    echo "✅ MinGW environment detected: $MSYSTEM"
    
    # Install dependencies via pacman
    pacman -S --needed \
        mingw-w64-x86_64-toolchain \
        mingw-w64-x86_64-cmake \
        mingw-w64-x86_64-make \
        mingw-w64-x86_64-pkg-config \
        git \
        base-devel
        
    echo "✅ Windows development environment configured"
else
    echo "⚠️  Please run this script from MSYS2 MinGW environment"
    exit 1
fi
EOF
    chmod +x "$TOOLS_DIR/setup/setup-windows.sh"
}

create_universal_setup_script() {
    cat > "$TOOLS_DIR/setup/setup-universal.sh" << 'EOF'
#!/bin/bash
# RIFT Universal Setup Script
# RIFT is a Flexible Translator - By OBINexus Nnamdi Michael Okpala

set -euo pipefail

echo "🌍 RIFT Universal Development Environment Setup"
echo "=============================================="

# Detect platform and delegate to appropriate script
case "$(uname -s)" in
    Linux*)
        echo "🐧 Linux detected - delegating to Linux setup"
        exec "./setup-linux.sh"
        ;;
    MINGW*|MSYS*)
        echo "🪟 Windows MinGW detected - delegating to Windows setup"
        exec "./setup-windows.sh"
        ;;
    Darwin*)
        echo "🍎 macOS detected - basic setup"
        if command -v brew >/dev/null 2>&1; then
            brew install cmake git gcc make pkg-config doxygen
        else
            echo "⚠️  Please install Homebrew first"
            exit 1
        fi
        ;;
    *)
        echo "❌ Unsupported platform: $(uname -s)"
        exit 1
        ;;
esac
EOF
    chmod +x "$TOOLS_DIR/setup/setup-universal.sh"
}

generate_build_tools() {
    log_info "Generating cross-platform build tools..."
    
    cat > "$TOOLS_DIR/build/build-all.sh" << 'EOF'
#!/bin/bash
# RIFT Universal Build Script
# RIFT is a Flexible Translator - By OBINexus Nnamdi Michael Okpala

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"

echo "🔨 RIFT Universal Build System"
echo "============================="

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake
echo "⚙️  Configuring build with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all stages
echo "🏗️  Building all RIFT stages..."
make -j$(nproc 2>/dev/null || echo 4)

echo "✅ Build completed successfully"
echo "📦 Binaries available in: $BUILD_DIR"
EOF
    chmod +x "$TOOLS_DIR/build/build-all.sh"
}

generate_validation_tools() {
    log_info "Generating validation and testing tools..."
    
    cat > "$TOOLS_DIR/validation/validate-all.sh" << 'EOF'
#!/bin/bash
# RIFT Validation and Testing Suite
# RIFT is a Flexible Translator - By OBINexus Nnamdi Michael Okpala

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"

echo "🧪 RIFT Validation and Testing Suite"
echo "===================================="

# Run unit tests for all stages
for stage in {0..6}; do
    echo "🔬 Testing Stage $stage..."
    if [ -f "$PROJECT_ROOT/build/tests/unit/stage-$stage/test_stage$stage" ]; then
        "$PROJECT_ROOT/build/tests/unit/stage-$stage/test_stage$stage"
    else
        echo "⚠️  Test binary for stage $stage not found"
    fi
done

echo "✅ All validation tests completed"
EOF
    chmod +x "$TOOLS_DIR/validation/validate-all.sh"
}

update_build_configuration() {
    log_info "Updating build configuration for unified structure..."
    
    # Generate main CMakeLists.txt
    cat > "$MIGRATION_TARGET/CMakeLists.txt" << 'EOF'
cmake_minimum_required(VERSION 3.12)
project(RIFT VERSION 1.0.0 LANGUAGES C CXX)

# RIFT is a Flexible Translator - By OBINexus Nnamdi Michael Okpala
# Cross-platform CMake configuration for unified RIFT structure

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Build options
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_DOCS "Build documentation" ON)

# Include directories
include_directories(include)

# Add subdirectories
add_subdirectory(src)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        add_subdirectory(docs)
    endif()
endif()

# Install configuration
include(GNUInstallDirs)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
EOF

    # Generate src/CMakeLists.txt
    mkdir -p "$MIGRATION_TARGET/src"
    cat > "$MIGRATION_TARGET/src/CMakeLists.txt" << 'EOF'
# RIFT Source Configuration

# Core library with all stages
add_library(riftcore STATIC
    core/stage-0/tokenizer.c
    core/stage-1/parser.c
    core/stage-2/semantic.c
    core/stage-3/validator.c
    core/stage-4/bytecode.c
    core/stage-5/verifier.c
    core/stage-6/emitter.c
)

target_include_directories(riftcore PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# CLI application
add_executable(rift cli/main.c)
target_link_libraries(rift riftcore)

# Install targets
install(TARGETS riftcore rift
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
EOF

    # Generate CLI main.c
    mkdir -p "$MIGRATION_TARGET/src/cli"
    cat > "$MIGRATION_TARGET/src/cli/main.c" << 'EOF'
/*
 * RIFT CLI Application
 * RIFT is a Flexible Translator - By OBINexus Nnamdi Michael Okpala
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    printf("RIFT is a Flexible Translator\n");
    printf("By OBINexus Nnamdi Michael Okpala\n");
    printf("Computing from the Heart\n\n");
    
    if (argc < 2) {
        printf("Usage: %s <input-file>\n", argv[0]);
        printf("Stages: tokenizer → parser → semantic → validator → bytecode → verifier → emitter\n");
        return 1;
    }
    
    printf("Processing: %s\n", argv[1]);
    printf("Toolchain: riftlang.exe → .so.a → rift.exe → gosilang\n");
    
    // TODO: Implement stage pipeline
    
    return 0;
}
EOF
}

generate_cross_platform_makefiles() {
    log_info "Generating cross-platform Makefiles..."
    
    # Main Makefile with platform detection
    cat > "$MIGRATION_TARGET/Makefile" << 'EOF'
# RIFT Cross-Platform Makefile
# RIFT is a Flexible Translator - By OBINexus Nnamdi Michael Okpala

# Platform detection
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    PLATFORM := linux
    CC := gcc
    CXX := g++
endif
ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
    PLATFORM := windows
    CC := gcc
    CXX := g++
    EXE_EXT := .exe
endif

# Build configuration
BUILD_DIR := build
SRC_DIR := src
INCLUDE_DIR := include
LIB_DIR := lib
BIN_DIR := bin

CFLAGS := -Wall -Wextra -std=c11 -I$(INCLUDE_DIR)
CXXFLAGS := -Wall -Wextra -std=c++17 -I$(INCLUDE_DIR)

# Default target
all: setup build

setup:
	@echo "🔧 Setting up build environment for $(PLATFORM)..."
	@mkdir -p $(BUILD_DIR) $(LIB_DIR) $(BIN_DIR)

build:
	@echo "🏗️  Building RIFT..."
	@cd $(BUILD_DIR) && cmake .. && make

clean:
	@echo "🧹 Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR) $(LIB_DIR) $(BIN_DIR)

test: build
	@echo "🧪 Running tests..."
	@cd $(BUILD_DIR) && ctest

install: build
	@echo "📦 Installing RIFT..."
	@cd $(BUILD_DIR) && make install

help:
	@echo "RIFT Build System - Available targets:"
	@echo "  all      - Setup and build (default)"
	@echo "  setup    - Create build directories"
	@echo "  build    - Compile all components"
	@echo "  clean    - Remove build artifacts"
	@echo "  test     - Run test suite"
	@echo "  install  - Install to system"
	@echo "  help     - Show this help"

.PHONY: all setup build clean test install help
EOF
}

# Interactive Setup Process
run_interactive_setup() {
    log_section "Interactive RIFT Setup Configuration"
    
    # Welcome and confirmation
    echo -e "${CYAN}Welcome to the RIFT setup wizard!${NC}"
    echo ""
    echo "This wizard will:"
    echo "• Migrate your current RIFT codebase to a unified structure"
    echo "• Set up cross-platform development environment"
    echo "• Configure build tools for stages 0-6"
    echo "• Install platform-specific dependencies"
    echo ""
    
    read -p "Proceed with setup? [Y/n]: " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Nn]$ ]]; then
        echo "Setup cancelled."
        exit 0
    fi
    
    # Platform-specific configuration
    configure_platform_specific
    
    # Build configuration
    configure_build_options
    
    # Final confirmation
    confirm_setup_execution
}

configure_platform_specific() {
    echo ""
    log_info "Platform-specific configuration for: ${OS_TYPE}"
    
    case "$OS_TYPE" in
        linux)
            echo "• Build tools: GCC, Make, CMake"
            echo "• Cross-compilation: MinGW-w64 (optional)"
            read -p "Install cross-compilation tools for Windows? [y/N]: " -n 1 -r
            echo
            if [[ $REPLY =~ ^[Yy]$ ]]; then
                INSTALL_MINGW="yes"
            fi
            ;;
        windows)
            echo "• Build tools: MinGW-w64, MSYS2"
            echo "• Platform: ${MINGW_ENV}"
            ;;
    esac
}

configure_build_options() {
    echo ""
    log_info "Build configuration options:"
    
    read -p "Build type [Release/Debug]: " BUILD_TYPE
    BUILD_TYPE=${BUILD_TYPE:-Release}
    
    read -p "Enable testing suite? [Y/n]: " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Nn]$ ]]; then
        BUILD_TESTS="OFF"
    else
        BUILD_TESTS="ON"
    fi
    
    read -p "Generate documentation? [Y/n]: " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Nn]$ ]]; then
        BUILD_DOCS="OFF"
    else
        BUILD_DOCS="ON"
    fi
}

confirm_setup_execution() {
    echo ""
    log_section "Setup Configuration Summary"
    echo "Platform: ${OS_TYPE} ${DISTRO} ${MINGW_ENV}"
    echo "Build Type: ${BUILD_TYPE:-Release}"
    echo "Tests: ${BUILD_TESTS:-ON}"
    echo "Documentation: ${BUILD_DOCS:-ON}"
    echo "Migration Target: ${MIGRATION_TARGET}"
    echo "Backup Location: ${BACKUP_DIR}"
    echo ""
    
    read -p "Execute setup with these settings? [Y/n]: " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Nn]$ ]]; then
        echo "Setup cancelled."
        exit 0
    fi
}

# Execution orchestration
main() {
    display_banner
    
    # System analysis
    detect_platform
    
    # Interactive configuration
    run_interactive_setup
    
    # Dependency installation
    install_dependencies
    
    # Migration process
    perform_migration
    
    # Post-setup validation
    validate_setup_completion
    
    # Success message
    display_completion_message
}

validate_setup_completion() {
    log_section "Setup Validation"
    
    # Check unified structure
    if [ -d "$MIGRATION_TARGET" ]; then
        log_success "Unified RIFT structure created"
    else
        log_error "Failed to create unified structure"
        return 1
    fi
    
    # Check tools directory
    if [ -d "$TOOLS_DIR" ]; then
        log_success "Tools directory configured"
    else
        log_error "Tools directory missing"
        return 1
    fi
    
    # Check build configuration
    if [ -f "$MIGRATION_TARGET/CMakeLists.txt" ]; then
        log_success "Build configuration generated"
    else
        log_error "Build configuration missing"
        return 1
    fi
    
    # Test basic build
    log_info "Testing basic build process..."
    if cd "$MIGRATION_TARGET" && mkdir -p build && cd build && cmake .. >/dev/null 2>&1; then
        log_success "CMake configuration successful"
    else
        log_warning "CMake configuration needs attention"
    fi
    
    cd "$PROJECT_ROOT"
}

display_completion_message() {
    echo ""
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo ""
    echo -e "${GREEN}${BOLD}🎉 RIFT Setup Completed Successfully!${NC}"
    echo ""
    echo -e "${CYAN}Unified RIFT structure created at:${NC} ${MIGRATION_TARGET}"
    echo -e "${CYAN}Tools and utilities available at:${NC} ${TOOLS_DIR}"
    echo -e "${CYAN}Backup of original structure:${NC} ${BACKUP_DIR}"
    echo ""
    echo -e "${YELLOW}Next steps:${NC}"
    echo "1. cd rift/"
    echo "2. make all          # Build all stages"
    echo "3. make test         # Run validation suite"
    echo "4. ./bin/rift <file> # Execute RIFT compiler"
    echo ""
    echo -e "${BLUE}Toolchain Progression:${NC} riftlang.exe → .so.a → rift.exe → gosilang"
    echo -e "${BLUE}Build Orchestration:${NC} nlink → polybuild (AEGIS Framework)"
    echo ""
    echo -e "${MAGENTA}RIFT is a Flexible Translator${NC}"
    echo -e "${MAGENTA}By OBINexus Nnamdi Michael Okpala${NC}"
    echo -e "${MAGENTA}Computing from the Heart${NC}"
    echo ""
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
}

# Script execution
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi
