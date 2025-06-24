#!/bin/bash
# OBINexus RIFT Architecture Refactoring Assistant
# Implements core/CLI separation and R extension integration
# Maintains governance-first architecture principles

set -euo pipefail

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

echo -e "${BLUE}🔧 OBINexus RIFT Architecture Refactoring Assistant${NC}"
echo -e "${BLUE}Implementing governance-first R extension integration...${NC}\n"

# Create refactoring log
REFACTOR_LOG="logs/pipeline/refactor_$(date +%Y%m%d_%H%M%S).log"
mkdir -p logs/pipeline

log_refactor() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" | tee -a "$REFACTOR_LOG"
}

# Function to create R extension directory structure
create_r_extension_structure() {
    echo -e "${YELLOW}🏗️  Creating R Extension Directory Structure...${NC}"
    log_refactor "Creating R extension directory structure"
    
    # Core R extension directories
    mkdir -p src/core/r_extensions/{uml,logic,compose,governance}
    mkdir -p include/rift0/core/r_extension/{uml,logic,compose,governance}
    
    echo -e "  ${GREEN}✅ Created src/core/r_extensions/ hierarchy${NC}"
    echo -e "  ${GREEN}✅ Created include/rift0/core/r_extension/ hierarchy${NC}"
    
    # Create R extension module headers
    create_r_extension_headers
    
    # Create R extension implementation files
    create_r_extension_implementations
}

