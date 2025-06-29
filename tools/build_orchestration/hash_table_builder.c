/*
 * RIFT Hash Table Build System Implementation
 */

#include "hash_table_builder.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

rift_feature_hash_table_t* rift_hash_table_create(void) {
    rift_feature_hash_table_t* table = calloc(1, sizeof(rift_feature_hash_table_t));
    if (!table) return NULL;
    
    table->zero_trust_enforced = true;
    return table;
}

void rift_hash_table_destroy(rift_feature_hash_table_t* table) {
    if (!table) return;
    
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        feature_entry_t* entry = table->buckets[i];
        while (entry) {
            feature_entry_t* next = entry->next;
            free(entry);
            entry = next;
        }
    }
    free(table);
}

uint32_t rift_hash_djb2(const char* str) {
    uint32_t hash = 5381;
    int c;
    
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    
    return hash % HASH_TABLE_SIZE;
}

bool rift_hash_verify_crypto_signature(const char* feature, uint64_t signature) {
    // Simplified crypto verification - in production use proper cryptography
    uint64_t expected = 0;
    for (const char* p = feature; *p; p++) {
        expected = (expected << 1) ^ (*p);
    }
    return (signature ^ expected) == 0xDEADBEEFCAFEBABE;
}

bool rift_hash_table_insert_feature(
    rift_feature_hash_table_t* table,
    const char* feature_name,
    void* implementation,
    uint64_t crypto_signature
) {
    if (!table || !feature_name || !implementation) return false;
    
    // Zero trust validation
    if (table->zero_trust_enforced) {
        if (!rift_hash_verify_crypto_signature(feature_name, crypto_signature)) {
            printf("Zero Trust Violation: Invalid crypto signature for %s\n", feature_name);
            return false;
        }
    }
    
    uint32_t hash = rift_hash_djb2(feature_name);
    
    // Check for existing entry
    feature_entry_t* existing = table->buckets[hash];
    while (existing) {
        if (strcmp(existing->feature_name, feature_name) == 0) {
            // Update existing entry
            existing->implementation_ptr = implementation;
            existing->crypto_signature = crypto_signature;
            existing->zero_trust_validated = true;
            return true;
        }
        existing = existing->next;
    }
    
    // Create new entry
    feature_entry_t* entry = malloc(sizeof(feature_entry_t));
    if (!entry) return false;
    
    strncpy(entry->feature_name, feature_name, FEATURE_NAME_MAX - 1);
    entry->feature_name[FEATURE_NAME_MAX - 1] = '\0';
    entry->implementation_ptr = implementation;
    entry->feature_hash = hash;
    entry->zero_trust_validated = true;
    entry->crypto_signature = crypto_signature;
    
    // Insert at head of bucket (collision resolution)
    entry->next = table->buckets[hash];
    if (table->buckets[hash]) {
        table->collision_count++;
    }
    table->buckets[hash] = entry;
    table->entry_count++;
    
    return true;
}

void* rift_hash_table_lookup_feature(
    rift_feature_hash_table_t* table,
    const char* feature_name
) {
    if (!table || !feature_name) return NULL;
    
    uint32_t hash = rift_hash_djb2(feature_name);
    feature_entry_t* entry = table->buckets[hash];
    
    while (entry) {
        if (strcmp(entry->feature_name, feature_name) == 0) {
            if (table->zero_trust_enforced && !entry->zero_trust_validated) {
                printf("Zero Trust Violation: Feature %s not validated\n", feature_name);
                return NULL;
            }
            return entry->implementation_ptr;
        }
        entry = entry->next;
    }
    
    return NULL;
}

bool rift_hash_table_validate_zero_trust(
    rift_feature_hash_table_t* table,
    const char* feature_name
) {
    if (!table || !feature_name) return false;
    
    uint32_t hash = rift_hash_djb2(feature_name);
    feature_entry_t* entry = table->buckets[hash];
    
    while (entry) {
        if (strcmp(entry->feature_name, feature_name) == 0) {
            return entry->zero_trust_validated;
        }
        entry = entry->next;
    }
    
    return false;
}
