/**
 * @file token_access_envelope.h
 * @brief RIFTLang Token Access Envelope and Governance Contract System
 * @author Aegis Development Team
 * @version 1.0.0
 * 
 * Implementation of comprehensive token access validation and governance 
 * contract enforcement for RIFTLang parallelism and concurrency system.
 * 
 * Supports:
 * - Full scope envelope validation for Gate 1 PolicyValidationRatio = 85%
 * - Memory segment boundary enforcement (span<row>, span<fixed>, span<superposed>)
 * - R/W/X policy validation bitfields
 * - Parent thread authority inheritance markers
 * - Integration with existing rift_telemetry.c infrastructure
 */

#ifndef TOKEN_ACCESS_ENVELOPE_H
#define TOKEN_ACCESS_ENVELOPE_H

#include "rift_common.h"
#include "rift_telemetry.h"
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

// =============================================================================
// MEMORY SEGMENT BOUNDARY DEFINITIONS
// =============================================================================

/**
 * @brief Memory span type classifications for governance contract validation
 */
typedef enum {
    SPAN_ROW,          // Ordered, expandable contexts with explicit anchors
    SPAN_FIXED,        // Singleton authority with permanent role binding
    SPAN_SUPERPOSED    // Tokens existing in multiple states simultaneously
} rift_span_type_t;

/**
 * @brief Memory segment boundary structure for governance enforcement
 */
typedef struct {
    rift_span_type_t span_type;     // Memory span classification
    void* memory_base;              // Base memory address
    size_t segment_size;            // Segment size in bytes
    uint32_t alignment_bits;        // Memory alignment requirements (4096 for classical, 8 for quantum)
    bool is_mutable;                // Mutability flag for governance enforcement
    uint64_t parent_segment_id;     // Parent segment for inheritance validation
} rift_memory_segment_t;

// =============================================================================
// POLICY VALIDATION BITFIELD DEFINITIONS
// =============================================================================

/**
 * @brief Access permission flags for R/W/X policy validation
 */
#define RIFT_ACCESS_READ    0x01    // Read permission
#define RIFT_ACCESS_WRITE   0x02    // Write permission  
#define RIFT_ACCESS_EXECUTE 0x04    // Execute permission
#define RIFT_ACCESS_CREATE  0x08    // Create new resources permission
#define RIFT_ACCESS_DELETE  0x10    // Delete resources permission
#define RIFT_ACCESS_INHERIT 0x20    // Inherit permissions to child threads

/**
 * @brief Policy validation matrix for governance contract enforcement
 */
typedef struct {
    uint32_t access_permissions;    // Bitfield of allowed access operations
    uint32_t restricted_operations; // Bitfield of explicitly denied operations
    uint64_t policy_version;        // Policy version for compatibility validation
    char policy_name[64];           // Human-readable policy identifier
    struct timespec policy_expiry;  // Policy expiration timestamp
} rift_policy_matrix_t;

// =============================================================================
// PARENT THREAD AUTHORITY INHERITANCE
// =============================================================================

/**
 * @brief Thread authority inheritance markers for governance chain validation
 */
typedef struct {
    uint64_t parent_thread_id;      // Parent RIFT thread identifier
    uint64_t authority_chain_depth; // Depth in authority inheritance chain
    rift_policy_matrix_t inherited_policy; // Policy inherited from parent
    uint32_t authority_restrictions; // Additional restrictions applied at this level
    bool can_delegate_authority;    // Whether this thread can create child threads
    uint32_t max_child_threads;     // Maximum number of child threads allowed
} rift_thread_authority_t;

// =============================================================================
// TOKEN ACCESS ENVELOPE STRUCTURE
// =============================================================================

/**
 * @brief Comprehensive token access envelope for governance contract validation
 * 
 * This structure implements the full scope envelope as specified for Phase 1
 * implementation, supporting Gate 1 PolicyValidationRatio requirements.
 */
typedef struct {
    // Memory segment boundaries
    rift_memory_segment_t* accessible_segments;
    uint32_t segment_count;
    
    // Policy validation matrix
    rift_policy_matrix_t policy;
    
    // Thread authority inheritance
    rift_thread_authority_t authority;
    
    // Envelope metadata
    uint64_t envelope_id;           // Unique envelope identifier
    struct timespec creation_time;  // Envelope creation timestamp
    uint64_t creator_thread_id;     // Thread that created this envelope
    uint32_t validation_checksum;   // Integrity validation checksum
    bool is_validated;              // Validation status flag
    
    // Governance contract enforcement
    pthread_mutex_t envelope_mutex; // Thread-safe access control
    uint32_t access_violation_count; // Number of access violations detected
    char violation_log[256];        // Brief violation history
} rift_token_access_envelope_t;

// =============================================================================
// JOB CONTEXT EXTENSION STRUCTURE
// =============================================================================

/**
 * @brief Extended job context structure integrating with existing telemetry
 * 
 * Extends rift_spawn_telemetry_t as specified in technical direction to
 * leverage proven PID/TID tracking and hierarchical spawn validation.
 */
