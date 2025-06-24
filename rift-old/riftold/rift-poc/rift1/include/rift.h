/**
 * @file rift.h
 * @brief RIFT Stage 1 Parser Engine - AEGIS Framework Implementation
 * @author OBINexus Computing
 * @version 1.0.0
 *
 * RIFT Stage 1 implements the parser component of the AEGIS (Automaton Engine
 * for Generative Interpretation & Syntax) framework. This stage consumes
 * .rift.0 token streams and produces .rift.1 enriched structures including
 * validated tokens, Abstract Syntax Trees (AST), and semantic annotations.
 *
 * AEGIS Methodology Compliance:
 * - Type/Value Separation: Enforced throughout token processing pipeline
 * - Stateful Automata: Regex-based pattern matching with state preservation
 * - Stage-Based Processing: .rift.0 → TokenMemory → Parser → .rift.1
 * - Compile-Time Safety: Explicit type contracts, no void* polymorphism
 * - Classical/Quantum Modes: Dual processing paradigms for
 * deterministic/probabilistic logic
 *
 * Architecture Flow:
 * Developer → .rift → Preprocessor → Compiler → Postprocessor → Target
 *                      ↓
 *             TokenMemory Bridge (.rift.0 → .rift.1)
 *                      ↓
 *             Parser Engine (Pattern Matching + AST Generation)
 *                      ↓
 *             Optimization Layer (State Minimization + Memory Efficiency)
 */

#ifndef RIFT1_H
#define RIFT1_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ===== FORWARD DECLARATIONS ===== */
typedef enum RiftResult_enum RiftResult;
typedef struct RiftPattern RiftPattern;
typedef struct RiftState RiftState;
typedef struct RiftTransition RiftTransition;
typedef struct RiftAutomaton RiftAutomaton;
typedef struct RiftToken RiftToken;
typedef struct RiftTokenMemory RiftTokenMemory;
typedef struct RiftASTNode RiftASTNode;
typedef struct Rift1ParseEngine Rift1ParseEngine;
typedef struct RiftConfig RiftConfig;
typedef struct RiftEngine RiftEngine;
typedef struct RiftIR RiftIR;

/* ===== RESULT CODES & ERROR HANDLING ===== */

/**
 * @brief RIFT operation result codes
 *
 * All RIFT operations return a RiftResult to ensure consistent error handling
 * and proper resource cleanup throughout the processing pipeline.
 */
enum RiftResult_enum {
  RIFT_SUCCESS = 0,                   /**< Operation completed successfully */
  RIFT_ERROR_NULL_POINTER = -1,       /**< Invalid null pointer argument */
  RIFT_ERROR_MEMORY_ALLOCATION = -2,  /**< Memory allocation failed */
  RIFT_ERROR_PATTERN_INVALID = -3,    /**< Invalid regex pattern syntax */
  RIFT_ERROR_COMPILATION_FAILED = -4, /**< Pattern compilation failed */
  RIFT_ERROR_IO = -5,                 /**< File I/O operation failed */
  RIFT_ERROR_PARSE_FAILED = -6,       /**< Token parsing failed */
  RIFT_ERROR_AST_INVALID = -7,        /**< AST structure validation failed */
  RIFT_ERROR_TOKEN_VALIDATION = -8,   /**< Token validation failed */
  RIFT_ERROR_STATE_TRANSITION = -9,   /**< Invalid automaton state transition */
  RIFT_ERROR_QUANTUM_ENTANGLEMENT = -10, /**< Quantum entanglement error */
  RIFT_ERROR_STAGE_MISMATCH = -11        /**< Incompatible stage processing */
};

/* ===== PROCESSING MODES ===== */

/**
 * @brief RIFT processing modes for classical and quantum paradigms
 */
typedef enum {
  RIFT_MODE_CLASSICAL = 0, /**< Deterministic, predictable processing */
  RIFT_MODE_QUANTUM = 1    /**< Probabilistic, superposition-based processing */
} RiftProcessingMode;

/**
 * @brief AST node types for syntax tree construction
 */
typedef enum {
  AST_NODE_PROGRAM = 0,           /**< Root program node */
  AST_NODE_DECLARATION,           /**< Variable/function declaration */
  AST_NODE_STATEMENT,             /**< Executable statement */
  AST_NODE_EXPRESSION,            /**< Expression evaluation */
  AST_NODE_IDENTIFIER,            /**< Named identifier */
  AST_NODE_LITERAL,               /**< Literal value */
  AST_NODE_OPERATOR,              /**< Operator node */
  AST_NODE_BLOCK,                 /**< Code block */
  AST_NODE_FUNCTION,              /**< Function definition */
  AST_NODE_QUANTUM_SUPERPOSITION, /**< Quantum superposition state */
  AST_NODE_ENTANGLED_IDENTITY,    /**< Quantum entangled identity */
  AST_NODE_MACRO_EXPANSION,       /**< Macro expansion result */
  AST_NODE_TYPE_VALUE_ASSOCIATION /**< Type-value binding */
} RiftASTNodeType;

