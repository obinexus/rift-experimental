# RIFT Token Architecture: Memory-Type Associations

## Token Component Relationships

In the RIFT ecosystem, tokens follow a triplet structure where memory precedes type, which precedes value:

```
token = (token_memory, token_type, token_value)
```

## Memory-Type Associations

| Memory Type | Token Types (Classical) | Token Types (Quantum) | Value Binding |
|-------------|-------------------------|------------------------|---------------|
| `span<fixed>` | INT, ROLE, MASK, OP | *Not compatible* | Immediate (`:=`) |
| `span<row>` | INT, FLOAT, STRING | *Not compatible* | Immediate (`:=`) |
| `span<continuous>` | ARRAY, VECTOR, MAP | *Not compatible* | Immediate (`:=`) |
| `span<superposed>` | *Not compatible* | QBYTE, QROLE, QMATRIX | Deferred (`=:`) |
| `span<entangled>` | *Not compatible* | QBYTE, QROLE, QMATRIX | Deferred (`=:`) |
| `nil` | *Base initialization* | *Base initialization* | None |

## Governance Policies by Mode

| Feature | Classical Mode | Quantum Mode |
|---------|----------------|--------------|
| **Memory Declaration** | Must precede type declaration | Must precede type declaration |
| **Memory Alignment** | Fixed 4096-bit | Dynamic 8-qubit |
| **Type Checking** | Immediate, eager | Deferred, lazy |
| **Value Assignment** | Direct (`:=`) | Superposition (`=:`) |
| **Value Resolution** | Deterministic | Probabilistic (Bayesian DAG) |
| **Policy Enforcement** | At assignment time | At observation time |

## Value Assignment Examples

### Classical Mode

```rift
// Memory declaration first
align span<row> {
  direction: right -> left,
  bytes: 4096,
  type: continuous
}

// Type declaration second
type INT = { bit_width: 32, signed: true }

// Value assignment last
x := 42  // Immediate binding with type inference
```

### Quantum Mode

```rift
// Memory declaration first
align span<superposed> {
  direction: bidirectional,
  bytes: dynamic,
  type: entangled
}

// Type declaration second
type QINT = { 
  bit_width: 32, 
  signed: true,
  superposition: enabled
}

// Value assignment last
y =: superpose(1, 2, 3)  // Deferred binding
```

## Memory-Type Policy Enforcement

1. **Classical**: Memory must be aligned before type declaration, and type must be declared before value assignment. Policy enforcement happens immediately.

2. **Quantum**: Memory alignment and type declaration follow the same precedence, but policy enforcement is deferred until observation or explicit collapse.

Memory is not just storage-it's a governance contract that enforces how tokens can be created, accessed, and transformed throughout their lifecycle.
