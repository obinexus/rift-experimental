/*
 * rift/include/rift/core/common.h
 * RIFT Core Common Framework Header
 * OBINexus Computing Framework - AEGIS Methodology
 * Technical Lead: Nnamdi Michael Okpala
 */

#ifndef RIFT_CORE_COMMON_H
#define RIFT_CORE_COMMON_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

// AEGIS Framework Version Information
#define RIFT_FRAMEWORK_VERSION_MAJOR 1
#define RIFT_FRAMEWORK_VERSION_MINOR 0
#define RIFT_FRAMEWORK_VERSION_PATCH 0
#define RIFT_FRAMEWORK_VERSION_STRING "1.0.0"

// AEGIS Compliance Constants
#define RIFT_AEGIS_ENABLED 1
#define RIFT_ZERO_TRUST_ENABLED 1
#define RIFT_MEMORY_ALIGNMENT 4096
#define RIFT_MAX_PATH_LENGTH 4096
#define RIFT_MAX_IDENTIFIER_LENGTH 256
#define RIFT_MAX_ERROR_MESSAGE_LENGTH 512

// RIFT Error Code Registry - Systematic Classification
typedef enum {
    // Success Codes
    RIFT_SUCCESS = 0,
    RIFT_SUCCESS_WITH_WARNINGS = 1,
    
    // General Error Codes (-1 to -99)
    RIFT_ERROR_INVALID_ARGUMENT = -1,
    RIFT_ERROR_MEMORY_ALLOCATION = -2,
    RIFT_ERROR_INVALID_STATE = -3,
    RIFT_ERROR_FILE_NOT_FOUND = -4,
    RIFT_ERROR_FILE_ACCESS = -5,
    RIFT_ERROR_BUFFER_OVERFLOW = -6,
    RIFT_ERROR_NULL_POINTER = -7,
    RIFT_ERROR_OUT_OF_BOUNDS = -8,
    RIFT_ERROR_TIMEOUT = -9,
    RIFT_ERROR_INTERRUPTED = -10,
    RIFT_ERROR_NOT_IMPLEMENTED = -11,
    
    // Tokenizer Error Codes (-100 to -199)
    RIFT_ERROR_TOKEN_BUFFER_OVERFLOW = -100,
    RIFT_ERROR_TOKENIZATION_FAILED = -101,
    RIFT_ERROR_INVALID_TOKEN = -102,
    RIFT_ERROR_TOKEN_TOO_LONG = -103,
    RIFT_ERROR_UNTERMINATED_STRING = -104,
    RIFT_ERROR_INVALID_NUMBER_FORMAT = -105,
    RIFT_ERROR_END_OF_INPUT = -106,
    
    // Parser Error Codes (-200 to -299)
    RIFT_ERROR_PARSE_FAILED = -200,
    RIFT_ERROR_SYNTAX_ERROR = -201,
    RIFT_ERROR_UNEXPECTED_TOKEN = -202,
    RIFT_ERROR_MISSING_SEMICOLON = -203,
    RIFT_ERROR_UNMATCHED_PARENTHESES = -204,
    RIFT_ERROR_INVALID_EXPRESSION = -205,
    RIFT_ERROR_AST_NODE_ALLOCATION = -206,
    
    // Semantic Analysis Error Codes (-300 to -399)
    RIFT_ERROR_TYPE_MISMATCH = -300,
    RIFT_ERROR_UNDEFINED_VARIABLE = -301,
    RIFT_ERROR_DUPLICATE_DECLARATION = -302,
    RIFT_ERROR_SCOPE_RESOLUTION_FAILED = -303,
    RIFT_ERROR_INCOMPATIBLE_TYPES = -304,
    RIFT_ERROR_INVALID_OPERATION = -305,
    
    // Validation Error Codes (-400 to -499)
    RIFT_ERROR_VALIDATION_FAILED = -400,
    RIFT_ERROR_CONSTRAINT_VIOLATION = -401,
    RIFT_ERROR_RANGE_CHECK_FAILED = -402,
    RIFT_ERROR_INVARIANT_VIOLATION = -403,
    
    // Code Generation Error Codes (-500 to -599)
    RIFT_ERROR_CODEGEN_FAILED = -500,
    RIFT_ERROR_BYTECODE_GENERATION = -501,
    RIFT_ERROR_INVALID_INSTRUCTION = -502,
    RIFT_ERROR_REGISTER_ALLOCATION = -503,
    
    // Verification Error Codes (-600 to -699)
    RIFT_ERROR_VERIFICATION_FAILED = -600,
    RIFT_ERROR_BYTECODE_VERIFICATION = -601,
    RIFT_ERROR_SECURITY_CHECK_FAILED = -602,
    
    // Emission Error Codes (-700 to -799)
    RIFT_ERROR_EMISSION_FAILED = -700,
    RIFT_ERROR_OUTPUT_GENERATION = -701,
    RIFT_ERROR_SERIALIZATION_FAILED = -702,
    
    // Governance Error Codes (-800 to -899)
    RIFT_ERROR_GOVERNANCE_VIOLATION = -800,
    RIFT_ERROR_POLICY_VIOLATION = -801,
    RIFT_ERROR_SECURITY_VIOLATION = -802,
    RIFT_ERROR_COMPLIANCE_VIOLATION = -803,
    RIFT_ERROR_AUDIT_FAILED = -804,
    
    // System Error Codes (-900 to -999)
    RIFT_ERROR_SYSTEM_ERROR = -900,
    RIFT_ERROR_RESOURCE_EXHAUSTED = -901,
    RIFT_ERROR_DEADLOCK_DETECTED = -902,
    RIFT_ERROR_THREAD_SAFETY_VIOLATION = -903
} rift_error_code_t;

