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
