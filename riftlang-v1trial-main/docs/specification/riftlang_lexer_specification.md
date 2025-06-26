# RIFTLang Lexer Specification: Classic and Quantum Mode Token System

## 1. Overview and Purpose

This specification defines the token system for the RIFTLang lexer with particular emphasis on the dual-mode processing capability that supports both classical and quantum computational paradigms. The lexer represents the initial phase in the compilation pipeline, responsible for converting source text into semantically meaningful tokens while maintaining memory safety, thread safety, and policy enforcement across both operational modes.

## 2. Token Structure Specification

### 2.1 Core Token Representation

The foundational token structure implements a memory-safe architecture with explicit bitfield constraints:

```
typedef struct {
    TokenType type;          // Token categorization
    TokenValue value;        // Semantic content
    TokenMemorySize mem_size; // Memory allocation boundaries
    uint32_t validation_bits; // 0x01: allocated, 0x02: initialized, 0x04: locked, 0x08: quantum_enabled
    LockContext* lock_ctx;    // Thread safety context
    QuantumState* q_state;    // Present only when in quantum mode
} Token;

```

Note the addition of the `q_state` pointer, which is only allocated when operating in quantum mode or when a token may potentially transition between modes.

### 2.2 Token Value Union

The `TokenValue` union provides type-specific storage with appropriate memory constraints:

```
typedef union {
    StringValue str;       // String literals
    NumericValue num;      // Numeric constants
    IdentifierValue ident; // Variable and function names
    KeywordValue keyword;  // Reserved language keywords
    OperatorValue op;      // Operation symbols
    QuantumValue quantum;  // Quantum-specific values (entangled/superposition)
} TokenValue;

```

Each member of this union follows memory-safe allocation patterns with validation at both compile-time and runtime.

## 3. Mode-Specific Token Processing

Quantum mode introduces a unique class of computational behavior that diverges sharply from classical interpretation. To support this, the lexer must validate and govern token structures using strict interface constructs designed to prevent semantic ambiguity and runtime failure.

### Quantum Interface Concepts:

* **Superposition**: Tokens carry multiple possible values prior to collapse.
* **Entanglement**: Tokens are linked in a shared state space with state resolution dependencies.
* **Distribution**: Token state spreads over qubit-aligned bitfields.
* **Cutting Mode**: Enables scoped resolution boundaries across nested quantum states.
* **Bit Alignment Contracts**: Quantum tokens are validated through byte-precise storage integrity.
* **BitLexPolicy Enforcement**: All token transitions are checked against schema to prevent decoherence or mismatched state transitions.
* **Interference Handling**: Quantum bitfields are validated for constructive or destructive interference patterns.

Each token processed in quantum mode must follow a compliance schema:

1. Validate interference patterns in the bit-level structure
2. Register entanglement bindings if adjacent or correlating tokens exist
3. Allocate `QuantumState` memory space based on declared amplitudes
4. Allow deferred type resolution until parser observation
5. Collapse or store resolved state explicitly with structural integrity

These constructs ensure that quantum-mode tokens do not introduce ungoverned behavior, and provide a robust framework for implementing mixed classical-quantum interfaces in a safe and repeatable manner.

### 3.1 Classic Mode Operation

In classic mode, tokens follow traditional lexical analysis patterns:

1. Pattern recognition using automaton-based matching
2. Type classification into standard categories (identifier, keyword, literal, etc.)
3. Memory allocation with strict boundary validation
4. Thread safety enforcement through the `LockContext`
5. Single-state deterministic resolution

Classic mode prioritizes memory safety and single-pass processing with no semantic ambiguity.

### 3.2 Quantum Mode Operation

Quantum mode extends the token system with four critical capabilities:

#### 3.2.1 Superposition

Tokens in superposition maintain multiple potential states simultaneously through the quantum state vector:

```
typedef struct {
    uint8_t state_vector[MAX_SUPERPOSITION_STATES];
    float amplitudes[MAX_SUPERPOSITION_STATES];
    uint8_t coherence_mask;
} QuantumState;

```

This enables:

* Delayed type resolution until observation
* Multiple concurrent token interpretations
* Context-dependent collapse based on surrounding tokens

#### 3.2.2 Entanglement

Entanglement creates persistent relationships between tokens:

```
struct EntanglementPair {
    Token* token_a;
    Token* token_b;
    EntanglementType type;  // CORRELATION, ANTI_CORRELATION, OBSERVER
    uint16_t strength;      // Binding strength (0-1000)
};

```

Entangled tokens:

* Share state information across lexical boundaries
* Coordinate type resolution when observed
* Maintain semantic relationships during AST construction

#### 3.2.3 Bit Alignment Contract

Quantum tokens adhere to strict bit alignment contracts:

* Each character is stored as one byte (8 qubits)
* Memory allocation occurs in quantum byte boundaries
* Bit structures support vectorized alignment for parallel processing
* Interference patterns are validated against schema definitions

#### 3.2.4 Distribution

Quantum tokens implement structured distribution:

* State possibilities mapped across qubit allocation
* Entropy balancing across tokens in the same context
* Probabilistic state resolution with deterministic outcomes

### 3.3 Mode Interaction

The token system handles transitions between classic and quantum modes through:

1. Mode detection markers: `<quantum>` and `</quantum>` delimiters
2. Implicit mode switching based on token context
3. Explicit quantization directives via policy declarations
4. Automatic classic-to-quantum promotion when encountering entanglement

Quantum mode takes precedence when dedicated through explicit declaration or when entanglement is detected in the token stream.

## 4. Lexer Interface

### 4.1 Core Interface Definition

