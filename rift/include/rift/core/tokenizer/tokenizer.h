/*
 * tokenizer.h - RIFT tokenizer Interface
 * RIFT: RIFT Is a Flexible Translator
 * OBINexus Computing Framework
 */

#ifndef RIFT_CORE_TOKENIZER_H
#define RIFT_CORE_TOKENIZER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* tokenizer context structure */
typedef struct rift_tokenizer_context {
    uint32_t version;
    bool initialized;
    uint32_t flags;
    void *private_data;
} rift_tokenizer_context_t;

/* tokenizer API functions */
rift_tokenizer_context_t* rift_tokenizer_init(uint32_t flags);
int rift_tokenizer_process(rift_tokenizer_context_t *ctx, 
                              void *input, size_t input_size,
                              void **output, size_t *output_size);
void rift_tokenizer_cleanup(rift_tokenizer_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_TOKENIZER_H */
