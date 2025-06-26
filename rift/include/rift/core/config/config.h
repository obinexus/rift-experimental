/*
 * config.h - RIFT config Interface
 * RIFT: RIFT Is a Flexible Translator
 * OBINexus Computing Framework
 */

#ifndef RIFT_CORE_CONFIG_H
#define RIFT_CORE_CONFIG_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* config context structure */
typedef struct rift_config_context {
    uint32_t version;
    bool initialized;
    uint32_t flags;
    void *private_data;
} rift_config_context_t;

/* config API functions */
rift_config_context_t* rift_config_init(uint32_t flags);
int rift_config_process(rift_config_context_t *ctx, 
                              void *input, size_t input_size,
                              void **output, size_t *output_size);
void rift_config_cleanup(rift_config_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CORE_CONFIG_H */