```
// Token acquisition functions
Token* getNextToken(LexerContext* context);
Token* peekNextToken(LexerContext* context, int lookahead);
void rewindToken(LexerContext* context, int count);

// Mode control functions
void setLexerMode(LexerContext* context, LexerMode mode);
LexerMode getLexerMode(LexerContext* context);
bool isTokenQuantum(Token* token);

// Quantum-specific operations
void entangleTokens(Token* a, Token* b, EntanglementType type);
QuantumState* getTokenSuperposition(Token* token);
void collapseToken(Token* token, ObservationContext* context);

// Error handling functions
LexerError getLastError(LexerContext* context);
char* getErrorMessage(LexerError error);

```

### 4.2 Mode Transition Protocol

The interface implements a consistent protocol for handling transitions between classical and quantum modes:

1. **Detection Phase**: Analyze token pattern for quantum markers
2. **Preparation Phase**: Set up appropriate state context
3. **Allocation Phase**: Allocate memory with appropriate quantum capabilities
4. **Processing Phase**: Apply mode-specific pattern matching
5. **Validation Phase**: Verify token integrity according to mode requirements

### 4.3 Entanglement Handling

The entanglement protocol follows a strict procedure:

1. Register potential entanglement candidates during lexical analysis
2. Establish bidirectional references between tokens
3. Create entanglement metadata with relationship type and strength
4. Register the entanglement pair in the global entanglement registry
5. Apply appropriate memory barriers to ensure thread safety
6. Set up observation triggers for coordinated state resolution

## 5. Policy Framework Integration

### 5.0 Nil vs Null: Memory Assurance in Token Contracts

The lexer supports the RIFT philosophy of **memory protection through nil, not null**. A `nil` token state means "known but intentionally empty," as opposed to `null`, which is "unknown or unassigned."

* `nil` states are policy-compliant placeholders.
* Tokens in `nil` state must carry full type signatures and memory context.
* No token may compile unless its nil-handling behavior is declared in the policy scope.

This provides the basis for predictable and safe allocation across classic and quantum paths - avoiding compiler ambiguity while enforcing intention.

We stay. We listen. We compile what has been governed.
Gosi compiles based on `.rift` policies. These policies ensure safe threading, memory protection (`nil`, not `null`), and constraint adherence. Nothing compiles unless it meets governance rules.

### 5.1 BitLexPolicy Mechanism

The `BitLexPolicy` provides validation and enforcement across the token system:

```
typedef struct {
    PolicyScope scope;            // STATE, ACTION, RENDER
    PolicyConstraint constraints; // Bit-mapped validation rules
    PolicyAction actions;         // Triggered on constraint violation
    uint16_t validation_threshold; // Minimum acceptance threshold (0-1000)
} BitLexPolicy;

```

Policies validate:

* Token memory allocation boundaries
* State transitions between classic and quantum modes
* Entanglement relationship consistency
* Bit alignment contracts in quantum mode

### 5.2 Policy Application Points

The lexer applies policies at critical junctions:

1. Token creation
2. Mode transition
3. Entanglement establishment
4. Superposition resolution
5. Token reallocation or destruction

## 6. Implementation Requirements

### 6.1 Classic Mode Implementation

1. Implement the automaton-based pattern matching with raw string literals
2. Apply explicit memory boundaries through bitfield constraints
3. Enforce thread safety through the `LockContext`
4. Implement token lifecycle management for create/destroy/validate operations
5. Add use-after-free detection with policy enforcement

### 6.2 Quantum Mode Extensions

1. Implement `QuantumState` structure with superposition support
2. Create entanglement registry for token relationship tracking
3. Implement bit alignment validation for quantum tokens
4. Add interference pattern detection and validation
5. Implement probabilistic state resolution with deterministic outcomes

### 6.3 Critical Code Paths

1. Mode transition boundary handling
2. Entanglement establishment and maintenance
3. Superposition state vector management
4. Observation-triggered state collapse
5. Boundary validation at allocation and reallocation

## 7. Testing Requirements

### 7.1 Classic Mode Testing

1. Verify correct token type identification across all categories
2. Validate memory safety through boundary testing
3. Confirm thread safety in concurrent lexical analysis
4. Test error recovery and reporting
5. Validate pattern matching for all defined token types

### 7.2 Quantum Mode Testing

1. Verify correct superposition state maintenance
2. Test entanglement relationship consistency
3. Validate bit alignment adherence
4. Test observation-triggered collapse with expected outcomes
5. Verify interference pattern validation
6. Test mode transition integrity

### 7.3 Combined Testing

1. Verify mode precedence rules function correctly
2. Confirm policy enforcement across mode boundaries
3. Test entanglement across classic and quantum tokens
4. Validate memory safety in mixed-mode operations
5. Confirm thread safety in concurrent mixed-mode analysis

## 8. Integration with Parser

The token system integrates with the parser through:

1. Token stream generation with validation checks
2. Support for the bottom-up parser with BFS traversal
3. Quantum state consideration during AST construction
4. Entanglement relationship preservation in the AST
5. Mode-appropriate error recovery mechanisms

## 9. Documentation Requirements

1. Document the token structure with bitfield explanations
2. Detail quantum mode operations and entanglement protocols
3. Provide examples of mode transitions and token handling
4. Document policy integration and enforcement
5. Create comprehensive API references with pre/post conditions

## 10. Conclusion

This specification defines the RIFTLang lexer token system with dual-mode operation. It establishes the interface, entanglement mechanism, superposition handling, and mode transition protocols required for implementing the lexer component. The token system serves as the foundation for the entire RIFTLang compilation pipeline, with particular emphasis on preserving semantic intent across both classical and quantum computational paradigms.

The implementation must adhere to the RIFTer's Way philosophy that "Each token is a breath. Each breath is a truth," ensuring that tokens maintain semantic weight beyond mere syntax, particularly in quantum contexts where meaning may exist in superposition until observed.