/* ===== CORE AEGIS STRUCTURES ===== */

/**
 * @brief RIFT pattern structure for regex-based token recognition
 *
 * Implements AEGIS pattern syntax: R"pattern/flags[mode]"
 * - pattern: Regular expression pattern string
 * - flags: Modifier flags (i=case-insensitive, g=global, m=multiline)
 * - mode: Processing mode hint (c=classical, q=quantum, b=bitfield)
 */
struct RiftPattern {
  char *pattern_string; /**< Original pattern string */
  char *regex_pattern;  /**< Compiled regex pattern */
  char *flags;          /**< Pattern modifier flags */
  char *mode;           /**< Processing mode hint */
  bool compiled;        /**< Pattern compilation status */
  void *compiled_regex; /**< Platform-specific compiled regex */
  double weight;        /**< Pattern matching priority weight */
  uint32_t pattern_id;  /**< Unique pattern identifier */
};

/**
 * @brief Automaton state representation
 *
 * Each state represents a point in the regex automaton where specific
 * patterns can be matched. States preserve token classification for
 * AEGIS type/value separation.
 */
struct RiftState {
  char *name;              /**< State identifier name */
  char *pattern;           /**< Associated pattern string */
  bool is_final;           /**< Final/accepting state flag */
  bool is_accepting;       /**< Token acceptance flag */
  int state_id;            /**< Unique state identifier */
  double priority;         /**< State matching priority */
  RiftProcessingMode mode; /**< Classical/quantum processing mode */
  void *semantic_data;     /**< State-specific semantic information */
};

/**
 * @brief State transition representation
 *
 * Defines valid transitions between automaton states based on input symbols
 * and optional condition functions for complex pattern matching.
 */
struct RiftTransition {
  RiftState *from_state; /**< Source state */
  char *input_symbol;    /**< Triggering input symbol */
  RiftState *to_state;   /**< Destination state */
  bool (*condition)(const char *input, size_t pos); /**< Transition condition */
  double weight; /**< Transition weight for optimization */
};

/**
 * @brief AEGIS regex automaton engine
 *
 * Core automaton structure implementing the 5-tuple (Q, Σ, δ, q0, F) for
 * systematic token recognition and classification.
 */
struct RiftAutomaton {
  RiftState **states;    /**< Array of automaton states */
  size_t state_count;    /**< Number of states */
  size_t state_capacity; /**< Allocated state capacity */

  RiftTransition **transitions; /**< Array of state transitions */
  size_t transition_count;      /**< Number of transitions */
  size_t transition_capacity;   /**< Allocated transition capacity */

  RiftState *initial_state; /**< Starting state (q0) */
  RiftState **final_states; /**< Array of final states (F) */
  size_t final_count;       /**< Number of final states */
  size_t final_capacity;    /**< Allocated final state capacity */

  RiftState *current_state; /**< Current execution state */
  char *input_alphabet;     /**< Input symbol alphabet (Σ) */
  bool optimize_enabled;    /**< State minimization enabled */
};

/* ===== ENHANCED TOKEN ARCHITECTURE ===== */

/**
 * @brief RIFT token structure with AEGIS type/value separation
 *
 * Core token representation maintaining strict separation between token
 * classification (type) and content (value) for compile-time safety.
 * Preserves automaton state for AST minimization and optimization.
 */
struct RiftToken {
  /* AEGIS Core: Type/Value Separation (Stage 0 compliance) */
  char *type;   /**< Token classification (CRITICAL) */
  char *value;  /**< Token content (CRITICAL) */
  char *lexeme; /**< Original source text */

  /* Position tracking for error reporting and debugging */
  size_t line;     /**< Source line number */
  size_t column;   /**< Source column number */
  size_t position; /**< Absolute character position */
  size_t length;   /**< Token length in characters */

  /* AEGIS Memory: Token state preservation for AST minimization */
  RiftState
      *matched_state; /**< Automaton state that matched token (CRITICAL) */

  /* Stage 1 Extensions: Semantic data for parser */
  void *semantic_data; /**< Stage-specific semantic information */
  uint32_t flags;      /**< Token processing flags */

