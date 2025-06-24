#ifndef RIFT1_ERRORS_H
#define RIFT1_ERRORS_H

/**
 * RIFT1 Error Constants
 * OBINexus AEGIS Framework - Stage 1
 * 
 * Standardized error codes for RIFT1 operations
 */

typedef enum {
    RIFT_SUCCESS = 0,
    RIFT_ERROR_NULL_POINTER = -1,        // Missing pointer argument
    RIFT_ERROR_NOT_FOUND = -2,           // Resource not found
    RIFT_ERROR_MEMORY_ALLOCATION = -3,   // Memory allocation failed
    RIFT_ERROR_PATTERN_INVALID = -4,     // Invalid pattern syntax
    RIFT_ERROR_COMPILATION_FAILED = -5,  // Pattern compilation failed
    RIFT_ERROR_AST_INVALID = -6,         // Invalid AST structure
    RIFT_ERROR_TOKEN_VALIDATION = -7,    // Token validation failed
    RIFT_ERROR_IO = -8,                  // I/O operation failed
    RIFT_ERROR_PARSE_FAILED = -9         // Parsing operation failed
} RiftResult;

// Error handling macros
#define RIFT_CHECK_NULL(ptr) \
    do { if (!(ptr)) return RIFT_ERROR_NULL_POINTER; } while(0)

#define RIFT_CHECK_RESULT(result) \
    do { if ((result) != RIFT_SUCCESS) return (result); } while(0)

#endif // RIFT1_ERRORS_H
