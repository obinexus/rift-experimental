/*
 * governance.h - RIFT governance Interface
 * RIFT: RIFT Is a Flexible Translator
 * OBINexus Computing Framework
 */

#ifndef RIFT_CORE_GOVERNANCE_H
#define RIFT_CORE_GOVERNANCE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* governance context structure */
typedef struct rift_governance_context {
    uint32_t version;
    bool initialized;
    uint32_t flags;
    void *private_data;
} rift_governance_context_t;

/* governance API functions */
rift_governance_context_t* rift_governance_init(uint32_t flags);
int rift_governance_process(rift_governance_context_t *ctx, 
                              void *input, size_t input_size,
                              void **output, size_t *output_size);
void rift_governance_cleanup(rift_governance_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_GOVERNANCE_H */
