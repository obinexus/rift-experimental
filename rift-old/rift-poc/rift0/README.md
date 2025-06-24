# RIFT Stage 0: Enhanced Architecture Implementation

Part of the OBINexus RIFT POC implementing AST-Aware bytecode generation with AEGIS methodology and Zero Trust governance.

## Architecture Overview

RIFT Stage 0 implements the **AEGIS** (Automaton Engine for Generative Interpretation & Syntax) framework using formal automaton theory for systematic language processing.

### Core Components

1. **RegexAutomatonEngine** - 5-tuple automaton (Q, Σ, δ, q0, F)
2. **Pattern Parser** - RIFT syntax: `R"pattern/flags[mode]"`
3. **Token Generator** - Type/value separated IR tokens
4. **CLI System** - Professional command-line interface
5. **Build System** - Enhanced Makefile with architecture verification
6. **QA Framework** - Comprehensive testing and validation

### AEGIS Methodology Compliance

- **Token Type/Value Separation**: Enforced throughout pipeline
- **matched_state Preservation**: Maintained for AST minimization
- **Strict Compiler Compliance**: Zero warnings with -Werror
- **Memory Safety**: Comprehensive error handling and cleanup
- **Architectural Validation**: Automated verification scripts

## Quick Start

```bash
# Complete bootstrap and validation
./run.sh

# Manual build process
make bootstrap

# Run architecture validation
./scripts/validation/validate-architecture.sh
```

## CLI Usage

```bash
# Basic usage
./bin/rift0.exe input.rift

# With options
./bin/rift0.exe --verbose --output ./out input.rift

# Show help
./bin/rift0.exe --help
```

## Development Workflow

```bash
# Clean build
make clean

# Build all components
make all

# Run comprehensive tests
make test

# Verify architecture compliance
make verify-architecture

# Generate sample files
./scripts/generate-samples.sh
```

## Directory Structure

```
rift0/
├── src/core/          # Core AEGIS implementation
├── src/cli/           # Command-line interface
├── include/rift0/    # Public API headers
├── tests/qa_mocks/    # QA testing framework
├── scripts/           # Utility scripts
│   ├── build/         # Build automation
│   ├── validation/    # Architecture validation
│   └── deployment/    # Deployment scripts
├── examples/          # Sample RIFT programs
├── bin/              # Compiled executables
├── lib/              # Static libraries
├── out/              # Output artifacts
└── docs/             # Documentation
```

## Architecture Validation

The project includes automated validation to ensure AEGIS methodology compliance:

- **Token Architecture**: Type/value separation verification
- **Memory Safety**: Comprehensive cleanup validation
- **Compiler Compliance**: Strict warning enforcement
- **State Preservation**: matched_state integrity checks

## OBINexus Computing

"Structure IS the syntax." - RIFT implements systematic solutions to language engineering complexity through formal automaton theory and professional software development practices.

**Technical Implementation**: Production-ready codebase with comprehensive error handling, memory safety, and architectural validation.