typedef struct {
    rift_spawn_telemetry_t base_telemetry;  // Inherit existing tracking
    rift_governance_policy_t job_policy;    // Job-specific governance policy
    uint64_t job_hydration_id;              // Unique job identifier
    rift_token_access_envelope_t envelope;  // Token access boundaries
    
    // Job lifecycle management
    enum {
        JOB_STATE_CREATED,
        JOB_STATE_HYDRATED,
        JOB_STATE_DISPATCHED,
        JOB_STATE_EXECUTING,
        JOB_STATE_COMPLETED,
        JOB_STATE_FAILED,
        JOB_STATE_TERMINATED
    } job_state;
    
    // Cooperative multitasking support
    struct timespec last_yield_time;        // Last cooperative yield timestamp
    uint32_t governance_checkpoint_count;   // Number of governance checkpoints passed
    bool yield_requested;                   // Cooperative yield request flag
    
    // Job execution metrics
    uint64_t memory_allocations;            // Number of memory allocations
    uint64_t token_accesses;                // Number of token access operations
    uint64_t policy_validations;            // Number of policy validation checks
} rift_job_context_t;

// =============================================================================
// GOVERNANCE CONTRACT VALIDATION FUNCTIONS
// =============================================================================

/**
 * @brief Initialize a token access envelope with comprehensive validation
 * @param envelope Pointer to envelope structure to initialize
 * @param parent_authority Parent thread authority for inheritance
 * @param policy_name Name of policy to apply
 * @return 0 on success, error code on failure
 */
int rift_envelope_init(rift_token_access_envelope_t* envelope,
                       const rift_thread_authority_t* parent_authority,
                       const char* policy_name);

/**
 * @brief Validate token access against envelope permissions
 * @param envelope Token access envelope to validate against
 * @param memory_address Memory address being accessed
 * @param access_type Type of access being requested (R/W/X flags)
 * @return true if access is permitted, false if denied
 */
bool rift_envelope_validate_access(const rift_token_access_envelope_t* envelope,
                                   void* memory_address,
                                   uint32_t access_type);

/**
 * @brief Create memory segment boundary for governance enforcement
 * @param segment Pointer to segment structure to initialize
 * @param base_address Base memory address for segment
 * @param size Size of memory segment in bytes
 * @param span_type Type of memory span (row/fixed/superposed)
 * @return 0 on success, error code on failure
 */
int rift_segment_create(rift_memory_segment_t* segment,
                        void* base_address,
                        size_t size,
                        rift_span_type_t span_type);

/**
 * @brief Validate thread authority inheritance chain
 * @param authority Authority structure to validate
 * @param parent_context Parent job context for inheritance validation
 * @return true if authority chain is valid, false otherwise
 */
bool rift_authority_validate_chain(const rift_thread_authority_t* authority,
                                   const rift_job_context_t* parent_context);

/**
 * @brief Perform governance checkpoint validation
 * @param job_context Job context to validate
 * @param checkpoint_type Type of checkpoint being performed
 * @return 0 on successful validation, error code on failure
 */
int rift_governance_checkpoint(rift_job_context_t* job_context,
                               const char* checkpoint_type);

/**
 * @brief Calculate policy validation ratio for Gate 1 compliance
 * @param job_context Job context to analyze
 * @return Policy validation ratio as percentage (0.0 to 1.0)
 */
double rift_calculate_policy_validation_ratio(const rift_job_context_t* job_context);

/**
 * @brief Log access violation for governance audit trail
 * @param envelope Token access envelope where violation occurred
 * @param violation_type Description of violation type
 * @param memory_address Address where violation occurred
 */
void rift_envelope_log_violation(rift_token_access_envelope_t* envelope,
                                 const char* violation_type,
                                 void* memory_address);

/**
 * @brief Clean up and destroy token access envelope
 * @param envelope Envelope to clean up
 */
void rift_envelope_destroy(rift_token_access_envelope_t* envelope);

// =============================================================================
// GOVERNANCE CONTRACT ENFORCEMENT MACROS
// =============================================================================

/**
 * @brief Macro for governance checkpoint validation with automatic failure handling
 */
#define RIFT_GOVERNANCE_CHECKPOINT(job_ctx, checkpoint_name) \
    do { \
        int _checkpoint_result = rift_governance_checkpoint(job_ctx, checkpoint_name); \
        if (_checkpoint_result != 0) { \
            fprintf(stderr, "[GOVERNANCE] Checkpoint failed: %s (error %d)\n", \
                    checkpoint_name, _checkpoint_result); \
            return _checkpoint_result; \
        } \
    } while(0)

/**
 * @brief Macro for token access validation with violation logging
 */
#define RIFT_VALIDATE_TOKEN_ACCESS(envelope, addr, access_flags) \
    do { \
        if (!rift_envelope_validate_access(envelope, addr, access_flags)) { \
            rift_envelope_log_violation(envelope, "UNAUTHORIZED_ACCESS", addr); \
            return -1; \
        } \
    } while(0)

/**
 * @brief Macro for cooperative yield with governance state preservation
 */
#define RIFT_COOPERATIVE_YIELD(job_ctx) \
    do { \
        clock_gettime(CLOCK_MONOTONIC, &(job_ctx)->last_yield_time); \
        (job_ctx)->yield_requested = false; \
        pthread_yield(); \
    } while(0)

#endif // TOKEN_ACCESS_ENVELOPE_H
