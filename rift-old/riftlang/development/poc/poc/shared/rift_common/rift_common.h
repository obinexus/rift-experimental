/**
 * @file rift_common.h
 * @brief RIFT Common Definitions and Structures
 * @author Aegis Development Team
 * @version 1.0.0
 * 
 * Shared definitions for RIFT concurrency governance system.
 * Provides core structures for telemetry, policy enforcement, and thread management.
 */

#ifndef RIFT_COMMON_H
#define RIFT_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/types.h>
#include <time.h>

// RIFT Governance Constants
#define RIFT_MAX_CHILDREN_PER_PROCESS 32
#define RIFT_MAX_HIERARCHY_DEPTH 8
#define RIFT_MAX_THREAD_COUNT 256
#define RIFT_SPAWN_LOCATION_MAX 128

// Forward declarations
typedef struct rift_spawn_telemetry rift_spawn_telemetry_t;
typedef struct rift_governance_policy rift_governance_policy_t;
typedef struct rift_thread_context rift_thread_context_t;
typedef struct rift_memory_token rift_memory_token_t;

// Concurrency mode enumeration
typedef enum {
    CONCURRENCY_SIMULATED,          // Single-thread cooperative
    CONCURRENCY_TRUE_THREAD,        // Multi-thread within process
    CONCURRENCY_TRUE_PROCESS        // Multi-process hierarchy
} rift_concurrency_mode_t;

// Destruction policy enumeration
typedef enum {
    DESTROY_CASCADE,                // Cascade destruction to children
    DESTROY_KEEP_ALIVE,            // Allow children to survive
    DESTROY_GRACEFUL,              // Graceful shutdown signal
    DESTROY_IMMEDIATE              // Immediate termination
} rift_destroy_policy_t;

// Core telemetry structure
struct rift_spawn_telemetry {
    pid_t process_id;               // System process ID
    pthread_t thread_id;            // POSIX thread ID
    uint64_t rift_thread_id;        // Internal RIFT thread identifier
    pid_t parent_process_id;        // Parent process PID
    uint64_t parent_rift_id;        // Parent RIFT thread ID
    struct timespec spawn_time;     // Thread/process creation timestamp
    char spawn_location[RIFT_SPAWN_LOCATION_MAX]; // Source location of spawn
    uint32_t hierarchy_depth;       // Depth in parent-child tree
    uint32_t child_count;           // Number of children spawned
    bool is_daemon;                 // Daemon thread flag
};

// Governance policy structure
struct rift_governance_policy {
    uint64_t rift_id;              // Internal RIFT identifier
    rift_concurrency_mode_t mode;  // Concurrency execution mode
    rift_destroy_policy_t destroy_policy; // Child destruction policy
    uint32_t max_children;         // Maximum children allowed
    uint32_t max_execution_time_ms; // Execution time limit
    bool trace_capped;             // Enable hierarchy depth limits
    uint32_t max_hierarchy_depth;  // Maximum tree depth
    bool daemon_mode;              // Daemon thread flag
    bool keep_alive;               // Survival policy flag
};

// Thread context structure
struct rift_thread_context {
    rift_spawn_telemetry_t telemetry;     // Spawn tracking and telemetry
    rift_governance_policy_t policy;      // Governance and policy constraints
    struct timespec last_heartbeat;       // Last activity timestamp
    uint32_t context_switches;            // Context switch counter
    volatile bool should_terminate;       // Termination signal
    void* module_specific_data;           // Module-specific context
};

// Memory governance token
struct rift_memory_token {
    uint64_t token_id;             // Unique token identifier
    uint64_t owner_rift_id;        // Owning RIFT thread ID
    pid_t owner_process_id;        // Owning process ID
    uint32_t access_mask;          // Permission bit mask (R/W/X)
    char resource_name[64];        // Resource identifier
    struct timespec acquisition_time; // When token was acquired
    uint32_t validation_bits;      // Token state validation
    bool is_transferable;          // Can be transferred between threads
};

// Function declarations
int rift_common_init(void);
void rift_common_cleanup(void);
uint64_t rift_generate_id(void);
const char* rift_mode_to_string(rift_concurrency_mode_t mode);
const char* rift_destroy_policy_to_string(rift_destroy_policy_t policy);

#endif // RIFT_COMMON_H
