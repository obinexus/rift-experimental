/*
 * validator.h - RIFT validator Interface
 * RIFT: RIFT Is a Flexible Translator
 * OBINexus Computing Framework
 */

#ifndef RIFT_CORE_VALIDATOR_H
#define RIFT_CORE_VALIDATOR_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* validator context structure */
typedef struct rift_validator_context {
    uint32_t version;
    bool initialized;
    uint32_t flags;
    void *private_data;
} rift_validator_context_t;

/* validator API functions */
rift_validator_context_t* rift_validator_init(uint32_t flags);
int rift_validator_process(rift_validator_context_t *ctx, 
                              void *input, size_t input_size,
                              void **output, size_t *output_size);
void rift_validator_cleanup(rift_validator_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_VALIDATOR_H */
