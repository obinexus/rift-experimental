/**
 * =================================================================
 * cli_interface.h - RIFT-0 Command Line Interface
 * RIFT: RIFT Is a Flexible Translator
 * Component: CLI interface definitions and utilities
 * OBINexus Computing Framework - Stage 0 Implementation
 * =================================================================
 */

#ifndef RIFT_0_CLI_INTERFACE_H
#define RIFT_0_CLI_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "rift-0/core/tokenizer_rules.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * CLI Exit Codes following POSIX conventions
 */
typedef enum {
    CLI_SUCCESS = 0,
    CLI_ERROR_ARGS = 1,
    CLI_ERROR_FILE = 2,
    CLI_ERROR_MEMORY = 3,
    CLI_ERROR_TOKENIZER = 4,
    CLI_ERROR_QA = 5,
    CLI_ERROR_POLICY = 6
} CLIExitCode;

/**
 * Output format specifications
 */
typedef enum {
    OUTPUT_FORMAT_HUMAN,    /* Human-readable format */
    OUTPUT_FORMAT_CSV,      /* CSV format for analysis */
    OUTPUT_FORMAT_JSON,     /* JSON format for integration */
    OUTPUT_FORMAT_XML       /* XML format for enterprise */
} OutputFormat;

/**
 * CLI Configuration Structure
 * Encapsulates all command-line options and operational parameters
 */
typedef struct {
    /* Input/Output Configuration */
    char* input_file;           /* Source file path */
    char* output_file;          /* Output file path */
    char* config_file;          /* .riftrc configuration file */
    OutputFormat format;        /* Output format specification */
    
    /* Tokenization Parameters */
    char* pattern_override;     /* Override pattern for testing */
    uint32_t dfa_flags;        /* DFA processing flags */
    bool enable_r_macros;      /* Enable R-macro processing */
    
    /* Operational Modes */
    bool verbose_mode;         /* Verbose logging */
    bool debug_mode;          /* Debug output */
    bool qa_mode;             /* QA validation mode */
    bool benchmark_mode;      /* Performance benchmarking */
    bool dry_run;             /* Parse but don't execute */
    
    /* Performance Configuration */
    int thread_count;         /* Number of processing threads */
    size_t buffer_size;       /* I/O buffer size */
    bool enable_simd;         /* SIMD optimization */
    
    /* Governance and Policy */
    bool strict_policy;       /* Strict PoliC enforcement */
    bool validate_triplets;   /* TokenTriplet validation */
    char* policy_level;       /* Policy enforcement level */
} CLIConfiguration;

/**
 * Token output formatting functions
 */

/**
 * Format token as CSV record
 * Produces CSV-compatible output for data analysis
 * 
 * @param output Output stream
 * @param token Token to format
 * @param source_text Original source text
 * @param sequence_id Token sequence number
 * @param line_number Line number in source
 * @param column_number Column number in source
 * @return 0 on success, negative on error
 */
int format_token_csv(FILE* output, const TokenTriplet* token, 
                    const char* source_text, size_t sequence_id,
                    size_t line_number, size_t column_number);

/**
 * Format token as human-readable text
 * Produces structured text output for manual inspection
 * 
 * @param output Output stream
 * @param token Token to format
 * @param source_text Original source text
 * @param sequence_id Token sequence number
 * @param verbose Enable verbose details
 * @return 0 on success, negative on error
 */
int format_token_human(FILE* output, const TokenTriplet* token,
                      const char* source_text, size_t sequence_id,
                      bool verbose);

/**
 * Format token as JSON object
 * Produces JSON-compatible output for API integration
 * 
 * @param output Output stream
 * @param token Token to format
 * @param source_text Original source text
 * @param sequence_id Token sequence number
 * @param is_last Whether this is the final token
 * @return 0 on success, negative on error
 */
int format_token_json(FILE* output, const TokenTriplet* token,
                     const char* source_text, size_t sequence_id,
                     bool is_last);

/**
 * File I/O utility functions
 */

/**
 * Read entire file into memory buffer
 * Handles memory allocation and error checking
 * 
 * @param filename Path to file
 * @param content Output buffer pointer (caller must free)
 * @param size Output size of content
 * @return 0 on success, negative on error
 */
int read_file_content(const char* filename, char** content, size_t* size);

/**
 * Write content to file with atomic operation
 * Ensures complete write or rollback on failure
 * 
 * @param filename Path to output file
 * @param content Content to write
 * @param size Size of content
 * @return 0 on success, negative on error
 */
int write_file_content(const char* filename, const char* content, size_t size);

/**
 * Configuration management functions
 */

/**
 * Initialize CLI configuration with defaults
 * Sets up default values following OBINexus standards
 * 
 * @param config Configuration structure to initialize
 * @return 0 on success, negative on error
 */
int init_cli_config(CLIConfiguration* config);

/**
 * Load configuration from .riftrc file
 * Parses governance configuration and CLI overrides
 * 
 * @param config Configuration structure to populate
 * @param config_file Path to .riftrc file
 * @return 0 on success, negative on error
 */
int load_riftrc_config(CLIConfiguration* config, const char* config_file);

/**
 * Validate CLI configuration consistency
 * Ensures configuration meets PoliC governance requirements
 * 
 * @param config Configuration to validate
 * @return 0 if valid, negative error code if invalid
 */
int validate_cli_config(const CLIConfiguration* config);

