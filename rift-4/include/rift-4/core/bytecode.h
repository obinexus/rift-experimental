/*
 * bytecode.h - RIFT bytecode Interface
 * RIFT: RIFT Is a Flexible Translator
 * Stage: rift-4
 * OBINexus Computing Framework - Technical Implementation
 */

#ifndef RIFT_BYTECODE_H
#define RIFT_BYTECODE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* bytecode stage configuration */
#define RIFT_STAGE_BYTECODE 1
#define RIFT_BYTECODE_VERSION 0x040000

/* AEGIS methodology compliance structures */
typedef struct rift_bytecode_context {
    uint32_t version;
    bool initialized;
    uint32_t thread_count;
    bool dual_mode_enabled;
    bool aegis_compliant;
    void *stage_data;
    void *next_stage_input;
} rift_bytecode_context_t;

typedef struct rift_bytecode_config {
    uint32_t processing_flags;
    uint32_t validation_level;
    bool trust_tagging_enabled;
    bool preserve_matched_state;
    char *output_format;
} rift_bytecode_config_t;

/* Stage execution result */
typedef enum {
    RIFT_BYTECODE_SUCCESS = 0,
    RIFT_BYTECODE_ERROR_INVALID_INPUT = -1,
    RIFT_BYTECODE_ERROR_PROCESSING = -2,
    RIFT_BYTECODE_ERROR_VALIDATION = -3,
    RIFT_BYTECODE_ERROR_MEMORY = -4
} rift_bytecode_result_t;

/* Core API functions */
rift_bytecode_context_t* rift_bytecode_init(rift_bytecode_config_t *config);
rift_bytecode_result_t rift_bytecode_process(
    rift_bytecode_context_t *ctx,
    const void *input,
    size_t input_size,
    void **output,
    size_t *output_size
);
rift_bytecode_result_t rift_bytecode_validate(rift_bytecode_context_t *ctx);
void rift_bytecode_cleanup(rift_bytecode_context_t *ctx);

/* Stage-specific functions */
/* Bytecode generation with trust tagging */
rift_bytecode_result_t rift_bytecode_set_architecture(rift_bytecode_context_t *ctx, const char *arch);
rift_bytecode_result_t rift_bytecode_generate_with_trust_tags(rift_bytecode_context_t *ctx);
rift_bytecode_result_t rift_bytecode_emit_rbc(rift_bytecode_context_t *ctx, const char *output_path);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_${component^^}_H */
