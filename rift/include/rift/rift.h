/*
 * rift.h - Main RIFT Compiler Interface
 * RIFT: RIFT Is a Flexible Translator
 * OBINexus Computing Framework
 */

#ifndef RIFT_H
#define RIFT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* RIFT version information */
#define RIFT_VERSION_MAJOR 4
#define RIFT_VERSION_MINOR 0
#define RIFT_VERSION_PATCH 0
#define RIFT_VERSION_STRING "4.0.0"

/* Pipeline stage identifiers */
typedef enum {
    RIFT_STAGE_TOKENIZATION = 0,
    RIFT_STAGE_PARSING = 1,
    RIFT_STAGE_SEMANTIC = 2,
    RIFT_STAGE_VALIDATION = 3,
    RIFT_STAGE_BYTECODE = 4,
    RIFT_STAGE_VERIFICATION = 5,
    RIFT_STAGE_EMISSION = 6
} rift_stage_t;

/* Main pipeline context */
typedef struct rift_context {
    uint32_t version;
    bool strict_mode;
    bool debug_enabled;
    uint32_t thread_count;
    char *config_path;
    void *stage_data[7];
} rift_context_t;

/* Core API functions */
rift_context_t* rift_init(const char *config_path);
int rift_compile(rift_context_t *ctx, const char *input_file, const char *output_file);
void rift_cleanup(rift_context_t *ctx);

/* Pipeline stage functions */
int rift_execute_stage(rift_context_t *ctx, rift_stage_t stage, void *input, void **output);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_H */
