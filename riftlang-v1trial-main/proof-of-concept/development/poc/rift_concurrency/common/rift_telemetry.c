/**
 * @file rift_telemetry.c
 * @brief RIFT Telemetry and Process Hierarchy Management
 * @author Aegis Development Team
 * @version 1.0.0
 * 
 * Implementation of PID/TID tracking with spawn location telemetry
 * and 32-child process hierarchy governance per RIFT specification.
 */

#include "rift_telemetry.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <errno.h>

// =============================================================================
// TELEMETRY REGISTRY AND TRACKING
// =============================================================================

// Global telemetry registry with thread-safe access
typedef struct {
    rift_spawn_telemetry_t registry[RIFT_MAX_THREAD_COUNT];
    bool registry_active[RIFT_MAX_THREAD_COUNT];
    uint32_t active_count;
    uint64_t next_rift_id;
    pthread_rwlock_t registry_lock;
    pthread_mutex_t id_generation_mutex;
} rift_telemetry_registry_t;

static rift_telemetry_registry_t g_telemetry_registry = {0};

// Process hierarchy tracking
typedef struct {
    pid_t process_id;
    uint64_t rift_id;
    uint32_t child_count;
    uint64_t children[RIFT_MAX_CHILDREN_PER_PROCESS];
    struct timespec spawn_time;
    char spawn_location[128];
} rift_process_node_t;

static rift_process_node_t g_process_hierarchy[RIFT_MAX_THREAD_COUNT];
static uint32_t g_process_count = 0;
static pthread_mutex_t g_hierarchy_mutex = PTHREAD_MUTEX_INITIALIZER;

// Telemetry configuration
static bool g_telemetry_initialized = false;
static FILE* g_telemetry_log = NULL;

// =============================================================================
// TELEMETRY INITIALIZATION AND MANAGEMENT
// =============================================================================

/**
 * @brief Initialize telemetry subsystem with process tracking
 */
int rift_telemetry_init(void) {
    if (g_telemetry_initialized) {
        return 0; // Already initialized
    }
    
    // Initialize registry locks
    if (pthread_rwlock_init(&g_telemetry_registry.registry_lock, NULL) != 0) {
        fprintf(stderr, "[TELEMETRY] Failed to initialize registry lock\n");
        return -1;
    }
    
    if (pthread_mutex_init(&g_telemetry_registry.id_generation_mutex, NULL) != 0) {
        fprintf(stderr, "[TELEMETRY] Failed to initialize ID generation mutex\n");
        pthread_rwlock_destroy(&g_telemetry_registry.registry_lock);
        return -1;
    }
    
    // Initialize registry state
    memset(g_telemetry_registry.registry, 0, sizeof(g_telemetry_registry.registry));
    memset(g_telemetry_registry.registry_active, false, sizeof(g_telemetry_registry.registry_active));
    g_telemetry_registry.active_count = 0;
    g_telemetry_registry.next_rift_id = 1;
    
    // Initialize process hierarchy
    memset(g_process_hierarchy, 0, sizeof(g_process_hierarchy));
    g_process_count = 0;
    
    // Open telemetry log file
    g_telemetry_log = fopen("rift_telemetry.log", "a");
    if (!g_telemetry_log) {
        fprintf(stderr, "[TELEMETRY] Warning: Could not open telemetry log file\n");
    }
    
    // Register main process
    rift_thread_context_t main_context = {0};
    main_context.telemetry.process_id = getpid();
    main_context.telemetry.thread_id = pthread_self();
    main_context.telemetry.parent_process_id = getppid();
    clock_gettime(CLOCK_MONOTONIC, &main_context.telemetry.spawn_time);
    strncpy(main_context.telemetry.spawn_location, "main()", 
            sizeof(main_context.telemetry.spawn_location) - 1);
    
    if (rift_telemetry_register_spawn(&main_context, "main()") < 0) {
        fprintf(stderr, "[TELEMETRY] Failed to register main process\n");
        return -1;
    }
    
    g_telemetry_initialized = true;
    
    printf("[TELEMETRY] Initialized - Main Process PID: %d, TID: %lu\n", 
           getpid(), (unsigned long)pthread_self());
    
    if (g_telemetry_log) {
        fprintf(g_telemetry_log, "[INIT] Telemetry initialized for PID %d at %ld.%09ld\n",
                getpid(), main_context.telemetry.spawn_time.tv_sec, 
                main_context.telemetry.spawn_time.tv_nsec);
        fflush(g_telemetry_log);
    }
    
    return 0;
}

