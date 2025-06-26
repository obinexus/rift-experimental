/*
 * config.c - RIFT Configuration System Implementation
 * RIFT: RIFT Is a Flexible Translator
 * OBINexus Computing Framework - IOC Implementation
 */

#include "rift/config/config.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

rift_config_t* rift_config_load(const char *config_path) {
    rift_config_t *config = calloc(1, sizeof(rift_config_t));
    if (!config) return NULL;
    
    FILE *file = fopen(config_path, "r");
    if (!file) {
        // Use defaults if config file doesn't exist
        config->version = strdup("4.0.0");
        config->strict_mode = true;
        config->debug_mode = false;
        config->default_threads = 32;
        config->dual_mode_parsing = true;
        config->bottom_up_enabled = true;
        config->top_down_enabled = true;
        config->default_architecture = strdup("amd_ryzen");
        config->trust_tagging = true;
        config->aegis_compliance = true;
        
        return config;
    }
    
    // Parse configuration file
    char line[1024];
    char key[256], value[768];
    
    while (fgets(line, sizeof(line), file)) {
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n' || line[0] == '[') continue;
        
        if (sscanf(line, "%255s = %767s", key, value) == 2) {
            // Remove quotes from value
            if (value[0] == '"' && value[strlen(value)-1] == '"') {
                value[strlen(value)-1] = '\0';
                memmove(value, value+1, strlen(value));
            }
            
            // Parse configuration values
            if (strcmp(key, "version") == 0) {
                config->version = strdup(value);
            } else if (strcmp(key, "strict_mode") == 0) {
                config->strict_mode = strcmp(value, "true") == 0;
            } else if (strcmp(key, "debug_mode") == 0) {
                config->debug_mode = strcmp(value, "true") == 0;
            } else if (strcmp(key, "default_threads") == 0) {
                config->default_threads = atoi(value);
            } else if (strcmp(key, "dual_mode") == 0) {
                config->dual_mode_parsing = strcmp(value, "true") == 0;
            } else if (strcmp(key, "bottom_up_enabled") == 0) {
                config->bottom_up_enabled = strcmp(value, "true") == 0;
            } else if (strcmp(key, "top_down_enabled") == 0) {
                config->top_down_enabled = strcmp(value, "true") == 0;
            } else if (strcmp(key, "architecture") == 0) {
                config->default_architecture = strdup(value);
            } else if (strcmp(key, "trust_tagging") == 0) {
                config->trust_tagging = strcmp(value, "enabled") == 0;
            } else if (strcmp(key, "aegis_compliance") == 0) {
                config->aegis_compliance = strcmp(value, "required") == 0;
            }
        }
    }
    
    fclose(file);
    return config;
}

void rift_config_free(rift_config_t *config) {
    if (!config) return;
    
    free(config->version);
    free(config->default_architecture);
    
    for (size_t i = 0; i < config->validation_hook_count; i++) {
        free(config->validation_hooks[i]);
    }
    free(config->validation_hooks);
    
    free(config);
}

int rift_config_validate(rift_config_t *config) {
    if (!config) return -1;
    
    if (!config->version) return -1;
    if (config->default_threads == 0) return -1;
    if (!config->default_architecture) return -1;
    
    printf("Configuration validation passed\n");
    return 0;
}

const char* rift_config_get_string(rift_config_t *config, const char *key) {
    if (!config || !key) return NULL;
    
    if (strcmp(key, "version") == 0) return config->version;
    if (strcmp(key, "architecture") == 0) return config->default_architecture;
    
    return NULL;
}

bool rift_config_get_bool(rift_config_t *config, const char *key) {
    if (!config || !key) return false;
    
    if (strcmp(key, "strict_mode") == 0) return config->strict_mode;
    if (strcmp(key, "debug_mode") == 0) return config->debug_mode;
    if (strcmp(key, "dual_mode") == 0) return config->dual_mode_parsing;
    if (strcmp(key, "trust_tagging") == 0) return config->trust_tagging;
    
    return false;
}

uint32_t rift_config_get_uint32(rift_config_t *config, const char *key) {
    if (!config || !key) return 0;
    
    if (strcmp(key, "threads") == 0) return config->default_threads;
    
    return 0;
}
