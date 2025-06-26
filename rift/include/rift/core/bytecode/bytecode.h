/*
 * bytecode.h - RIFT bytecode Interface
 * RIFT: RIFT Is a Flexible Translator
 * OBINexus Computing Framework
 */

#ifndef RIFT_CORE_BYTECODE_H
#define RIFT_CORE_BYTECODE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* bytecode context structure */
typedef struct rift_bytecode_context {
    uint32_t version;
    bool initialized;
    uint32_t flags;
    void *private_data;
} rift_bytecode_context_t;

/* bytecode API functions */
rift_bytecode_context_t* rift_bytecode_init(uint32_t flags);
int rift_bytecode_process(rift_bytecode_context_t *ctx, 
                              void *input, size_t input_size,
                              void **output, size_t *output_size);
void rift_bytecode_cleanup(rift_bytecode_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_BYTECODE_H */
