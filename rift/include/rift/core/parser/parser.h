/*
 * parser.h - RIFT parser Interface
 * RIFT: RIFT Is a Flexible Translator
 * OBINexus Computing Framework
 */

#ifndef RIFT_CORE_PARSER_H
#define RIFT_CORE_PARSER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* parser context structure */
typedef struct rift_parser_context {
    uint32_t version;
    bool initialized;
    uint32_t flags;
    void *private_data;
} rift_parser_context_t;

/* parser API functions */
rift_parser_context_t* rift_parser_init(uint32_t flags);
int rift_parser_process(rift_parser_context_t *ctx, 
                              void *input, size_t input_size,
                              void **output, size_t *output_size);
void rift_parser_cleanup(rift_parser_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_PARSER_H */
