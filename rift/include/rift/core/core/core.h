/*
 * core.h - RIFT core Interface
 * RIFT: RIFT Is a Flexible Translator
 * OBINexus Computing Framework
 */

#ifndef RIFT_CORE_CORE_H
#define RIFT_CORE_CORE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* core context structure */
typedef struct rift_core_context {
    uint32_t version;
    bool initialized;
    uint32_t flags;
    void *private_data;
} rift_core_context_t;

/* core API functions */
rift_core_context_t* rift_core_init(uint32_t flags);
int rift_core_process(rift_core_context_t *ctx, 
                              void *input, size_t input_size,
                              void **output, size_t *output_size);
void rift_core_cleanup(rift_core_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_CORE_H */
