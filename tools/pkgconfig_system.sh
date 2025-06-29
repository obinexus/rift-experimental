#!/usr/bin/env bash
# =================================================================
# RIFT AEGIS Cross-Platform PKG-Config Orchestration System
# OBINexus Computing Framework - Technical Lead: Nnamdi Michael Okpala
# VERSION: 1.2.0-PRODUCTION
# FEATURES: Universal dependency management, platform-specific optimization
# =================================================================

set -e

# PKG-Config system configuration
PKGCONFIG_VERSION="1.2.0"
RIFT_PROJECT_NAME="rift-aegis"
RIFT_VERSION="1.2.0"

# Cross-platform detection and configuration
detect_platform() {
    local uname_output=$(uname -s)
    case "$uname_output" in
        Linux*)     echo "linux" ;;
        Darwin*)    echo "macos" ;;
        MINGW*|MSYS*|CYGWIN*) echo "windows" ;;
        *)          echo "unknown" ;;
    esac
}

PLATFORM=$(detect_platform)
PROJECT_ROOT="$(pwd)"
PKGCONFIG_DIR="$PROJECT_ROOT/pkgconfig"
BUILD_DIR="$PROJECT_ROOT/build"

# Platform-specific pkg-config paths and tools
case "$PLATFORM" in
    linux)
        PKG_CONFIG_CMD="pkg-config"
        PKG_CONFIG_PATHS="/usr/lib/pkgconfig:/usr/share/pkgconfig:/usr/local/lib/pkgconfig"
        LIB_PREFIX="lib"
        LIB_SUFFIX=".so"
        EXEC_SUFFIX=""
        ;;
    macos)
        PKG_CONFIG_CMD="pkg-config"
        PKG_CONFIG_PATHS="/usr/local/lib/pkgconfig:/opt/homebrew/lib/pkgconfig:/usr/lib/pkgconfig"
        LIB_PREFIX="lib"
        LIB_SUFFIX=".dylib"
        EXEC_SUFFIX=""
        # Check for Homebrew pkg-config
        if command -v brew >/dev/null 2>&1; then
            HOMEBREW_PREFIX=$(brew --prefix)
            PKG_CONFIG_PATHS="$HOMEBREW_PREFIX/lib/pkgconfig:$PKG_CONFIG_PATHS"
        fi
        ;;
    windows)
        PKG_CONFIG_CMD="pkg-config.exe"
        PKG_CONFIG_PATHS="/mingw64/lib/pkgconfig:/usr/lib/pkgconfig:/c/msys64/mingw64/lib/pkgconfig"
        LIB_PREFIX=""
        LIB_SUFFIX=".dll"
        EXEC_SUFFIX=".exe"
        ;;
    *)
        echo "Unsupported platform: $PLATFORM"
        exit 1
        ;;
esac

# Color codes for professional output
readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly BLUE='\033[0;34m'
readonly PURPLE='\033[0;35m'
readonly NC='\033[0m'

log_pkgconfig() {
    local level="$1"
    local message="$2"
    echo -e "${BLUE}[PKG-CONFIG-$level]${NC} $message"
}

log_success() {
    local message="$1"
    echo -e "${GREEN}[SUCCESS]${NC} $message"
}

log_error() {
    local message="$1"
    echo -e "${RED}[ERROR]${NC} $message"
}

# Initialize pkg-config system
initialize_pkgconfig_system() {
    log_pkgconfig "INIT" "Initializing cross-platform pkg-config system"
    log_pkgconfig "PLATFORM" "Target platform: $PLATFORM"
    
    # Create pkg-config directory structure
    mkdir -p "$PKGCONFIG_DIR"
    mkdir -p "$BUILD_DIR/pkgconfig"
    
    # Validate pkg-config availability
    if ! command -v "$PKG_CONFIG_CMD" >/dev/null 2>&1; then
        log_error "pkg-config not found. Installing for platform: $PLATFORM"
        install_pkgconfig_for_platform
    else
        local version=$($PKG_CONFIG_CMD --version)
        log_success "pkg-config found: version $version"
    fi
    
    # Set up environment
    export PKG_CONFIG_PATH="$PKG_CONFIG_PATHS:$PKGCONFIG_DIR:$PKG_CONFIG_PATH"
    log_pkgconfig "ENV" "PKG_CONFIG_PATH configured"
    
    generate_rift_pkgconfig_files
}