  /* Classical/Quantum mode support */
  RiftProcessingMode processing_mode; /**< Processing paradigm */

  /* Memory management and validation */
  bool validated;         /**< Token validation status */
  char *validation_error; /**< Validation error message (if any) */
};

/**
 * @brief AST node structure with classical/quantum mode support
 *
 * Represents nodes in the Abstract Syntax Tree with support for both
 * deterministic (classical) and probabilistic (quantum) processing modes.
 * Includes optimization metadata for AEGIS state minimization.
 */
struct RiftASTNode {
  RiftASTNodeType type;    /**< Node type classification */
  char *value;             /**< Node value content */
  RiftToken *source_token; /**< Originating token (may be NULL) */

  /* Tree structure */
  RiftASTNode **children; /**< Array of child nodes */
  size_t child_count;     /**< Number of children */
  size_t child_capacity;  /**< Allocated child capacity */
  RiftASTNode *parent;    /**< Parent node reference */

  /* AEGIS optimization: State machine minimization data */
  RiftState *reduced_state;   /**< Post-minimization state */
  double optimization_weight; /**< Node reduction priority */

  /* Processing mode data */
  RiftProcessingMode mode; /**< Node processing mode */

  /* Mode-specific data union */
  union {
    struct {                     /* Classical mode */
      void *deterministic_value; /**< Resolved deterministic value */
      bool is_resolved;          /**< Resolution status */
    } classical;

    struct {                         /* Quantum mode */
      void **superposition_states;   /**< Array of superposition states */
      size_t state_count;            /**< Number of superposition states */
      double *probabilities;         /**< State probability distribution */
      bool is_entangled;             /**< Entanglement status */
      RiftASTNode **entangled_nodes; /**< Entangled node references */
      size_t entanglement_count;     /**< Number of entangled nodes */
    } quantum;
  } mode_data;

  /* Semantic analysis results */
  char *semantic_type; /**< Inferred semantic type */
  void *type_info;     /**< Type system information */
};

/**
 * @brief TokenMemory bridge for Stage 0 → Stage 1 processing
 *
 * Manages the transition from .rift.0 token streams to enhanced tokens
 * suitable for parsing. Provides validation, enhancement, and optimization
 * services while preserving AEGIS type/value separation.
 */
struct RiftTokenMemory {
  /* Input: Stage 0 tokens (.rift.0) */
  RiftToken **input_tokens; /**< Original tokens from Stage 0 */
  size_t input_count;       /**< Number of input tokens */

  /* Processing: Validation & enhancement */
  RiftToken **validated_tokens; /**< Tokens after validation */
  size_t validated_count;       /**< Number of validated tokens */

  /* Output: Enhanced tokens for parser */
  RiftToken **enhanced_tokens; /**< Enhanced tokens for Stage 1 */
  size_t enhanced_count;       /**< Number of enhanced tokens */

  /* Token validation results (compile-time safety) */
  struct {
    bool *type_validation;     /**< Type safety check results */
    bool *value_validation;    /**< Value constraint check results */
    bool *sequence_validation; /**< Token sequence validity */
    char **error_messages;     /**< Detailed error descriptions */
  } validation_results;

  /* Memory management and optimization */
  size_t total_capacity;      /**< Total allocated capacity */
  bool is_optimized;          /**< AST minimization applied flag */
  double optimization_factor; /**< Memory usage optimization factor */

  /* Stage metadata */
  char *source_stage; /**< Source stage identifier (.rift.0) */
  char *target_stage; /**< Target stage identifier (.rift.1) */
};

/**
 * @brief Stage 1 parse engine for token → AST transformation
 *
 * Core parsing engine that consumes TokenMemory and produces Abstract Syntax
 * Trees through AEGIS pattern-based parsing with optional state minimization.
 */
struct Rift1ParseEngine {
  /* Input: TokenMemory bridge */
  RiftTokenMemory *token_memory; /**< Input token memory */

  /* Processing: AEGIS pattern-based parsing */
  RiftAutomaton *syntax_automaton; /**< Syntax recognition automaton */
  RiftPattern **grammar_patterns;  /**< Grammar pattern definitions */
  size_t pattern_count;            /**< Number of grammar patterns */

  /* Output: AST generation */
  RiftASTNode *parse_tree; /**< Generated syntax tree root */
  bool parsing_complete;   /**< Parsing completion status */

  /* Error handling and diagnostics */
  char **parse_errors;   /**< Parse error messages */
  size_t error_count;    /**< Number of parse errors */
  size_t error_capacity; /**< Allocated error capacity */

