/*
 * verifier.h - RIFT verifier Interface
 * RIFT: RIFT Is a Flexible Translator
 * OBINexus Computing Framework
 */

#ifndef RIFT_CORE_VERIFIER_H
#define RIFT_CORE_VERIFIER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* verifier context structure */
typedef struct rift_verifier_context {
    uint32_t version;
    bool initialized;
    uint32_t flags;
    void *private_data;
} rift_verifier_context_t;

/* verifier API functions */
rift_verifier_context_t* rift_verifier_init(uint32_t flags);
int rift_verifier_process(rift_verifier_context_t *ctx, 
                              void *input, size_t input_size,
                              void **output, size_t *output_size);
void rift_verifier_cleanup(rift_verifier_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_VERIFIER_H */