/**
 * @brief Generate unique RIFT thread ID with thread safety
 */
static uint64_t generate_rift_id(void) {
    pthread_mutex_lock(&g_telemetry_registry.id_generation_mutex);
    uint64_t id = g_telemetry_registry.next_rift_id++;
    pthread_mutex_unlock(&g_telemetry_registry.id_generation_mutex);
    return id;
}

/**
 * @brief Get current thread ID (Linux-specific)
 */
static pid_t get_thread_id(void) {
    return syscall(SYS_gettid);
}

// =============================================================================
// SPAWN REGISTRATION AND TRACKING
// =============================================================================

/**
 * @brief Register new thread/process spawn with comprehensive telemetry
 */
int rift_telemetry_register_spawn(rift_thread_context_t* context, const char* spawn_location) {
    if (!g_telemetry_initialized) {
        return -1;
    }
    
    if (!context || !spawn_location) {
        return -1;
    }
    
    // Generate unique RIFT ID
    context->telemetry.rift_thread_id = generate_rift_id();
    context->telemetry.process_id = getpid();
    context->telemetry.thread_id = pthread_self();
    
    // Copy spawn location with bounds checking
    strncpy(context->telemetry.spawn_location, spawn_location, 
            sizeof(context->telemetry.spawn_location) - 1);
    context->telemetry.spawn_location[sizeof(context->telemetry.spawn_location) - 1] = '\0';
    
    // Set spawn timestamp
    clock_gettime(CLOCK_MONOTONIC, &context->telemetry.spawn_time);
    
    // Acquire write lock for registry modification
    pthread_rwlock_wrlock(&g_telemetry_registry.registry_lock);
    
    // Find available registry slot
    int slot = -1;
    for (int i = 0; i < RIFT_MAX_THREAD_COUNT; i++) {
        if (!g_telemetry_registry.registry_active[i]) {
            slot = i;
            break;
        }
    }
    
    if (slot == -1) {
        pthread_rwlock_unlock(&g_telemetry_registry.registry_lock);
        fprintf(stderr, "[TELEMETRY] Error: Registry full, cannot register new spawn\n");
        return -1;
    }
    
    // Register the spawn
    g_telemetry_registry.registry[slot] = context->telemetry;
    g_telemetry_registry.registry_active[slot] = true;
    g_telemetry_registry.active_count++;
    
    pthread_rwlock_unlock(&g_telemetry_registry.registry_lock);
    
    // Log spawn event
    printf("[TELEMETRY] Registered spawn - RIFT ID: %lu, PID: %d, TID: %lu, Location: %s\n",
           context->telemetry.rift_thread_id, context->telemetry.process_id,
           (unsigned long)context->telemetry.thread_id, spawn_location);
    
    if (g_telemetry_log) {
        fprintf(g_telemetry_log, "[SPAWN] RIFT:%lu PID:%d TID:%lu Parent:%d Location:%s Time:%ld.%09ld\n",
                context->telemetry.rift_thread_id, context->telemetry.process_id,
                (unsigned long)context->telemetry.thread_id, context->telemetry.parent_process_id,
                spawn_location, context->telemetry.spawn_time.tv_sec, 
                context->telemetry.spawn_time.tv_nsec);
        fflush(g_telemetry_log);
    }
    
    return 0;
}

// =============================================================================
// PROCESS HIERARCHY MANAGEMENT
// =============================================================================

/**
 * @brief Validate spawn against hierarchy constraints
 */
bool rift_telemetry_validate_spawn(uint64_t parent_rift_id, const rift_governance_policy_t* proposed_policy) {
    if (!g_telemetry_initialized || !proposed_policy) {
        return false;
    }
    
    pthread_mutex_lock(&g_hierarchy_mutex);
    
    // Find parent process in hierarchy
    rift_process_node_t* parent_node = NULL;
    for (uint32_t i = 0; i < g_process_count; i++) {
        if (g_process_hierarchy[i].rift_id == parent_rift_id) {
            parent_node = &g_process_hierarchy[i];
            break;
        }
    }
    
    bool validation_result = true;
    
    if (parent_node) {
        // Check child count limit (32 children per process)
        if (parent_node->child_count >= RIFT_MAX_CHILDREN_PER_PROCESS) {
            printf("[TELEMETRY] Spawn validation failed: Parent %lu has reached child limit (%d/%d)\n",
                   parent_rift_id, parent_node->child_count, RIFT_MAX_CHILDREN_PER_PROCESS);
            validation_result = false;
        }
        
        // Check proposed hierarchy depth
        if (proposed_policy->max_hierarchy_depth > RIFT_MAX_HIERARCHY_DEPTH) {
            printf("[TELEMETRY] Spawn validation failed: Proposed hierarchy depth %d exceeds limit %d\n",
                   proposed_policy->max_hierarchy_depth, RIFT_MAX_HIERARCHY_DEPTH);
            validation_result = false;
        }
    }
    
    // Check global thread count limit
    if (g_telemetry_registry.active_count >= RIFT_MAX_THREAD_COUNT) {
        printf("[TELEMETRY] Spawn validation failed: Global thread count limit reached (%d/%d)\n",
               g_telemetry_registry.active_count, RIFT_MAX_THREAD_COUNT);
        validation_result = false;
    }
    
    pthread_mutex_unlock(&g_hierarchy_mutex);
    
    if (validation_result) {
        printf("[TELEMETRY] Spawn validation passed for parent %lu\n", parent_rift_id);
    }
    
    return validation_result;
}