# Install pkg-config for specific platforms
install_pkgconfig_for_platform() {
    case "$PLATFORM" in
        linux)
            if command -v apt-get >/dev/null 2>&1; then
                sudo apt-get update && sudo apt-get install -y pkg-config
            elif command -v yum >/dev/null 2>&1; then
                sudo yum install -y pkgconfig
            elif command -v pacman >/dev/null 2>&1; then
                sudo pacman -S --noconfirm pkg-config
            else
                log_error "Cannot automatically install pkg-config on this Linux distribution"
                exit 1
            fi
            ;;
        macos)
            if command -v brew >/dev/null 2>&1; then
                brew install pkg-config
            else
                log_error "Homebrew not found. Please install Homebrew first"
                exit 1
            fi
            ;;
        windows)
            log_error "Please install pkg-config via MSYS2: pacman -S pkg-config"
            exit 1
            ;;
    esac
}

# Generate RIFT-specific pkg-config files
generate_rift_pkgconfig_files() {
    log_pkgconfig "GENERATE" "Creating RIFT AEGIS pkg-config files"
    
    # Generate main RIFT pkg-config file
    generate_main_rift_pkgconfig
    
    # Generate stage-specific pkg-config files
    local stages=(0 1 2 3 4 5 6)
    local stage_names=("tokenizer" "parser" "semantic" "validator" "bytecode" "verifier" "emitter")
    
    for i in "${!stages[@]}"; do
        local stage_id="${stages[$i]}"
        local stage_name="${stage_names[$i]}"
        generate_stage_pkgconfig "$stage_id" "$stage_name"
    done
    
    # Generate dependency validation file
    generate_dependency_validation
    
    log_success "All pkg-config files generated successfully"
}

# Generate main RIFT pkg-config file
generate_main_rift_pkgconfig() {
    local pkgconfig_file="$PKGCONFIG_DIR/rift-aegis.pc"
    
    cat > "$pkgconfig_file" << EOF
# RIFT AEGIS Main PKG-Config File
# Platform: $PLATFORM
# Version: $RIFT_VERSION

prefix=$PROJECT_ROOT
exec_prefix=\${prefix}
libdir=\${exec_prefix}/lib
includedir=\${prefix}/include
bindir=\${exec_prefix}/bin

Name: RIFT AEGIS
Description: RIFT AEGIS Flexible Translator Framework
Version: $RIFT_VERSION
URL: https://github.com/obinexus/rift-aegis

# Platform-specific library configuration
Libs: -L\${libdir}$(generate_platform_libs)
Libs.private: $(generate_platform_private_libs)
Cflags: -I\${includedir} -I\${includedir}/rift/core$(generate_platform_cflags)

# Dependencies
Requires: $(generate_platform_requires)
Requires.private: $(generate_platform_requires_private)
EOF
    
    log_success "Main pkg-config file created: $pkgconfig_file"
}

# Generate platform-specific library flags
generate_platform_libs() {
    case "$PLATFORM" in
        linux|macos)
            echo " -lrift-core -lrift-bridge"
            ;;
        windows)
            echo " -lrift-core -lrift-bridge -lws2_32"
            ;;
    esac
}

generate_platform_private_libs() {
    case "$PLATFORM" in
        linux)
            echo "-lpthread -ldl -lm"
            ;;
        macos)
            echo "-lpthread -lm"
            ;;
        windows)
            echo "-lpthread -lws2_32 -ladvapi32"
            ;;
    esac
}

generate_platform_cflags() {
    case "$PLATFORM" in
        linux)
            echo " -D_GNU_SOURCE -D_REENTRANT"
            ;;
        macos)
            echo " -D_DARWIN_C_SOURCE"
            ;;
        windows)
            echo " -D_WIN32_WINNT=0x0601 -DWIN32_LEAN_AND_MEAN"
            ;;
    esac
}

generate_platform_requires() {
    case "$PLATFORM" in
        linux|macos)
            echo "openssl >= 1.1.0, zlib >= 1.2.0"
            ;;
        windows)
            echo "openssl >= 1.1.0"
            ;;
    esac
}

