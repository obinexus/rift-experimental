#ifndef RIFT_H
#define RIFT_H

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
#include <unistd.h>
#include <sys/stat.h>
#include <libgen.h>
#include <ctype.h>

// Enhanced pattern syntax flags
typedef enum {
    RIFT_SYNTAX_BASIC = 0,
    RIFT_SYNTAX_RAW_STRING = 1,    // R"" syntax support
    RIFT_SYNTAX_ALTERNATE = 2,     // R'' syntax support  
    RIFT_SYNTAX_TOP_DOWN = 4,      // [t] mode flag
    RIFT_SYNTAX_BOTTOM_UP = 8      // [b] mode flag
} RiftSyntaxFlags;

typedef enum {
    RIFT_MODE_TOP_DOWN = 't',
    RIFT_MODE_BOTTOM_UP = 'b',
    RIFT_MODE_AUTO = 'a'
} RiftMatchMode;

typedef enum {
    RIFT_SUCCESS = 0,
    RIFT_ERROR_NULL_POINTER = -1,
    RIFT_ERROR_MEMORY_ALLOCATION = -2,
    RIFT_ERROR_PATTERN_INVALID = -3,
    RIFT_ERROR_COMPILATION_FAILED = -4,
    RIFT_ERROR_IO = -5,
    RIFT_ERROR_SYNTAX_ERROR = -6,
    RIFT_ERROR_MODE_INVALID = -7
} RiftResult;

// Enhanced pattern structure with R"" syntax support
typedef struct RiftPattern {
    char* raw_pattern;              // Original pattern string
    char* regex_pattern;            // Processed regex pattern
    char* flags;                    // Pattern flags
    RiftMatchMode mode;             // Parsing mode ([t], [b], [a])
    RiftSyntaxFlags syntax_flags;   // Syntax type flags
    regex_t compiled_regex;         // Compiled regex
    bool is_compiled;               // Compilation status
    bool is_multiline;              // Multiline pattern support
    char* description;              // Pattern description (from comments)
} RiftPattern;

typedef struct RiftState {
    uint32_t id;
    char* pattern_string;
    RiftPattern* pattern;
    bool is_final;
    bool is_initial;
    void* semantic_data;
    char* token_type;               // Associated token type
    int priority;                   // State matching priority
} RiftState;

typedef struct RiftTransition {
    RiftState* from_state;
    char* input_symbol;
    RiftState* to_state;
    bool (*condition)(const char* input, size_t pos);
    double weight;                  // Transition weight for optimization
} RiftTransition;

typedef struct RiftAutomaton {
    RiftState** states;
    size_t state_count;
    size_t state_capacity;
    RiftTransition** transitions;
    size_t transition_count;
    size_t transition_capacity;
    RiftState* initial_state;
    RiftState** final_states;
    size_t final_count;
    size_t final_capacity;
    RiftState* current_state;
    char* input_alphabet;
    bool optimize_enabled;          // Automaton optimization
} RiftAutomaton;

// Enhanced token structure for type/value separation
typedef struct RiftToken {
    char* type;                     // Token type
    char* value;                    // Token value
    char* lexeme;                   // Original lexeme
    size_t line;                    // Line number
    size_t column;                  // Column number
    size_t position;                // Absolute position
    size_t length;                  // Token length
    RiftState* matched_state;       // State that matched this token
    void* semantic_data;            // Associated semantic information
    uint32_t flags;                 // Token flags
} RiftToken;

typedef struct RiftEngine {
    RiftAutomaton* automaton;
    RiftToken** tokens;
    size_t token_count;
    size_t token_capacity;
    char* current_input;
    size_t input_position;
    size_t input_length;
    size_t current_line;
    size_t current_column;
    bool error_recovery_enabled;    // Error recovery mode
} RiftEngine;

typedef struct RiftConfig {
    char* output_dir;
    char* stage_name;
    bool debug_mode;
    bool verbose;
    char* log_level;
    bool enable_r_syntax;           // Enable R"" syntax
    bool enable_multiline;          // Enable multiline patterns
    int max_backtrack_depth;        // Maximum backtracking depth
} RiftConfig;

