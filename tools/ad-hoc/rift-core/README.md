# RIFT Core - Foundational Layer

## Overview

RIFT Core is the foundational layer for the RIFT-N toolchain within the OBINexus project. It provides shared libraries, token definitions, thread models, and telemetry schemas for all RIFT stages.

## Architecture

### Directory Structure

```
rift-core/
├── include/         # Header files and shared definitions
│   └── rift-core/   # Core API headers
├── src/             # Core source implementations
├── build/           # Build artifacts
│   ├── debug/       # Debug build outputs
│   ├── prod/        # Production build outputs
│   ├── bin/         # Executable binaries
│   ├── obj/         # Object files
│   └── lib/         # Static/shared libraries
├── config/          # Governance and configuration files
├── tests/           # Test suites
└── docs/            # Documentation
```

## Key Features

### Thread Safety & Semantic Control

- **NULL/nil Semantics**: Automatic conversion of C-style NULL to RIFT nil
- **Yoda-Style Safety**: Enforced condition ordering to prevent assignment errors
- **Thread Lifecycle**: Bit-encoded state management for concurrent execution
- **Parity Elimination**: Alternative to mutex locks for parallel processing

### Audit & Telemetry

- **Audit Streams**: Separate tracking for stdin, stderr, stdout
- **State Verification**: Cryptographic hash validation
- **Telemetry Collection**: GUID, UUID, and hash-based tracing
- **Exception Classification**: Structured error level handling

### Governance Compliance

- **Policy Enforcement**: Compile-time validation of governance rules
- **Stage Validation**: Per-stage compliance checking
- **Traceability**: Complete audit trail for all operations

## Building

```bash
mkdir build && cd build
cmake ..
make
```

## Testing

```bash
make test
```

## Integration

RIFT Core is designed to be used by all RIFT-N stages (rift-0 through rift-6). Each stage links against the core library and inherits its governance and telemetry capabilities.

## Version

Current version: 0.1.0-alpha
Governance Schema: 1.0
Audit Schema: 1.0
