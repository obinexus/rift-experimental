// RIFT Advanced Configuration - Pipeline and Threading Support
// OBINexus Governance-First Architecture
#ifndef RIFT_CONFIG_ADVANCED_H
#define RIFT_CONFIG_ADVANCED_H

#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>

// Threading model enumeration (OBINexus Model 1 & 2)
typedef enum {
    RIFT_THREADING_MODEL_1_PARALLEL = 1,      // True parallelism with governance-bound workers
    RIFT_THREADING_MODEL_2_CONCURRENT = 2,    // Shared-core concurrent threading
    RIFT_THREADING_HYBRID = 3                 // Hybrid approach
} RiftThreadingModel;

// Pipeline processing mode
typedef enum {
    RIFT_PIPELINE_SEQUENTIAL = 1,
    RIFT_PIPELINE_PARALLEL = 2,
    RIFT_PIPELINE_HYBRID = 3
} RiftPipelineMode;

// Worker pool configuration
typedef struct {
    uint32_t default_pool_size;
    uint32_t max_pool_size;
    uint32_t min_pool_size;
    uint32_t worker_timeout;
    bool governance_validation_per_worker;
    
    // Specialized pools
    uint32_t tokenizer_pool_size;
    uint32_t parser_pool_size;
    uint32_t ast_pool_size;
    uint32_t validator_pool_size;
} RiftWorkerPoolConfig;

// Threading configuration
typedef struct {
    RiftThreadingModel model;
    bool model1_enabled;
    bool model1_dedicated_cores;
    bool model1_governance_isolation;
    bool model1_cryptographic_identity;
    
    bool model2_enabled;
    bool model2_time_sliced;
    bool model2_governance_reconciliation;
    bool model2_parent_child_hierarchy;
} RiftThreadingConfig;

// Pipeline configuration
typedef struct {
    bool enabled;
    RiftPipelineMode mode;
    uint32_t max_parallel_stages;
    bool sequence_validation;
    bool artifact_tracking;
    
    // Stage-specific settings
    bool stage0_parallel;
    bool stage1_parallel;
    bool stage2_sequential;
    bool final_sequential;
} RiftPipelineConfig;

// Governance configuration
typedef struct {
    bool enabled;
    bool strict_mode;
    bool cryptographic_validation;
    bool audit_trail;
    
    // Thread governance
    bool thread_governance_isolation;
    bool worker_cryptographic_identity;
    bool cross_thread_validation;
    bool governance_context_inheritance;
    
    // Governance triangle thresholds
    double attack_risk_threshold;
    double rollback_cost_threshold;
    double stability_impact_threshold;
    double max_governance_vector;
} RiftGovernanceConfig;

// Advanced RIFT configuration structure
typedef struct {
    // Basic configuration
    char* output_dir;
    char* stage_name;
    bool debug_mode;
    bool verbose;
    char* log_level;
    
    // Advanced pipeline configuration
    RiftPipelineConfig pipeline;
    RiftThreadingConfig threading;
    RiftWorkerPoolConfig worker_pools;
    RiftGovernanceConfig governance;
    
    // Performance settings
    bool optimization_enabled;
    bool memory_pool_enabled;
    bool cache_enabled;
    bool work_stealing_enabled;
    
} RiftAdvancedConfig;

// Function declarations
RiftAdvancedConfig* rift_advanced_config_create(void);
void rift_advanced_config_destroy(RiftAdvancedConfig* config);
int rift_advanced_config_load(RiftAdvancedConfig* config, const char* config_file);
int rift_advanced_config_validate(const RiftAdvancedConfig* config);

#endif // RIFT_CONFIG_ADVANCED_H
