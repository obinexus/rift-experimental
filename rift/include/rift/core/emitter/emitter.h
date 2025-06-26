/*
 * emitter.h - RIFT emitter Interface
 * RIFT: RIFT Is a Flexible Translator
 * OBINexus Computing Framework
 */

#ifndef RIFT_CORE_EMITTER_H
#define RIFT_CORE_EMITTER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* emitter context structure */
typedef struct rift_emitter_context {
    uint32_t version;
    bool initialized;
    uint32_t flags;
    void *private_data;
} rift_emitter_context_t;

/* emitter API functions */
rift_emitter_context_t* rift_emitter_init(uint32_t flags);
int rift_emitter_process(rift_emitter_context_t *ctx, 
                              void *input, size_t input_size,
                              void **output, size_t *output_size);
void rift_emitter_cleanup(rift_emitter_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_EMITTER_H */
