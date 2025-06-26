/*
 * validator.h - RIFT validation Interface
 * RIFT: RIFT Is a Flexible Translator
 * Stage: rift-3
 * OBINexus Computing Framework - Technical Implementation
 */

#ifndef RIFT_VALIDATOR_H
#define RIFT_VALIDATOR_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* validation stage configuration */
#define RIFT_STAGE_VALIDATION 1
#define RIFT_VALIDATOR_VERSION 0x040000

/* AEGIS methodology compliance structures */
typedef struct rift_validator_context {
    uint32_t version;
    bool initialized;
    uint32_t thread_count;
    bool dual_mode_enabled;
    bool aegis_compliant;
    void *stage_data;
    void *next_stage_input;
} rift_validator_context_t;

typedef struct rift_validator_config {
    uint32_t processing_flags;
    uint32_t validation_level;
    bool trust_tagging_enabled;
    bool preserve_matched_state;
    char *output_format;
} rift_validator_config_t;

/* Stage execution result */
typedef enum {
    RIFT_VALIDATOR_SUCCESS = 0,
    RIFT_VALIDATOR_ERROR_INVALID_INPUT = -1,
    RIFT_VALIDATOR_ERROR_PROCESSING = -2,
    RIFT_VALIDATOR_ERROR_VALIDATION = -3,
    RIFT_VALIDATOR_ERROR_MEMORY = -4
} rift_validator_result_t;

/* Core API functions */
rift_validator_context_t* rift_validator_init(rift_validator_config_t *config);
rift_validator_result_t rift_validator_process(
    rift_validator_context_t *ctx,
    const void *input,
    size_t input_size,
    void **output,
    size_t *output_size
);
rift_validator_result_t rift_validator_validate(rift_validator_context_t *ctx);
void rift_validator_cleanup(rift_validator_context_t *ctx);

/* Stage-specific functions */

#ifdef __cplusplus
}
#endif

#endif /* RIFT_${component^^}_H */