// RIFT Memory Management - AEGIS Compliant
typedef struct {
    void* ptr;
    size_t size;
    size_t alignment;
    bool is_aligned;
    const char* allocator_name;
} rift_memory_block_t;

// RIFT Source Location Tracking
typedef struct {
    const char* filename;
    size_t line_number;
    size_t column_number;
    size_t character_offset;
} rift_source_location_t;

// RIFT Error Context - Enhanced Debugging
typedef struct {
    rift_error_code_t error_code;
    char message[RIFT_MAX_ERROR_MESSAGE_LENGTH];
    rift_source_location_t location;
    const char* function_name;
    const char* component_name;
    int severity_level;
    uint64_t timestamp;
} rift_error_context_t;

// RIFT Performance Metrics
typedef struct {
    uint64_t start_time;
    uint64_t end_time;
    size_t memory_peak_usage;
    size_t memory_current_usage;
    size_t allocations_count;
    size_t complexity_score;
} rift_performance_metrics_t;

/*
 * Memory Management Functions - AEGIS Compliance
 */

/**
 * rift_aligned_alloc - AEGIS-compliant aligned memory allocation
 * @size: Size of memory to allocate
 * @alignment: Memory alignment requirement (must be power of 2)
 * 
 * Returns: Pointer to aligned memory, or NULL on failure
 */
void* rift_aligned_alloc(size_t size, size_t alignment);

/**
 * rift_aligned_free - Free aligned memory
 * @ptr: Pointer to aligned memory
 */
void rift_aligned_free(void* ptr);

/**
 * rift_memory_block_init - Initialize memory block tracking
 * @block: Memory block structure to initialize
 * @ptr: Allocated memory pointer
 * @size: Size of allocated memory
 * @alignment: Memory alignment used
 * @allocator_name: Name of allocator for debugging
 */
void rift_memory_block_init(rift_memory_block_t* block, void* ptr, 
                           size_t size, size_t alignment, 
                           const char* allocator_name);

/**
 * rift_memory_block_cleanup - Cleanup memory block
 * @block: Memory block to cleanup
 */
void rift_memory_block_cleanup(rift_memory_block_t* block);

/*
 * Error Handling Functions
 */

/**
 * rift_error_to_string - Convert error code to human-readable string
 * @error_code: RIFT error code
 * 
 * Returns: Static string describing the error
 */
const char* rift_error_to_string(rift_error_code_t error_code);

/**
 * rift_error_context_init - Initialize error context
 * @context: Error context structure to initialize
 * @error_code: Error code
 * @message: Error message
 * @location: Source location where error occurred
 * @function_name: Function name where error occurred
 * @component_name: Component name where error occurred
 */
void rift_error_context_init(rift_error_context_t* context,
                            rift_error_code_t error_code,
                            const char* message,
                            const rift_source_location_t* location,
                            const char* function_name,
                            const char* component_name);

/**
 * rift_error_context_print - Print formatted error context
 * @context: Error context to print
 * @output: File pointer for output
 */
