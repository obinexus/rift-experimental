// src/core/token_memory/token_memory.c - RIFT1 Enhanced Token Memory
// Implementation OBINexus AEGIS Framework - Zero-trust Stage 0→1 bridge with
// quantum coherence
#include "../../../include/rift1/token_memory/token_memory.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Default token memory configuration
static const RiftTokenMemoryConfig DEFAULT_CONFIG = {
    .initial_capacity = 1024,
    .growth_factor = 2,
    .thread_safe = true,
    .zero_trust_validation = true,
    .quantum_coherence_tracking = false};

RiftTokenMemory *rift_token_memory_create(const RiftTokenMemoryConfig *config) {
  RiftTokenMemory *memory = calloc(1, sizeof(RiftTokenMemory));
  if (!memory)
    return NULL;

  // Use provided config or default
  memory->config = config ? *config : DEFAULT_CONFIG;

  // Initialize Stage 0 storage
  memory->stage0_capacity = memory->config.initial_capacity;
  memory->stage0_lexemes =
      calloc(memory->stage0_capacity, sizeof(RiftLexeme *));
  if (!memory->stage0_lexemes) {
    free(memory);
    return NULL;
  }

  // Initialize Stage 1 storage
  memory->stage1_capacity = memory->config.initial_capacity;
  memory->stage1_tokens =
      calloc(memory->stage1_capacity, sizeof(RiftMemoryToken *));
  if (!memory->stage1_tokens) {
    free(memory->stage0_lexemes);
    free(memory);
    return NULL;
  }

  // Initialize thread safety if enabled
  if (memory->config.thread_safe) {
    if (pthread_mutex_init(&memory->mutex, NULL) != 0) {
      free(memory->stage1_tokens);
      free(memory->stage0_lexemes);
      free(memory);
      return NULL;
    }

    if (pthread_cond_init(&memory->processing_complete_cond, NULL) != 0) {
      pthread_mutex_destroy(&memory->mutex);
      free(memory->stage1_tokens);
      free(memory->stage0_lexemes);
      free(memory);
      return NULL;
    }
  }

  memory->validation_enabled = memory->config.zero_trust_validation;

  return memory;
}

void rift_token_memory_destroy(RiftTokenMemory *memory) {
  if (!memory)
    return;

  // Cleanup Stage 0 lexemes
  if (memory->stage0_lexemes) {
    for (size_t i = 0; i < memory->stage0_count; i++) {
      if (memory->stage0_lexemes[i]) {
        rift_lexeme_destroy(memory->stage0_lexemes[i]);
        free(memory->stage0_lexemes[i]);
      }
    }
    free(memory->stage0_lexemes);
  }

  // Cleanup Stage 1 tokens
  if (memory->stage1_tokens) {
    for (size_t i = 0; i < memory->stage1_count; i++) {
      if (memory->stage1_tokens[i]) {
        rift_token_value_destroy(memory->stage1_tokens[i]->token_value);
        if (memory->stage1_tokens[i]->validation.error_message) {
          free(memory->stage1_tokens[i]->validation.error_message);
        }
        free(memory->stage1_tokens[i]);
      }
    }
    free(memory->stage1_tokens);
  }

  // Cleanup thread safety objects
  if (memory->config.thread_safe) {
    pthread_cond_destroy(&memory->processing_complete_cond);
    pthread_mutex_destroy(&memory->mutex);
  }

  // Cleanup error message
  if (memory->last_error) {
    free(memory->last_error);
  }

  free(memory);
}

bool rift_token_memory_add_lexeme(RiftTokenMemory *memory,
                                  const RiftLexeme *lexeme) {
  if (!memory || !lexeme)
    return false;

  if (memory->config.thread_safe) {
    pthread_mutex_lock(&memory->mutex);
  }

  // Check capacity and grow if needed
  if (memory->stage0_count >= memory->stage0_capacity) {
    size_t new_capacity =
        memory->stage0_capacity * memory->config.growth_factor;
    RiftLexeme **new_lexemes =
        realloc(memory->stage0_lexemes, new_capacity * sizeof(RiftLexeme *));
    if (!new_lexemes) {
      if (memory->config.thread_safe) {
        pthread_mutex_unlock(&memory->mutex);
      }
      return false;
    }

    memory->stage0_lexemes = new_lexemes;
    memory->stage0_capacity = new_capacity;

    // Initialize new slots
    for (size_t i = memory->stage0_count; i < new_capacity; i++) {
      memory->stage0_lexemes[i] = NULL;
    }
  }

  // Copy lexeme for storage
  memory->stage0_lexemes[memory->stage0_count] = rift_lexeme_copy(lexeme);
  if (!memory->stage0_lexemes[memory->stage0_count]) {
    if (memory->config.thread_safe) {
      pthread_mutex_unlock(&memory->mutex);
    }
    return false;
  }

  memory->stage0_count++;

  if (memory->config.thread_safe) {
    pthread_mutex_unlock(&memory->mutex);
  }

  return true;
}

