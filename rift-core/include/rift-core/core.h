#ifndef RIFT_CORE_H
#define RIFT_CORE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* RIFT-Core Version Information */
#define RIFT_CORE_VERSION_MAJOR 2
#define RIFT_CORE_VERSION_MINOR 1
#define RIFT_CORE_VERSION_PATCH 0
#define RIFT_CORE_VERSION_STRING "2.1.0-core"

/* RIFT Exception Classification System */
typedef enum {
    RIFT_SUCCESS = 0,
    RIFT_ERROR_BASIC = 1,      /* 0-4: Non-blocking warnings */
    RIFT_ERROR_MODERATE = 5,   /* 5-6: Recoverable pause states */
    RIFT_ERROR_HIGH = 7,       /* 7-8: Escalated governance check */
    RIFT_ERROR_CRITICAL = 9    /* 9-12: Execution halt + panic mode */
} rift_result_t;

/* RIFT Context Management */
typedef struct {
    uint64_t rift_id;
    char rift_uuid[37];
    char rift_hash[65];
    uint32_t rift_prng_seed;
    uint64_t rift_timestamp;
} rift_context_t;

/* RIFT NULL/nil Semantics (Pure RIFT Implementation) */
#define RIFT_NULL ((void*)0)
#define RIFT_NIL  ((void*)0)

/* RIFT Memory Safety Macros */
#define RIFT_SAFE_FREE(ptr) do { \
    if ((ptr) != RIFT_NIL) { \
        free(ptr); \
        (ptr) = RIFT_NIL; \
    } \
} while(0)

/* RIFT Yoda-Style Condition Macros */
#define RIFT_YODA_EQ(constant, variable) ((constant) == (variable))
#define RIFT_YODA_NE(constant, variable) ((constant) != (variable))
#define RIFT_YODA_LT(constant, variable) ((constant) < (variable))
#define RIFT_YODA_GT(constant, variable) ((constant) > (variable))

/* RIFT-Core Initialization and Cleanup */
rift_result_t rift_core_init(void);
void rift_core_cleanup(void);

/* RIFT Context Management */
rift_result_t rift_context_create(rift_context_t *ctx);
void rift_context_destroy(rift_context_t *ctx);

/* RIFT Color Governance (Native Implementation) */
void rift_color_log(const char* level, const char* message);
void rift_color_log_critical(const char* message);
void rift_color_log_high(const char* message);
void rift_color_log_moderate(const char* message);
void rift_color_log_info(const char* message);
void rift_color_log_success(const char* message);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_H */
