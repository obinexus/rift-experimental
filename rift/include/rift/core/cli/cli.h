/*
 * cli.h - RIFT cli Interface
 * RIFT: RIFT Is a Flexible Translator
 * OBINexus Computing Framework
 */

#ifndef RIFT_CORE_CLI_H
#define RIFT_CORE_CLI_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* cli context structure */
typedef struct rift_cli_context {
    uint32_t version;
    bool initialized;
    uint32_t flags;
    void *private_data;
} rift_cli_context_t;

/* cli API functions */
rift_cli_context_t* rift_cli_init(uint32_t flags);
int rift_cli_process(rift_cli_context_t *ctx, 
                              void *input, size_t input_size,
                              void **output, size_t *output_size);
void rift_cli_cleanup(rift_cli_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_CLI_H */
