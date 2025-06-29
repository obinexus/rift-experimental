/*
 * RIFT Hash Table Build System - O(1) Feature Lookup
 * Zero Trust Enforcement with Cryptographic Validation
 */

#ifndef RIFT_HASH_TABLE_BUILDER_H
#define RIFT_HASH_TABLE_BUILDER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HASH_TABLE_SIZE 1024
#define FEATURE_NAME_MAX 256

// Feature entry for O(1) lookup
typedef struct feature_entry {
    char feature_name[FEATURE_NAME_MAX];
    void* implementation_ptr;
    uint32_t feature_hash;
    bool zero_trust_validated;
    uint64_t crypto_signature;
    struct feature_entry* next;  // For collision resolution
} feature_entry_t;

// Hash table for build system features
typedef struct {
    feature_entry_t* buckets[HASH_TABLE_SIZE];
    uint32_t entry_count;
    uint32_t collision_count;
    bool zero_trust_enforced;
} rift_feature_hash_table_t;

// Hash table operations
rift_feature_hash_table_t* rift_hash_table_create(void);
void rift_hash_table_destroy(rift_feature_hash_table_t* table);

// O(1) feature operations
bool rift_hash_table_insert_feature(
    rift_feature_hash_table_t* table,
    const char* feature_name,
    void* implementation,
    uint64_t crypto_signature
);

void* rift_hash_table_lookup_feature(
    rift_feature_hash_table_t* table,
    const char* feature_name
);

bool rift_hash_table_validate_zero_trust(
    rift_feature_hash_table_t* table,
    const char* feature_name
);

// Build system integration
uint32_t rift_hash_djb2(const char* str);
bool rift_hash_verify_crypto_signature(const char* feature, uint64_t signature);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_HASH_TABLE_BUILDER_H */
