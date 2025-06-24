/**
 * @file r_uml_extensions_demo.c
 * @brief OBINexus RIFT R Syntax Extensions for UML Relationships & Functional Composition
 * 
 * Demonstrates the power of R"" and R'' syntax for:
 * - UML relationship modeling (Composition, Association, Aggregation, Inheritance)
 * - Point-free functional composition with lambda-like logic
 * - Boolean operations as macros (AND, OR, XOR, NOT, NAND, NOR)
 * - Data-agnostic functions without module complexity overhead
 * 
 * Part of OBINexus Computing RIFT Governance-First Architecture
 * 
 * Compile with: gcc -I./include -L./lib -lrift0 r_uml_extensions_demo.c -o r_uml_demo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "librift/regex/automaton/automaton.h"
#include "librift/regex/engine/pattern.h"
#include "librift/regex/runtime/context.h"
#include "librift/regex/syntax/integration.h"

/* =============================================================================
 * OBINexus R Syntax Boolean Logic Macros
 * Implementation of governance-first logical operations
 * ============================================================================= */

// Core Boolean Operations
#define R_AND(a, b)     ((a) & (b))
#define R_OR(a, b)      ((a) | (b))
#define R_XOR(a, b)     ((a) ^ (b))
#define R_NOT(a)        (~(a))

// Extended Boolean Operations
#define R_NAND(a, b)    R_NOT(R_AND(a, b))
#define R_NOR(a, b)     R_NOT(R_OR(a, b))

// Functional Composition Macros
#define R_COMPOSE(f, g) (lambda(x) -> f(g(x)))
#define R_CHAIN(...)    (__VA_ARGS__)
#define R_PIPE(x, f)    f(x)

/* =============================================================================
 * UML Relationship Pattern Definitions
 * Using R"" and R'' syntax for relationship modeling
 * ============================================================================= */

// UML Composition Pattern - "has-a" strong ownership
#define R_COMPOSITION_PATTERN \
    R"class\s+(\w+)\s*{\s*([^}]*\bcomposed_of\s+(\w+)[^}]*)\s*}"

// UML Association Pattern - basic relationship
#define R_ASSOCIATION_PATTERN \
    R"class\s+(\w+)\s*{\s*([^}]*\bassociated_with\s+(\w+)[^}]*)\s*}"

// UML Aggregation Pattern - "has-a" weak ownership
#define R_AGGREGATION_PATTERN \
    R"class\s+(\w+)\s*{\s*([^}]*\baggregates\s+(\w+)[^}]*)\s*}"

// UML Inheritance Pattern - "is-a" relationship
#define R_INHERITANCE_PATTERN \
    R"class\s+(\w+)\s+extends\s+(\w+)\s*{"

/* =============================================================================
 * R Syntax Extension Structures
 * Governance-first data structures for relationship modeling
 * ============================================================================= */

typedef enum {
    R_REL_COMPOSITION,
    R_REL_ASSOCIATION, 
    R_REL_AGGREGATION,
    R_REL_INHERITANCE
} r_relationship_type_t;

typedef struct {
    char *source_class;
    char *target_class;
    r_relationship_type_t type;
    bool governance_validated;
} r_relationship_t;

typedef struct {
    r_relationship_t **relationships;
    size_t count;
    size_t capacity;
} r_model_t;

typedef struct {
    rift_regex_pattern_t *pattern;
    r_relationship_type_t type;
    const char *name;
} r_pattern_registry_t;

/* =============================================================================
 * R Extension Functions - Point-Free Functional Programming
 * ============================================================================= */

/**
 * R.extend - Extend functionality using pointer/reference for async features
 */
r_model_t* R_extend(r_model_t *base_model, r_relationship_t *new_relationship) {
    if (!base_model || !new_relationship) return NULL;
    
    // Governance validation check
    if (!new_relationship->governance_validated) {
        printf("[R.extend] Governance validation required for relationship extension\n");
        return NULL;
    }
    
    // Extend capacity if needed
    if (base_model->count >= base_model->capacity) {
        base_model->capacity *= 2;
        base_model->relationships = realloc(base_model->relationships, 
            base_model->capacity * sizeof(r_relationship_t*));
    }
    
    base_model->relationships[base_model->count] = new_relationship;
    base_model->count++;
    
    printf("[R.extend] Extended model with %s -> %s (%d)\n", 
           new_relationship->source_class, 
           new_relationship->target_class,
           new_relationship->type);
    
    return base_model;
}

/**
 * R.compose - Function composition for data-agnostic operations
 */
typedef void* (*r_transform_fn)(void*);

