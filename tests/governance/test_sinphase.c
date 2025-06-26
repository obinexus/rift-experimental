#include "rift/governance/sinphase.h"
#include <stdio.h>
#include <assert.h>

int main(void)
{
    rift_component_metrics_t metrics = {1, 2, 0, 3, 1, 1, 0.1f};
    float weights[5] = {0.1f, 0.1f, 0.1f, 0.1f, 0.1f};

    float cost = rift_sinphase_compute_cost(&metrics, weights);
    /* Expected cost = sum(metrics * 0.1) + 0.2*circular + temporal*/
    float expected = (1+2+0+3+1)*0.1f + 1*0.2f + 0.1f;
    if (cost < expected - 1e-6 || cost > expected + 1e-6) {
        printf("Cost mismatch: expected %f, got %f\n", expected, cost);
        return 1;
    }

    if (!rift_sinphase_exceeds_threshold(cost, 0.5f)) {
        printf("Threshold evaluation failed\n");
        return 1;
    }

    printf("All Sinphase tests passed\n");
    return 0;
}
