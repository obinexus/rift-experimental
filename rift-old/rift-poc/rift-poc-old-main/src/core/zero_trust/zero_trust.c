/**
 * @file zero_trust.c
 * @brief Zero Trust implementation for Rift pipeline governance
 */

#include "zero_trust/zero_trust.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

zero_trust_context_t *zero_trust_init(const char *private_key, const char *public_key) {
    if (!private_key || !public_key) return NULL;
    
    zero_trust_context_t *ctx = malloc(sizeof(zero_trust_context_t));
    if (!ctx) return NULL;
    
    ctx->private_key_path = strdup(private_key);
    ctx->public_key_path = strdup(public_key);
    ctx->signature_algorithm = strdup("sha256");
    ctx->strict_mode = true;
    ctx->audit_enabled = true;
    
    if (!ctx->private_key_path || !ctx->public_key_path || !ctx->signature_algorithm) {
        zero_trust_free(ctx);
        return NULL;
    }
    
    return ctx;
}

zero_trust_result_t zero_trust_sign_artifact(zero_trust_context_t *ctx, const char *artifact_path) {
    if (!ctx || !artifact_path) return ZERO_TRUST_CRYPTO_ERROR;
    
    // Check if artifact exists
    if (access(artifact_path, F_OK) != 0) {
        return ZERO_TRUST_ARTIFACT_MISSING;
    }
    
    // Construct signature path
    char signature_path[1024];
    snprintf(signature_path, sizeof(signature_path), "%s.sig", artifact_path);
    
    // Build OpenSSL command
    char command[2048];
    snprintf(command, sizeof(command), 
             "openssl dgst -%s -sign \"%s\" -out \"%s\" \"%s\" 2>/dev/null",
             ctx->signature_algorithm, ctx->private_key_path, 
             signature_path, artifact_path);
    
    // Execute signing
    int result = system(command);
    if (result != 0) {
        return ZERO_TRUST_CRYPTO_ERROR;
    }
    
    return ZERO_TRUST_VALID;
}

zero_trust_result_t zero_trust_verify_artifact(zero_trust_context_t *ctx, const char *artifact_path) {
    if (!ctx || !artifact_path) return ZERO_TRUST_CRYPTO_ERROR;
    
    // Check if artifact exists
    if (access(artifact_path, F_OK) != 0) {
        return ZERO_TRUST_ARTIFACT_MISSING;
    }
    
    // Check if signature exists
    char signature_path[1024];
    snprintf(signature_path, sizeof(signature_path), "%s.sig", artifact_path);
    if (access(signature_path, F_OK) != 0) {
        return ZERO_TRUST_MISSING_SIGNATURE;
    }
    
    // Build OpenSSL verification command
    char command[2048];
    snprintf(command, sizeof(command),
             "openssl dgst -%s -verify \"%s\" -signature \"%s\" \"%s\" 2>/dev/null",
             ctx->signature_algorithm, ctx->public_key_path,
             signature_path, artifact_path);
    
    // Execute verification
    int result = system(command);
    if (result != 0) {
        return ZERO_TRUST_INVALID_SIGNATURE;
    }
    
    return ZERO_TRUST_VALID;
}

bool zero_trust_enforce_residual_trust(zero_trust_context_t *ctx, const char *artifact_path) {
    if (!ctx || !artifact_path) return false;
    
    zero_trust_result_t result = zero_trust_verify_artifact(ctx, artifact_path);
    
    if (ctx->strict_mode && result != ZERO_TRUST_VALID) {
        return false;
    }
    
    return result == ZERO_TRUST_VALID;
}

void zero_trust_free(zero_trust_context_t *ctx) {
    if (!ctx) return;
    
    free(ctx->private_key_path);
    free(ctx->public_key_path);
    free(ctx->signature_algorithm);
    free(ctx);
}

const char *zero_trust_result_string(zero_trust_result_t result) {
    switch (result) {
        case ZERO_TRUST_VALID: return "VALID";
        case ZERO_TRUST_INVALID_SIGNATURE: return "INVALID_SIGNATURE";
        case ZERO_TRUST_MISSING_SIGNATURE: return "MISSING_SIGNATURE";
        case ZERO_TRUST_ARTIFACT_MISSING: return "ARTIFACT_MISSING";
        case ZERO_TRUST_CRYPTO_ERROR: return "CRYPTO_ERROR";
        default: return "UNKNOWN";
    }
}

bool zero_trust_keys_exist(const char *private_key, const char *public_key) {
    return (access(private_key, F_OK) == 0) && (access(public_key, F_OK) == 0);
}

bool zero_trust_generate_keys(const char *private_key, const char *public_key) {
    char command[1024];
    
    // Generate private key
    snprintf(command, sizeof(command), 
             "openssl genrsa -out \"%s\" 4096 2>/dev/null", private_key);
    if (system(command) != 0) return false;
    
    // Set private key permissions
    chmod(private_key, 0600);
    
    // Extract public key
    snprintf(command, sizeof(command),
             "openssl rsa -in \"%s\" -pubout -out \"%s\" 2>/dev/null",
             private_key, public_key);
    if (system(command) != 0) return false;
    
    // Set public key permissions
    chmod(public_key, 0644);
    
    return true;
}
