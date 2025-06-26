#ifndef RIFT_H
#define RIFT_H

// Ensure strdup and other POSIX functions are available
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <errno.h>

// RIFT Pattern Syntax: R"pattern/flags[mode]"
// mode: 't' = top-down matching, 'b' = bottom-up matching

typedef enum {
    RIFT_MODE_TOP_DOWN = 't',
    RIFT_MODE_BOTTOM_UP = 'b'
} RiftMatchMode;

typedef enum {
    RIFT_SUCCESS = 0,
    RIFT_ERROR_NULL_POINTER = -1,
    RIFT_ERROR_MEMORY_ALLOCATION = -2,
    RIFT_ERROR_PATTERN_INVALID = -3,
    RIFT_ERROR_COMPILATION_FAILED = -4,
    RIFT_ERROR_IO = -5
} RiftResult;

typedef struct RiftPattern {
    char* raw_pattern;       // Original R"..." string
    char* regex_pattern;     // Extracted regex pattern
    char* flags;             // gmi flags
    RiftMatchMode mode;      // t or b
    regex_t compiled_regex;  // Compiled POSIX regex
    bool is_compiled;
} RiftPattern;

// AEGIS Automaton State (Q in 5-tuple)
typedef struct RiftState {
    uint32_t id;
    char* pattern_string;
    RiftPattern* pattern;
    bool is_final;
    bool is_initial;
    void* semantic_data;     // For IR generation
} RiftState;

// AEGIS Transition (δ in 5-tuple)
typedef struct RiftTransition {
    RiftState* from_state;
    char* input_symbol;      // Σ alphabet
    RiftState* to_state;
    bool (*condition)(const char* input, size_t pos);
} RiftTransition;

// AEGIS Regex Automaton (5-tuple: Q, Σ, δ, q0, F)
typedef struct RiftAutomaton {
    RiftState** states;          // Q: finite set of states
    size_t state_count;
    size_t state_capacity;
    
    RiftTransition** transitions; // δ: transition function  
    size_t transition_count;
    size_t transition_capacity;
    
    RiftState* initial_state;    // q0: initial state
    RiftState** final_states;    // F: set of accepting states
    size_t final_count;
    size_t final_capacity;
    
    RiftState* current_state;    // Current execution state
    char* input_alphabet;        // Σ: input alphabet
} RiftAutomaton;

// Token for IR Generation (CRITICAL: type/value separation + matched_state preservation)
typedef struct RiftToken {
    char* type;              // ✅ SEPARATE - automaton classification
    char* value;             // ✅ SEPARATE - matched text content
    char* lexeme;
    size_t line;
    size_t column;
    size_t position;
    RiftState* matched_state; // ✅ CRITICAL - token memory for AST minimization
} RiftToken;

// AEGIS RegexAutomatonEngine Component
typedef struct RiftEngine {
    RiftAutomaton* automaton;
    RiftToken** tokens;
    size_t token_count;
    size_t token_capacity;
    char* current_input;
    size_t input_position;
    size_t input_length;
} RiftEngine;

// Configuration system for .riftrc
typedef struct RiftConfig {
    char* output_dir;
    char* stage_name;
    bool debug_mode;
    bool verbose;
    char* log_level;
} RiftConfig;

// IR Output structure for .rift.0 files
typedef struct RiftIR {
    char* stage;
    char* version;
    char* source_file;
    RiftToken** tokens;
    size_t token_count;
    char* metadata;
} RiftIR;

// ===== CORE API FUNCTIONS =====

// Configuration management
RiftConfig* rift_config_create(void);
void rift_config_destroy(RiftConfig* config);
RiftResult rift_config_load(RiftConfig* config, const char* config_file);
RiftResult rift_config_save(RiftConfig* config, const char* config_file);

// Pattern parsing (R"pattern/flags[mode]")
RiftPattern* rift_parse_pattern(const char* pattern_string);
void rift_pattern_destroy(RiftPattern* pattern);
RiftResult rift_pattern_compile(RiftPattern* pattern);

// Automaton management
RiftAutomaton* rift_automaton_create(void);
void rift_automaton_destroy(RiftAutomaton* automaton);
RiftState* rift_automaton_add_state(RiftAutomaton* automaton, 
                                   const char* pattern_str, 
                                   bool is_final);
