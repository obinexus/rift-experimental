/**
 * RIFT Concurrency Governance - Modular Architecture
 * Aegis Development Team - Waterfall Implementation
 * 
 * Project Structure:
 * rift_concurrency/
 * ÃÄÄ common/
 * ³   ÃÄÄ rift_common.h           # Shared definitions and structures
 * ³   ÃÄÄ rift_telemetry.c/h      # PID/TID tracking and spawn telemetry
 * ³   ÀÄÄ rift_governance.c/h     # Core governance contracts
 * ÃÄÄ simulated/
 * ³   ÃÄÄ rift_simulated.c        # Single-thread cooperative multitasking
 * ³   ÃÄÄ rift_simulated.h        # Simulated concurrency interface
 * ³   ÀÄÄ Makefile                # Simulated concurrency build
 * ÃÄÄ true_concurrency/
 * ³   ÃÄÄ rift_true_concurrency.c # Multi-thread true concurrency
 * ³   ÃÄÄ rift_process_hierarchy.c # Parent-child process management
 * ³   ÃÄÄ rift_true_concurrency.h # True concurrency interface
 * ³   ÀÄÄ Makefile                # True concurrency build
 * ÃÄÄ tests/
 * ³   ÃÄÄ test_simulated.c        # Simulated concurrency tests
 * ³   ÀÄÄ test_true_concurrency.c # True concurrency tests
 * ÀÄÄ Makefile.master             # Master build coordination
 */

// =============================================================================
// COMMON DEFINITIONS - rift_common.h
// =============================================================================

#ifndef RIFT_COMMON_H
#define RIFT_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/types.h>
#include <time.h>

// Maximum hierarchy constraints per RIFT governance
#define RIFT_MAX_CHILDREN_PER_PROCESS 32
#define RIFT_MAX_HIERARCHY_DEPTH 8
#define RIFT_MAX_THREAD_COUNT 256

// Telemetry and tracking structures
typedef struct {
    pid_t process_id;               // System process ID
    pthread_t thread_id;            // Thread ID (if applicable)
    uint64_t rift_thread_id;        // Internal RIFT thread identifier
    pid_t parent_process_id;        // Parent process PID
    uint64_t parent_rift_id;        // Parent RIFT thread ID
    struct timespec spawn_time;     // Thread/process creation timestamp
    char spawn_location[128];       // Where in code this was spawned
    uint32_t hierarchy_depth;       // Depth in parent-child tree
    uint32_t child_count;           // Number of children spawned
    bool is_daemon;                 // Daemon thread flag
} rift_spawn_telemetry_t;

// Governance policy structure (shared between modules)
typedef enum {
    CONCURRENCY_SIMULATED,          // Single-thread cooperative
    CONCURRENCY_TRUE_THREAD,        // Multi-thread within process
    CONCURRENCY_TRUE_PROCESS        // Multi-process hierarchy
} rift_concurrency_mode_t;

typedef enum {
    DESTROY_CASCADE,                // Cascade destruction to children
    DESTROY_KEEP_ALIVE,            // Allow children to survive
    DESTROY_GRACEFUL,              // Graceful shutdown signal
    DESTROY_IMMEDIATE              // Immediate termination
} rift_destroy_policy_t;

typedef struct {
    uint64_t rift_id;              // Internal RIFT identifier
    rift_concurrency_mode_t mode;  // Concurrency execution mode
    rift_destroy_policy_t destroy_policy; // Child destruction policy
    uint32_t max_children;         // Maximum children allowed
    uint32_t max_execution_time_ms; // Execution time limit
    bool trace_capped;             // Enable hierarchy depth limits
    uint32_t max_hierarchy_depth;  // Maximum tree depth
    bool daemon_mode;              // Daemon thread flag
    bool keep_alive;               // Survival policy flag
} rift_governance_policy_t;

// Thread context structure (shared between modules)
typedef struct {
    rift_spawn_telemetry_t telemetry;     // Spawn tracking and telemetry
    rift_governance_policy_t policy;      // Governance and policy constraints
    struct timespec last_heartbeat;       // Last activity timestamp
    uint32_t context_switches;            // Context switch counter
    volatile bool should_terminate;       // Termination signal
    void* module_specific_data;           // Module-specific context
} rift_thread_context_t;

// Memory token for resource governance
typedef struct {
    uint64_t token_id;             // Unique token identifier
    uint64_t owner_rift_id;        // Owning RIFT thread ID
    pid_t owner_process_id;        // Owning process ID
    uint32_t access_mask;          // Permission bit mask (R/W/X)
    char resource_name[64];        // Resource identifier
    struct timespec acquisition_time; // When token was acquired
    uint32_t validation_bits;      // Token state validation
    bool is_transferable;          // Can be transferred between threads
} rift_memory_token_t;

#endif // RIFT_COMMON_H

// =============================================================================
// TELEMETRY IMPLEMENTATION - rift_telemetry.h
// =============================================================================

#ifndef RIFT_TELEMETRY_H
#define RIFT_TELEMETRY_H

#include "rift_common.h"

/**
 * @brief Initialize telemetry subsystem
 * @return 0 on success, error code otherwise
 */
int rift_telemetry_init(void);

/**
 * @brief Register new thread/process spawn with telemetry
 * @param context Thread context to register
 * @param spawn_location Source location where spawn occurred
 * @return 0 on success, error code otherwise
 */
int rift_telemetry_register_spawn(rift_thread_context_t* context, const char* spawn_location);

