# POCRIFT: A Flexible Token Parser and Translator

## How it works?

POCRIFT implements a regex-based automaton for flexible token parsing and translation. Here's the core functionality:

1. **Pattern Matching**
    - Uses regex patterns like `/[^A-Z0-9\b]/` to match tokens
    - Supports complex pattern recognition for:
      - Uppercase letters (A-Z)
      - Numbers (0-9)
      - Word boundaries
    - Implements state transitions based on pattern matches

2. **Components**
    - RegexAutomaton: Manages states and transitions
    - TokenNode: Represents parsed tokens
    - IRGenerator: Generates intermediate representation

3. **Features**
    - Flexible token parsing
    - Extensible state machine
    - Dynamic pattern matching
    - Memory-safe implementation

The purpose of POCRIFT is to provide a flexible translation layer that allows you to:
- Add/remove language features
- Handle deprecated syntax
- Modernize code without extensive refactoring
- Support rapid SDLC integration

For implementation details, see the source code in `pocrift.c`.
