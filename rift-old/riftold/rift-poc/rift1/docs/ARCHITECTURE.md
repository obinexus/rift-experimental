# RIFT Stage 1 Architecture Documentation

## AEGIS Framework

The Automaton Engine for Generative Interpretation & Syntax (AEGIS) implements a formal approach to language processing using:

- **Finite State Automaton**: 5-tuple (Q, Σ, δ, q0, F) for token recognition
- **Pattern Matching**: POSIX regex for robust token classification  
- **Dual Parsing**: Bottom-up and top-down strategies for flexibility
- **AST Generation**: Tree construction with node optimization

## Component Architecture

### Core Library (`librift1.a`)
- `rift_parser.c` - Main parsing engine
- `rift_ast.c` - AST construction and manipulation
- `rift_automaton.c` - AEGIS automaton implementation
- `rift_token_memory.c` - Token memory management
- `rift_ir.c` - Intermediate representation handling

### CLI Interface (`rift1.exe`)
- `main.c` - Command-line interface
- `cli_args.c` - Argument processing
- `cli_interface.c` - User interaction

## OBINexus Integration

Part of the governance-first compilation architecture with:
- **Zero Trust**: Cryptographic validation at each stage
- **Build Orchestration**: nlink → polybuild integration
- **Quality Assurance**: Automated validation and testing
