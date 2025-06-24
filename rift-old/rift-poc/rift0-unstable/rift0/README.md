# RIFT Stage 0: Enhanced Tokenization/Lexical Analysis

Part of the RIFT POC (Rapid Iteration Framework Technology) implementing AST-Aware bytecode generation with Zero Trust governance.

## Architecture

RIFT Stage 0 implements the **AEGIS** (Automaton Engine for Generative Interpretation & Syntax) framework for tokenization and lexical analysis using regex automaton theory.

### Core Components

1. **RegexAutomatonEngine** - 5-tuple automaton (Q, Σ, δ, q0, F)
2. **Pattern Parser** - RIFT syntax: `R"pattern/flags[mode]"`
3. **Token Generator** - IR-ready token stream
4. **CLI System** - Enhanced command-line interface
5. **Configuration System** - `.riftrc` support
6. **QA Framework** - Comprehensive edge case testing

### RIFT Pattern Syntax

```
R"regex_pattern/flags[mode]"
```

- **regex_pattern**: POSIX extended regex
- **flags**: `g` (global), `m` (multiline), `i` (case-insensitive)  
- **mode**: `t` (top-down matching) or `b` (bottom-up matching)

### CLI Usage

```bash
# Basic usage
./bin/rift0.exe input.rift

# With options
./bin/rift0.exe --verbose --output ./build input.rift

# With custom configuration
./bin/rift0.exe --config custom.riftrc input.rift

# Show help
./bin/rift0.exe --help
```

### Configuration (.riftrc)

```ini
# RIFT Configuration File
output_dir=./output
debug_mode=false
verbose=true
log_level=INFO
```

## Building

```bash
# Build CLI and demo
make all

# Build CLI only
make cli

# Build demo only  
make demo

# Run QA tests
make test

# Generate default config
make config

# Clean build
make clean

# Install system-wide
make install
```

## Quick Start

```bash
# Bootstrap and run everything
./run.sh
```

## Directory Structure (Enhanced Sinphase Compliant)

```
rift0/
├── src/                    # Core implementation
│   ├── rift_automaton.c   # Automaton engine
│   ├── rift_pattern.c     # Pattern parser
│   ├── rift_engine.c      # Token engine
│   ├── rift_config.c      # Configuration & IR
│   └── main.c             # Demo program
├── cli/                    # CLI interface
│   └── main.c             # CLI main program
├── include/rift0/         # Headers
│   └── rift.h            # Enhanced core API
├── tests/qa_mocks/        # QA framework
│   └── test_automaton.c  # Comprehensive tests
├── QA/                    # Documentation
│   └── edge_case_registry.md # 15 edge cases
├── examples/              # Sample files
│   └── sample.rift       # Example RIFT program
├── bin/                   # Executables
│   └── rift0.exe         # CLI executable
├── build/                 # Build output
├── config/               # Configuration templates
└── output/               # Default IR output
```

## IR Output Format (.rift.0)

```
# RIFT Intermediate Representation
stage=rift.0
version=1.0.0
source=examples/sample.rift
token_count=42

# Tokens
TOKEN 0: type="PATTERN_MATCH" value="function" pos=(1,1)
TOKEN 1: type="CHAR" value=" " pos=(1,9)
TOKEN 2: type="PATTERN_MATCH" value="hello" pos=(1,10)
...
```

## Compiler Compliance

Built with strict warning enforcement:
- `-std=c11` - C11 standard compliance
- `-Wall -Wextra -Wpedantic` - All warnings enabled
- `-Werror` - Warnings treated as errors  
- `-D_GNU_SOURCE` - POSIX function availability

## Integration

RIFT Stage 0 outputs `.rift.0` IR files ready for Stage 1 (Parsing/Grammar Analysis). The automaton-based approach enables:

- **Flexible Syntax**: Regex patterns instead of rigid grammars
- **Semantic Preservation**: Token metadata for IR generation  
- **Performance**: O(n) tokenization with state minimization
- **Security**: Input validation through pattern matching
- **Reliability**: Comprehensive error handling and memory management

## Zero Trust Pipeline

Future integration with cryptographic governance:

```bash
make zero-trust-init  # Generate RSA-4096 keys
make verify-keys      # Verify key integrity  
```

## Error Handling

Comprehensive error system with proper cleanup:
- `RIFT_SUCCESS` - Operation successful
- `RIFT_ERROR_NULL_POINTER` - Invalid parameters
- `RIFT_ERROR_MEMORY_ALLOCATION` - Out of memory
- `RIFT_ERROR_PATTERN_INVALID` - Invalid RIFT pattern
- `RIFT_ERROR_COMPILATION_FAILED` - Regex compilation failed
- `RIFT_ERROR_IO` - File I/O error

## QA Framework

15 documented edge cases covering:
- Pattern parsing edge cases
- Memory management failures
- Compiler warning elimination
- CLI and configuration edge cases
- IR generation edge cases

## OBINexus Computing

"Structure is the final syntax." - RIFT implements structural solutions to language engineering complexity through automaton theory and data-oriented design.

**Technical Implementation**: Zero-warning compilation with comprehensive error handling and memory safety.