/**
 * Cleanup CLI configuration
 * Releases allocated memory and resources
 * 
 * @param config Configuration to cleanup
 */
void cleanup_cli_config(CLIConfiguration* config);

/**
 * Flag parsing and validation
 */

/**
 * Parse DFA flags from string representation
 * Converts flag string (e.g., "gmi") to flag bitmask
 * 
 * @param flag_string String representation of flags
 * @param flags Output bitmask
 * @return 0 on success, negative on parse error
 */
int parse_dfa_flags(const char* flag_string, uint32_t* flags);

/**
 * Validate flag combinations
 * Ensures flag combinations are semantically valid
 * 
 * @param flags Flag bitmask to validate
 * @return 0 if valid, negative if invalid combination
 */
int validate_flag_combination(uint32_t flags);

/**
 * Convert flags to string representation
 * Produces human-readable flag description
 * 
 * @param flags Flag bitmask
 * @param buffer Output buffer
 * @param buffer_size Size of output buffer
 * @return 0 on success, negative on error
 */
int flags_to_string(uint32_t flags, char* buffer, size_t buffer_size);

/**
 * Progress reporting and status
 */

/**
 * Display tokenization progress
 * Shows progress for large file processing
 * 
 * @param current Current position
 * @param total Total size
 * @param tokens_generated Number of tokens generated
 */
void display_progress(size_t current, size_t total, size_t tokens_generated);

/**
 * Display performance statistics
 * Shows timing and throughput metrics
 * 
 * @param tokens_processed Number of tokens processed
 * @param elapsed_time Processing time in seconds
 * @param memory_used Peak memory usage
 */
void display_performance_stats(size_t tokens_processed, double elapsed_time, size_t memory_used);

/**
 * Error reporting and diagnostics
 */

/**
 * Report tokenization error with context
 * Provides detailed error information for debugging
 * 
 * @param error_code Error code from tokenizer
 * @param source_text Source text being processed
 * @param position Error position in source
 * @param context_lines Lines of context to show
 */
void report_tokenization_error(int error_code, const char* source_text,
                              size_t position, int context_lines);

/**
 * Generate diagnostic report
 * Creates comprehensive diagnostic output for analysis
 * 
 * @param output Output stream
 * @param config CLI configuration
 * @param tokens Token array
 * @param token_count Number of tokens
 * @param source_text Original source
 * @return 0 on success, negative on error
 */
int generate_diagnostic_report(FILE* output, const CLIConfiguration* config,
                              const TokenTriplet* tokens, size_t token_count,
                              const char* source_text);

/**
 * R-pattern testing utilities
 */

/**
 * Test single R-pattern against input
 * Validates R-pattern functionality with detailed output
 * 
 * @param pattern R-pattern string to test
 * @param input Test input string
 * @param flags DFA flags to apply
 * @param verbose Enable verbose output
 * @return 0 on successful match, negative on error
 */
int test_r_pattern(const char* pattern, const char* input, 
                  uint32_t flags, bool verbose);

/**
 * Run R-macro composition tests
 * Tests R.compose, R.aggregate, and boolean operations
 * 
 * @param verbose Enable verbose output
 * @return 0 if all tests pass, negative if any fail
 */
int test_r_macros(bool verbose);

/**
 * Thread safety and performance testing
 */

/**
 * Run concurrent tokenization test
 * Tests thread safety with multiple workers
 * 
 * @param thread_count Number of threads to use
 * @param test_data Test input data
 * @param iterations Number of test iterations
 * @return 0 on success, negative on thread safety violation
 */
int test_concurrent_tokenization(int thread_count, const char* test_data, int iterations);

/**
 * Benchmark tokenization performance
 * Measures throughput and latency characteristics
 * 
 * @param test_files Array of test file paths
 * @param file_count Number of test files
 * @param thread_count Number of threads to use
 * @return 0 on completion, negative on error
 */
int benchmark_tokenization(const char** test_files, int file_count, int thread_count);

/**
 * Constants and limits
 */

/* Maximum input file size (64MB) */
#define CLI_MAX_FILE_SIZE (64 * 1024 * 1024)

/* Maximum number of tokens per file */
#define CLI_MAX_TOKENS 1000000

/* Default buffer size for I/O operations */
#define CLI_DEFAULT_BUFFER_SIZE 8192

/* Maximum length for pattern strings */
#define CLI_MAX_PATTERN_LENGTH 1024

/* Maximum context lines for error reporting */
#define CLI_MAX_CONTEXT_LINES 5

/* Performance threshold constants */
#define CLI_SLOW_TOKEN_THRESHOLD_MS 10
#define CLI_MEMORY_WARNING_THRESHOLD_MB 100

/**
 * Macro definitions for common operations
 */

/* Check if configuration is in verbose mode */
#define IS_VERBOSE(config) ((config)->verbose_mode)

/* Check if configuration enables debug output */
#define IS_DEBUG(config) ((config)->debug_mode)

/* Get effective thread count (min 1, max system cores) */
#define EFFECTIVE_THREAD_COUNT(config) \
    ((config)->thread_count > 0 ? (config)->thread_count : 1)

/* Check if strict policy enforcement is enabled */
#define IS_STRICT_POLICY(config) ((config)->strict_policy)

#ifdef __cplusplus
}
#endif

#endif /* RIFT_0_CLI_INTERFACE_H */