RiftResult rift_automaton_add_transition(RiftAutomaton* automaton,
                                        RiftState* from, 
                                        RiftState* to,
                                        const char* input_symbol);

// Engine operations
RiftEngine* rift_engine_create(void);
void rift_engine_destroy(RiftEngine* engine);
RiftResult rift_engine_process_input(RiftEngine* engine, const char* input);
RiftToken* rift_engine_next_token(RiftEngine* engine);

// Token management (CRITICAL: preserve type/value separation)
RiftToken* rift_token_create(const char* type, const char* value, 
                           size_t line, size_t column);
void rift_token_destroy(RiftToken* token);

// IR generation and output (CRITICAL: maintain type/value separation in output)
RiftIR* rift_ir_create(const char* source_file);
void rift_ir_destroy(RiftIR* ir);
RiftResult rift_ir_add_token(RiftIR* ir, RiftToken* token);
RiftResult rift_ir_write_file(RiftIR* ir, const char* output_file);
RiftResult rift_ir_read_file(RiftIR* ir, const char* input_file);

// CLI utilities
RiftResult rift_process_file(const char* input_file, const char* output_file, 
                           RiftConfig* config);
void rift_print_version(void);
void rift_print_usage(const char* program_name);

// Error handling
const char* rift_result_string(RiftResult result);

#endif // RIFT_H
/* Merged from rift.h */

// Ensure strdup and other POSIX functions are available
#define _GNU_SOURCE
#endif
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <errno.h>

// RIFT Pattern Syntax: R"pattern/flags[mode]"
// mode: 't' = top-down matching, 'b' = bottom-up matching

typedef enum {
    RIFT_MODE_TOP_DOWN = 't',
    RIFT_MODE_BOTTOM_UP = 'b'
} RiftMatchMode;

typedef enum {
    RIFT_SUCCESS = 0,
    RIFT_ERROR_NULL_POINTER = -1,
    RIFT_ERROR_MEMORY_ALLOCATION = -2,
    RIFT_ERROR_PATTERN_INVALID = -3,
    RIFT_ERROR_COMPILATION_FAILED = -4,
    RIFT_ERROR_IO = -5
} RiftResult;

typedef struct RiftPattern {
    char* raw_pattern;       // Original R"..." string
    char* regex_pattern;     // Extracted regex pattern
    char* flags;             // gmi flags
    RiftMatchMode mode;      // t or b
    regex_t compiled_regex;  // Compiled POSIX regex
    bool is_compiled;
} RiftPattern;

// AEGIS Automaton State (Q in 5-tuple)
typedef struct RiftState {
    uint32_t id;
    char* pattern_string;
    RiftPattern* pattern;
    bool is_final;
    bool is_initial;
    void* semantic_data;     // For IR generation
} RiftState;

// AEGIS Transition (δ in 5-tuple)
typedef struct RiftTransition {
    RiftState* from_state;
    char* input_symbol;      // Σ alphabet
    RiftState* to_state;
    bool (*condition)(const char* input, size_t pos);
} RiftTransition;

// AEGIS Regex Automaton (5-tuple: Q, Σ, δ, q0, F)
typedef struct RiftAutomaton {
    RiftState** states;          // Q: finite set of states
    size_t state_count;
    size_t state_capacity;
    
    RiftTransition** transitions; // δ: transition function  
    size_t transition_count;
    size_t transition_capacity;
    
    RiftState* initial_state;    // q0: initial state
    RiftState** final_states;    // F: set of accepting states
    size_t final_count;
    size_t final_capacity;
    
    RiftState* current_state;    // Current execution state
    char* input_alphabet;        // Σ: input alphabet
} RiftAutomaton;

// Token for IR Generation (CRITICAL: type/value separation + matched_state preservation)
typedef struct RiftToken {
    char* type;              // ✅ SEPARATE - automaton classification
    char* value;             // ✅ SEPARATE - matched text content
    char* lexeme;
    size_t line;
    size_t column;
    size_t position;
    RiftState* matched_state; // ✅ CRITICAL - token memory for AST minimization
} RiftToken;