generate_platform_requires_private() {
    case "$PLATFORM" in
        linux)
            echo "libcrypto, libssl"
            ;;
        macos)
            echo "libcrypto, libssl"
            ;;
        windows)
            echo "libcrypto, libssl, wsock32"
            ;;
    esac
}

# Generate stage-specific pkg-config files
generate_stage_pkgconfig() {
    local stage_id="$1"
    local stage_name="$2"
    local pkgconfig_file="$PKGCONFIG_DIR/rift-stage-$stage_id.pc"
    
    cat > "$pkgconfig_file" << EOF
# RIFT AEGIS Stage $stage_id ($stage_name) PKG-Config File
# Platform: $PLATFORM
# Version: $RIFT_VERSION

prefix=$PROJECT_ROOT
exec_prefix=\${prefix}
libdir=\${exec_prefix}/lib
includedir=\${prefix}/include
bindir=\${exec_prefix}/bin
stage_name=$stage_name
stage_id=$stage_id

Name: RIFT Stage $stage_id - $stage_name
Description: RIFT AEGIS Stage $stage_id: $stage_name processing module
Version: $RIFT_VERSION
URL: https://github.com/obinexus/rift-aegis

# Stage-specific configuration
Libs: -L\${libdir} -lrift-$stage_id
Cflags: -I\${includedir} -I\${includedir}/rift/core/stage-$stage_id

# Dependencies on previous stages
$(generate_stage_dependencies "$stage_id")
EOF
    
    log_pkgconfig "STAGE" "Generated pkg-config for stage $stage_id ($stage_name)"
}

# Generate stage dependencies
generate_stage_dependencies() {
    local stage_id="$1"
    
    if [ "$stage_id" -eq 0 ]; then
        echo "Requires: rift-aegis"
    else
        local prev_stage=$((stage_id - 1))
        echo "Requires: rift-stage-$prev_stage, rift-aegis"
    fi
}

# Generate dependency validation script
generate_dependency_validation() {
    local validation_script="$BUILD_DIR/pkgconfig/validate_dependencies.sh"
    
    cat > "$validation_script" << 'EOF'
#!/usr/bin/env bash
# RIFT AEGIS Dependency Validation Script

set -e

PLATFORM=$(uname -s | tr '[:upper:]' '[:lower:]')
case "$PLATFORM" in
    linux*)     PLATFORM="linux" ;;
    darwin*)    PLATFORM="macos" ;;
    mingw*|msys*|cygwin*) PLATFORM="windows" ;;
esac

echo "Validating RIFT AEGIS dependencies for platform: $PLATFORM"

# Required dependencies
REQUIRED_DEPS=("openssl" "zlib")
OPTIONAL_DEPS=("libcurl" "sqlite3")

# Validate required dependencies
for dep in "${REQUIRED_DEPS[@]}"; do
    if pkg-config --exists "$dep"; then
        version=$(pkg-config --modversion "$dep")
        echo "✅ $dep: $version"
    else
        echo "❌ $dep: NOT FOUND"
        exit 1
    fi
done

# Check optional dependencies
for dep in "${OPTIONAL_DEPS[@]}"; do
    if pkg-config --exists "$dep"; then
        version=$(pkg-config --modversion "$dep")
        echo "✅ $dep (optional): $version"
    else
        echo "⚠️  $dep (optional): NOT FOUND"
    fi
done

# Validate RIFT-specific packages
RIFT_STAGES=(0 1 2 3 4 5 6)
for stage in "${RIFT_STAGES[@]}"; do
    if pkg-config --exists "rift-stage-$stage"; then
        echo "✅ rift-stage-$stage: CONFIGURED"
    else
        echo "❌ rift-stage-$stage: NOT CONFIGURED"
    fi
done

echo "Dependency validation completed for $PLATFORM"
EOF
    
    chmod +x "$validation_script"
    log_success "Dependency validation script created: $validation_script"
}

