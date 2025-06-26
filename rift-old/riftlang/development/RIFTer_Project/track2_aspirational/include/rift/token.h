/**
 * @file token.h
 * @brief RIFTlang Semantic Token Architecture
 * 
 * Implements the triplet model: (token_memory, token_type, token_value)
 * Following The RIFTer's Way - Computing from the Heart
 */

#ifndef RIFT_TOKEN_H
#define RIFT_TOKEN_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @brief Memory governance contract for tokens
 * 
 * Implements span<row> memory architecture with role-based access
 */
typedef struct rift_token_memory {
    void *memory_span;           /* Memory allocation context */
    size_t span_size;           /* Size of governed memory */
    unsigned int access_level;   /* Role-based access permissions */
    bool is_mutable;            /* Mutability governance */
    char *governance_policy;     /* Policy contract reference */
} rift_token_memory_t;

/**
 * @brief Semantic type classification for tokens
 * 
 * Provides semantic contracts within memory governance constraints
 */
typedef struct rift_token_type {
    unsigned int type_id;        /* Semantic type identifier */
    char *semantic_class;        /* Human-readable type classification */
    bool is_validated;          /* Bayesian DAG validation status */
    void *type_constraints;     /* Policy-enforced type constraints */
} rift_token_type_t;

/**
 * @brief Token value with semantic preservation
 * 
 * Contains actual data only after governance and type validation
 */
typedef struct rift_token_value {
    void *data;                 /* Actual token data */
    size_t data_size;          /* Size of contained data */
    char *semantic_context;     /* Preserved human intent */
    bool is_resolved;          /* Semantic resolution status */
} rift_token_value_t;

/**
 * @brief Complete RIFTlang semantic token
 * 
 * Implements the triplet model with memory-first architecture
 */
typedef struct rift_token {
    rift_token_memory_t memory;  /* Memory governance (first) */
    rift_token_type_t type;      /* Semantic type (second) */
    rift_token_value_t value;    /* Actual value (third) */
    
    /* Metadata for semantic preservation */
    char *creation_context;      /* Development context when created */
    unsigned long timestamp;     /* Creation timestamp */
    bool is_breath_complete;     /* "Each token is a breath" validation */
} rift_token_t;

/**
 * @brief Initialize a new semantic token
 * 
 * Follows memory-first initialization pattern
 */
bool rift_token_init(rift_token_t *token, size_t memory_size, 
                     const char *governance_policy);

/**
 * @brief Validate token semantic integrity
 * 
 * Implements Bayesian DAG resolution validation
 */
bool rift_token_validate(rift_token_t *token);

/**
 * @brief Preserve token semantic context
 * 
 * Implements "import disk" context preservation
 */
bool rift_token_preserve_context(rift_token_t *token, const char *context);

#endif /* RIFT_TOKEN_H */