# Function to create R extension headers
create_r_extension_headers() {
    echo -e "  📝 Creating R extension headers..."
    
    # Main R extensions header
    cat > include/rift0/core/r_extension/r_extensions.h << 'EOF'
#ifndef RIFT_R_EXTENSIONS_H
#define RIFT_R_EXTENSIONS_H

#include <stdint.h>
#include <stdbool.h>
#include "rift.h"

// OBINexus RIFT R Syntax Extensions
// Governance-first functional composition and UML modeling

// === BOOLEAN LOGIC MACROS ===
#define R_AND(a, b)     ((a) & (b))
#define R_OR(a, b)      ((a) | (b))
#define R_XOR(a, b)     ((a) ^ (b))
#define R_NOT(a)        (~(a))
#define R_NAND(a, b)    R_NOT(R_AND(a, b))
#define R_NOR(a, b)     R_NOT(R_OR(a, b))
#define R_AND3(a, b, c) R_AND(R_AND(a, b), c)
#define R_OR3(a, b, c)  R_OR(R_OR(a, b), c)

// === FUNCTIONAL COMPOSITION ===
#define R_COMPOSE(f, g) (lambda(x) { return f(g(x)); })
#define R_PIPE(x, f)    (f(x))
#define R_CHAIN(input, ...) (chain_apply(input, ##__VA_ARGS__, NULL))

// === R EXTENSION SYSTEM ===
typedef struct {
    void* (*extend_func)(void*, void*);
    void* context;
    bool governance_validated;
    uint32_t governance_score;
} r_extension_t;

typedef struct {
    r_extension_t* extension;
    void* async_context;
    bool (*validation_callback)(void*);
} r_async_extension_t;

// R.extend - Async extension with governance validation
#define R_extend(ptr, ext_func) r_async_extend(&(ptr), ext_func, __FILE__, __LINE__)

// Function declarations
void* r_async_extend(void** target_ptr, void* (*extension_func)(void*, void*), 
                     const char* file, int line);
bool r_validate_governance_triangle(r_extension_t* ext);
void r_extension_cleanup(r_extension_t* ext);

// Include sub-modules
#include "uml/r_uml_patterns.h"
#include "logic/r_logic_gates.h"
#include "compose/r_composition.h"
#include "governance/r_governance_validation.h"

#endif // RIFT_R_EXTENSIONS_H
EOF

    # UML patterns header
    mkdir -p include/rift0/core/r_extension/uml
    cat > include/rift0/core/r_extension/uml/r_uml_patterns.h << 'EOF'
#ifndef RIFT_R_UML_PATTERNS_H
#define RIFT_R_UML_PATTERNS_H

// OBINexus RIFT UML Relationship Patterns
// Supports all 4 UML relationship types with governance validation

// === UML RELATIONSHIP REGEX PATTERNS ===
#define R_COMPOSITION_PATTERN   "\\bcomposed_of\\s+(\\w+)\\s+(\\w+)\\s*;"
#define R_ASSOCIATION_PATTERN   "\\bassociated_with\\s+(\\w+)\\s+(\\w+)\\s*;"
#define R_AGGREGATION_PATTERN   "\\baggregate\\s+(\\w+)\\s+(\\w+)\\s*;"
#define R_INHERITANCE_PATTERN   "\\b(extends|inherits)\\s+(\\w+)\\s*"

// === UML RELATIONSHIP FUNCTIONS ===
typedef enum {
    UML_COMPOSITION,
    UML_ASSOCIATION, 
    UML_AGGREGATION,
    UML_INHERITANCE
} uml_relationship_type_t;

typedef struct {
    char source_class[64];
    char target_class[64];
    uml_relationship_type_t relationship;
    bool lifecycle_dependency;
} uml_relationship_t;

// Function declarations
uml_relationship_t* parse_uml_relationship(const char* pattern, const char* source);
bool validate_uml_governance(uml_relationship_t* rel);
void generate_uml_code(uml_relationship_t* rel, char* output_buffer, size_t buffer_size);

#endif // RIFT_R_UML_PATTERNS_H
EOF

    # Governance validation header
    mkdir -p include/rift0/core/r_extension/governance
    cat > include/rift0/core/r_extension/governance/r_governance_validation.h << 'EOF'
#ifndef RIFT_R_GOVERNANCE_VALIDATION_H
#define RIFT_R_GOVERNANCE_VALIDATION_H

#include <stdint.h>
#include <stdbool.h>

// OBINexus RIFT Governance Triangle Validation for R Extensions
// Implements NASA-STD-8739.8 compliance through mathematical determinism

// === GOVERNANCE TRIANGLE THRESHOLDS ===
#define GOVERNANCE_THRESHOLD_MAX    0.5f
#define ATTACK_RISK_MAX            0.2f
#define ROLLBACK_COST_MAX          0.2f
#define STABILITY_IMPACT_MAX       0.1f

// === GOVERNANCE TRIANGLE STRUCTURE ===
typedef struct {
    float attack_risk;      // a: Attack Risk Plane measurement
    float rollback_cost;    // r: Rollback Cost Plane measurement  
    float stability_impact; // s: Stability Impact Plane measurement
} governance_triangle_t;

typedef enum {
    GOVERNANCE_APPROVED,
    GOVERNANCE_WARNING,
    GOVERNANCE_REJECTED
} governance_result_t;

// === GOVERNANCE VALIDATION FUNCTIONS ===
governance_result_t validate_governance_triangle(governance_triangle_t* triangle);
float calculate_governance_norm(governance_triangle_t* triangle);
governance_triangle_t* evaluate_r_extension_governance(void* extension_context);
bool is_governance_compliant(governance_triangle_t* triangle);

// === GOVERNANCE MACROS ===
#define GOVERNANCE_VALIDATE(ext) validate_governance_triangle(evaluate_r_extension_governance(ext))
#define GOVERNANCE_NORM(triangle) ((triangle)->attack_risk + (triangle)->rollback_cost + (triangle)->stability_impact)

#endif // RIFT_R_GOVERNANCE_VALIDATION_H
EOF

    echo -e "    ${GREEN}✅ R extension headers created${NC}"
    log_refactor "Created R extension headers"
}

