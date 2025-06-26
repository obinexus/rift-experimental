# RIFT Import Enforcement Guide
## OBINexus Computing - AEGIS Project Implementation
**Version:** 1.0.0-dev  
**Stage:** Implementation Gate  
**Classification:** Git-RAF Enforced  
**Author:** AEGIS Core Engineering Team  
**Date:** June 22, 2025

## Executive Summary

The RIFT Import Enforcement system implements cryptographically verified, slot-based module resolution with mandatory semantic versioning constraints. This specification ensures all module imports undergo governance validation while preventing URI injection vulnerabilities and maintaining deterministic dependency resolution across all compilation stages.

## Semantic Versioning Architecture (smever.x)

### Bracketed Syntax Requirements
All RIFT imports must utilize bracketed semantic versioning syntax to prevent URI injection attacks and ensure cryptographic verification of module dependencies.

#### Canonical Import Format
```rift
// ✅ CORRECT: Bracketed semantic versioning with slot resolution
from rift_std.core.sync[stable@1.4.2] import RiftLock
from rift_std.crypto.hash[beta@2.1.0-rc1] import Blake3Context
from rift_std.governance.policy[alpha@0.9.5-dev] import PolicyValidator

// ❌ FORBIDDEN: Unsafe @ symbol usage (URI injection vulnerability)
from rift_std.core.sync@stable-1.4.2 import RiftLock
from rift_std.crypto.hash@beta/2.1.0 import Blake3Context
```

#### Version Constraint Syntax
```rift
// Exact version constraint
from module.path[stable@1.4.2] import Component

// Compatible version range (semantic versioning rules)
from module.path[stable@^1.4.0] import Component  // >= 1.4.0, < 2.0.0
from module.path[beta@~2.1.0] import Component    // >= 2.1.0, < 2.2.0

// Pre-release channel specification
from module.path[alpha@1.0.0-rc1+build.123] import Component
```

## Slot-Hash Lookup Architecture

### Cryptographic Module Resolution
Each module import resolves through a cryptographically verified slot-hash lookup system that maps semantic versions to content-addressed storage.

```c
// Slot-hash lookup structure
typedef struct {
    char module_namespace[128];        // Fully qualified module namespace
    char version_constraint[32];       // Semantic version constraint string
    char channel_identifier[16];       // Release channel (stable/beta/alpha)
    uint8_t content_hash[32];          // BLAKE3 hash of module content
    uint8_t signature[64];             // Ed25519 signature of module metadata
    uint64_t build_timestamp;          // UTC timestamp of module build
} rift_module_slot_t;

// Module lookup cache structure
typedef struct {
    rift_module_slot_t slots[1024];    // Pre-allocated slot array
    uint32_t slot_count;               // Current number of populated slots
    uint8_t cache_integrity_hash[32];  // BLAKE3 hash of entire cache
    uint8_t cache_signature[64];       // Ed25519 signature of cache state
} rift_module_cache_t;
```

### Slot Resolution Protocol
```c
// Resolve module import to content hash
int rift_resolve_module_import(
    const char* namespace_path,
    const char* version_constraint, 
    const char* channel,
    rift_module_slot_t* resolved_slot
);

// Validate resolved module against governance policies
int rift_validate_module_governance(
    const rift_module_slot_t* slot,
    const uint8_t* policy_requirements,
    size_t policy_length
);
```

## Canonical Import Path Specifications

### Namespace Hierarchy Requirements
```
rift_std.{domain}.{subdomain}[{channel}@{version}]
├── rift_std.core.*           # Core language runtime components
├── rift_std.crypto.*         # Cryptographic primitives and protocols  
├── rift_std.governance.*     # Policy enforcement and validation
├── rift_std.io.*             # Input/output and networking
├── rift_std.concurrent.*     # Thread-safe concurrency primitives
└── rift_std.hardware.*       # Hardware abstraction layer
```

### Channel Classification System
```rift
// Stable channel - production-ready, governance-validated
from rift_std.core.types[stable@2.1.4] import SafeInteger

// Beta channel - feature-complete, pre-production validation
from rift_std.experimental.quantum[beta@1.0.0-beta.3] import QuantumContext

// Alpha channel - development features, governance review pending
from rift_std.research.neural[alpha@0.8.2-dev] import NetworkLayer

// Internal channel - organization-specific, restricted access
from obnx_internal.accounting.ledger[internal@1.2.1] import TransactionValidator
```

### Third-Party Module Integration
```rift
// External module import with cryptographic verification
from external.{org}.{module}[{channel}@{version}+{build_hash}] import Component

// Example: External cryptographic library
from external.sodium.crypto[stable@1.0.18+blake3.a1b2c3d4] import SecretBox

// Example: External d