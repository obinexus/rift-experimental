#include "rift/governance/sinphase.h"

float rift_sinphase_compute_cost(const rift_component_metrics_t *metrics,
                                 const float weights[5])
{
    if (!metrics || !weights) {
        return 0.0f;
    }

    float cost = 0.0f;
    cost += (float)metrics->include_depth   * weights[0];
    cost += (float)metrics->function_calls  * weights[1];
    cost += (float)metrics->external_deps   * weights[2];
    cost += (float)metrics->complexity      * weights[3];
    cost += (float)metrics->link_deps       * weights[4];

    /* Circular dependency penalty */
    cost += (float)metrics->circular_deps * 0.2f;

    /* Temporal pressure directly increases cost */
    cost += metrics->temporal_pressure;

    return cost;
}
