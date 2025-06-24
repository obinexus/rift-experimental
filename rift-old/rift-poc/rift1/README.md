# RIFT Stage 1: AEGIS Parser Implementation

Part of the OBINexus RIFT POC implementing AST-Aware bytecode generation with AEGIS methodology and Zero Trust governance.

## Architecture Overview

RIFT Stage 1 implements the **AEGIS** (Automaton Engine for Generative Interpretation & Syntax) framework using formal automaton theory for systematic language processing.

### Core Components

1. **RegexAutomatonEngine** - 5-tuple automaton (Q, Σ, δ, q0, F)
2. **Dual Parser Strategies** - Bottom-up (shift-reduce) and Top-down (recursive descent)
3. **Token Classification** - POSIX regex pattern matching
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
./bootstrap.sh

# Manual build process
make bootstrap

# Run architecture validation
./scripts/validation/validate_architecture.sh
```

## CLI Usage

```bash
# Basic usage
./bin/rift1.exe input.rift.0

# With options
./bin/rift1.exe --verbose --mode top-down --output ./out input.rift.0

# Show help
./bin/rift1.exe --help
```

## OBINexus Computing

"Structure IS the syntax." - RIFT implements systematic solutions to language engineering complexity through formal automaton theory and professional software development practices.

**Toolchain Integration**: riftlang.exe → .so.a → rift.exe → gosilang
**Build Orchestration**: nlink → polybuild
**Zero Trust Governance**: Milestone-based investment with #NoGhosting policy
