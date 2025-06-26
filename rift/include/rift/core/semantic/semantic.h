/*
 * semantic.h - RIFT semantic Interface
 * RIFT: RIFT Is a Flexible Translator
 * OBINexus Computing Framework
 */

#ifndef RIFT_CORE_SEMANTIC_H
#define RIFT_CORE_SEMANTIC_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* semantic context structure */
typedef struct rift_semantic_context {
    uint32_t version;
    bool initialized;
    uint32_t flags;
    void *private_data;
} rift_semantic_context_t;

/* semantic API functions */
rift_semantic_context_t* rift_semantic_init(uint32_t flags);
int rift_semantic_process(rift_semantic_context_t *ctx, 
                              void *input, size_t input_size,
                              void **output, size_t *output_size);
void rift_semantic_cleanup(rift_semantic_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_SEMANTIC_H */
