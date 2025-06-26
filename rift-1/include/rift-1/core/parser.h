/*
 * parser.h - RIFT parsing Interface
 * RIFT: RIFT Is a Flexible Translator
 * Stage: rift-1
 * OBINexus Computing Framework - Technical Implementation
 */

#ifndef RIFT_PARSER_H
#define RIFT_PARSER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* parsing stage configuration */
#define RIFT_STAGE_PARSING 1
#define RIFT_PARSER_VERSION 0x040000

/* AEGIS methodology compliance structures */
typedef struct rift_parser_context {
    uint32_t version;
    bool initialized;
    uint32_t thread_count;
    bool dual_mode_enabled;
    bool aegis_compliant;
    void *stage_data;
    void *next_stage_input;
} rift_parser_context_t;

typedef struct rift_parser_config {
    uint32_t processing_flags;
    uint32_t validation_level;
    bool trust_tagging_enabled;
    bool preserve_matched_state;
    char *output_format;
} rift_parser_config_t;

/* Stage execution result */
typedef enum {
    RIFT_PARSER_SUCCESS = 0,
    RIFT_PARSER_ERROR_INVALID_INPUT = -1,
    RIFT_PARSER_ERROR_PROCESSING = -2,
    RIFT_PARSER_ERROR_VALIDATION = -3,
    RIFT_PARSER_ERROR_MEMORY = -4
} rift_parser_result_t;

/* Core API functions */
rift_parser_context_t* rift_parser_init(rift_parser_config_t *config);
rift_parser_result_t rift_parser_process(
    rift_parser_context_t *ctx,
    const void *input,
    size_t input_size,
    void **output,
    size_t *output_size
);
rift_parser_result_t rift_parser_validate(rift_parser_context_t *ctx);
void rift_parser_cleanup(rift_parser_context_t *ctx);

/* Stage-specific functions */
/* Dual-mode parsing functions */
rift_parser_result_t rift_parser_set_dual_mode(rift_parser_context_t *ctx, bool bottom_up, bool top_down);
rift_parser_result_t rift_parser_execute_bottom_up(rift_parser_context_t *ctx);
rift_parser_result_t rift_parser_execute_top_down(rift_parser_context_t *ctx);
rift_parser_result_t rift_parser_validate_consistency(rift_parser_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_${component^^}_H */
