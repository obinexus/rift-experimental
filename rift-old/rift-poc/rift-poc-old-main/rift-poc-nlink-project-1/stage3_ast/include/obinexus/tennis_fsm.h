#ifndef TENNIS_FSM_H
#define TENNIS_FSM_H

#include <stdint.h>
#include <stdbool.h>

typedef struct TennisFSMValidator TennisFSMValidator;
typedef struct ParseTree ParseTree;
typedef struct AST AST;

typedef struct {
    uint32_t original_states;
    uint32_t minimized_states;
    float reduction_percentage;
    bool equivalence_proven;
} TennisFSMResult;

typedef struct {
    uint32_t original_states;
    uint32_t minimized_states;
} TennisResult;

TennisFSMValidator* tennis_fsm_validator_create(void);
TennisFSMResult tennis_fsm_validate_minimization(TennisFSMValidator* validator, 
                                               ParseTree* original_ast, 
                                               AST* minimized_ast);
void tennis_fsm_validator_free(TennisFSMValidator* validator);

// Semantic equivalence verification
bool ast_verify_semantic_equivalence(ParseTree* original, AST* minimized);

#endif
