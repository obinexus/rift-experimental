#ifndef RIFT_R_UML_PATTERNS_H
#define RIFT_R_UML_PATTERNS_H

// OBINexus RIFT UML Relationship Patterns
// Supports all 4 UML relationship types with governance validation

// === UML RELATIONSHIP REGEX PATTERNS ===
#define R_COMPOSITION_PATTERN "\\bcomposed_of\\s+(\\w+)\\s+(\\w+)\\s*;"
#define R_ASSOCIATION_PATTERN "\\bassociated_with\\s+(\\w+)\\s+(\\w+)\\s*;"
#define R_AGGREGATION_PATTERN "\\baggregate\\s+(\\w+)\\s+(\\w+)\\s*;"
#define R_INHERITANCE_PATTERN "\\b(extends|inherits)\\s+(\\w+)\\s*"

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
uml_relationship_t *parse_uml_relationship(const char *pattern,
                                           const char *source);
bool validate_uml_governance(uml_relationship_t *rel);
void generate_uml_code(uml_relationship_t *rel, char *output_buffer,
                       size_t buffer_size);

#endif // RIFT_R_UML_PATTERNS_H