// AEGIS RegexAutomatonEngine Component
typedef struct RiftEngine {
    RiftAutomaton* automaton;
    RiftToken** tokens;
    size_t token_count;
    size_t token_capacity;
    char* current_input;
    size_t input_position;
    size_t input_length;
} RiftEngine;

// Configuration system for .riftrc
typedef struct RiftConfig {
    char* output_dir;
    char* stage_name;
    bool debug_mode;
    bool verbose;
    char* log_level;
} RiftConfig;

// IR Output structure for .rift.0 files
typedef struct RiftIR {
    char* stage;
    char* version;
    char* source_file;
    RiftToken** tokens;
    size_t token_count;
    char* metadata;
} RiftIR;

// ===== CORE API FUNCTIONS =====

// Configuration management
RiftConfig* rift_config_create(void);
void rift_config_destroy(RiftConfig* config);
RiftResult rift_config_load(RiftConfig* config, const char* config_file);
RiftResult rift_config_save(RiftConfig* config, const char* config_file);

// Pattern parsing (R"pattern/flags[mode]")
RiftPattern* rift_parse_pattern(const char* pattern_string);
void rift_pattern_destroy(RiftPattern* pattern);
RiftResult rift_pattern_compile(RiftPattern* pattern);

// Automaton management
RiftAutomaton* rift_automaton_create(void);
void rift_automaton_destroy(RiftAutomaton* automaton);
RiftState* rift_automaton_add_state(RiftAutomaton* automaton, 
                                   const char* pattern_str, 
                                   bool is_final);
RiftResult rift_automaton_add_transition(RiftAutomaton* automaton,
                                        RiftState* from, 
                                        RiftState* to,
                                        const char* input_symbol);

// Engine operations
RiftEngine* rift_engine_create(void);
void rift_engine_destroy(RiftEngine* engine);
RiftResult rift_engine_process_input(RiftEngine* engine, const char* input);
RiftToken* rift_engine_next_token(RiftEngine* engine);

// Token management (CRITICAL: preserve type/value separation)
RiftToken* rift_token_create(const char* type, const char* value, 
                           size_t line, size_t column);
void rift_token_destroy(RiftToken* token);

// IR generation and output (CRITICAL: maintain type/value separation in output)
RiftIR* rift_ir_create(const char* source_file);
void rift_ir_destroy(RiftIR* ir);
RiftResult rift_ir_add_token(RiftIR* ir, RiftToken* token);
RiftResult rift_ir_write_file(RiftIR* ir, const char* output_file);
RiftResult rift_ir_read_file(RiftIR* ir, const char* input_file);

// CLI utilities
RiftResult rift_process_file(const char* input_file, const char* output_file, 
                           RiftConfig* config);
void rift_print_version(void);
void rift_print_usage(const char* program_name);

// Error handling
const char* rift_result_string(RiftResult result);

/* End merge from rift.h */
/* Merged from rift.h */

// Ensure strdup and other POSIX functions are available
#define _GNU_SOURCE
#endif
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <errno.h>

// RIFT Pattern Syntax: R"pattern/flags[mode]"
// mode: 't' = top-down matching, 'b' = bottom-up matching

typedef enum {
    RIFT_MODE_TOP_DOWN = 't',
    RIFT_MODE_BOTTOM_UP = 'b'
} RiftMatchMode;

typedef enum {
    RIFT_SUCCESS = 0,
    RIFT_ERROR_NULL_POINTER = -1,
    RIFT_ERROR_MEMORY_ALLOCATION = -2,
    RIFT_ERROR_PATTERN_INVALID = -3,
    RIFT_ERROR_COMPILATION_FAILED = -4,
    RIFT_ERROR_IO = -5
} RiftResult;

typedef struct RiftPattern {
    char* raw_pattern;       // Original R"..." string
    char* regex_pattern;     // Extracted regex pattern
    char* flags;             // gmi flags
    RiftMatchMode mode;      // t or b
    regex_t compiled_regex;  // Compiled POSIX regex
    bool is_compiled;
} RiftPattern;

// AEGIS Automaton State (Q in 5-tuple)
typedef struct RiftState {
    uint32_t id;
    char* pattern_string;
    RiftPattern* pattern;
    bool is_final;
    bool is_initial;
    void* semantic_data;     // For IR generation
} RiftState;

