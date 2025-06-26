/*
 * token_type.h - RIFT token_type Interface
 * RIFT: RIFT Is a Flexible Translator
 * OBINexus Computing Framework
 */

#ifndef RIFT_CORE_TOKEN_TYPE_H
#define RIFT_CORE_TOKEN_TYPE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* token_type context structure */
typedef struct rift_token_type_context {
    uint32_t version;
    bool initialized;
    uint32_t flags;
    void *private_data;
} rift_token_type_context_t;

/* token_type API functions */
rift_token_type_context_t* rift_token_type_init(uint32_t flags);
int rift_token_type_process(rift_token_type_context_t *ctx, 
                              void *input, size_t input_size,
                              void **output, size_t *output_size);
void rift_token_type_cleanup(rift_token_type_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_TOKEN_TYPE_H */
