/**
 * @file memory_governance.h
 * @brief RIFT Memory Governance Token System
 * 
 * Implementation of Memory as Governance Contract model for the
 * RIFT Auto-Seal Loader pipeline with stage-based verification.
 * 
 * @copyright Copyright (c) 2025 OBINexus Computing
 * @license Proprietary - All Rights Reserved
 */

#ifndef RIFT_MEMORY_GOVERNANCE_H
#define RIFT_MEMORY_GOVERNANCE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief RIFT Memory Token Structure
 */
typedef struct rift_memory_token {
    uint64_t token_id;                      /**< Unique token identifier */
    uint32_t stage_level;                   /**< Current RIFT stage (0,1,3,4,5) */
    uint64_t memory_hash;                   /**< SHA-512 memory fingerprint */
    uint64_t entropy_signature;             /**< Shannon entropy signature */
    uint64_t context_checksum;              /**< Context-aware integrity checksum */
    uint32_t governance_flags;              /**< Governance policy flags */
    size_t allocated_bytes;                 /**< Memory allocation size */
    void *memory_region;                    /**< Pointer to governed memory */
    char stage_signature[64];               /**< Stage-specific signature */
    bool anti_reversion_lock;               /**< Prevents backward transitions */
    uint64_t timestamp_created;             /**< Token creation timestamp */
    uint64_t timestamp_last_verified;       /**< Last verification timestamp */
} rift_memory_token_t;

/**
 * @brief Memory Governance Contract Structure
 */
typedef struct rift_memory_governance_contract {
    uint32_t contract_version;              /**< Contract version */
    uint32_t minimum_security_level;        /**< Minimum required security level */
    uint64_t governance_policy_hash;        /**< Policy hash for validation */
    size_t max_memory_allocation;           /**< Maximum allowed allocation */
    uint32_t allowed_stage_transitions;     /**< Bitmask of allowed transitions */
    bool enforce_anti_reversion;            /**< Enable anti-reversion protection */
    uint32_t entropy_threshold_percent;     /**< Minimum entropy threshold */
    char policy_name[128];                  /**< Human-readable policy name */
} rift_memory_governance_contract_t;

/**
 * @brief Governance Violation Record
 */
typedef struct rift_governance_violation {
    uint64_t violation_id;                  /**< Unique violation identifier */
    uint64_t token_id;                      /**< Associated token ID */
    uint32_t violation_type;                /**< Type of violation */
    uint32_t attempted_stage;               /**< Stage that was attempted */
    uint32_t current_stage;                 /**< Current valid stage */
    char violation_message[256];            /**< Detailed violation message */
    uint64_t timestamp;                     /**< Violation timestamp */
    bool governance_override_available;     /**< Can be overridden by governance */
} rift_governance_violation_t;

/**
 * @brief Memory Governance Engine
 */
typedef struct rift_memory_governance_engine {
    rift_memory_governance_contract_t *active_contract;
    rift_memory_token_t **token_registry;
    size_t token_count;
    size_t token_capacity;
    rift_governance_violation_t **violation_log;
    size_t violation_count;
    size_t violation_capacity;
    bool governance_active;
    uint64_t engine_start_time;
} rift_memory_governance_engine_t;

/* Core governance functions */

/**
 * @brief Verify Stage 0 memory governance (basic optional)
 */
bool verify_stage_0_memory_governance(
    rift_memory_token_t *token,
    const rift_memory_governance_contract_t *contract
);

/**
 * @brief Verify Stage 1 memory governance (sealed + signature)
 */
bool verify_stage_1_memory_governance(
    rift_memory_token_t *token,
    const rift_memory_governance_contract_t *contract
);

/**
 * @brief Verify Stage 3 memory governance (obfuscated + minimized + entropy-aware)
 */
bool verify_stage_3_memory_governance(
    rift_memory_token_t *token,
    const rift_memory_governance_contract_t *contract
);

/**
 * @brief Verify Stage 4 memory governance (hardened + encrypted + context-validated)
 */
bool verify_stage_4_memory_governance(
    rift_memory_token_t *token,
    const rift_memory_governance_contract_t *contract
);

/**
 * @brief Verify Stage 5 memory governance (fully sealed + anti-reversion + perfect integrity)
 */
bool verify_stage_5_memory_governance(
    rift_memory_token_t *token,
    const rift_memory_governance_contract_t *contract
);

/**
 * @brief Check for memory governance reversion attempts
 */
bool check_memory_governance_reversion(
    rift_memory_token_t *token,
    uint32_t target_stage,
    bool governance_mode
);

/* Utility functions */

/**
 * @brief Create a new memory governance engine
 */
rift_memory_governance_engine_t* rift_memory_governance_engine_create(
    const rift_memory_governance_contract_t *contract
);

/**
 * @brief Create a new memory token
 */
rift_memory_token_t* rift_memory_token_create(
    uint32_t stage_level,
    size_t memory_size,
    void *memory_region
);

/**
 * @brief Free memory governance engine
 */
void rift_memory_governance_engine_free(rift_memory_governance_engine_t *engine);

/**
 * @brief Free memory token
 */
void rift_memory_token_free(rift_memory_token_t *token);

/* Governance violation types */
#define RIFT_VIOLATION_INVALID_STAGE_TRANSITION    0x01
#define RIFT_VIOLATION_ENTROPY_THRESHOLD_FAILED    0x02
#define RIFT_VIOLATION_ANTI_REVERSION_BLOCKED      0x04
#define RIFT_VIOLATION_MEMORY_CORRUPTION           0x08
#define RIFT_VIOLATION_SIGNATURE_MISMATCH          0x10
#define RIFT_VIOLATION_CONTEXT_INTEGRITY_FAILED    0x20
#define RIFT_VIOLATION_GOVERNANCE_POLICY_BREACH    0x40

/* Stage transition flags */
#define RIFT_TRANSITION_ALLOW_0_TO_1    0x01
#define RIFT_TRANSITION_ALLOW_1_TO_3    0x02
#define RIFT_TRANSITION_ALLOW_3_TO_4    0x04
#define RIFT_TRANSITION_ALLOW_4_TO_5    0x08
#define RIFT_TRANSITION_GOVERNANCE_OVERRIDE 0x80

#endif /* RIFT_MEMORY_GOVERNANCE_H */
