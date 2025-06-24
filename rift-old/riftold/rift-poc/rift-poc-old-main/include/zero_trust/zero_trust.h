/**
 * @file zero_trust.h
 * @brief Zero Trust Cryptographic Governance Integration
 * 
 * Provides cryptographic signing and verification capabilities for
 * the Rift compilation pipeline with Residual Trust enforcement.
 * 
 * @copyright Copyright (c) 2025 OBINexus Computing
 * @license Proprietary - All Rights Reserved
 */

#ifndef ZERO_TRUST_H
#define ZERO_TRUST_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Zero Trust validation result
 */
typedef enum {
    ZERO_TRUST_VALID = 0,
    ZERO_TRUST_INVALID_SIGNATURE = 1,
    ZERO_TRUST_MISSING_SIGNATURE = 2,
    ZERO_TRUST_ARTIFACT_MISSING = 3,
    ZERO_TRUST_CRYPTO_ERROR = 4
} zero_trust_result_t;

/**
 * @brief Zero Trust context
 */
typedef struct {
    char *private_key_path;
    char *public_key_path;
    char *signature_algorithm;
    bool strict_mode;
    bool audit_enabled;
} zero_trust_context_t;

/* Core Zero Trust functions */
zero_trust_context_t *zero_trust_init(const char *private_key, const char *public_key);
zero_trust_result_t zero_trust_sign_artifact(zero_trust_context_t *ctx, const char *artifact_path);
zero_trust_result_t zero_trust_verify_artifact(zero_trust_context_t *ctx, const char *artifact_path);
bool zero_trust_enforce_residual_trust(zero_trust_context_t *ctx, const char *artifact_path);
void zero_trust_free(zero_trust_context_t *ctx);

/* Utility functions */
const char *zero_trust_result_string(zero_trust_result_t result);
bool zero_trust_keys_exist(const char *private_key, const char *public_key);
bool zero_trust_generate_keys(const char *private_key, const char *public_key);

#endif /* ZERO_TRUST_H */
