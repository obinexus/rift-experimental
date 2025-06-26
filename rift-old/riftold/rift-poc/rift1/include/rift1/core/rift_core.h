// include/rift1/core/rift_core.h
// OBINexus AEGIS Framework - Core Type Definitions
#ifndef RIFT1_CORE_H
#define RIFT1_CORE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ================================
// AEGIS Core Result Types
// ================================
typedef enum {
    RIFT_SUCCESS = 0,
    RIFT_ERROR_NULL_POINTER,
    RIFT_ERROR_MEMORY_ALLOCATION,
    RIFT_ERROR_PATTERN_INVALID,
    RIFT_ERROR_COMPILATION_FAILED,
    RIFT_ERROR_IO,
    RIFT_ERROR_GOVERNANCE_VIOLATION,
    RIFT_ERROR_TYPE_MISMATCH,
    RIFT_ERROR_PARSER_FAILURE
} RiftResult;

// ================================
// AEGIS Governance Configuration
// ================================
typedef struct {
    bool enabled;                        // Primary governance toggle
    bool thread_governance_isolation;
    double attack_risk_threshold;
    double rollback_cost_threshold;
    double stability_impact_threshold;
    double max_governance_vector;
    size_t max_validation_depth;
    size_t governance_buffer_size;
    bool enable_quantum_governance;
    bool strict_type_enforcement;
} RiftGovernanceConfig;

// ================================
// AEGIS Pipeline Configuration
// ================================
typedef struct {
    bool enable_stage0_processing;
    bool enable_stage1_processing;
    bool enable_validation;
    bool enable_enhancement;
    
    // Governance integration
    RiftGovernanceConfig governance;
    
    // Tokenizer subsystem
    struct {
        bool enable_unicode_support;
        bool enable_quantum_tokenization;
        size_t lexer_buffer_size;
        size_t pattern_cache_size;
    } tokenizer;
    
    // Parser subsystem
    struct {
        bool enable_bottom_up_parsing;
        bool enable_top_down_parsing;
        bool enable_hybrid_parsing;
        size_t parser_stack_size;
        size_t ast_node_pool_size;
    } parser;
    
    // Memory management subsystem
    struct {
        size_t initial_token_capacity;
        size_t growth_factor;
        bool enable_memory_validation;
        bool enable_gc;
    } memory;
} RiftPipelineConfig;

// ================================
// AEGIS Advanced Configuration
// ================================
typedef struct {
    RiftPipelineConfig pipeline;
    
    // Parser mode configuration
    struct {
        bool bottom_up_enabled;
        bool top_down_enabled;
        char *regex_pattern;
        char mode_flag;
    } parser_mode;
    
    // Runtime configuration
    struct {
        bool verbose_output;
        bool debug_mode;
        char *log_file_path;
        size_t max_log_size;
    } runtime;
} RiftAdvancedConfig;

// ================================
// AEGIS Pipeline Context
// ================================
typedef struct {
    const RiftAdvancedConfig *config;   // ← Key: This provides the structure access
    void *tokenizer_state;
    void *parser_state;
    void *ast_root;
    RiftResult last_result;
    char *error_message;
} RiftPipelineContext;

// ================================
// AEGIS Function Declarations
// ================================

// Core initialization and cleanup
RiftResult rift_init_pipeline(RiftPipelineContext *ctx, const RiftAdvancedConfig *config);
RiftResult rift_cleanup_pipeline(RiftPipelineContext *ctx);

// Configuration utilities
RiftResult rift_validate_config(const RiftAdvancedConfig *config);
const char* rift_result_string(RiftResult result);

// Utility functions
void rift_print_version(void);
void rift_print_usage(const char* program_name);

// ================================
// AEGIS Governance Integration
// ================================

// Function that MUST receive proper type
RiftResult rift_demo_pipeline_config(const RiftAdvancedConfig* config);

// Context-based accessor (recommended pattern)
static inline const RiftAdvancedConfig* rift_get_config(const RiftPipelineContext* ctx) {
    return ctx ? ctx->config : NULL;
}

// Governance validation (fixes your compilation error)
static inline bool rift_governance_enabled(const RiftAdvancedConfig* config) {
    return config && config->pipeline.governance.enabled;
}

#ifdef __cplusplus
}
#endif

#endif // RIFT1_CORE_H