typedef struct RiftIR {
    char* stage;
    char* version;
    char* source_file;
    RiftToken** tokens;
    size_t token_count;
    char* metadata;
    char* architecture_info;        // OBINexus architecture metadata
} RiftIR;

// CSV-specific structures for enhanced parsing
typedef enum {
    CSV_FIELD_UNQUOTED,
    CSV_FIELD_QUOTED,
    CSV_DELIMITER,
    CSV_NEWLINE,
    CSV_EOF
} CSVTokenType;

typedef struct RiftCSVToken {
    CSVTokenType type;
    char* value;
    size_t position;
    size_t length;
    bool has_escapes;
} RiftCSVToken;

typedef struct RiftCSVParser {
    RiftEngine* engine;
    char delimiter;
    char quote_char;
    char escape_char;
    bool strict_mode;
    RiftCSVToken** tokens;
    size_t token_count;
} RiftCSVParser;

// Core API Functions
RiftConfig* rift_config_create(void);
void rift_config_destroy(RiftConfig* config);
RiftResult rift_config_load(RiftConfig* config, const char* config_file);

// Enhanced pattern functions with R"" syntax support
RiftPattern* rift_parse_pattern(const char* pattern_string);
RiftPattern* rift_parse_pattern_with_flags(const char* pattern_string, RiftSyntaxFlags flags);
void rift_pattern_destroy(RiftPattern* pattern);
RiftResult rift_pattern_compile(RiftPattern* pattern);
bool rift_pattern_is_raw_string(const char* pattern_string);
char* rift_pattern_process_raw_string(const char* pattern_string, RiftSyntaxFlags* flags, RiftMatchMode* mode);

RiftAutomaton* rift_automaton_create(void);
void rift_automaton_destroy(RiftAutomaton* automaton);
RiftState* rift_automaton_add_state(RiftAutomaton* automaton, const char* pattern_str, bool is_final);
RiftState* rift_automaton_add_state_with_type(RiftAutomaton* automaton, const char* pattern_str, const char* token_type, bool is_final);
RiftResult rift_automaton_add_transition(RiftAutomaton* automaton, RiftState* from, RiftState* to, const char* input_symbol);
void rift_automaton_optimize(RiftAutomaton* automaton);

RiftEngine* rift_engine_create(void);
void rift_engine_destroy(RiftEngine* engine);
RiftResult rift_engine_process_input(RiftEngine* engine, const char* input);
RiftToken* rift_engine_next_token(RiftEngine* engine);
void rift_engine_reset(RiftEngine* engine);

RiftToken* rift_token_create(const char* type, const char* value, size_t line, size_t column);
RiftToken* rift_token_create_full(const char* type, const char* value, const char* lexeme, 
                                  size_t line, size_t column, size_t position, size_t length);
void rift_token_destroy(RiftToken* token);

RiftIR* rift_ir_create(const char* source_file);
void rift_ir_destroy(RiftIR* ir);
RiftResult rift_ir_add_token(RiftIR* ir, RiftToken* token);
RiftResult rift_ir_write_file(RiftIR* ir, const char* output_file);

// CSV-specific functions
RiftCSVParser* rift_csv_parser_create(char delimiter, char quote_char);
void rift_csv_parser_destroy(RiftCSVParser* parser);
RiftResult rift_csv_parse_input(RiftCSVParser* parser, const char* input);
RiftCSVToken* rift_csv_next_token(RiftCSVParser* parser);

RiftResult rift_process_file(const char* input_file, const char* output_file, RiftConfig* config);
void rift_print_version(void);
void rift_print_usage(const char* program_name);

const char* rift_result_string(RiftResult result);
const char* rift_mode_string(RiftMatchMode mode);
const char* rift_syntax_flags_string(RiftSyntaxFlags flags);

// R"" Syntax Processing Functions
bool rift_is_raw_string_pattern(const char* pattern);
char* rift_extract_raw_string_content(const char* pattern, RiftMatchMode* mode, char** description);
RiftResult rift_process_multiline_pattern(const char* pattern, char** processed_pattern);

// Debug and utility functions
void rift_debug_print_pattern(const RiftPattern* pattern);
void rift_debug_print_automaton(const RiftAutomaton* automaton);
void rift_debug_print_tokens(RiftToken** tokens, size_t count);

#endif // RIFT_H