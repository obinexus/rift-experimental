/*
 * lexer.h - RIFT lexer Interface
 * RIFT: RIFT Is a Flexible Translator
 * OBINexus Computing Framework
 */

#ifndef RIFT_CORE_LEXER_H
#define RIFT_CORE_LEXER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* lexer context structure */
typedef struct rift_lexer_context {
    uint32_t version;
    bool initialized;
    uint32_t flags;
    void *private_data;
} rift_lexer_context_t;

/* lexer API functions */
rift_lexer_context_t* rift_lexer_init(uint32_t flags);
int rift_lexer_process(rift_lexer_context_t *ctx, 
                              void *input, size_t input_size,
                              void **output, size_t *output_size);
void rift_lexer_cleanup(rift_lexer_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_LEXER_H */
