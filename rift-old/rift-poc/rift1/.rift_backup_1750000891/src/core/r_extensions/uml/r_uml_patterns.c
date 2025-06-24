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
