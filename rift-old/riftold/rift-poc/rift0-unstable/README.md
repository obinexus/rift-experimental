# RIFT Stage 0: Tokenization/Lexical Analysis

Part of the RIFT POC (Rapid Iteration Framework Technology) implementing AST-Aware bytecode generation with Zero Trust governance.

## Architecture

RIFT Stage 0 implements the **AEGIS** (Automaton Engine for Generative Interpretation & Syntax) framework for tokenization and lexical analysis using regex automaton theory.

### Core Components

1. **RegexAutomatonEngine** - 5-tuple automaton (Q, Σ, δ, q0, F)
2. **Pattern Parser** - RIFT syntax: `R"pattern/flags[mode]"`
3. **Token Generator** - IR-ready token stream
4. **QA Framework** - Edge case validation and testing

### RIFT Pattern Syntax

```
R"regex_pattern/flags[mode]"
```

- **regex_pattern**: POSIX extended regex
- **flags**: `g` (global), `m` (multiline), `i` (case-insensitive)  
- **mode**: `t` (top-down matching) or `b` (bottom-up matching)

### Examples

```c
// Match uppercase identifiers (top-down)
R"[A-Z][A-Z0-9_]*/gmi[t]"

// Match whitespace (bottom-up)
R"\s+/gm[b]"

// Match special characters
R"[^A-Za-z0-9\s]+/g[t]"
```

## Building

```bash
# Build RIFT Stage 0
make

# Run demo
./build/rift0

# Run QA tests  
make test

# Clean build
make clean
```

## Quick Start

```bash
# Bootstrap and run
./run.sh
```

## Directory Structure (Sinphase Compliant)

```
rift0/
├── src/                    # Implementation
│   ├── main.c             # Demo program
│   ├── rift_automaton.c   # Automaton engine
│   ├── rift_pattern.c     # Pattern parser
│   └── rift_engine.c      # Token engine
├── include/rift0/         # Headers
│   └── rift.h            # Core API
├── tests/qa_mocks/        # QA framework
│   └── test_automaton.c  # Unit tests
├── QA/                    # Documentation
│   └── edge_case_registry.md
├── examples/              # Sample files
│   └── test.rift         # Example RIFT program
└── build/                 # Build output
```

## Integration

RIFT Stage 0 outputs tokens ready for Stage 1 (Parsing/Grammar Analysis). The automaton-based approach enables:

- **Flexible Syntax**: Regex patterns instead of rigid grammars
- **Semantic Preservation**: Token metadata for IR generation  
- **Performance**: O(n) tokenization with state minimization
- **Security**: Input validation through pattern matching

## Zero Trust Pipeline

Future integration with cryptographic governance:

```bash
make zero-trust-init  # Generate signing keys
make verify-keys      # Verify key integrity  
```

## OBINexus Computing

"Structure is the final syntax." - RIFT implements structural solutions to language engineering complexity through automaton theory and data-oriented design.