/**
 * @brief Update heartbeat for thread/process
 * @param rift_id RIFT thread identifier
 * @return 0 on success, error code otherwise
 */
int rift_telemetry_heartbeat(uint64_t rift_id);

/**
 * @brief Get spawn telemetry for specific thread
 * @param rift_id RIFT thread identifier
 * @return Pointer to telemetry data, NULL if not found
 */
rift_spawn_telemetry_t* rift_telemetry_get(uint64_t rift_id);

/**
 * @brief Print comprehensive telemetry report
 */
void rift_telemetry_print_report(void);

/**
 * @brief Validate hierarchy constraints for new spawn
 * @param parent_rift_id Parent RIFT thread ID
 * @param proposed_policy Proposed governance policy
 * @return true if spawn allowed, false if violates constraints
 */
bool rift_telemetry_validate_spawn(uint64_t parent_rift_id, const rift_governance_policy_t* proposed_policy);

#endif // RIFT_TELEMETRY_H

// =============================================================================
// SIMULATED CONCURRENCY MODULE - rift_simulated.h
// =============================================================================

#ifndef RIFT_SIMULATED_H
#define RIFT_SIMULATED_H

#include "rift_common.h"

// Simulated concurrency specific structures
typedef struct {
    rift_thread_context_t base_context;   // Common thread context
    uint32_t time_slice_us;               // Time slice in microseconds
    uint32_t current_slice;               // Current time slice counter
    bool yield_requested;                 // Cooperative yield flag
    void (*work_function)(void*);         // Work function pointer
    void* work_data;                      // Work function data
} rift_simulated_context_t;

/**
 * @brief Initialize simulated concurrency subsystem
 * @return 0 on success, error code otherwise
 */
int rift_simulated_init(void);

/**
 * @brief Create new simulated concurrent task
 * @param parent_id Parent RIFT thread ID (0 for root)
 * @param policy Governance policy for new task
 * @param work_func Work function to execute
 * @param work_data Data to pass to work function
 * @param spawn_location Source location of spawn
 * @return RIFT thread ID on success, 0 on failure
 */
uint64_t rift_simulated_spawn(uint64_t parent_id, 
                              const rift_governance_policy_t* policy,
                              void (*work_func)(void*), 
                              void* work_data,
                              const char* spawn_location);

/**
 * @brief Execute one scheduling cycle of simulated concurrency
 * @return Number of active tasks processed
 */
int rift_simulated_schedule_cycle(void);

/**
 * @brief Request cooperative yield from current task
 */
void rift_simulated_yield(void);

/**
 * @brief Terminate simulated concurrent task
 * @param rift_id RIFT thread ID to terminate
 * @return 0 on success, error code otherwise
 */
int rift_simulated_terminate(uint64_t rift_id);

/**
 * @brief Clean up simulated concurrency subsystem
 */
void rift_simulated_cleanup(void);

#endif // RIFT_SIMULATED_H

// =============================================================================
// TRUE CONCURRENCY MODULE - rift_true_concurrency.h  
// =============================================================================

#ifndef RIFT_TRUE_CONCURRENCY_H
#define RIFT_TRUE_CONCURRENCY_H

#include "rift_common.h"

// True concurrency specific structures
typedef struct {
    rift_thread_context_t base_context;   // Common thread context
    pthread_t pthread_handle;             // POSIX thread handle
    pid_t child_process_id;               // Child process ID (if process mode)
    pthread_mutex_t lifecycle_mutex;      // Thread lifecycle synchronization
    pthread_cond_t lifecycle_condition;   // Thread lifecycle condition
    void (*work_function)(void*);         // Work function pointer
    void* work_data;                      // Work function data
} rift_true_context_t;

/**
 * @brief Initialize true concurrency subsystem
 * @return 0 on success, error code otherwise
 */
int rift_true_concurrency_init(void);

/**
 * @brief Spawn new thread with true concurrency
 * @param parent_id Parent RIFT thread ID
 * @param policy Governance policy for new thread
 * @param work_func Work function to execute
 * @param work_data Data to pass to work function
 * @param spawn_location Source location of spawn
 * @return RIFT thread ID on success, 0 on failure
 */
uint64_t rift_true_spawn_thread(uint64_t parent_id,
                                const rift_governance_policy_t* policy,
                                void (*work_func)(void*),
                                void* work_data,
                                const char* spawn_location);

/**
 * @brief Spawn new process with true concurrency
 * @param parent_id Parent RIFT thread ID
 * @param policy Governance policy for new process
 * @param work_func Work function to execute
 * @param work_data Data to pass to work function
 * @param spawn_location Source location of spawn
 * @return RIFT thread ID on success, 0 on failure
 */
uint64_t rift_true_spawn_process(uint64_t parent_id,
                                 const rift_governance_policy_t* policy,
                                 void (*work_func)(void*),
                                 void* work_data,
                                 const char* spawn_location);

/**
 * @brief Handle parent destruction with policy enforcement
 * @param parent_id RIFT ID of destroyed parent
 * @return 0 on success, error code otherwise
 */
int rift_true_handle_parent_destruction(uint64_t parent_id);

/**
 * @brief Terminate thread or process
 * @param rift_id RIFT thread ID to terminate
 * @return 0 on success, error code otherwise
 */
int rift_true_terminate(uint64_t rift_id);

/**
 * @brief Clean up true concurrency subsystem
 */
void rift_true_concurrency_cleanup(void);

#endif // RIFT_TRUE_CONCURRENCY_H
