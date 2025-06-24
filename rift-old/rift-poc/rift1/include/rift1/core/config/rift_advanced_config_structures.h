// include/rift1/core/config/rift_advanced_config_structures.h
// OBINexus AEGIS Framework - Complete configuration structure definitions
#ifndef RIFT1_ADVANCED_CONFIG_STRUCTURES_H
#define RIFT1_ADVANCED_CONFIG_STRUCTURES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Governance configuration structure
typedef struct {
  bool thread_governance_isolation;
  double attack_risk_threshold;
  double rollback_cost_threshold;
  double stability_impact_threshold;
  double max_governance_vector;

  // Additional governance parameters
  size_t max_validation_depth;
  size_t governance_buffer_size;
  bool enable_quantum_governance;
  bool strict_type_enforcement;
} RiftGovernanceConfig;

// Enhanced pipeline configuration with governance integration
typedef struct {
  // Core pipeline configuration
  bool enable_stage0_processing;
  bool enable_stage1_processing;
  bool enable_validation;
  bool enable_enhancement;

  // Governance configuration
  RiftGovernanceConfig governance;

  // Tokenizer configuration
  struct {
    bool enable_unicode_support;
    bool enable_quantum_tokenization;
    size_t lexer_buffer_size;
    size_t pattern_cache_size;
  } tokenizer;

  // Parser configuration
  struct {
    bool enable_bottom_up_parsing;
    bool enable_top_down_parsing;
    bool enable_hybrid_parsing;
    size_t parser_stack_size;
    size_t ast_node_pool_size;
  } parser;

  // Memory management
  struct {
    size_t initial_token_capacity;
    size_t growth_factor;
    bool enable_memory_validation;
    bool enable_gc;
  } memory;

} RiftPipelineConfig;

// Parser mode selection structure
typedef struct {
  bool bottom_up_enabled; // Shift-reduce parsing
  bool top_down_enabled;  // Recursive descent parsing
  char *regex_pattern;    // Pattern: R"/[^A-Z0-9\b]/gmi[t|b]"
  char mode_flag;         // 't' for top-down, 'b' for bottom-up
} RiftParserModeConfig;

// Complete AEGIS configuration structure
typedef struct {
  RiftPipelineConfig pipeline;
  RiftParserModeConfig parser_mode;

  // AEGIS-specific configuration
  struct {
    bool enable_pattern_composition;
    bool enable_functional_transforms;
    bool enable_data_orientation;
    bool enable_unified_pipeline;
  } aegis;

  // Validation configuration
  struct {
    bool enable_compile_time_validation;
    bool enable_runtime_validation;
    bool enable_type_safety_checks;
    bool enable_bounds_checking;
  } validation;

} RiftAegisConfig;

// Function declarations for configuration management
RiftAegisConfig *rift_config_create_default(void);
bool rift_config_validate(const RiftAegisConfig *config);
void rift_config_destroy(RiftAegisConfig *config);

// Governance-specific functions
bool rift_governance_initialize(RiftGovernanceConfig *gov_config);
bool rift_governance_validate_threshold(const RiftGovernanceConfig *config,
                                        double value,
                                        const char *threshold_name);

#ifdef __cplusplus
}
#endif

#endif // RIFT1_ADVANCED_CONFIG_STRUCTURES_H