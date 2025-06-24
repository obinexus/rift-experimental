
// cli/command/pattern_manager.h
#ifndef RIFT_PATTERN_MANAGER_H
#define RIFT_PATTERN_MANAGER_H

#include "../../include/rift.h"
#include "config_handler.h"

// Pattern management for top-down vs bottom-up matching
typedef struct RiftPatternManager {
    PatternMode mode;
    RiftAutomaton* automaton;
    RiftPattern** patterns;
    size_t pattern_count;
    size_t pattern_capacity;
    bool thread_safe_mode;
    pthread_mutex_t pattern_mutex;
} RiftPatternManager;

// Pattern manager lifecycle
RiftPatternManager* rift_pattern_manager_create(PatternMode mode);
void rift_pattern_manager_destroy(RiftPatternManager* manager);

// Pattern processing based on mode
RiftResult rift_pattern_manager_add_pattern(RiftPatternManager* manager, 
                                           const char* pattern_string);
RiftResult rift_pattern_manager_process_input(RiftPatternManager* manager,
                                             const char* input,
                                             RiftToken** tokens,
                                             size_t* token_count);

// Mode-specific processing
RiftResult rift_process_top_down(RiftPatternManager* manager, 
                                const char* input,
                                RiftToken** tokens,
                                size_t* token_count);
RiftResult rift_process_bottom_up(RiftPatternManager* manager,
                                 const char* input, 
                                 RiftToken** tokens,
                                 size_t* token_count);

// Pattern parsing enhancement for RIFT syntax R"pattern/flags[mode]"
typedef struct RiftPatternSyntax {
    char* raw_pattern;      // Full R"pattern/flags[mode]" 
    char* regex_pattern;    // Extracted "pattern" part
    char* flags;           // Extracted "flags" part  
    char mode;             // Extracted 'mode' character
    bool is_valid;
} RiftPatternSyntax;

RiftPatternSyntax* rift_parse_rift_syntax(const char* rift_pattern);
void rift_pattern_syntax_destroy(RiftPatternSyntax* syntax);

#endif // RIFT_PATTERN_MANAGER_H