/**
 * @brief Add child to process hierarchy
 */
int rift_telemetry_add_child(uint64_t parent_rift_id, uint64_t child_rift_id, const char* spawn_location) {
    pthread_mutex_lock(&g_hierarchy_mutex);
    
    // Find or create parent node
    rift_process_node_t* parent_node = NULL;
    for (uint32_t i = 0; i < g_process_count; i++) {
        if (g_process_hierarchy[i].rift_id == parent_rift_id) {
            parent_node = &g_process_hierarchy[i];
            break;
        }
    }
    
    if (!parent_node && g_process_count < RIFT_MAX_THREAD_COUNT) {
        // Create new parent node
        parent_node = &g_process_hierarchy[g_process_count++];
        parent_node->rift_id = parent_rift_id;
        parent_node->process_id = getpid();
        parent_node->child_count = 0;
        clock_gettime(CLOCK_MONOTONIC, &parent_node->spawn_time);
        strncpy(parent_node->spawn_location, "system_create", 
                sizeof(parent_node->spawn_location) - 1);
    }
    
    int result = -1;
    if (parent_node && parent_node->child_count < RIFT_MAX_CHILDREN_PER_PROCESS) {
        // Add child to parent's children array
        parent_node->children[parent_node->child_count] = child_rift_id;
        parent_node->child_count++;
        result = 0;
        
        printf("[TELEMETRY] Added child %lu to parent %lu (%d/%d children)\n",
               child_rift_id, parent_rift_id, parent_node->child_count, 
               RIFT_MAX_CHILDREN_PER_PROCESS);
        
        if (g_telemetry_log) {
            fprintf(g_telemetry_log, "[HIERARCHY] Parent:%lu Child:%lu Count:%d Location:%s\n",
                    parent_rift_id, child_rift_id, parent_node->child_count, spawn_location);
            fflush(g_telemetry_log);
        }
    }
    
    pthread_mutex_unlock(&g_hierarchy_mutex);
    return result;
}

// =============================================================================
// TELEMETRY QUERY AND REPORTING
// =============================================================================

/**
 * @brief Get spawn telemetry for specific RIFT thread
 */
rift_spawn_telemetry_t* rift_telemetry_get(uint64_t rift_id) {
    if (!g_telemetry_initialized) {
        return NULL;
    }
    
    pthread_rwlock_rdlock(&g_telemetry_registry.registry_lock);
    
    rift_spawn_telemetry_t* result = NULL;
    for (int i = 0; i < RIFT_MAX_THREAD_COUNT; i++) {
        if (g_telemetry_registry.registry_active[i] && 
            g_telemetry_registry.registry[i].rift_thread_id == rift_id) {
            result = &g_telemetry_registry.registry[i];
            break;
        }
    }
    
    pthread_rwlock_unlock(&g_telemetry_registry.registry_lock);
    return result;
}

/**
 * @brief Update heartbeat for thread/process
 */
int rift_telemetry_heartbeat(uint64_t rift_id) {
    rift_spawn_telemetry_t* telemetry = rift_telemetry_get(rift_id);
    if (!telemetry) {
        return -1;
    }
    
    // Update heartbeat timestamp (this would need thread safety in production)
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    
    // For now, we just log the heartbeat
    if (g_telemetry_log) {
        fprintf(g_telemetry_log, "[HEARTBEAT] RIFT:%lu PID:%d Time:%ld.%09ld\n",
                rift_id, telemetry->process_id, current_time.tv_sec, current_time.tv_nsec);
        fflush(g_telemetry_log);
    }
    
    return 0;
}

