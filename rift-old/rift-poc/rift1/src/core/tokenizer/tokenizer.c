// src/core/tokenizer/tokenizer.c - RIFT1 Tokenizer Orchestrator Implementation
// OBINexus AEGIS Framework - Complete Stage 0→1 pipeline coordination
#include "../../../include/rift1/tokenizer/tokenizer.h"
#include "../../../include/rift1/lexer/lexer.h"
#include "../../../include/rift1/token_memory/token_memory.h"
#include "../../../include/rift1/token_types/token_type.h"
#include "../../../include/rift1/token_values/token_value.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// AEGIS Tokenizer Orchestrator Structure
typedef struct {
  // Component dependencies
  RiftLexer *lexer;
  RiftTokenTypeMatcher *type_matcher;
  RiftValueResolver *value_resolver;
  RiftTokenMemory *token_memory;

  // Configuration
  RiftAegisConfig *config;

  // Pipeline state
  bool initialized;
  bool processing_active;
  size_t tokens_processed;

  // Dual-mode parser configuration
  RiftParserModeConfig parser_mode;
  regex_t mode_regex;
  bool regex_compiled;

  // Performance metrics
  double processing_time_ms;
  size_t memory_usage_bytes;
  double validation_rate;

} RiftTokenizerOrchestrator;

// Parser mode detection structure
typedef struct {
  bool is_bottom_up; // Shift-reduce parsing
  bool is_top_down;  // Recursive descent parsing
  char *matched_pattern;
  size_t pattern_length;
  char mode_flag; // 't' or 'b'
} RiftParserModeResult;

// Function prototypes
static bool compile_mode_regex(RiftTokenizerOrchestrator *orchestrator);
static RiftParserModeResult
detect_parser_mode(const RiftTokenizerOrchestrator *orchestrator,
                   const char *input, size_t input_length);
static bool
validate_token_sequence(const RiftTokenizerOrchestrator *orchestrator);
static bool enhance_token_semantics(RiftTokenizerOrchestrator *orchestrator);

// ===== TOKENIZER ORCHESTRATOR IMPLEMENTATION =====

RiftTokenizerOrchestrator *
rift_tokenizer_create(const RiftAegisConfig *config) {
  if (!config)
    return NULL;

  RiftTokenizerOrchestrator *orchestrator =
      calloc(1, sizeof(RiftTokenizerOrchestrator));
  if (!orchestrator)
    return NULL;

  // Store configuration
  orchestrator->config = malloc(sizeof(RiftAegisConfig));
  if (!orchestrator->config) {
    free(orchestrator);
    return NULL;
  }
  memcpy(orchestrator->config, config, sizeof(RiftAegisConfig));

  // Initialize lexer component
  RiftLexerConfig lexer_config = {
      .unicode_support = config->pipeline.tokenizer.enable_unicode_support,
      .quantum_mode = config->pipeline.tokenizer.enable_quantum_tokenization,
      .buffer_size = config->pipeline.tokenizer.lexer_buffer_size,
      .max_lexeme_length = 512};
  orchestrator->lexer = rift_lexer_create(&lexer_config);
  if (!orchestrator->lexer) {
    free(orchestrator->config);
    free(orchestrator);
    return NULL;
  }

  // Initialize token type matcher
  orchestrator->type_matcher = rift_token_type_matcher_create();
  if (!orchestrator->type_matcher) {
    rift_lexer_destroy(orchestrator->lexer);
    free(orchestrator->config);
    free(orchestrator);
    return NULL;
  }

  // Initialize value resolver
  RiftValueResolverConfig value_config = {
      .strict_type_checking = config->validation.enable_type_safety_checks,
      .quantum_mode_enabled =
          config->pipeline.tokenizer.enable_quantum_tokenization,
      .auto_type_promotion = false,
      .max_string_length = 4096,
      .numeric_precision = 16};
  orchestrator->value_resolver = rift_value_resolver_create(&value_config);
  if (!orchestrator->value_resolver) {
    rift_token_type_matcher_destroy(orchestrator->type_matcher);
    rift_lexer_destroy(orchestrator->lexer);
    free(orchestrator->config);
    free(orchestrator);
    return NULL;
  }

  // Initialize token memory bridge
  RiftTokenMemoryConfig memory_config = {
      .initial_capacity = config->pipeline.memory.initial_token_capacity,
      .growth_factor = config->pipeline.memory.growth_factor,
      .thread_safe = true,
      .zero_trust_validation = config->validation.enable_runtime_validation,
      .quantum_coherence_tracking =
          config->pipeline.tokenizer.enable_quantum_tokenization};
  orchestrator->token_memory = rift_token_memory_create(&memory_config);
  if (!orchestrator->token_memory) {
    rift_value_resolver_destroy(orchestrator->value_resolver);
    rift_token_type_matcher_destroy(orchestrator->type_matcher);
    rift_lexer_destroy(orchestrator->lexer);
    free(orchestrator->config);
    free(orchestrator);
    return NULL;
  }

  // Initialize parser mode configuration
  orchestrator->parser_mode = config->parser_mode;

  // Compile regex for parser mode detection
  if (!compile_mode_regex(orchestrator)) {
    rift_token_memory_destroy(orchestrator->token_memory);
    rift_value_resolver_destroy(orchestrator->value_resolver);
    rift_token_type_matcher_destroy(orchestrator->type_matcher);
    rift_lexer_destroy(orchestrator->lexer);
    free(orchestrator->config);
    free(orchestrator);
    return NULL;
  }

  orchestrator->initialized = true;
  return orchestrator;
}

