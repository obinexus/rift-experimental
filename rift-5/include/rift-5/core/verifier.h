/*
 * verifier.h - RIFT verification Interface
 * RIFT: RIFT Is a Flexible Translator
 * Stage: rift-5
 * OBINexus Computing Framework - Technical Implementation
 */

#ifndef RIFT_VERIFIER_H
#define RIFT_VERIFIER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* verification stage configuration */
#define RIFT_STAGE_VERIFICATION 1
#define RIFT_VERIFIER_VERSION 0x040000

/* AEGIS methodology compliance structures */
typedef struct rift_verifier_context {
    uint32_t version;
    bool initialized;
    uint32_t thread_count;
    bool dual_mode_enabled;
    bool aegis_compliant;
    void *stage_data;
    void *next_stage_input;
} rift_verifier_context_t;

typedef struct rift_verifier_config {
    uint32_t processing_flags;
    uint32_t validation_level;
    bool trust_tagging_enabled;
    bool preserve_matched_state;
    char *output_format;
} rift_verifier_config_t;

/* Stage execution result */
typedef enum {
    RIFT_VERIFIER_SUCCESS = 0,
    RIFT_VERIFIER_ERROR_INVALID_INPUT = -1,
    RIFT_VERIFIER_ERROR_PROCESSING = -2,
    RIFT_VERIFIER_ERROR_VALIDATION = -3,
    RIFT_VERIFIER_ERROR_MEMORY = -4
} rift_verifier_result_t;

/* Core API functions */
rift_verifier_context_t* rift_verifier_init(rift_verifier_config_t *config);
rift_verifier_result_t rift_verifier_process(
    rift_verifier_context_t *ctx,
    const void *input,
    size_t input_size,
    void **output,
    size_t *output_size
);
rift_verifier_result_t rift_verifier_validate(rift_verifier_context_t *ctx);
void rift_verifier_cleanup(rift_verifier_context_t *ctx);

/* Stage-specific functions */

#ifdef __cplusplus
}
#endif

#endif /* RIFT_${component^^}_H */
