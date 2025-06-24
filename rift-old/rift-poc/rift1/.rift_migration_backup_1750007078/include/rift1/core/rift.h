#ifndef RIFT1_H
#define RIFT1_H

/**
 * RIFT1 - Consolidated Header
 * OBINexus AEGIS Framework - Stage 1 Parser Architecture
 * 
 * Single-source header to eliminate redefinition conflicts
 * Includes all RIFT1 core functionality
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

// Include all component headers
#include "rift_errors.h"
#include "rift_ast.h"

// ===== FORWARD DECLARATIONS =====
typedef struct RiftPattern RiftPattern;
typedef struct RiftState RiftState;
typedef struct RiftTransition RiftTransition;
typedef struct RiftAutomaton RiftAutomaton;
typedef struct RiftToken RiftToken;
typedef struct RiftEngine RiftEngine;
typedef struct RiftConfig RiftConfig;
typedef struct RiftIR RiftIR;

// ===== RIFT1 ENHANCEMENTS =====
typedef struct RiftTokenMemory RiftTokenMemory;
typedef struct Rift1ParseEngine Rift1ParseEngine;

// ===== CORE AEGIS STRUCTURES =====
struct RiftPattern {
    char* pattern_string;
    char* regex_pattern;
    char* flags;
    char* mode;
    bool compiled;
    void* compiled_regex;
};

struct RiftState {
    char* name;
    char* pattern;
    bool is_final;
    bool is_accepting;
    int state_id;
    double priority;
};

struct RiftTransition {
    RiftState* from_state;
    RiftState* to_state;
    RiftPattern* pattern;
    char* label;
    bool is_epsilon;
};

struct RiftAutomaton {
    RiftState** states;
    size_t state_count;
    RiftTransition** transitions;
    size_t transition_count;
    RiftState* initial_state;
    RiftState** final_states;
    size_t final_state_count;
};

struct RiftToken {
    char* type;
    char* value;
    size_t position;
    size_t length;
    bool matched_state;    // AEGIS: Token architecture
};

struct RiftTokenMemory {
    RiftToken** tokens;
    size_t token_count;
    size_t token_capacity;
    pthread_mutex_t mutex;  // Thread safety
};

struct RiftEngine {
    RiftAutomaton* automaton;
    RiftTokenMemory* token_memory;
    RiftConfig* config;
};

// ===== CORE FUNCTION DECLARATIONS =====

// Engine management
RiftEngine* rift_engine_create(void);
RiftResult rift_engine_destroy(RiftEngine* engine);
RiftResult rift_engine_compile_pattern(RiftEngine* engine, const char* pattern);

// Token memory operations
RiftTokenMemory* rift_token_memory_create(void);
RiftResult rift_token_memory_destroy(RiftTokenMemory* memory);
RiftResult rift_token_memory_add(RiftTokenMemory* memory, RiftToken* token);

// Parsing operations
RiftResult rift_parse_input(RiftEngine* engine, const char* input);
RiftResult rift_get_tokens(RiftEngine* engine, RiftToken*** tokens, size_t* count);

#ifdef __cplusplus
}
#endif

#endif // RIFT1_H