void rift_tokenizer_destroy(RiftTokenizerOrchestrator *orchestrator) {
  if (!orchestrator)
    return;

  // Cleanup regex
  if (orchestrator->regex_compiled) {
    regfree(&orchestrator->mode_regex);
  }

  // Cleanup components
  if (orchestrator->token_memory) {
    rift_token_memory_destroy(orchestrator->token_memory);
  }
  if (orchestrator->value_resolver) {
    rift_value_resolver_destroy(orchestrator->value_resolver);
  }
  if (orchestrator->type_matcher) {
    rift_token_type_matcher_destroy(orchestrator->type_matcher);
  }
  if (orchestrator->lexer) {
    rift_lexer_destroy(orchestrator->lexer);
  }

  // Cleanup configuration
  if (orchestrator->config) {
    free(orchestrator->config);
  }

  free(orchestrator);
}

// ===== STAGE 0→1 PROCESSING PIPELINE =====

bool rift_tokenizer_process_input(RiftTokenizerOrchestrator *orchestrator,
                                  const char *input, size_t input_length) {
  if (!orchestrator || !input || input_length == 0)
    return false;
  if (!orchestrator->initialized)
    return false;

  orchestrator->processing_active = true;
  orchestrator->tokens_processed = 0;

  // Step 1: Detect parser mode from input
  RiftParserModeResult mode_result =
      detect_parser_mode(orchestrator, input, input_length);

  // Step 2: Configure lexer with input
  if (!rift_lexer_set_input(orchestrator->lexer, input, input_length)) {
    orchestrator->processing_active = false;
    return false;
  }

  // Step 3: Lexical analysis - Extract lexemes (Stage 0)
  RiftLexeme *lexeme;
  while ((lexeme = rift_lexer_next_lexeme(orchestrator->lexer)) != NULL) {
    // Add lexeme to token memory Stage 0 storage
    if (!rift_token_memory_add_lexeme(orchestrator->token_memory, lexeme)) {
      rift_lexeme_destroy(lexeme);
      free(lexeme);
      continue;
    }

    rift_lexeme_destroy(lexeme);
    free(lexeme);
  }

  // Step 4: Stage 0→1 processing pipeline
  bool processing_success = rift_token_memory_process_stage0_to_stage1(
      orchestrator->token_memory, orchestrator->value_resolver,
      orchestrator->type_matcher);

  if (!processing_success) {
    orchestrator->processing_active = false;
    return false;
  }

  // Step 5: Validation phase
  if (orchestrator->config->validation.enable_runtime_validation) {
    if (!validate_token_sequence(orchestrator)) {
      orchestrator->processing_active = false;
      return false;
    }
  }

  // Step 6: Enhancement phase
  if (orchestrator->config->pipeline.enable_enhancement) {
    if (!enhance_token_semantics(orchestrator)) {
      orchestrator->processing_active = false;
      return false;
    }
  }

  orchestrator->tokens_processed =
      rift_token_memory_get_token_count(orchestrator->token_memory);
  orchestrator->processing_active = false;

  return true;
}

// ===== DUAL-MODE PARSER INTEGRATION =====

static bool compile_mode_regex(RiftTokenizerOrchestrator *orchestrator) {
  if (!orchestrator)
    return false;

  // Compile the regex pattern: R"/[^A-Z0-9\b]/gmi[t|b]"
  // Simplified version for mode detection
  const char *pattern = "[^A-Z0-9[:space:]]+[tb]$";

  int regex_result =
      regcomp(&orchestrator->mode_regex, pattern, REG_EXTENDED | REG_ICASE);
  if (regex_result != 0) {
    return false;
  }

  orchestrator->regex_compiled = true;
  return true;
}