void* R_compose(r_transform_fn f, r_transform_fn g, void *data) {
    if (!f || !g || !data) return NULL;
    return f(g(data));
}

/**
 * R.aggregate - Combine multiple relationships into collections
 */
r_model_t* R_aggregate(r_relationship_t **relationships, size_t count) {
    r_model_t *model = malloc(sizeof(r_model_t));
    if (!model) return NULL;
    
    model->relationships = malloc(count * 2 * sizeof(r_relationship_t*));
    model->count = 0;
    model->capacity = count * 2;
    
    for (size_t i = 0; i < count; i++) {
        if (relationships[i] && relationships[i]->governance_validated) {
            model->relationships[model->count++] = relationships[i];
        }
    }
    
    printf("[R.aggregate] Aggregated %zu validated relationships\n", model->count);
    return model;
}

/**
 * R.association - Create basic relationship associations
 */
r_relationship_t* R_association(const char *source, const char *target) {
    r_relationship_t *rel = malloc(sizeof(r_relationship_t));
    if (!rel) return NULL;
    
    rel->source_class = strdup(source);
    rel->target_class = strdup(target);
    rel->type = R_REL_ASSOCIATION;
    rel->governance_validated = true; // Basic validation for demo
    
    return rel;
}

/**
 * R.inherit - Create inheritance relationships with governance checks
 */
r_relationship_t* R_inherit(const char *child, const char *parent) {
    r_relationship_t *rel = malloc(sizeof(r_relationship_t));
    if (!rel) return NULL;
    
    rel->source_class = strdup(child);
    rel->target_class = strdup(parent);
    rel->type = R_REL_INHERITANCE;
    rel->governance_validated = true;
    
    printf("[R.inherit] %s inherits from %s\n", child, parent);
    return rel;
}

/* =============================================================================
 * UML Relationship Pattern Recognition Engine
 * ============================================================================= */

void demonstrate_r_syntax_patterns(void) {
    rift_regex_error_t error;
    rift_regex_flags_t flags = RIFT_REGEX_FLAG_RIFT_SYNTAX;
    
    printf("==============================================\n");
    printf("  R Syntax UML Relationship Patterns\n");
    printf("==============================================\n\n");
    
    // Define pattern registry
    r_pattern_registry_t patterns[] = {
        {NULL, R_REL_COMPOSITION, "Composition"},
        {NULL, R_REL_ASSOCIATION, "Association"},
        {NULL, R_REL_AGGREGATION, "Aggregation"},
        {NULL, R_REL_INHERITANCE, "Inheritance"}
    };
    
    // Compile patterns using R"" syntax
    patterns[0].pattern = rift_regex_compile(R_COMPOSITION_PATTERN, flags, &error);
    patterns[1].pattern = rift_regex_compile(R_ASSOCIATION_PATTERN, flags, &error);
    patterns[2].pattern = rift_regex_compile(R_AGGREGATION_PATTERN, flags, &error);
    patterns[3].pattern = rift_regex_compile(R_INHERITANCE_PATTERN, flags, &error);
    
    // Sample UML code to parse
    const char *sample_uml[] = {
        "class Car { composed_of Engine engine; }",
        "class Student { associated_with University school; }",
        "class Department { aggregates Employee staff; }",
        "class SportsCar extends Car {"
    };
    
    printf("Parsing UML relationships using R\"\" patterns:\n\n");
    
    for (int i = 0; i < 4; i++) {
        printf("Sample %d: %s\n", i+1, sample_uml[i]);
        
        for (int j = 0; j < 4; j++) {
            if (!patterns[j].pattern) continue;
            
            rift_regex_matcher_t *matcher = rift_matcher_create(
                patterns[j].pattern, RIFT_MATCHER_OPTION_NONE);
            
            if (rift_matcher_set_input(matcher, sample_uml[i], strlen(sample_uml[i]))) {
                rift_regex_match_t match;
                if (rift_matcher_matches(matcher, &match)) {
                    printf("  ✓ Matched %s pattern\n", patterns[j].name);
                    if (match.group_count >= 2) {
                        printf("    Source: %s\n", match.groups[1].value);
                        if (match.group_count >= 3) {
                            printf("    Target: %s\n", 
                                   patterns[j].type == R_REL_INHERITANCE ? 
                                   match.groups[2].value : match.groups[3].value);
                        }
                    }
                    rift_match_free(&match);
                }
            }
            
            rift_matcher_free(matcher);
        }
        printf("\n");
    }
    
    // Cleanup
    for (int i = 0; i < 4; i++) {
        if (patterns[i].pattern) {
            rift_regex_pattern_free(patterns[i].pattern);
        }
    }
}

