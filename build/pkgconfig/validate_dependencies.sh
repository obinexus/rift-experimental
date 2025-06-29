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
