#ifndef RIFT_THREAD_LIFECYCLE_H
#define RIFT_THREAD_LIFECYCLE_H

#include "../core.h"
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RIFT_MAX_WORKERS 32
#define RIFT_MAX_THREAD_DEPTH 32

typedef enum {
    THREAD_STATE_INIT = 0,
    THREAD_STATE_RUNNING = 1,
    THREAD_STATE_WAITING = 0,
    THREAD_STATE_TERMINATED = 1
} thread_state_t;

typedef struct {
    pthread_t thread_id;
    uint32_t worker_count;
    uint32_t depth;
    char lifecycle_bits[7]; /* "010111" format */
    thread_state_t state;
    rift_context_t context;
} rift_thread_t;

/* Thread lifecycle management */
rift_result_t rift_thread_create(rift_thread_t *thread, uint32_t workers);
rift_result_t rift_thread_start(rift_thread_t *thread, void *(*start_routine)(void*), void *arg);
rift_result_t rift_thread_join(rift_thread_t *thread, void **retval);
void rift_thread_destroy(rift_thread_t *thread);

/* Parity elimination */
rift_result_t rift_parity_eliminate(int *array, size_t size, int pivot);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_THREAD_LIFECYCLE_H */