/* =============================================================================
 * Point-Free Functional Composition Demonstration
 * ============================================================================= */

// Data transformation functions for composition
void* transform_to_uppercase(void *data) {
    char *str = (char*)data;
    char *result = malloc(strlen(str) + 1);
    for (int i = 0; str[i]; i++) {
        result[i] = str[i] >= 'a' && str[i] <= 'z' ? str[i] - 32 : str[i];
    }
    result[strlen(str)] = '\0';
    return result;
}

void* transform_add_prefix(void *data) {
    char *str = (char*)data;
    char *result = malloc(strlen(str) + 10);
    snprintf(result, strlen(str) + 10, "OBINexus_%s", str);
    return result;
}

void demonstrate_r_composition(void) {
    printf("==============================================\n");
    printf("  R.compose Functional Composition Demo\n");
    printf("==============================================\n\n");
    
    const char *input = "governance_model";
    printf("Input: %s\n", input);
    
    // Demonstrate R.compose with function pipeline
    char *input_copy = strdup(input);
    void *result = R_compose(transform_add_prefix, transform_to_uppercase, input_copy);
    
    printf("R.compose(add_prefix, uppercase, \"%s\") = %s\n\n", input, (char*)result);
    
    // Clean up
    free(input_copy);
    free(result);
}

/* =============================================================================
 * Boolean Logic Operations Demonstration
 * ============================================================================= */

void demonstrate_r_boolean_operations(void) {
    printf("==============================================\n");
    printf("  R Boolean Logic Operations\n");
    printf("==============================================\n\n");
    
    int a = 5;  // 101 in binary
    int b = 3;  // 011 in binary
    
    printf("Input values: a = %d (101), b = %d (011)\n\n", a, b);
    
    printf("Basic Operations:\n");
    printf("R_AND(a, b)  = %d (should be 1)\n", R_AND(a, b));
    printf("R_OR(a, b)   = %d (should be 7)\n", R_OR(a, b));
    printf("R_XOR(a, b)  = %d (should be 6)\n", R_XOR(a, b));
    printf("R_NOT(a)     = %d\n", R_NOT(a));
    
    printf("\nExtended Operations:\n");
    printf("R_NAND(a, b) = %d\n", R_NAND(a, b));
    printf("R_NOR(a, b)  = %d\n", R_NOR(a, b));
    
    printf("\nGovernance Validation Example:\n");
    bool governance_check = R_AND(a > 0, b > 0);
    printf("R_AND(a > 0, b > 0) = %s\n", governance_check ? "VALID" : "INVALID");
}

/* =============================================================================
 * R Extension Integration Demo
 * ============================================================================= */

void demonstrate_r_extensions(void) {
    printf("==============================================\n");
    printf("  R Extension System Integration\n");
    printf("==============================================\n\n");
    
    // Create relationships using R extensions
    r_relationship_t *rel1 = R_association("Customer", "Order");
    r_relationship_t *rel2 = R_inherit("Manager", "Employee");
    r_relationship_t *rel3 = R_association("Order", "Product");
    
    // Aggregate relationships
    r_relationship_t *relationships[] = {rel1, rel2, rel3};
    r_model_t *model = R_aggregate(relationships, 3);
    
    // Extend model with new relationship
    r_relationship_t *rel4 = R_association("Product", "Category");
    R_extend(model, rel4);
    
    printf("Final model contains %zu relationships\n", model->count);
    
    // Cleanup
    for (size_t i = 0; i < model->count; i++) {
        free(model->relationships[i]->source_class);
        free(model->relationships[i]->target_class);
        free(model->relationships[i]);
    }
    free(model->relationships);
    free(model);
}

/* =============================================================================
 * Main Demonstration Function
 * ============================================================================= */

int main(int argc, char *argv[]) {
    printf("==============================================\n");
    printf("  OBINexus RIFT R Syntax Extensions Demo\n");
    printf("  Governance-First UML & Functional Composition\n");
    printf("==============================================\n\n");
    
    printf("R Syntax Features:\n");
    printf("- R\"\" and R'' pattern syntax for UML relationships\n");
    printf("- Point-free functional composition\n");
    printf("- Boolean logic macros (AND, OR, XOR, NOT, NAND, NOR)\n");
    printf("- Data-agnostic function extensions\n");
    printf("- Governance-first validation\n\n");
    
    // Run demonstrations
    demonstrate_r_syntax_patterns();
    demonstrate_r_composition();
    demonstrate_r_boolean_operations();
    demonstrate_r_extensions();
    
    printf("==============================================\n");
    printf("  R Syntax Extensions Demo Complete\n");
    printf("==============================================\n");
    
    return 0;
}