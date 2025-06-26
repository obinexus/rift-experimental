#ifndef RIFT_POLIC_H
#define RIFT_POLIC_H

/**
 * RIFT PoLiC Security Framework
 * Zero Trust Architecture Implementation
 * Technical Team + Nnamdi Okpala Collaborative Development
 */

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    RIFT_SUCCESS = 0,
    RIFT_ERROR_INVALID_INPUT = -1,
    RIFT_ERROR_SECURITY_VIOLATION = -2,
    RIFT_ERROR_STATE_MACHINE_FAILURE = -3
} rift_result_t;

typedef struct {
    char* policy_id;
    int security_level;
    bool zero_trust_enabled;
    bool state_minimization_active;
} rift_polic_config_t;

// Core PoLiC functions
rift_result_t rift_polic_init(rift_polic_config_t* config);
rift_result_t rift_polic_validate(const char* input);
rift_result_t rift_polic_cleanup(void);

// State machine integration points
rift_result_t rift_polic_validate_state_transition(int from_state, int to_state);

#endif // RIFT_POLIC_H
