// rift/src/governance/config.c
#include <rift/governance/governance.h>

rift_result_t rift_governance_init(rift_governance_t *governance,
                                   const char *config_file) {
    if (!governance || !config_file) {
        return RIFT_ERROR_INVALID_ARGUMENT;
    }
    
    // Initialize governance state
    memset(governance, 0, sizeof(rift_governance_t));
    governance->config_file_path = strdup(config_file);
    
    // Load policy from .riftrc
    rift_result_t result = rift_governance_load_policy(governance, config_file);
    if (result != RIFT_SUCCESS) {
        return result;
    }
    
    // Set initialized flag
    governance->initialized = true;
    return RIFT_SUCCESS;
}

rift_result_t rift_governance_validate_memory_alignment(rift_governance_t *governance,
                                                        uint32_t required_alignment) {
    if (!governance || !governance->initialized) {
        return RIFT_ERROR_NOT_INITIALIZED;
    }
    
    // Validate against policy requirements
    if (governance->policy.memory_alignment_bits != required_alignment) {
        snprintf(governance->last_error_message, sizeof(governance->last_error_message),
                "Memory alignment mismatch: required %u, configured %u",
                required_alignment, governance->policy.memory_alignment_bits);
        return RIFT_ERROR_GOVERNANCE_VIOLATION;
    }
    
    governance->memory_alignment_validated = true;
    return RIFT_SUCCESS;
}