  /* AEGIS optimization settings */
  bool enable_state_minimization; /**< State minimization enabled */
  bool enable_ast_optimization;   /**< AST optimization enabled */
  bool enable_quantum_mode;       /**< Quantum processing enabled */

  /* Performance metrics */
  size_t tokens_processed; /**< Total tokens processed */
  size_t nodes_generated;  /**< Total AST nodes generated */
  double parse_time;       /**< Parse execution time */
};

/**
 * @brief Generic RIFT engine for automaton processing
 *
 * General-purpose automaton engine used by various RIFT stages for
 * pattern matching and token generation.
 */
struct RiftEngine {
  RiftAutomaton *automaton; /**< Associated automaton */
  RiftToken **tokens;       /**< Generated token array */
  size_t token_count;       /**< Number of tokens */
  size_t token_capacity;    /**< Allocated token capacity */

  char *current_input;   /**< Current input string */
  size_t input_position; /**< Current input position */
  size_t input_length;   /**< Total input length */
  size_t current_line;   /**< Current line number */
  size_t current_column; /**< Current column number */

  bool error_recovery_enabled;        /**< Error recovery mode */
  RiftProcessingMode processing_mode; /**< Engine processing mode */
};

/**
 * @brief RIFT configuration for runtime behavior control
 */
struct RiftConfig {
  char *output_dir; /**< Output directory path */
  char *stage_name; /**< Current stage identifier */
  bool debug_mode;  /**< Debug output enabled */
  bool verbose;     /**< Verbose logging enabled */
  char *log_level;  /**< Logging level (error/warn/info/debug) */

  /* RIFT-specific features */
  bool enable_r_syntax;         /**< Enable R"pattern" syntax */
  bool enable_multiline;        /**< Enable multiline patterns */
  bool enable_quantum_mode;     /**< Enable quantum processing */
  bool enable_ast_optimization; /**< Enable AST optimization */

  /* Performance tuning */
  size_t max_tokens;             /**< Maximum token limit */
  size_t max_ast_depth;          /**< Maximum AST depth */
  double optimization_threshold; /**< Optimization threshold */
};

/**
 * @brief RIFT Intermediate Representation for stage outputs
 *
 * Represents the output of a RIFT processing stage, containing tokens,
 * AST structures, and metadata for subsequent stage consumption.
 */
struct RiftIR {
  char *stage;       /**< Stage identifier (e.g., "rift.1") */
  char *version;     /**< RIFT version string */
  char *source_file; /**< Source file path */

  /* Token data */
  RiftToken **tokens; /**< Token array */
  size_t token_count; /**< Number of tokens */

  /* Stage 1 extensions */
  RiftASTNode *ast_root;         /**< Generated AST root (for .rift.1) */
  RiftTokenMemory *token_memory; /**< Enhanced token processing */

  /* Metadata and diagnostics */
  char *metadata;        /**< Stage-specific metadata */
  char **processing_log; /**< Processing log entries */
  size_t log_count;      /**< Number of log entries */

  /* Processing statistics */
  double processing_time;     /**< Total processing time */
  size_t memory_usage;        /**< Peak memory usage */
  size_t optimization_passes; /**< Number of optimization passes */
};

/* ===== CORE API FUNCTIONS ===== */

/* Configuration Management */
RiftConfig *rift_config_create(void);
void rift_config_destroy(RiftConfig *config);
RiftResult rift_config_load(RiftConfig *config, const char *config_file);
RiftResult rift_config_save(RiftConfig *config, const char *config_file);

/* Pattern Management */
RiftPattern *rift_parse_pattern(const char *pattern_string);
void rift_pattern_destroy(RiftPattern *pattern);
RiftResult rift_pattern_compile(RiftPattern *pattern);
bool rift_pattern_match(RiftPattern *pattern, const char *input,
                        size_t *match_length);

/* Automaton Management */
RiftAutomaton *rift_automaton_create(void);
void rift_automaton_destroy(RiftAutomaton *automaton);
RiftState *rift_automaton_add_state(RiftAutomaton *automaton,
                                    const char *pattern_str, bool is_final);
RiftResult rift_automaton_add_transition(RiftAutomaton *automaton,
                                         RiftState *from, RiftState *to,
                                         const char *input_symbol);
RiftResult rift_automaton_optimize(RiftAutomaton *automaton);

