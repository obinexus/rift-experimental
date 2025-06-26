/*
 * emitter.h - RIFT emission Interface
 * RIFT: RIFT Is a Flexible Translator
 * Stage: rift-6
 * OBINexus Computing Framework - Technical Implementation
 */

#ifndef RIFT_EMITTER_H
#define RIFT_EMITTER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* emission stage configuration */
#define RIFT_STAGE_EMISSION 1
#define RIFT_EMITTER_VERSION 0x040000

/* AEGIS methodology compliance structures */
typedef struct rift_emitter_context {
    uint32_t version;
    bool initialized;
    uint32_t thread_count;
    bool dual_mode_enabled;
    bool aegis_compliant;
    void *stage_data;
    void *next_stage_input;
} rift_emitter_context_t;

typedef struct rift_emitter_config {
    uint32_t processing_flags;
    uint32_t validation_level;
    bool trust_tagging_enabled;
    bool preserve_matched_state;
    char *output_format;
} rift_emitter_config_t;

/* Stage execution result */
typedef enum {
    RIFT_EMITTER_SUCCESS = 0,
    RIFT_EMITTER_ERROR_INVALID_INPUT = -1,
    RIFT_EMITTER_ERROR_PROCESSING = -2,
    RIFT_EMITTER_ERROR_VALIDATION = -3,
    RIFT_EMITTER_ERROR_MEMORY = -4
} rift_emitter_result_t;

/* Core API functions */
rift_emitter_context_t* rift_emitter_init(rift_emitter_config_t *config);
rift_emitter_result_t rift_emitter_process(
    rift_emitter_context_t *ctx,
    const void *input,
    size_t input_size,
    void **output,
    size_t *output_size
);
rift_emitter_result_t rift_emitter_validate(rift_emitter_context_t *ctx);
void rift_emitter_cleanup(rift_emitter_context_t *ctx);

/* Stage-specific functions */

#ifdef __cplusplus
}
#endif

#endif /* RIFT_${component^^}_H */
