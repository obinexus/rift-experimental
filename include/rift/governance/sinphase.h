#ifndef RIFT_GOVERNANCE_SINPHASE_H
#define RIFT_GOVERNANCE_SINPHASE_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Phase state enumeration for Sinphase governance */
typedef enum {
    RIFT_PHASE_RESEARCH,
    RIFT_PHASE_IMPLEMENTATION,
    RIFT_PHASE_VALIDATION,
    RIFT_PHASE_ISOLATION
} rift_phase_state_t;

/* Component metrics used in cost calculation */
typedef struct {
    size_t include_depth;
    size_t function_calls;
    size_t external_deps;
    size_t complexity;
    size_t link_deps;
    size_t circular_deps;
    float temporal_pressure;
} rift_component_metrics_t;

/* Cost evaluation result */
float rift_sinphase_compute_cost(const rift_component_metrics_t *metrics,
                                 const float weights[5]);

static inline bool rift_sinphase_exceeds_threshold(float cost, float threshold)
{
    return cost > threshold;
}

#ifdef __cplusplus
}
#endif

#endif /* RIFT_GOVERNANCE_SINPHASE_H */