# Test pkg-config configuration
test_pkgconfig_configuration() {
    log_pkgconfig "TEST" "Testing pkg-config configuration"
    
    # Test main RIFT package
    if $PKG_CONFIG_CMD --exists rift-aegis; then
        log_success "rift-aegis pkg-config: VALID"
        
        # Test compilation flags
        local cflags=$($PKG_CONFIG_CMD --cflags rift-aegis)
        local libs=$($PKG_CONFIG_CMD --libs rift-aegis)
        
        log_pkgconfig "TEST" "CFLAGS: $cflags"
        log_pkgconfig "TEST" "LIBS: $libs"
    else
        log_error "rift-aegis pkg-config: INVALID"
        return 1
    fi
    
    # Test stage packages
    local stages=(0 1 2 3 4 5 6)
    for stage in "${stages[@]}"; do
        if $PKG_CONFIG_CMD --exists "rift-stage-$stage"; then
            log_success "rift-stage-$stage pkg-config: VALID"
        else
            log_error "rift-stage-$stage pkg-config: INVALID"
        fi
    done
    
    # Run dependency validation
    if [ -x "$BUILD_DIR/pkgconfig/validate_dependencies.sh" ]; then
        "$BUILD_DIR/pkgconfig/validate_dependencies.sh"
    fi
    
    log_success "PKG-Config configuration testing completed"
}

# Generate platform-specific installation guide
generate_installation_guide() {
    local guide_file="$BUILD_DIR/pkgconfig/INSTALLATION_GUIDE_$PLATFORM.md"
    
    cat > "$guide_file" << EOF
# RIFT AEGIS PKG-Config Installation Guide - $PLATFORM

## Platform: $PLATFORM

### Prerequisites

EOF
    
    case "$PLATFORM" in
        linux)
            cat >> "$guide_file" << EOF
For Ubuntu/Debian:
\`\`\`bash
sudo apt-get update
sudo apt-get install pkg-config libssl-dev zlib1g-dev
\`\`\`

For CentOS/RHEL:
\`\`\`bash
sudo yum install pkgconfig openssl-devel zlib-devel
\`\`\`

For Arch Linux:
\`\`\`bash
sudo pacman -S pkg-config openssl zlib
\`\`\`
EOF
            ;;
        macos)
            cat >> "$guide_file" << EOF
Install Homebrew (if not already installed):
\`\`\`bash
/bin/bash -c "\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
\`\`\`

Install dependencies:
\`\`\`bash
brew install pkg-config openssl zlib
\`\`\`
EOF
            ;;
        windows)
            cat >> "$guide_file" << EOF
Install MSYS2 and dependencies:
\`\`\`bash
# In MSYS2 terminal
pacman -S pkg-config mingw-w64-x86_64-openssl mingw-w64-x86_64-zlib
\`\`\`
EOF
            ;;
    esac
    
    cat >> "$guide_file" << EOF

### Environment Setup

Add to your shell profile (.bashrc, .zshrc, etc.):
\`\`\`bash
export PKG_CONFIG_PATH="$PKG_CONFIG_PATHS:\$PKG_CONFIG_PATH"
\`\`\`

### Validation

Run the dependency validation script:
\`\`\`bash
$BUILD_DIR/pkgconfig/validate_dependencies.sh
\`\`\`

### Usage Example

Compile with RIFT AEGIS:
\`\`\`bash
gcc \$(pkg-config --cflags rift-aegis) program.c \$(pkg-config --libs rift-aegis)
\`\`\`
EOF
    
    log_success "Installation guide created: $guide_file"
}

# Main execution function
main() {
    case "${1:-}" in
        --init)
            initialize_pkgconfig_system
            ;;
        --test)
            test_pkgconfig_configuration
            ;;
        --generate-guide)
            generate_installation_guide
            ;;
        --validate)
            if [ -x "$BUILD_DIR/pkgconfig/validate_dependencies.sh" ]; then
                "$BUILD_DIR/pkgconfig/validate_dependencies.sh"
            else
                log_error "Validation script not found. Run --init first."
                exit 1
            fi
            ;;
        --full-setup)
            initialize_pkgconfig_system
            test_pkgconfig_configuration
            generate_installation_guide
            ;;
        --help)
            echo "RIFT AEGIS Cross-Platform PKG-Config System v$PKGCONFIG_VERSION"
            echo "Usage: $0 [option]"
            echo ""
            echo "Options:"
            echo "  --init           Initialize pkg-config system"
            echo "  --test           Test pkg-config configuration"
            echo "  --validate       Validate dependencies"
            echo "  --generate-guide Generate platform installation guide"
            echo "  --full-setup     Complete setup and testing"
            echo "  --help           Show this help message"
            echo ""
            echo "Platform: $PLATFORM"
            ;;
        *)
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
}

# Execute main function with provided arguments
main "$@"