static RiftParserModeResult
detect_parser_mode(const RiftTokenizerOrchestrator *orchestrator,
                   const char *input, size_t input_length) {
  RiftParserModeResult result = {0};

  if (!orchestrator || !input || !orchestrator->regex_compiled) {
    return result;
  }

  // Search for parser mode indicators in input
  regmatch_t match;
  int regex_result = regexec(&orchestrator->mode_regex, input, 1, &match, 0);

  if (regex_result == 0) {
    // Extract the mode flag from the match
    size_t match_start = match.rm_so;
    size_t match_end = match.rm_eo;

    if (match_end > 0 && match_end <= input_length) {
      char mode_flag =
          input[match_end - 1]; // Last character should be 't' or 'b'

      result.mode_flag = mode_flag;
      result.pattern_length = match_end - match_start;
      result.matched_pattern = malloc(result.pattern_length + 1);

      if (result.matched_pattern) {
        memcpy(result.matched_pattern, input + match_start,
               result.pattern_length);
        result.matched_pattern[result.pattern_length] = '\0';
      }

      // Set parsing mode based on flag
      switch (mode_flag) {
      case 't':
      case 'T':
        result.is_top_down = true;
        result.is_bottom_up = false;
        break;
      case 'b':
      case 'B':
        result.is_bottom_up = true;
        result.is_top_down = false;
        break;
      default:
        // Default to configuration settings
        result.is_top_down = orchestrator->parser_mode.top_down_enabled;
        result.is_bottom_up = orchestrator->parser_mode.bottom_up_enabled;
        break;
      }
    }
  } else {
    // No mode indicator found, use default configuration
    result.is_top_down = orchestrator->parser_mode.top_down_enabled;
    result.is_bottom_up = orchestrator->parser_mode.bottom_up_enabled;
  }

  return result;
}

// ===== VALIDATION AND ENHANCEMENT =====

static bool
validate_token_sequence(const RiftTokenizerOrchestrator *orchestrator) {
  if (!orchestrator || !orchestrator->token_memory)
    return false;

  // Implement AEGIS validation rules
  size_t token_count =
      rift_token_memory_get_token_count(orchestrator->token_memory);

  for (size_t i = 0; i < token_count; i++) {
    RiftMemoryToken *token =
        rift_token_memory_get_token(orchestrator->token_memory, i);
    if (!token)
      continue;

    // Validate individual token
    if (!rift_token_memory_validate_token(token)) {
      return false;
    }

    // Check type safety compliance
    if (orchestrator->config->validation.enable_type_safety_checks) {
      if (!rift_value_check_type_safety(token->token_value)) {
        return false;
      }
    }

    // Check bounds if enabled
    if (orchestrator->config->validation.enable_bounds_checking) {
      if (!rift_value_check_bounds(token->token_value)) {
        return false;
      }
    }
  }

  return true;
}

static bool enhance_token_semantics(RiftTokenizerOrchestrator *orchestrator) {
  if (!orchestrator || !orchestrator->token_memory)
    return false;

  // Implement semantic enhancement for AEGIS compliance
  return rift_token_memory_enhance_tokens(orchestrator->token_memory);
}

// ===== PUBLIC API FUNCTIONS =====

size_t
rift_tokenizer_get_token_count(const RiftTokenizerOrchestrator *orchestrator) {
  return orchestrator
             ? rift_token_memory_get_token_count(orchestrator->token_memory)
             : 0;
}

RiftMemoryToken *
rift_tokenizer_get_token(const RiftTokenizerOrchestrator *orchestrator,
                         size_t index) {
  return orchestrator
             ? rift_token_memory_get_token(orchestrator->token_memory, index)
             : NULL;
}

double rift_tokenizer_get_validation_rate(
    const RiftTokenizerOrchestrator *orchestrator) {
  return orchestrator
             ? rift_token_memory_get_validation_rate(orchestrator->token_memory)
             : 0.0;
}

bool rift_tokenizer_save_rift0(const RiftTokenizerOrchestrator *orchestrator,
                               const char *filename) {
  if (!orchestrator || !filename)
    return false;
  return rift_token_memory_save_rift0(orchestrator->token_memory, filename);
}

bool rift_tokenizer_save_rift1(const RiftTokenizerOrchestrator *orchestrator,
                               const char *filename) {
  if (!orchestrator || !filename)
    return false;
  return rift_token_memory_save_rift1(orchestrator->token_memory, filename);
}

bool rift_tokenizer_is_processing(
    const RiftTokenizerOrchestrator *orchestrator) {
  return orchestrator ? orchestrator->processing_active : false;
}