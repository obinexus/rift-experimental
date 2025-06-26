#ifndef RIFT_CORE_H
#define RIFT_CORE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Version information */
#define RIFT_CORE_VERSION_MAJOR 0
#define RIFT_CORE_VERSION_MINOR 1
#define RIFT_CORE_VERSION_PATCH 0

/* Core types */
typedef enum {
    RIFT_SUCCESS = 0,
    RIFT_ERROR_BASIC = 1,
    RIFT_ERROR_MODERATE = 5,
    RIFT_ERROR_HIGH = 7,
    RIFT_ERROR_CRITICAL = 9
} rift_result_t;

typedef struct {
    uint64_t id;
    char uuid[37];
    char hash[65];
    uint32_t prng_seed;
} rift_context_t;

/* Null and nil semantics */
#define RIFT_NULL ((void*)0)
#define RIFT_NIL  ((void*)0)

/* Memory safety macros */
#define RIFT_SAFE_FREE(ptr) do { \
    if ((ptr) != RIFT_NIL) { \
        free(ptr); \
        (ptr) = RIFT_NIL; \
    } \
} while(0)

/* Yoda-style condition macro */
#define RIFT_YODA_EQ(constant, variable) ((constant) == (variable))
#define RIFT_YODA_NE(constant, variable) ((constant) != (variable))

/* Core initialization and cleanup */
rift_result_t rift_core_init(void);
void rift_core_cleanup(void);

/* Context management */
rift_result_t rift_context_create(rift_context_t *ctx);
void rift_context_destroy(rift_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_H */
