/*
 * semantic.h - RIFT semantic Interface
 * RIFT: RIFT Is a Flexible Translator
 * Stage: rift-2
 * OBINexus Computing Framework - Technical Implementation
 */

#ifndef RIFT_SEMANTIC_H
#define RIFT_SEMANTIC_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* semantic stage configuration */
#define RIFT_STAGE_SEMANTIC 1
#define RIFT_SEMANTIC_VERSION 0x040000

/* AEGIS methodology compliance structures */
typedef struct rift_semantic_context {
    uint32_t version;
    bool initialized;
    uint32_t thread_count;
    bool dual_mode_enabled;
    bool aegis_compliant;
    void *stage_data;
    void *next_stage_input;
} rift_semantic_context_t;

typedef struct rift_semantic_config {
    uint32_t processing_flags;
    uint32_t validation_level;
    bool trust_tagging_enabled;
    bool preserve_matched_state;
    char *output_format;
} rift_semantic_config_t;

/* Stage execution result */
typedef enum {
    RIFT_SEMANTIC_SUCCESS = 0,
    RIFT_SEMANTIC_ERROR_INVALID_INPUT = -1,
    RIFT_SEMANTIC_ERROR_PROCESSING = -2,
    RIFT_SEMANTIC_ERROR_VALIDATION = -3,
    RIFT_SEMANTIC_ERROR_MEMORY = -4
} rift_semantic_result_t;

/* Core API functions */
rift_semantic_context_t* rift_semantic_init(rift_semantic_config_t *config);
rift_semantic_result_t rift_semantic_process(
    rift_semantic_context_t *ctx,
    const void *input,
    size_t input_size,
    void **output,
    size_t *output_size
);
rift_semantic_result_t rift_semantic_validate(rift_semantic_context_t *ctx);
void rift_semantic_cleanup(rift_semantic_context_t *ctx);

/* Stage-specific functions */

#ifdef __cplusplus
}
#endif

#endif /* RIFT_${component^^}_H */
