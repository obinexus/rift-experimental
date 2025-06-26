/*
 * config.h - RIFT Configuration System
 * RIFT: RIFT Is a Flexible Translator
 * OBINexus Computing Framework - IOC Implementation
 */

#ifndef RIFT_CONFIG_H
#define RIFT_CONFIG_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Configuration container */
typedef struct rift_config {
    char *version;
    bool strict_mode;
    bool debug_mode;
    uint32_t default_threads;
    bool dual_mode_parsing;
    bool bottom_up_enabled;
    bool top_down_enabled;
    char *default_architecture;
    bool trust_tagging;
    bool aegis_compliance;
    char **validation_hooks;
    size_t validation_hook_count;
} rift_config_t;

/* Configuration functions */
rift_config_t* rift_config_load(const char *config_path);
void rift_config_free(rift_config_t *config);
int rift_config_validate(rift_config_t *config);
const char* rift_config_get_string(rift_config_t *config, const char *key);
bool rift_config_get_bool(rift_config_t *config, const char *key);
uint32_t rift_config_get_uint32(rift_config_t *config, const char *key);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CONFIG_H */