/* Engine Operations */
RiftEngine *rift_engine_create(void);
void rift_engine_destroy(RiftEngine *engine);
RiftResult rift_engine_process_input(RiftEngine *engine, const char *input);
RiftToken *rift_engine_next_token(RiftEngine *engine);

/* Token Management (CRITICAL: preserve type/value separation) */
RiftToken *rift_token_create(const char *type, const char *value, size_t line,
                             size_t column);
void rift_token_destroy(RiftToken *token);
RiftResult rift_token_validate(RiftToken *token);
RiftToken *rift_token_clone(const RiftToken *token);

/* ===== STAGE 1 TOKENMEMORY API ===== */
RiftTokenMemory *rift_tokenmemory_create(void);
void rift_tokenmemory_destroy(RiftTokenMemory *memory);
RiftResult rift_tokenmemory_load_stage0(RiftTokenMemory *memory,
                                        const char *rift0_file);
RiftResult rift_tokenmemory_validate_tokens(RiftTokenMemory *memory);
RiftResult rift_tokenmemory_enhance_tokens(RiftTokenMemory *memory);
RiftResult rift_tokenmemory_optimize(RiftTokenMemory *memory);

/* ===== STAGE 1 AST API ===== */
RiftASTNode *rift_ast_node_create(RiftASTNodeType type, const char *value);
void rift_ast_node_destroy(RiftASTNode *node);
RiftResult rift_ast_node_add_child(RiftASTNode *parent, RiftASTNode *child);
RiftResult rift_ast_node_remove_child(RiftASTNode *parent, size_t index);
RiftResult rift_ast_optimize(RiftASTNode *root);
RiftResult rift_ast_validate(RiftASTNode *root);

/* Quantum Mode AST Functions */
RiftResult rift_ast_node_create_superposition(RiftASTNode *node, void **states,
                                              double *probabilities,
                                              size_t state_count);
RiftResult rift_ast_node_entangle(RiftASTNode *node1, RiftASTNode *node2);
RiftResult rift_ast_node_collapse_superposition(RiftASTNode *node);

/* ===== STAGE 1 PARSE ENGINE API ===== */
Rift1ParseEngine *rift1_engine_create(void);
void rift1_engine_destroy(Rift1ParseEngine *engine);
RiftResult rift1_engine_load_tokenmemory(Rift1ParseEngine *engine,
                                         RiftTokenMemory *memory);
RiftResult rift1_engine_add_grammar_pattern(Rift1ParseEngine *engine,
                                            RiftPattern *pattern);
RiftResult rift1_parse_tokens(Rift1ParseEngine *engine);
RiftResult rift1_generate_ast(Rift1ParseEngine *engine);

/* Advanced Parser Functions */
RiftResult rift1_engine_set_processing_mode(Rift1ParseEngine *engine,
                                            RiftProcessingMode mode);
RiftResult rift1_engine_enable_optimization(Rift1ParseEngine *engine,
                                            bool enable);
RiftResult rift1_engine_get_parse_statistics(Rift1ParseEngine *engine,
                                             size_t *tokens_processed,
                                             size_t *nodes_generated,
                                             double *parse_time);

/* ===== INTERMEDIATE REPRESENTATION API ===== */
RiftIR *rift_ir_create(const char *source_file);
void rift_ir_destroy(RiftIR *ir);
RiftResult rift_ir_add_token(RiftIR *ir, RiftToken *token);
RiftResult rift_ir_set_ast(RiftIR *ir, RiftASTNode *ast_root);
RiftResult rift_ir_write_file(RiftIR *ir, const char *output_file);
RiftResult rift_ir_read_file(RiftIR *ir, const char *input_file);

/* ===== HIGH-LEVEL PROCESSING API ===== */
RiftResult rift_process_file(const char *input_file, const char *output_file,
                             RiftConfig *config);
RiftResult rift1_process_stage0_to_stage1(const char *rift0_file,
                                          const char *rift1_file,
                                          RiftConfig *config);

/* ===== UTILITY FUNCTIONS ===== */
const char *rift_result_string(RiftResult result);
const char *rift_ast_node_type_string(RiftASTNodeType type);
const char *rift_processing_mode_string(RiftProcessingMode mode);

void rift_print_version(void);
void rift_print_usage(const char *program_name);

/* Debugging and Diagnostics */
void rift_token_print(const RiftToken *token);
void rift_ast_print(const RiftASTNode *node, int indent);
void rift_tokenmemory_print_statistics(const RiftTokenMemory *memory);
void rift_engine_print_statistics(const Rift1ParseEngine *engine);

#ifdef __cplusplus
}
#endif

#endif /* RIFT1_H */