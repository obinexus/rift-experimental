// include/rift1/tokenizer/tokenizer.h - RIFT1 Tokenizer Orchestrator Interface
// OBINexus AEGIS Framework - Stage 0→1 pipeline coordination with dual-mode
// parser support
#ifndef RIFT1_TOKENIZER_H
#define RIFT1_TOKENIZER_H

#include "../core/config/rift_advanced_config_structures.h"
#include "../token_memory/token_memory.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct RiftTokenizerOrchestrator RiftTokenizerOrchestrator;

// Tokenizer processing metrics
typedef struct {
  size_t lexemes_processed;
  size_t tokens_generated;
  size_t validation_failures;
  size_t enhancement_successes;
  double processing_time_ms;
  double memory_usage_mb;
  double validation_rate;
  double enhancement_rate;
} RiftTokenizerMetrics;

// Parser mode detection result
typedef struct {
  bool is_bottom_up; // Shift-reduce parsing detected
  bool is_top_down;  // Recursive descent parsing detected
  bool is_hybrid;    // Both modes enabled
  char *detected_pattern;
  size_t pattern_length;
  char mode_flag;    // 't', 'b', or 'h' for hybrid
  double confidence; // Detection confidence (0.0-1.0)
} RiftParserModeDetection;

// ===== CORE ORCHESTRATOR OPERATIONS =====

/**
 * @brief Creates a new tokenizer orchestrator with AEGIS configuration
 * @param config AEGIS configuration including pipeline, parser, and validation
 * settings
 * @return Initialized orchestrator or NULL on failure
 */
RiftTokenizerOrchestrator *rift_tokenizer_create(const RiftAegisConfig *config);

/**
 * @brief Destroys tokenizer orchestrator and all associated components
 * @param orchestrator Orchestrator to destroy
 */
void rift_tokenizer_destroy(RiftTokenizerOrchestrator *orchestrator);

/**
 * @brief Processes input through complete Stage 0→1 pipeline
 * @param orchestrator Initialized orchestrator
 * @param input Source code input to process
 * @param input_length Length of input in bytes
 * @return true if processing completed successfully, false otherwise
 */
bool rift_tokenizer_process_input(RiftTokenizerOrchestrator *orchestrator,
                                  const char *input, size_t input_length);

/**
 * @brief Processes input from file through Stage 0→1 pipeline
 * @param orchestrator Initialized orchestrator
 * @param filename Path to source file
 * @return true if processing completed successfully, false otherwise
 */
bool rift_tokenizer_process_file(RiftTokenizerOrchestrator *orchestrator,
                                 const char *filename);

// ===== DUAL-MODE PARSER INTEGRATION =====

/**
 * @brief Detects parser mode from input using regex pattern
 * R"/[^A-Z0-9\b]/gmi[t|b]"
 * @param orchestrator Initialized orchestrator
 * @param input Source code to analyze
 * @param input_length Length of input
 * @return Parser mode detection result with confidence scoring
 */
RiftParserModeDetection
rift_tokenizer_detect_parser_mode(const RiftTokenizerOrchestrator *orchestrator,
                                  const char *input, size_t input_length);

/**
 * @brief Configures parser mode for subsequent processing
 * @param orchestrator Orchestrator to configure
 * @param bottom_up Enable shift-reduce parsing
 * @param top_down Enable recursive descent parsing
 * @return true if configuration applied successfully
 */
bool rift_tokenizer_set_parser_mode(RiftTokenizerOrchestrator *orchestrator,
                                    bool bottom_up, bool top_down);

/**
 * @brief Gets current parser mode configuration
 * @param orchestrator Orchestrator to query
 * @return Current parser mode detection result
 */
RiftParserModeDetection
rift_tokenizer_get_parser_mode(const RiftTokenizerOrchestrator *orchestrator);

// ===== TOKEN ACCESS AND RETRIEVAL =====

/**
 * @brief Gets total number of processed tokens
 * @param orchestrator Orchestrator to query
 * @return Number of tokens in Stage 1 storage
 */
size_t
rift_tokenizer_get_token_count(const RiftTokenizerOrchestrator *orchestrator);

/**
 * @brief Retrieves token by index from Stage 1 storage
 * @param orchestrator Orchestrator to query
 * @param index Token index (0-based)
 * @return Token at specified index or NULL if invalid
 */
RiftMemoryToken *
rift_tokenizer_get_token(const RiftTokenizerOrchestrator *orchestrator,
                         size_t index);

/**
 * @brief Retrieves tokens by type filter
 * @param orchestrator Orchestrator to query
 * @param type Token type to filter by
 * @param result_count Output parameter for number of matching tokens
 * @return Array of matching tokens (caller must free)
 */
