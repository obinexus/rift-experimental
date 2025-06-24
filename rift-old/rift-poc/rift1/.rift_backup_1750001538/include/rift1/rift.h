// include/rift1/rift.h - Consolidated RIFT1 Header  
// OBINexus AEGIS Framework - Stage 1 Parser Architecture
// CRITICAL: Single-source header to eliminate redefinition conflicts

#ifndef RIFT1_H
#define RIFT1_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ===== FORWARD DECLARATIONS =====
typedef enum RiftResult_enum RiftResult;
typedef struct RiftPattern RiftPattern;
typedef struct RiftState RiftState;
typedef struct RiftAutomaton RiftAutomaton;
typedef struct RiftToken RiftToken;
typedef struct RiftEngine RiftEngine;
typedef struct RiftConfig RiftConfig;
typedef struct RiftIR RiftIR;

// ===== RIFT1 ENHANCEMENTS =====
typedef struct RiftASTNode RiftASTNode;
typedef struct RiftTokenMemory RiftTokenMemory;
typedef struct Rift1ParseEngine Rift1ParseEngine;

// ===== ERROR HANDLING (SINGLE DEFINITION) =====
enum RiftResult_enum {
    RIFT_SUCCESS = 0,
    RIFT_ERROR_NULL_POINTER = -1,
    RIFT_ERROR_MEMORY_ALLOCATION = -2,
    RIFT_ERROR_PATTERN_INVALID = -3,
    RIFT_ERROR_COMPILATION_FAILED = -4,
    RIFT_ERROR_IO = -5,
    RIFT_ERROR_PARSE_FAILED = -6,
    RIFT_ERROR_AST_INVALID = -7,
    RIFT_ERROR_TOKEN_VALIDATION = -8
};

// Core structure definitions (consolidated, no duplicates)
// [Full structure definitions would be here]

// ===== API DECLARATIONS =====
// Token management
RiftToken* rift_token_create(const char* type, const char* value, size_t line, size_t column);
void rift_token_destroy(RiftToken* token);

// TokenMemory API (NEW)
RiftTokenMemory* rift_tokenmemory_create(void);
void rift_tokenmemory_destroy(RiftTokenMemory* memory);
RiftResult rift_tokenmemory_load_stage0(RiftTokenMemory* memory, const char* rift0_file);
RiftResult rift_tokenmemory_validate_tokens(RiftTokenMemory* memory);
RiftResult rift_tokenmemory_enhance_tokens(RiftTokenMemory* memory);

// AST API (NEW)
RiftASTNode* rift_ast_node_create(int type, const char* value);
void rift_ast_node_destroy(RiftASTNode* node);
RiftResult rift_ast_node_add_child(RiftASTNode* parent, RiftASTNode* child);
RiftResult rift_ast_optimize(RiftASTNode* root);

// Parse Engine API (NEW)
Rift1ParseEngine* rift1_engine_create(void);
void rift1_engine_destroy(Rift1ParseEngine* engine);
RiftResult rift1_engine_load_tokenmemory(Rift1ParseEngine* engine, RiftTokenMemory* memory);
RiftResult rift1_parse_tokens(Rift1ParseEngine* engine);
RiftResult rift1_generate_ast(Rift1ParseEngine* engine);

// Stage processing
RiftResult rift1_process_stage0_to_stage1(const char* rift0_file, const char* rift1_file, RiftConfig* config);

// Existing API (consolidated declarations)
RiftEngine* rift_engine_create(void);
void rift_engine_destroy(RiftEngine* engine);
RiftResult rift_engine_process_input(RiftEngine* engine, const char* input);
RiftToken* rift_engine_next_token(RiftEngine* engine);

RiftIR* rift_ir_create(const char* source_file);
void rift_ir_destroy(RiftIR* ir);
RiftResult rift_ir_add_token(RiftIR* ir, RiftToken* token);
RiftResult rift_ir_write_file(RiftIR* ir, const char* output_file);
RiftResult rift_ir_read_file(RiftIR* ir, const char* input_file);

RiftResult rift_process_file(const char* input_file, const char* output_file, RiftConfig* config);
const char* rift_result_string(RiftResult result);

#ifdef __cplusplus
}
#endif

#endif // RIFT1_H