// AEGIS Transition (δ in 5-tuple)
typedef struct RiftTransition {
    RiftState* from_state;
    char* input_symbol;      // Σ alphabet
    RiftState* to_state;
    bool (*condition)(const char* input, size_t pos);
} RiftTransition;

// AEGIS Regex Automaton (5-tuple: Q, Σ, δ, q0, F)
typedef struct RiftAutomaton {
    RiftState** states;          // Q: finite set of states
    size_t state_count;
    size_t state_capacity;
    
    RiftTransition** transitions; // δ: transition function  
    size_t transition_count;
    size_t transition_capacity;
    
    RiftState* initial_state;    // q0: initial state
    RiftState** final_states;    // F: set of accepting states
    size_t final_count;
    size_t final_capacity;
    
    RiftState* current_state;    // Current execution state
    char* input_alphabet;        // Σ: input alphabet
} RiftAutomaton;

// Token for IR Generation (CRITICAL: type/value separation + matched_state preservation)
typedef struct RiftToken {
    char* type;              // ✅ SEPARATE - automaton classification
    char* value;             // ✅ SEPARATE - matched text content
    char* lexeme;
    size_t line;
    size_t column;
    size_t position;
    RiftState* matched_state; // ✅ CRITICAL - token memory for AST minimization
} RiftToken;

// AEGIS RegexAutomatonEngine Component
typedef struct RiftEngine {
    RiftAutomaton* automaton;
    RiftToken** tokens;
    size_t token_count;
    size_t token_capacity;
    char* current_input;
    size_t input_position;
    size_t input_length;
} RiftEngine;

// Configuration system for .riftrc
typedef struct RiftConfig {
    char* output_dir;
    char* stage_name;
    bool debug_mode;
    bool verbose;
    char* log_level;
} RiftConfig;

// IR Output structure for .rift.0 files
typedef struct RiftIR {
    char* stage;
    char* version;
    char* source_file;
    RiftToken** tokens;
    size_t token_count;
    char* metadata;
} RiftIR;

// ===== CORE API FUNCTIONS =====

// Configuration management
RiftConfig* rift_config_create(void);
void rift_config_destroy(RiftConfig* config);
RiftResult rift_config_load(RiftConfig* config, const char* config_file);
RiftResult rift_config_save(RiftConfig* config, const char* config_file);

// Pattern parsing (R"pattern/flags[mode]")
RiftPattern* rift_parse_pattern(const char* pattern_string);
void rift_pattern_destroy(RiftPattern* pattern);
RiftResult rift_pattern_compile(RiftPattern* pattern);

// Automaton management
RiftAutomaton* rift_automaton_create(void);
void rift_automaton_destroy(RiftAutomaton* automaton);
RiftState* rift_automaton_add_state(RiftAutomaton* automaton, 
                                   const char* pattern_str, 
                                   bool is_final);
RiftResult rift_automaton_add_transition(RiftAutomaton* automaton,
                                        RiftState* from, 
                                        RiftState* to,
                                        const char* input_symbol);

// Engine operations
RiftEngine* rift_engine_create(void);
void rift_engine_destroy(RiftEngine* engine);
RiftResult rift_engine_process_input(RiftEngine* engine, const char* input);
RiftToken* rift_engine_next_token(RiftEngine* engine);

// Token management (CRITICAL: preserve type/value separation)
RiftToken* rift_token_create(const char* type, const char* value, 
                           size_t line, size_t column);
void rift_token_destroy(RiftToken* token);

// IR generation and output (CRITICAL: maintain type/value separation in output)
RiftIR* rift_ir_create(const char* source_file);
void rift_ir_destroy(RiftIR* ir);
RiftResult rift_ir_add_token(RiftIR* ir, RiftToken* token);
RiftResult rift_ir_write_file(RiftIR* ir, const char* output_file);
RiftResult rift_ir_read_file(RiftIR* ir, const char* input_file);

// CLI utilities
RiftResult rift_process_file(const char* input_file, const char* output_file, 
                           RiftConfig* config);
void rift_print_version(void);
void rift_print_usage(const char* program_name);

// Error handling
const char* rift_result_string(RiftResult result);

/* End merge from rift.h */