RiftMemoryToken **
rift_tokenizer_get_tokens_by_type(const RiftTokenizerOrchestrator *orchestrator,
                                  RiftTokenType type, size_t *result_count);

// ===== VALIDATION AND ENHANCEMENT =====

/**
 * @brief Validates all tokens using AEGIS compliance rules
 * @param orchestrator Orchestrator to validate
 * @return true if all tokens pass validation, false otherwise
 */
bool rift_tokenizer_validate_all(const RiftTokenizerOrchestrator *orchestrator);

/**
 * @brief Enhances tokens with semantic information
 * @param orchestrator Orchestrator to enhance
 * @return true if enhancement completed successfully
 */
bool rift_tokenizer_enhance_semantics(RiftTokenizerOrchestrator *orchestrator);

/**
 * @brief Gets validation rate for processed tokens
 * @param orchestrator Orchestrator to query
 * @return Validation success rate (0.0-1.0)
 */
double rift_tokenizer_get_validation_rate(
    const RiftTokenizerOrchestrator *orchestrator);

/**
 * @brief Gets enhancement rate for processed tokens
 * @param orchestrator Orchestrator to query
 * @return Enhancement success rate (0.0-1.0)
 */
double rift_tokenizer_get_enhancement_rate(
    const RiftTokenizerOrchestrator *orchestrator);

// ===== RIFT FILE I/O OPERATIONS =====

/**
 * @brief Saves Stage 0 lexemes to .rift.0 file
 * @param orchestrator Orchestrator containing lexeme data
 * @param filename Output file path
 * @return true if save completed successfully
 */
bool rift_tokenizer_save_rift0(const RiftTokenizerOrchestrator *orchestrator,
                               const char *filename);

/**
 * @brief Saves Stage 1 tokens to .rift.1 file
 * @param orchestrator Orchestrator containing token data
 * @param filename Output file path
 * @return true if save completed successfully
 */
bool rift_tokenizer_save_rift1(const RiftTokenizerOrchestrator *orchestrator,
                               const char *filename);

/**
 * @brief Loads Stage 0 lexemes from .rift.0 file
 * @param orchestrator Orchestrator to load into
 * @param filename Input file path
 * @return true if load completed successfully
 */
bool rift_tokenizer_load_rift0(RiftTokenizerOrchestrator *orchestrator,
                               const char *filename);

/**
 * @brief Loads Stage 1 tokens from .rift.1 file
 * @param orchestrator Orchestrator to load into
 * @param filename Input file path
 * @return true if load completed successfully
 */
bool rift_tokenizer_load_rift1(RiftTokenizerOrchestrator *orchestrator,
                               const char *filename);

// ===== PERFORMANCE MONITORING =====

/**
 * @brief Gets comprehensive processing metrics
 * @param orchestrator Orchestrator to query
 * @return Processing metrics structure
 */
RiftTokenizerMetrics
rift_tokenizer_get_metrics(const RiftTokenizerOrchestrator *orchestrator);

/**
 * @brief Resets performance counters and metrics
 * @param orchestrator Orchestrator to reset
 */
void rift_tokenizer_reset_metrics(RiftTokenizerOrchestrator *orchestrator);

/**
 * @brief Checks if orchestrator is currently processing
 * @param orchestrator Orchestrator to query
 * @return true if processing is active, false otherwise
 */
bool rift_tokenizer_is_processing(
    const RiftTokenizerOrchestrator *orchestrator);

// ===== ERROR HANDLING =====

/**
 * @brief Gets last error message from orchestrator
 * @param orchestrator Orchestrator to query
 * @return Error message string or NULL if no error
 */
const char *
rift_tokenizer_get_last_error(const RiftTokenizerOrchestrator *orchestrator);

/**
 * @brief Clears error state and messages
 * @param orchestrator Orchestrator to clear
 */
void rift_tokenizer_clear_errors(RiftTokenizerOrchestrator *orchestrator);

// ===== QUANTUM COHERENCE OPERATIONS (EXPERIMENTAL) =====

/**
 * @brief Establishes quantum coherence between tokens
 * @param orchestrator Orchestrator to process
 * @return true if coherence established successfully
 */
bool rift_tokenizer_establish_quantum_coherence(
    RiftTokenizerOrchestrator *orchestrator);

/**
 * @brief Collapses quantum superposition states to classical values
 * @param orchestrator Orchestrator to process
 * @return true if collapse completed successfully
 */
bool rift_tokenizer_collapse_quantum_states(
    RiftTokenizerOrchestrator *orchestrator);

#ifdef __cplusplus
}
#endif

#endif // RIFT1_TOKENIZER_H