/**
 * @file rift_common.c
 * @brief RIFT Common Implementation
 * @author Aegis Development Team
 */

#include "rift_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// Global state
static bool g_rift_common_initialized = false;
static uint64_t g_next_rift_id = 1;
static pthread_mutex_t g_id_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Initialize RIFT common subsystem
 */
int rift_common_init(void) {
    if (g_rift_common_initialized) {
        return 0;
    }
    
    printf("[RIFT_COMMON] Initializing common subsystem\n");
    g_rift_common_initialized = true;
    return 0;
}

/**
 * @brief Cleanup RIFT common subsystem
 */
void rift_common_cleanup(void) {
    if (!g_rift_common_initialized) {
        return;
    }
    
    printf("[RIFT_COMMON] Cleaning up common subsystem\n");
    g_rift_common_initialized = false;
}

/**
 * @brief Generate unique RIFT ID
 */
uint64_t rift_generate_id(void) {
    pthread_mutex_lock(&g_id_mutex);
    uint64_t id = g_next_rift_id++;
    pthread_mutex_unlock(&g_id_mutex);
    return id;
}

/**
 * @brief Convert concurrency mode to string
 */
const char* rift_mode_to_string(rift_concurrency_mode_t mode) {
    switch (mode) {
        case CONCURRENCY_SIMULATED: return "SIMULATED";
        case CONCURRENCY_TRUE_THREAD: return "TRUE_THREAD";
        case CONCURRENCY_TRUE_PROCESS: return "TRUE_PROCESS";
        default: return "UNKNOWN";
    }
}

/**
 * @brief Convert destroy policy to string
 */
const char* rift_destroy_policy_to_string(rift_destroy_policy_t policy) {
    switch (policy) {
        case DESTROY_CASCADE: return "CASCADE";
        case DESTROY_KEEP_ALIVE: return "KEEP_ALIVE";
        case DESTROY_GRACEFUL: return "GRACEFUL";
        case DESTROY_IMMEDIATE: return "IMMEDIATE";
        default: return "UNKNOWN";
    }
}
