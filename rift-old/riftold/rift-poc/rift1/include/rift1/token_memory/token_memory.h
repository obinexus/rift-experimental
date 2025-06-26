// include/rift1/token_memory/token_memory.h - RIFT1 Token Memory Bridge
// OBINexus AEGIS Framework - Stage 0→1 bridge with zero-trust validation
#ifndef RIFT1_TOKEN_MEMORY_H
#define RIFT1_TOKEN_MEMORY_H

#include "../lexer/lexer.h"
#include "../token_types/token_type.h"
#include "../token_values/token_value.h"
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// AEGIS Token Memory Configuration
typedef struct {
  size_t initial_capacity;
  size_t growth_factor;
  bool thread_safe;
  bool zero_trust_validation;
  bool quantum_coherence_tracking;
} RiftTokenMemoryConfig;

// Token validation results
typedef struct {
  bool type_validation;
  bool value_validation;
  bool sequence_validation;
  bool coherence_validation;
  char *error_message;
  double confidence_score;
} RiftTokenValidationResult;

// Token enhancement metadata
typedef struct {
  bool semantic_analysis_complete;
  bool dependency_analysis_complete;
  bool optimization_applied;
  size_t enhancement_level;
  double enhancement_confidence;
} RiftTokenEnhancement;

// Combined token structure for memory bridge
typedef struct {
  RiftTokenValue *token_value;
  RiftTokenValidationResult validation;
  RiftTokenEnhancement enhancement;

  // Stage tracking
  bool stage0_processed;
  bool stage1_ready;
  size_t processing_timestamp;

  // Quantum coherence tracking (experimental)
  bool is_quantum_coherent;
  size_t entanglement_group;
} RiftMemoryToken;

// Token memory bridge structure
typedef struct {
  // Configuration
  RiftTokenMemoryConfig config;

  // Token storage arrays by stage
  RiftLexeme **stage0_lexemes;
  size_t stage0_count;
  size_t stage0_capacity;

  RiftMemoryToken **stage1_tokens;
  size_t stage1_count;
  size_t stage1_capacity;

  // Processing state
  size_t current_processing_index;
  bool processing_complete;
  bool validation_enabled;

  // Thread safety
  pthread_mutex_t mutex;
  pthread_cond_t processing_complete_cond;

  // Statistics and metrics
  size_t total_processed;
  size_t validation_failures;
  size_t enhancement_successes;
  double average_confidence;

  // Error tracking
  char *last_error;
  size_t error_count;
} RiftTokenMemory;

// Core token memory operations
RiftTokenMemory *rift_token_memory_create(const RiftTokenMemoryConfig *config);
void rift_token_memory_destroy(RiftTokenMemory *memory);

// Stage 0 operations (lexeme input)
bool rift_token_memory_add_lexeme(RiftTokenMemory *memory,
                                  const RiftLexeme *lexeme);
bool rift_token_memory_load_stage0_file(RiftTokenMemory *memory,
                                        const char *rift0_file);

// Stage 0→1 processing pipeline
bool rift_token_memory_process_stage0_to_stage1(
    RiftTokenMemory *memory, RiftValueResolver *value_resolver,
    RiftTokenTypeMatcher *type_matcher);

// Stage 1 operations (processed tokens)
RiftMemoryToken *rift_token_memory_get_token(RiftTokenMemory *memory,
                                             size_t index);
size_t rift_token_memory_get_token_count(const RiftTokenMemory *memory);

// Validation operations
bool rift_token_memory_validate_all(RiftTokenMemory *memory);
bool rift_token_memory_validate_token(RiftMemoryToken *token);
RiftTokenValidationResult
rift_token_memory_get_validation_result(const RiftMemoryToken *token);

// Enhancement operations
bool rift_token_memory_enhance_tokens(RiftTokenMemory *memory);
bool rift_token_memory_enhance_token(RiftMemoryToken *token);

// RIFT file I/O operations
bool rift_token_memory_save_rift0(const RiftTokenMemory *memory,
                                  const char *filename);
bool rift_token_memory_save_rift1(const RiftTokenMemory *memory,
                                  const char *filename);
bool rift_token_memory_load_rift1(RiftTokenMemory *memory,
                                  const char *filename);

// Query and filtering operations
RiftMemoryToken **
rift_token_memory_find_tokens_by_type(const RiftTokenMemory *memory,
                                      RiftTokenType type, size_t *result_count);
RiftMemoryToken **
rift_token_memory_find_tokens_by_value(const RiftTokenMemory *memory,
                                       const char *value_pattern,
                                       size_t *result_count);

// Statistics and metrics
double rift_token_memory_get_validation_rate(const RiftTokenMemory *memory);
double rift_token_memory_get_enhancement_rate(const RiftTokenMemory *memory);
size_t rift_token_memory_get_error_count(const RiftTokenMemory *memory);

// Thread-safe operations
bool rift_token_memory_lock(RiftTokenMemory *memory);
bool rift_token_memory_unlock(RiftTokenMemory *memory);
bool rift_token_memory_wait_for_processing(RiftTokenMemory *memory,
                                           int timeout_ms);

// Quantum coherence operations (experimental)
bool rift_token_memory_establish_quantum_coherence(RiftTokenMemory *memory);
bool rift_token_memory_collapse_quantum_states(RiftTokenMemory *memory);

// Error handling
const char *rift_token_memory_get_last_error(const RiftTokenMemory *memory);
void rift_token_memory_clear_errors(RiftTokenMemory *memory);

#ifdef __cplusplus
}
#endif

#endif // RIFT1_TOKEN_MEMORY_H
