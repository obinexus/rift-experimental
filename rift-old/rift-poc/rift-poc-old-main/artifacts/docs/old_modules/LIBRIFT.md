```c

librift/
├── CMakeLists.txt                 # Main build configuration
├── README.md                      # Project documentation
├── LICENSE                        # License information
├── .gitignore
├── docs/                          # Documentation
│   ├── api/                       # API documentation
│   ├── examples/                  # Example usage docs
│   └── design/                    # Design documents
│
├── src/                           # Source code
│   ├── core/                      # Core RIFT functionality
│   │   ├── parser/               # Data-oriented parser implementation
│   │   │   ├── data_stream.h     # Data stream handling
│   │   │   ├── parser_state.h    # Immutable parser state
│   │   │   ├── rule_engine.h     # Rule processing engine
│   │   │   └── validators.h      # Rule validation
│   │   │
│   │   ├── ir/                   # Intermediate representation
│   │   │   ├── ir_builder.h      # Functional IR builder
│   │   │   ├── optimizers.h      # Pure optimization passes
│   │   │   └── ir_types.h        # IR type definitions
│   │   │
│   │   └── common/               # Shared core utilities
│   │       ├── types.h           # Common type definitions
│   │       └── errors.h          # Error handling
│   │
│   ├── function/                 # Function composition
│   │   ├── composer.h            # Function composition engine
│   │   ├── transforms.h          # Pure transformations
│   │   └── point_free.h         # Point-free rules
│   │
│   ├── dataflow/                 # Data flow analysis
│   │   ├── analyzer.h            # Flow analysis engine
│   │   ├── patterns.h            # Pattern matching
│   │   └── graph.h              # Immutable graph implementation
│   │
│   └── utils/                    # Utility functions
│       ├── immutable.h           # Immutable data structures
│       ├── functional.h          # Functional programming utilities
│       └── validation.h          # Validation helpers
│
├── include/                       # Public API headers
│   └── rift/                     # Main include directory
│       ├── parser.h              # Parser interface
│       ├── ir.h                  # IR interface
│       ├── composer.h            # Composer interface
│       └── analyzer.h            # Analyzer interface
│
├── tests/                        # Test suite
│   ├── unit/                     # Unit tests
│   │   ├── parser_tests/        # Parser unit tests
│   │   ├── ir_tests/           # IR unit tests
│   │   ├── composer_tests/     # Composer unit tests
│   │   └── analyzer_tests/     # Analyzer unit tests
│   │
│   ├── integration/             # Integration tests
│   └── performance/            # Performance benchmarks
│
├── examples/                     # Example code
│   ├── basic_parsing/          # Basic parsing examples
│   ├── ir_generation/         # IR generation examples
│   ├── function_composition/  # Function composition examples
│   └── flow_analysis/        # Flow analysis examples
│
├── tools/                       # Development tools
│   ├── build_scripts/         # Build helper scripts
│   └── analysis/             # Code analysis tools
│
└── third_party/               # Third-party dependencies
    └── README.md             # Dependency documentation
    ```