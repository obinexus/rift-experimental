// cli/command/config_handler.h
#ifndef RIFT_CONFIG_HANDLER_H
#define RIFT_CONFIG_HANDLER_H

#include "../../include/rift.h"

// Configuration command structure for .riftrc handling
typedef struct RiftConfigCommand {
  char *config_path;
  RiftConfig *config;
  bool readonly_mode;
  bool validate_only;
} RiftConfigCommand;

// Configuration processing results
typedef enum {
  CONFIG_SUCCESS = 0,
  CONFIG_ERROR_FILE_NOT_FOUND = -1,
  CONFIG_ERROR_INVALID_FORMAT = -2,
  CONFIG_ERROR_READONLY_VIOLATION = -3,
  CONFIG_ERROR_PATTERN_INVALID = -4
} ConfigResult;

// Core configuration command functions
RiftConfigCommand *rift_config_command_create(const char *config_path);
void rift_config_command_destroy(RiftConfigCommand *cmd);
ConfigResult rift_config_command_load(RiftConfigCommand *cmd);
ConfigResult rift_config_command_validate(RiftConfigCommand *cmd);
ConfigResult rift_config_command_apply(RiftConfigCommand *cmd,
                                       RiftEngine *engine);

// .riftrc specific processing
ConfigResult rift_process_riftrc(const char *riftrc_path, RiftConfig *config);
ConfigResult rift_validate_riftrc_readonly(const char *riftrc_path);

// Pattern mode configuration
typedef enum {
  PATTERN_MODE_TOP_DOWN = 't',
  PATTERN_MODE_BOTTOM_UP = 'b',
  PATTERN_MODE_AUTO = 'a'
} PatternMode;

ConfigResult rift_config_set_pattern_mode(RiftConfig *config, PatternMode mode);
PatternMode rift_config_get_pattern_mode(const RiftConfig *config);

#endif // RIFT_CONFIG_HANDLER_H

// cli/command/config_handler.c
#include "config_handler.h"
#include <atomic.h>
#include <pthread.h>

// Thread-safe configuration processing
static pthread_mutex_t config_mutex = PTHREAD_MUTEX_INITIALIZER;
static _Atomic bool config_processing = false;

RiftConfigCommand *rift_config_command_create(const char *config_path) {
  if (!config_path)
    return NULL;

  RiftConfigCommand *cmd = calloc(1, sizeof(RiftConfigCommand));
  if (!cmd)
    return NULL;

  cmd->config_path = strdup(config_path);
  cmd->config = rift_config_create();
  cmd->readonly_mode = true; // Default to readonly for security
  cmd->validate_only = false;

  return cmd;
}

void rift_config_command_destroy(RiftConfigCommand *cmd) {
  if (!cmd)
    return;

  free(cmd->config_path);
  rift_config_destroy(cmd->config);
  free(cmd);
}

ConfigResult rift_config_command_load(RiftConfigCommand *cmd) {
  if (!cmd || !cmd->config_path)
    return CONFIG_ERROR_INVALID_FORMAT;

  // Thread-safe configuration loading
  pthread_mutex_lock(&config_mutex);

  if (atomic_load(&config_processing)) {
    pthread_mutex_unlock(&config_mutex);
    return CONFIG_ERROR_READONLY_VIOLATION;
  }

  atomic_store(&config_processing, true);

  // Validate .riftrc exists and is readable
  FILE *riftrc = fopen(cmd->config_path, "r");
  if (!riftrc) {
    atomic_store(&config_processing, false);
    pthread_mutex_unlock(&config_mutex);
    return CONFIG_ERROR_FILE_NOT_FOUND;
  }
  fclose(riftrc);

  // Load configuration using core RIFT API
  RiftResult result = rift_config_load(cmd->config, cmd->config_path);

  atomic_store(&config_processing, false);
  pthread_mutex_unlock(&config_mutex);

  return (result == RIFT_SUCCESS) ? CONFIG_SUCCESS
                                  : CONFIG_ERROR_INVALID_FORMAT;
}

ConfigResult rift_process_riftrc(const char *riftrc_path, RiftConfig *config) {
  if (!riftrc_path || !config)
    return CONFIG_ERROR_INVALID_FORMAT;

  // Enhanced .riftrc processing with pattern mode support
  FILE *riftrc = fopen(riftrc_path, "r");
  if (!riftrc)
    return CONFIG_ERROR_FILE_NOT_FOUND;

  char line[1024];
  bool pattern_mode_set = false;

  while (fgets(line, sizeof(line), riftrc)) {
    // Skip comments and empty lines
    if (line[0] == '#' || line[0] == '\n')
      continue;

    // Parse configuration directives
    if (strncmp(line, "output_dir=", 11) == 0) {
      char *value = line + 11;
      value[strcspn(value, "\n")] = 0; // Remove newline
      free(config->output_dir);
      config->output_dir = strdup(value);
    } else if (strncmp(line, "pattern_mode=", 13) == 0) {
      char mode = line[13];
      if (mode == 't' || mode == 'b' || mode == 'a') {
        rift_config_set_pattern_mode(config, (PatternMode)mode);
        pattern_mode_set = true;
      }
    } else if (strncmp(line, "debug=", 6) == 0) {
      config->debug_mode = (strncmp(line + 6, "true", 4) == 0);
    } else if (strncmp(line, "verbose=", 8) == 0) {
      config->verbose = (strncmp(line + 8, "true", 4) == 0);
    }
  }

  fclose(riftrc);

  // Default to top-down mode if not specified
  if (!pattern_mode_set) {
    rift_config_set_pattern_mode(config, PATTERN_MODE_TOP_DOWN);
  }

  return CONFIG_SUCCESS;
}