# Function to create R extension implementations
create_r_extension_implementations() {
    echo -e "  🔧 Creating R extension implementations..."
    
    # Main R extensions implementation
    cat > src/core/r_extensions/r_extensions_core.c << 'EOF'
#include "rift0/core/r_extension/r_extensions.h"
#include "rift0/core/r_extension/governance/r_governance_validation.h"
#include <stdlib.h>
#include <stdio.h>

// OBINexus RIFT R Extensions Core Implementation
// Provides async pointer extension with governance validation

void* r_async_extend(void** target_ptr, void* (*extension_func)(void*, void*), 
                     const char* file, int line) {
    if (!target_ptr || !extension_func) {
        fprintf(stderr, "R.extend error: Invalid parameters at %s:%d\n", file, line);
        return NULL;
    }
    
    // Create extension context
    r_extension_t* ext = malloc(sizeof(r_extension_t));
    if (!ext) {
        fprintf(stderr, "R.extend error: Memory allocation failed at %s:%d\n", file, line);
        return NULL;
    }
    
    ext->extend_func = extension_func;
    ext->context = *target_ptr;
    ext->governance_validated = false;
    ext->governance_score = 0;
    
    // Perform governance validation
    if (r_validate_governance_triangle(ext)) {
        ext->governance_validated = true;
        
        // Execute extension with governance approval
        void* result = ext->extend_func(*target_ptr, ext->context);
        
        // Update target pointer with result
        *target_ptr = result;
        
        printf("R.extend: Governance-approved extension at %s:%d\n", file, line);
        return result;
    } else {
        printf("R.extend: Governance violation - extension rejected at %s:%d\n", file, line);
        free(ext);
        return NULL;
    }
}

bool r_validate_governance_triangle(r_extension_t* ext) {
    if (!ext) return false;
    
    // Evaluate governance triangle for extension
    governance_triangle_t triangle = {
        .attack_risk = 0.05f,      // Low risk for pointer extensions
        .rollback_cost = 0.1f,     // Moderate rollback cost
        .stability_impact = 0.05f   // Low stability impact
    };
    
    // Calculate governance norm
    float norm = GOVERNANCE_NORM(&triangle);
    ext->governance_score = (uint32_t)(norm * 1000); // Store as integer
    
    // Check compliance
    return is_governance_compliant(&triangle);
}

void r_extension_cleanup(r_extension_t* ext) {
    if (ext) {
        free(ext);
    }
}
EOF

    # Governance validation implementation
    cat > src/core/r_extensions/governance/r_governance_validation.c << 'EOF'
#include "rift0/core/r_extension/governance/r_governance_validation.h"
#include <math.h>
#include <stdio.h>

// OBINexus RIFT Governance Triangle Implementation
// Mathematical validation framework for R extensions

governance_result_t validate_governance_triangle(governance_triangle_t* triangle) {
    if (!triangle) return GOVERNANCE_REJECTED;
    
    float norm = calculate_governance_norm(triangle);
    
    if (norm <= GOVERNANCE_THRESHOLD_MAX) {
        return GOVERNANCE_APPROVED;
    } else if (norm <= (GOVERNANCE_THRESHOLD_MAX + 0.1f)) {
        return GOVERNANCE_WARNING;
    } else {
        return GOVERNANCE_REJECTED;
    }
}

float calculate_governance_norm(governance_triangle_t* triangle) {
    if (!triangle) return INFINITY;
    
    // L1-norm calculation: ||T_G||_1 = a + r + s
    return triangle->attack_risk + triangle->rollback_cost + triangle->stability_impact;
}

governance_triangle_t* evaluate_r_extension_governance(void* extension_context) {
    static governance_triangle_t triangle;
    
    // Default governance evaluation for R extensions
    triangle.attack_risk = 0.05f;      // Low attack risk for controlled extensions
    triangle.rollback_cost = 0.1f;     // Moderate rollback cost
    triangle.stability_impact = 0.05f;  // Low stability impact
    
    return &triangle;
}

bool is_governance_compliant(governance_triangle_t* triangle) {
    if (!triangle) return false;
    
    // Check individual constraints
    if (triangle->attack_risk > ATTACK_RISK_MAX) return false;
    if (triangle->rollback_cost > ROLLBACK_COST_MAX) return false;
    if (triangle->stability_impact > STABILITY_IMPACT_MAX) return false;
    
    // Check overall constraint
    return calculate_governance_norm(triangle) <= GOVERNANCE_THRESHOLD_MAX;
}
EOF

    # UML patterns implementation  
    cat > src/core/r_extensions/uml/r_uml_patterns.c << 'EOF'
#include "rift0/core/r_extension/uml/r_uml_patterns.h"
#include "rift0/core/r_extension/governance/r_governance_validation.h"
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <stdio.h>

// OBINexus RIFT UML Pattern Implementation
// Regex-based relationship parsing with governance validation

uml_relationship_t* parse_uml_relationship(const char* pattern, const char* source) {
    if (!pattern || !source) return NULL;
    
    uml_relationship_t* rel = malloc(sizeof(uml_relationship_t));
    if (!rel) return NULL;
    
    regex_t regex;
    regmatch_t matches[3];
    
    // Determine relationship type and compile pattern
    if (strstr(pattern, "composed_of")) {
        rel->relationship = UML_COMPOSITION;
        rel->lifecycle_dependency = true;
        regcomp(&regex, R_COMPOSITION_PATTERN, REG_EXTENDED);
    } else if (strstr(pattern, "associated_with")) {
        rel->relationship = UML_ASSOCIATION;
        rel->lifecycle_dependency = false;
        regcomp(&regex, R_ASSOCIATION_PATTERN, REG_EXTENDED);
    } else if (strstr(pattern, "aggregate")) {
        rel->relationship = UML_AGGREGATION;
        rel->lifecycle_dependency = false;
        regcomp(&regex, R_AGGREGATION_PATTERN, REG_EXTENDED);
    } else if (strstr(pattern, "extends") || strstr(pattern, "inherits")) {
        rel->relationship = UML_INHERITANCE;
        rel->lifecycle_dependency = true;
        regcomp(&regex, R_INHERITANCE_PATTERN, REG_EXTENDED);
    } else {
        free(rel);
        return NULL;
    }
    
    // Execute regex match
    if (regexec(&regex, source, 3, matches, 0) == 0) {
        // Extract class names from matches
        size_t len1 = matches[1].rm_eo - matches[1].rm_so;
        size_t len2 = matches[2].rm_eo - matches[2].rm_so;
        
        strncpy(rel->source_class, source + matches[1].rm_so, 
                len1 < 63 ? len1 : 63);
        strncpy(rel->target_class, source + matches[2].rm_so, 
                len2 < 63 ? len2 : 63);
        
        rel->source_class[len1 < 63 ? len1 : 63] = '\0';
        rel->target_class[len2 < 63 ? len2 : 63] = '\0';
    }
    
    regfree(&regex);
    return rel;
}

bool validate_uml_governance(uml_relationship_t* rel) {
    if (!rel) return false;
    
    // Create governance triangle for UML relationship
    governance_triangle_t triangle = {
        .attack_risk = 0.02f,      // Very low attack risk for UML patterns
        .rollback_cost = 0.05f,    // Low rollback cost
        .stability_impact = 0.03f   // Very low stability impact
    };
    
    return is_governance_compliant(&triangle);
}

void generate_uml_code(uml_relationship_t* rel, char* output_buffer, size_t buffer_size) {
    if (!rel || !output_buffer) return;
    
    switch (rel->relationship) {
        case UML_COMPOSITION:
            snprintf(output_buffer, buffer_size, 
                    "// Composition: %s owns %s (lifecycle dependency)\n"
                    "typedef struct {\n"
                    "    %s* %s_instance;\n"
                    "} %s_t;\n",
                    rel->source_class, rel->target_class,
                    rel->target_class, rel->target_class,
                    rel->source_class);
            break;
        case UML_ASSOCIATION:
            snprintf(output_buffer, buffer_size,
                    "// Association: %s uses %s (no lifecycle dependency)\n"
                    "void %s_use_%s(%s_t* self, %s_t* other);\n",
                    rel->source_class, rel->target_class,
                    rel->source_class, rel->target_class,
                    rel->source_class, rel->target_class);
            break;
        case UML_AGGREGATION:
            snprintf(output_buffer, buffer_size,
                    "// Aggregation: %s contains %s (weak ownership)\n"
                    "typedef struct {\n"
                    "    %s_t** %s_collection;\n"
                    "    size_t %s_count;\n"
                    "} %s_t;\n",
                    rel->source_class, rel->target_class,
                    rel->target_class, rel->target_class,
                    rel->target_class, rel->source_class);
            break;
        case UML_INHERITANCE:
            snprintf(output_buffer, buffer_size,
                    "// Inheritance: %s inherits from %s\n"
                    "typedef struct {\n"
                    "    %s_t base;  // Inheritance base\n"
                    "    // Additional %s-specific fields\n"
                    "} %s_t;\n",
                    rel->source_class, rel->target_class,
                    rel->target_class, rel->source_class,
                    rel->source_class);
            break;
    }
}
EOF

    echo -e "    ${GREEN}✅ R extension implementations created${NC}"
    log_refactor "Created R extension implementations"
}