bool rift_token_memory_process_stage0_to_stage1(
    RiftTokenMemory *memory, RiftValueResolver *value_resolver,
    RiftTokenTypeMatcher *type_matcher) {
  if (!memory || !value_resolver || !type_matcher)
    return false;

  if (memory->config.thread_safe) {
    pthread_mutex_lock(&memory->mutex);
  }

  // Process each Stage 0 lexeme into Stage 1 token
  for (size_t i = 0; i < memory->stage0_count; i++) {
    RiftLexeme *lexeme = memory->stage0_lexemes[i];
    if (!lexeme)
      continue;

    // Determine token type
    RiftTokenType type = rift_token_type_match(type_matcher, lexeme);
    if (type == RIFT_TOKEN_UNKNOWN) {
      memory->error_count++;
      continue;
    }

    // Resolve token value
    RiftTokenValue *token_value =
        rift_value_resolve(value_resolver, lexeme, type);
    if (!token_value) {
      memory->error_count++;
      continue;
    }

    // Create memory token
    RiftMemoryToken *memory_token = calloc(1, sizeof(RiftMemoryToken));
    if (!memory_token) {
      rift_token_value_destroy(token_value);
      memory->error_count++;
      continue;
    }

    memory_token->token_value = token_value;
    memory_token->stage0_processed = true;
    memory_token->processing_timestamp = time(NULL);

    // Validate token if enabled
    if (memory->validation_enabled) {
      bool validation_success = rift_token_memory_validate_token(memory_token);
      if (!validation_success) {
        memory->validation_failures++;
      }
    } else {
      // Default validation state for non-validated tokens
      memory_token->validation.type_validation = true;
      memory_token->validation.value_validation = true;
      memory_token->validation.sequence_validation = true;
      memory_token->validation.confidence_score = 1.0;
    }

    // Add to Stage 1 storage
    if (!rift_token_memory_add_stage1_token(memory, memory_token)) {
      rift_token_value_destroy(token_value);
      free(memory_token);
      memory->error_count++;
      continue;
    }

    memory->total_processed++;
  }

  memory->processing_complete = true;

  if (memory->config.thread_safe) {
    pthread_cond_broadcast(&memory->processing_complete_cond);
    pthread_mutex_unlock(&memory->mutex);
  }

  return memory->error_count == 0;
}

// Helper function to add Stage 1 token
static bool rift_token_memory_add_stage1_token(RiftTokenMemory *memory,
                                               RiftMemoryToken *token) {
  if (!memory || !token)
    return false;

  // Check capacity and grow if needed
  if (memory->stage1_count >= memory->stage1_capacity) {
    size_t new_capacity =
        memory->stage1_capacity * memory->config.growth_factor;
    RiftMemoryToken **new_tokens = realloc(
        memory->stage1_tokens, new_capacity * sizeof(RiftMemoryToken *));
    if (!new_tokens)
      return false;

    memory->stage1_tokens = new_tokens;
    memory->stage1_capacity = new_capacity;

    // Initialize new slots
    for (size_t i = memory->stage1_count; i < new_capacity; i++) {
      memory->stage1_tokens[i] = NULL;
    }
  }

  memory->stage1_tokens[memory->stage1_count] = token;
  memory->stage1_count++;
  token->stage1_ready = true;

  return true;
}

bool rift_token_memory_validate_token(RiftMemoryToken *token) {
  if (!token || !token->token_value)
    return false;

  // Validate token value using AEGIS type safety
  bool type_valid = rift_value_check_type_safety(token->token_value);
  bool value_valid = rift_value_validate(token->token_value);

  // Update validation results
  token->validation.type_validation = type_valid;
  token->validation.value_validation = value_valid;
  token->validation.sequence_validation =
      true; // TODO: Implement sequence validation
  token->validation.coherence_validation =
      true; // TODO: Implement coherence validation

  // Calculate confidence score
  int valid_checks = 0;
  valid_checks += type_valid ? 1 : 0;
  valid_checks += value_valid ? 1 : 0;
  valid_checks += token->validation.sequence_validation ? 1 : 0;
  valid_checks += token->validation.coherence_validation ? 1 : 0;

  token->validation.confidence_score = (double)valid_checks / 4.0;

  return type_valid && value_valid;
}

RiftMemoryToken *rift_token_memory_get_token(RiftTokenMemory *memory,
                                             size_t index) {
  if (!memory || index >= memory->stage1_count)
    return NULL;

  if (memory->config.thread_safe) {
    pthread_mutex_lock(&memory->mutex);
  }

  RiftMemoryToken *token = memory->stage1_tokens[index];

  if (memory->config.thread_safe) {
    pthread_mutex_unlock(&memory->mutex);
  }

  return token;
}

size_t rift_token_memory_get_token_count(const RiftTokenMemory *memory) {
  return memory ? memory->stage1_count : 0;
}

double rift_token_memory_get_validation_rate(const RiftTokenMemory *memory) {
  if (!memory || memory->total_processed == 0)
    return 0.0;

  size_t successful_validations =
      memory->total_processed - memory->validation_failures;
  return (double)successful_validations / (double)memory->total_processed;
}

const char *rift_token_memory_get_last_error(const RiftTokenMemory *memory) {
  return memory ? memory->last_error : "Invalid memory object";
}