/**
 * @brief Print comprehensive telemetry report
 */
void rift_telemetry_print_report(void) {
    if (!g_telemetry_initialized) {
        printf("[TELEMETRY] Not initialized\n");
        return;
    }
    
    printf("\n");
    printf("=== RIFT TELEMETRY REPORT ===\n");
    printf("Active Threads: %d/%d\n", g_telemetry_registry.active_count, RIFT_MAX_THREAD_COUNT);
    printf("Process Hierarchy Nodes: %d/%d\n", g_process_count, RIFT_MAX_THREAD_COUNT);
    printf("\n");
    
    pthread_rwlock_rdlock(&g_telemetry_registry.registry_lock);
    
    printf("SPAWN REGISTRY:\n");
    printf("%-10s %-8s %-12s %-12s %-8s %-20s %s\n", 
           "RIFT_ID", "PID", "TID", "PARENT_PID", "DEPTH", "SPAWN_TIME", "LOCATION");
    printf("%-10s %-8s %-12s %-12s %-8s %-20s %s\n", 
           "-------", "---", "---", "----------", "-----", "----------", "--------");
    
    for (int i = 0; i < RIFT_MAX_THREAD_COUNT; i++) {
        if (g_telemetry_registry.registry_active[i]) {
            rift_spawn_telemetry_t* t = &g_telemetry_registry.registry[i];
            printf("%-10lu %-8d %-12lu %-12d %-8d %-20ld %s\n",
                   t->rift_thread_id, t->process_id, (unsigned long)t->thread_id,
                   t->parent_process_id, t->hierarchy_depth, t->spawn_time.tv_sec,
                   t->spawn_location);
        }
    }
    
    pthread_rwlock_unlock(&g_telemetry_registry.registry_lock);
    
    pthread_mutex_lock(&g_hierarchy_mutex);
    
    printf("\nPROCESS HIERARCHY:\n");
    printf("%-10s %-8s %-8s %s\n", "RIFT_ID", "PID", "CHILDREN", "LOCATION");
    printf("%-10s %-8s %-8s %s\n", "-------", "---", "--------", "--------");
    
    for (uint32_t i = 0; i < g_process_count; i++) {
        rift_process_node_t* p = &g_process_hierarchy[i];
        printf("%-10lu %-8d %-8d %s\n",
               p->rift_id, p->process_id, p->child_count, p->spawn_location);
        
        // Print children with indentation
        for (uint32_t j = 0; j < p->child_count; j++) {
            printf("  юд Child: %lu\n", p->children[j]);
        }
    }
    
    pthread_mutex_unlock(&g_hierarchy_mutex);
    
    printf("\n=== END TELEMETRY REPORT ===\n\n");
}

/**
 * @brief Cleanup telemetry subsystem
 */
void rift_telemetry_cleanup(void) {
    if (!g_telemetry_initialized) {
        return;
    }
    
    printf("[TELEMETRY] Cleaning up telemetry subsystem\n");
    
    // Print final report
    rift_telemetry_print_report();
    
    // Close log file
    if (g_telemetry_log) {
        fprintf(g_telemetry_log, "[CLEANUP] Telemetry subsystem shutdown\n");
        fclose(g_telemetry_log);
        g_telemetry_log = NULL;
    }
    
    // Destroy synchronization primitives
    pthread_rwlock_destroy(&g_telemetry_registry.registry_lock);
    pthread_mutex_destroy(&g_telemetry_registry.id_generation_mutex);
    pthread_mutex_destroy(&g_hierarchy_mutex);
    
    g_telemetry_initialized = false;
}

// =============================================================================
// CONVENIENCE MACROS FOR SPAWN LOCATION TRACKING
// =============================================================================

/**
 * Macro to automatically capture spawn location for use in spawn calls
 */
#define RIFT_SPAWN_LOCATION() \
    (__FILE__ ":" #__LINE__ " in " __func__ "()")

/**
 * Macro to register spawn with automatic location capture
 */
#define RIFT_REGISTER_SPAWN(context) \
    rift_telemetry_register_spawn(context, RIFT_SPAWN_LOCATION())

/**
 * Macro to validate spawn with automatic location capture
 */
#define RIFT_VALIDATE_SPAWN(parent_id, policy) \
    rift_telemetry_validate_spawn(parent_id, policy)

/**
 * Macro to add child with automatic location capture
 */
#define RIFT_ADD_CHILD(parent_id, child_id) \
    rift_telemetry_add_child(parent_id, child_id, RIFT_SPAWN_LOCATION())