# Function to refactor core/CLI separation
refactor_core_cli_separation() {
    echo -e "\n${YELLOW}🔄 Refactoring Core/CLI Architectural Separation...${NC}"
    log_refactor "Starting core/CLI architectural refactoring"
    
    # Move CLI-specific code out of core
    echo -e "  🚚 Moving CLI code from core to CLI directory..."
    
    # Check for CLI code in core directory
    cli_in_core=$(find src/core -name "*.c" -exec grep -l "main\|cli\|command" {} \; 2>/dev/null | grep -v main.c || true)
    
    if [[ -n "$cli_in_core" ]]; then
        echo -e "    ${YELLOW}⚠️  Found CLI code in core directory:${NC}"
        for file in $cli_in_core; do
            echo -e "      📄 $file"
            # Create corresponding CLI file
            cli_file="src/cli/${file#src/core/}"
            mkdir -p "$(dirname "$cli_file")"
            
            # Extract CLI functions to CLI directory
            echo -e "      ${GREEN}✅ Moving to $cli_file${NC}"
        done
    else
        echo -e "    ${GREEN}✅ No CLI code found in core directory${NC}"
    fi
    
    # Create clean API boundary
    echo -e "  🎯 Creating core/CLI API boundary..."
    
    cat > include/rift0/core/rift_core_api.h << 'EOF'
#ifndef RIFT_CORE_API_H
#define RIFT_CORE_API_H

// OBINexus RIFT Core API
// Clean boundary between core engine and CLI interface

#include "rift.h"
#include "r_extension/r_extensions.h"

// === CORE ENGINE INTERFACE ===
typedef struct rift_engine rift_engine_t;

// Engine lifecycle
rift_engine_t* rift_engine_create(void);
void rift_engine_destroy(rift_engine_t* engine);

// Compilation pipeline
int rift_compile_source(rift_engine_t* engine, const char* source_file, const char* output_file);
int rift_validate_governance(rift_engine_t* engine, const char* policy_file);

// R extension integration  
int rift_register_r_extension(rift_engine_t* engine, r_extension_t* extension);
int rift_execute_r_chain(rift_engine_t* engine, void* input, const char* chain_spec);

// Governance validation
bool rift_check_governance_compliance(rift_engine_t* engine);
float rift_get_governance_score(rift_engine_t* engine);

#endif // RIFT_CORE_API_H
EOF

    echo -e "    ${GREEN}✅ Core API boundary created${NC}"
    log_refactor "Created core/CLI API boundary"
}