void rift_error_context_print(const rift_error_context_t* context, FILE* output);

/*
 * Utility Functions
 */

/**
 * rift_source_location_init - Initialize source location
 * @location: Location structure to initialize
 * @filename: Source filename
 * @line: Line number
 * @column: Column number
 * @offset: Character offset
 */
void rift_source_location_init(rift_source_location_t* location,
                              const char* filename,
                              size_t line, size_t column, size_t offset);

/**
 * rift_performance_metrics_start - Start performance measurement
 * @metrics: Performance metrics structure
 */
void rift_performance_metrics_start(rift_performance_metrics_t* metrics);

/**
 * rift_performance_metrics_end - End performance measurement
 * @metrics: Performance metrics structure
 */
void rift_performance_metrics_end(rift_performance_metrics_t* metrics);

/**
 * rift_performance_metrics_print - Print performance metrics
 * @metrics: Performance metrics to print
 * @output: File pointer for output
 */
void rift_performance_metrics_print(const rift_performance_metrics_t* metrics, 
                                   FILE* output);

/*
 * String Utilities
 */

/**
 * rift_strdup - Duplicate string with AEGIS-compliant allocation
 * @str: String to duplicate
 * 
 * Returns: Duplicated string, or NULL on failure
 */
char* rift_strdup(const char* str);

/**
 * rift_strndup - Duplicate string with length limit
 * @str: String to duplicate
 * @max_len: Maximum length to duplicate
 * 
 * Returns: Duplicated string, or NULL on failure
 */
char* rift_strndup(const char* str, size_t max_len);

/**
 * rift_str_free - Free string allocated by rift_strdup
 * @str: String to free
 */
void rift_str_free(char* str);

/*
 * Debug and Logging Macros
 */

#ifdef RIFT_DEBUG_ENABLED
#define RIFT_DEBUG(fmt, ...) \
    fprintf(stderr, "[RIFT-DEBUG] %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define RIFT_DEBUG(fmt, ...) ((void)0)
#endif

#define RIFT_LOG_ERROR(fmt, ...) \
    fprintf(stderr, "[RIFT-ERROR] %s:%d in %s(): " fmt "\n", \
            __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define RIFT_LOG_WARNING(fmt, ...) \
    fprintf(stderr, "[RIFT-WARNING] %s:%d: " fmt "\n", \
            __FILE__, __LINE__, ##__VA_ARGS__)

#define RIFT_LOG_INFO(fmt, ...) \
    printf("[RIFT-INFO] " fmt "\n", ##__VA_ARGS__)

/*
 * AEGIS Assertion Macros
 */

#ifdef RIFT_AEGIS_ENABLED
#define RIFT_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            RIFT_LOG_ERROR("AEGIS Assertion Failed: %s", message); \
            abort(); \
        } \
    } while(0)

#define RIFT_REQUIRE(condition, error_code) \
    do { \
        if (!(condition)) { \
            RIFT_LOG_ERROR("AEGIS Requirement Failed: %s", #condition); \
            return error_code; \
        } \
    } while(0)
#else
#define RIFT_ASSERT(condition, message) assert(condition)
#define RIFT_REQUIRE(condition, error_code) \
    do { if (!(condition)) return error_code; } while(0)
#endif

/*
 * Compiler Compatibility
 */

#ifdef __GNUC__
#define RIFT_LIKELY(x) __builtin_expect(!!(x), 1)
#define RIFT_UNLIKELY(x) __builtin_expect(!!(x), 0)
#define RIFT_FORCE_INLINE __attribute__((always_inline)) inline
#define RIFT_NO_INLINE __attribute__((noinline))
#else
#define RIFT_LIKELY(x) (x)
#define RIFT_UNLIKELY(x) (x)
#define RIFT_FORCE_INLINE inline
#define RIFT_NO_INLINE
#endif

/*
 * Version Information
 */

/**
 * rift_get_version - Get RIFT framework version
 * @major: Pointer to store major version
 * @minor: Pointer to store minor version
 * @patch: Pointer to store patch version
 */
void rift_get_version(int* major, int* minor, int* patch);

/**
 * rift_get_version_string - Get version as string
 * 
 * Returns: Static version string
 */
const char* rift_get_version_string(void);

/**
 * rift_get_build_info - Get build information
 * 
 * Returns: Static build information string
 */
const char* rift_get_build_info(void);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_COMMON_H */
