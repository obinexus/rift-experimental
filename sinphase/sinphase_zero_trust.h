/*
 * RIFT Sinphase Integration with Zero Trust Enforcement
 * Comprehensive build orchestration with cryptographic validation
 */

#ifndef RIFT_SINPHASE_ZERO_TRUST_H
#define RIFT_SINPHASE_ZERO_TRUST_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>  /* Added for size_t definition */

#ifdef __cplusplus
extern "C" {
#endif

// Sinphase stage enumeration
typedef enum {
    SINPHASE_STAGE_0_TOKENIZATION = 0,
    SINPHASE_STAGE_1_PARSING = 1,
    SINPHASE_STAGE_2_SEMANTIC = 2,
    SINPHASE_STAGE_3_VALIDATION = 3,
    SINPHASE_STAGE_4_BYTECODE = 4,
    SINPHASE_STAGE_5_VERIFICATION = 5,
    SINPHASE_STAGE_6_EMISSION = 6,
    SINPHASE_STAGE_COUNT
} sinphase_stage_t;

// Zero trust enforcement levels
typedef enum {
    ZERO_TRUST_DISABLED = 0,
    ZERO_TRUST_BASIC = 1,
    ZERO_TRUST_COMPREHENSIVE = 2,
    ZERO_TRUST_PARANOID = 3
} zero_trust_level_t;

// Sinphase execution context
typedef struct {
    sinphase_stage_t current_stage;
    zero_trust_level_t trust_level;
    uint64_t execution_signature;
    bool stage_validated[SINPHASE_STAGE_COUNT];
    void* stage_implementations[SINPHASE_STAGE_COUNT];
    char* audit_trail;
    size_t audit_trail_size;  /* Now properly defined with stddef.h inclusion */
} sinphase_context_t;

// Sinphase operations
sinphase_context_t* sinphase_create_context(zero_trust_level_t trust_level);
void sinphase_destroy_context(sinphase_context_t* context);

bool sinphase_register_stage(
    sinphase_context_t* context,
    sinphase_stage_t stage,
    void* implementation,
    uint64_t crypto_signature
);

bool sinphase_execute_stage(
    sinphase_context_t* context,
    sinphase_stage_t stage,
    void* input_data,
    void** output_data
);

bool sinphase_validate_zero_trust_chain(sinphase_context_t* context);
void sinphase_generate_audit_trail(sinphase_context_t* context, const char* event);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_SINPHASE_ZERO_TRUST_H */