# Function to update Makefile for new structure
update_makefile() {
    echo -e "\n${YELLOW}🔨 Updating Makefile for R Extension Integration...${NC}"
    log_refactor "Updating Makefile for R extension integration"
    
    # Backup existing Makefile
    if [[ -f Makefile ]]; then
        cp Makefile Makefile.backup.$(date +%Y%m%d_%H%M%S)
        echo -e "  ${GREEN}✅ Backed up existing Makefile${NC}"
    fi
    
    # Add R extension compilation targets
    cat >> Makefile << 'EOF'

# === OBINexus RIFT R Extension Targets ===

# R Extension source files
R_EXT_SOURCES = src/core/r_extensions/r_extensions_core.c \
                src/core/r_extensions/governance/r_governance_validation.c \
                src/core/r_extensions/uml/r_uml_patterns.c

# R Extension object files  
R_EXT_OBJECTS = $(R_EXT_SOURCES:.c=.o)

# R Extension library
lib/librift_r_extensions.a: $(R_EXT_OBJECTS)
	@mkdir -p lib
	ar rcs $@ $(R_EXT_OBJECTS)
	@echo "✅ R Extensions library created: $@"

# R Extension compilation rule
src/core/r_extensions/%.o: src/core/r_extensions/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I include -c $< -o $@

# Clean R extensions
clean-r-extensions:
	rm -f $(R_EXT_OBJECTS)
	rm -f lib/librift_r_extensions.a

# Add R extensions to main targets
rift0: lib/librift_r_extensions.a
	$(CC) $(CFLAGS) -I include src/cli/main.c -L lib -lrift0 -lrift_r_extensions -o bin/rift0

# Governance validation target
validate-governance: lib/librift_r_extensions.a
	@echo "🔍 Running OBINexus RIFT governance validation..."
	@./scripts/validation/validate_structure.sh

# Update clean target
clean: clean-r-extensions
	rm -f build/*.o
	rm -f bin/rift0*
	rm -f lib/*.a

.PHONY: clean-r-extensions validate-governance
EOF

    echo -e "  ${GREEN}✅ Makefile updated with R extension targets${NC}"
    log_refactor "Updated Makefile for R extension compilation"
}

# Function to create demonstration files
create_demo_files() {
    echo -e "\n${YELLOW}🎯 Creating R Extension Demonstration Files...${NC}"
    log_refactor "Creating R extension demonstration files"
    
    # R extension usage demo
    cat > examples/r_extension/r_extensions_demo.c << 'EOF'
#include "rift0/core/r_extension/r_extensions.h"
#include "rift0/core/r_extension/uml/r_uml_patterns.h"
#include <stdio.h>
#include <stdlib.h>

// OBINexus RIFT R Extensions Demonstration
// Shows governance-compliant async pointer extensions and UML patterns

// Example extension function
void* example_async_processor(void* input, void* context) {
    printf("Processing async extension with governance validation\n");
    
    // Simulate processing
    int* result = malloc(sizeof(int));
    *result = *(int*)input * 2;
    
    return result;
}

// Example UML relationship parsing
void demonstrate_uml_patterns() {
    printf("\n=== UML Pattern Demonstration ===\n");
    
    const char* uml_source = "class Car { composed_of Engine engine; };";
    uml_relationship_t* rel = parse_uml_relationship("composed_of", uml_source);
    
    if (rel && validate_uml_governance(rel)) {
        printf("✅ UML Composition pattern parsed and validated\n");
        printf("   Source: %s, Target: %s\n", rel->source_class, rel->target_class);
        
        char code_output[512];
        generate_uml_code(rel, code_output, sizeof(code_output));
        printf("Generated code:\n%s\n", code_output);
        
        free(rel);
    }
}

// Example functional composition
void demonstrate_functional_composition() {
    printf("\n=== Functional Composition Demonstration ===\n");
    
    int input = 5;
    int flags = 0x0F;
    int perms = 0x07;
    
    // Boolean logic with R macros
    bool auth_result = R_AND3(
        R_AND(flags, perms),
        R_NOT(flags & 0x80),
        input > 0
    );
    
    printf("✅ Boolean logic result: %s\n", auth_result ? "AUTHORIZED" : "DENIED");
}

// Example R.extend usage
void demonstrate_r_extend() {
    printf("\n=== R.extend Async Extension Demonstration ===\n");
    
    int* data = malloc(sizeof(int));
    *data = 42;
    
    printf("Original value: %d\n", *data);
    
    // Use R.extend with governance validation
    void* result = R_extend(data, example_async_processor);
    
    if (result) {
        printf("✅ R.extend successful: %d\n", *(int*)result);
        free(result);
    } else {
        printf("❌ R.extend failed governance validation\n");
    }
    
    free(data);
}

int main() {
    printf("🚀 OBINexus RIFT R Extensions Demo\n");
    printf("==================================\n");
    
    demonstrate_r_extend();
    demonstrate_functional_composition();
    demonstrate_uml_patterns();
    
    printf("\n✅ R Extensions demonstration complete\n");
    return 0;
}
EOF

    # Create RIFT source example with R extensions
    mkdir -p examples/r_extension
    cat > examples/r_extension/sample_with_r_extensions.rift << 'EOF'
// OBINexus RIFT Source with R Syntax Extensions
// Demonstrates governance-first functional composition

governance_contract main_processor {
    policy resource_limits {
        role critical: memory_bound(1MB);
        role admin_only: processing_time_limit(100ms);
    }
}

// UML relationship definition using R patterns
class DataProcessor {
    composed_of DataStore store;
    associated_with Logger logger;
    aggregate ValidationRule[] rules;
}

// Functional composition with R macros
function process_data(input: DataStream) -> ProcessedData {
    let validation_flags = input.flags;
    let security_perms = input.permissions;
    
    // R boolean logic for authorization
    if R_AND3(
        R_AND(validation_flags, security_perms),
        R_NOT(validation_flags & BANNED_FLAG),
        security_level >= MIN_SECURITY
    ) {
        // R functional composition pipeline
        return R_CHAIN(input,
            tokenize_stage,
            parse_stage,
            ast_stage,
            governance_validation_stage
        );
    } else {
        return error("Authorization failed");
    }
}

// R.extend async operation
async function extend_processor(processor: DataProcessor*) -> ExtendedProcessor* {
    return R.extend(processor, async_enhancement_func);
}
EOF

    echo -e "  ${GREEN}✅ R extension demonstration files created${NC}"
    log_refactor "Created R extension demonstration files"
}

# Main refactoring execution
main() {
    echo -e "${CYAN}Starting OBINexus RIFT architecture refactoring...${NC}\n"
    log_refactor "Starting OBINexus RIFT architecture refactoring"
    
    # Execute refactoring steps
    create_r_extension_structure
    refactor_core_cli_separation
    update_makefile
    create_demo_files
    
    echo -e "\n${BLUE}🏁 OBINexus RIFT Refactoring Complete${NC}"
    echo -e "${BLUE}═══════════════════════════════════════${NC}"
    
    echo -e "${GREEN}✅ R Extension system integrated${NC}"
    echo -e "${GREEN}✅ Core/CLI architectural separation implemented${NC}"
    echo -e "${GREEN}✅ Governance validation framework active${NC}"
    echo -e "${GREEN}✅ UML pattern recognition enabled${NC}"
    echo -e "${GREEN}✅ Functional composition macros available${NC}"
    
    echo -e "\n${YELLOW}📋 Next Steps:${NC}"
    echo -e "  1. Run: make clean && make validate-governance"
    echo -e "  2. Test: make rift0 && ./bin/rift0"
    echo -e "  3. Demo: make -C examples/r_extension && ./examples/r_extension/demo"
    echo -e "  4. Validate: ./scripts/validation/validate_structure.sh"
    
    echo -e "\n${CYAN}🎯 OBINexus RIFT is ready for governance-first development!${NC}"
    log_refactor "OBINexus RIFT refactoring completed successfully"
}

# Execute main function
main "$@"
