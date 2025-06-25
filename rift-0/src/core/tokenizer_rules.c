/**
 * =================================================================
 * tokenizer_rules.c - RIFT-0 Rule Processing Implementation
 * RIFT: RIFT Is a Flexible Translator
 * Component: DFA and regex rule implementation with pattern matching
 * OBINexus Computing Framework - Stage 0 Implementation
 * =================================================================
 */

#include "rift-0/core/tokenizer_rules.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

/* Forward declaration for internal helper */
static bool _compile_simple_pattern(RegexComposition* regex, const char* pattern);



/* =================================================================
 * DFA STATE MANAGEMENT IMPLEMENTATION
 * =================================================================
 */

DFAState* rift_dfa_create_state(uint32_t state_id, bool is_final) {
    DFAState* state = calloc(1, sizeof(DFAState));
    if (!state) return NULL;
    
    state->state_id = state_id;
    state->is_final = is_final;
    state->token_type = TOKEN_UNKNOWN;
    state->transition_count = 0;
    state->flags = 0;
    state->user_data = NULL;
    
    /* Allocate transition table for all 256 possible characters */
    state->transitions = calloc(256, sizeof(DFAState*));
    if (!state->transitions) {
        free(state);
        return NULL;
    }
    
    return state;
}

void rift_dfa_destroy_states(DFAState* root) {
    if (!root) return;
    
    /* Use breadth-first traversal to avoid deep recursion */
    DFAState** queue = malloc(DFA_MAX_STATES * sizeof(DFAState*));
    if (!queue) return;
    
    bool* visited = calloc(DFA_MAX_STATES, sizeof(bool));
    if (!visited) {
        free(queue);
        return;
    }
    
    size_t queue_front = 0, queue_back = 0;
    queue[queue_back++] = root;
    
    while (queue_front < queue_back) {
        DFAState* current = queue[queue_front++];
        
        if (!current || visited[current->state_id % DFA_MAX_STATES]) continue;
        visited[current->state_id % DFA_MAX_STATES] = true;
        
        /* Add all transition targets to queue */
        for (int i = 0; i < 256; i++) {
            if (current->transitions[i] && 
                !visited[current->transitions[i]->state_id % DFA_MAX_STATES]) {
                queue[queue_back++] = current->transitions[i];
            }
        }
        
        /* Free current state */
        free(current->transitions);
        free(current);
    }
    
    free(queue);
    free(visited);
}

bool rift_dfa_add_transition(DFAState* from, DFAState* to, char transition_char) {
    if (!from || !to || !from->transitions) return false;
    
    unsigned char index = (unsigned char)transition_char;
    from->transitions[index] = to;
    from->transition_count++;
    
    return true;
}

DFAState* rift_dfa_process_input(DFAState* start, const char* input, size_t length) {
    if (!start || !input || length == 0) return NULL;
    
    DFAState* current = start;
    
    for (size_t i = 0; i < length && current; i++) {
        unsigned char c = (unsigned char)input[i];
        current = current->transitions[c];
    }
    
    return current;
}

bool rift_dfa_is_accepting_state(DFAState* state) {
    return state ? state->is_final : false;
}

TokenType rift_dfa_get_token_type(DFAState* state) {
    return state ? state->token_type : TOKEN_UNKNOWN;
}

bool rift_dfa_set_token_type(DFAState* state, TokenType token_type) {
    if (!state) return false;
    
    state->token_type = token_type;
    return true;
}

/* =================================================================
 * REGEX COMPOSITION IMPLEMENTATION
 * =================================================================
 */

RegexComposition* rift_regex_compile(const char* pattern, TokenFlags flags) {
    if (!pattern) return NULL;
    
    RegexComposition* regex = calloc(1, sizeof(RegexComposition));
    if (!regex) return NULL;
    
    size_t pattern_len = strlen(pattern);
    regex->pattern = malloc(pattern_len + 1);
    if (!regex->pattern) {
        free(regex);
        return NULL;
    }
    
    strcpy(regex->pattern, pattern);
    regex->pattern_length = pattern_len;
    regex->flags = flags;
    regex->is_compiled = false;
    
    /* Create initial DFA state */
    regex->start_state = rift_dfa_create_state(0, false);
    if (!regex->start_state) {
        free(regex->pattern);
        free(regex);
        return NULL;
    }
    
    /* Allocate accept states array */
    regex->accept_states = calloc(16, sizeof(DFAState*));
    if (!regex->accept_states) {
        rift_dfa_destroy_states(regex->start_state);
        free(regex->pattern);
        free(regex);
        return NULL;
    }
    
    regex->accept_count = 0;
    
    /* Simple pattern compilation - extend for complex regex support */
    if (_compile_simple_pattern(regex, pattern)) {
        regex->is_compiled = true;
    }
    
    return regex;
}

void rift_regex_destroy(RegexComposition* regex) {
    if (!regex) return;
    
    if (regex->start_state) {
        rift_dfa_destroy_states(regex->start_state);
    }
    
    free(regex->accept_states);
    free(regex->pattern);
    free(regex);
}

bool rift_regex_match(const RegexComposition* regex, const char* input, size_t length) {
    if (!regex || !regex->is_compiled || !input) return false;
    
    DFAState* final_state = rift_dfa_process_input(regex->start_state, input, length);
    return rift_dfa_is_accepting_state(final_state);
}

size_t rift_regex_extract_matches(const RegexComposition* regex, 
                                  const char* input, 
                                  size_t length,
                                  TokenTriplet* matches, 
                                  size_t max_matches) {
    if (!regex || !input || !matches || max_matches == 0) return 0;
    
    size_t match_count = 0;
    size_t pos = 0;
    
    while (pos < length && match_count < max_matches) {
        /* Try to match at current position */
        for (size_t end = pos + 1; end <= length; end++) {
            if (rift_regex_match(regex, input + pos, end - pos)) {
                /* Create token for this match */
                matches[match_count] = rift_token_create(
                    rift_dfa_get_token_type(regex->start_state),
                    (uint16_t)pos,
                    (uint8_t)regex->flags
                );
                match_count++;
                pos = end;
                break;
            }
        }
        
        if (pos == length) break;
        pos++;
    }
    
    return match_count;
}

/* =================================================================
 * TOKEN TRIPLET OPERATIONS IMPLEMENTATION
 * =================================================================
 */

TokenTriplet rift_token_create(uint8_t type, uint16_t mem_ptr, uint8_t value) {
    TokenTriplet token;
    token.type = type;
    token.mem_ptr = mem_ptr;
    token.value = value;
    return token;
}

bool rift_token_validate(const TokenTriplet* token) {
    if (!token) return false;

    /* Validate token type is within defined TokenType enum range */
    switch ((TokenType)token->type) {
        case TOKEN_UNKNOWN:
        case TOKEN_IDENTIFIER:
        case TOKEN_LITERAL_NUMBER:
        case TOKEN_LITERAL_STRING:
        case TOKEN_OPERATOR:
        case TOKEN_KEYWORD:
        case TOKEN_WHITESPACE:
        case TOKEN_COMMENT:
        case TOKEN_ERROR:
            break;
        default:
            return false;
    }

    /* Validate memory pointer is within allowed bounds */
    if (token->mem_ptr >= (uint16_t)RIFT_TOKENIZER_MAX_TOKENS) return false;

    return true;
}

TokenType rift_token_get_type(const TokenTriplet* token) {
    return token ? (TokenType)token->type : TOKEN_UNKNOWN;
}

uint16_t rift_token_get_mem_ptr(const TokenTriplet* token) {
    return token ? token->mem_ptr : 0;
}

uint8_t rift_token_get_value(const TokenTriplet* token) {
    return token ? token->value : 0;
}

TokenTriplet rift_token_set_flags(TokenTriplet token, TokenFlags flags) {
    token.value = (uint8_t)flags;
    return token;
}

/* =================================================================
 * PATTERN MATCHING RULES IMPLEMENTATION
 * =================================================================
 */

bool rift_rules_register_pattern(TokenizerContext* ctx, 
                                 const char* pattern,
                                 TokenType token_type,
                                 TokenFlags flags) {
    if (!ctx || !pattern || ctx->pattern_count >= ctx->pattern_capacity) {
        if (ctx) {
            snprintf(ctx->error_message, RIFT_TOKENIZER_ERROR_MSG_SIZE,
                    "Cannot register pattern: invalid parameters or capacity exceeded");
            ctx->error_code = RIFT_TOKENIZER_ERROR_INVALID_INPUT;
            ctx->has_error = true;
        }
        return false;
    }
    
    /* Compile the pattern */
    RegexComposition* regex = rift_regex_compile(pattern, flags);
    if (!regex) {
        snprintf(ctx->error_message, RIFT_TOKENIZER_ERROR_MSG_SIZE,
                "Failed to compile regex pattern: %s", pattern);
        ctx->error_code = RIFT_TOKENIZER_ERROR_REGEX_COMPILATION_FAILED;
        ctx->has_error = true;
        return false;
    }
    
    /* Set the token type for the compiled regex */
    if (regex->start_state) {
        rift_dfa_set_token_type(regex->start_state, token_type);
    }
    
    /* Add to context patterns */
    ctx->regex_patterns[ctx->pattern_count] = regex;
    ctx->pattern_count++;
    
    /* Update statistics */
    ctx->stats.regex_patterns++;
    
    return true;
}

bool rift_rules_unregister_pattern(TokenizerContext* ctx, const char* pattern) {
    if (!ctx || !pattern) return false;
    
    /* Find pattern in registered patterns */
    for (size_t i = 0; i < ctx->pattern_count; i++) {
        RegexComposition* regex = ctx->regex_patterns[i];
        if (regex && regex->pattern && strcmp(regex->pattern, pattern) == 0) {
            /* Destroy the regex */
            rift_regex_destroy(regex);
            
            /* Shift remaining patterns */
            for (size_t j = i; j < ctx->pattern_count - 1; j++) {
                ctx->regex_patterns[j] = ctx->regex_patterns[j + 1];
            }
            
            ctx->pattern_count--;
            ctx->regex_patterns[ctx->pattern_count] = NULL;
            ctx->stats.regex_patterns--;
            
            return true;
        }
    }
    
    return false;
}

ssize_t rift_rules_apply_all(TokenizerContext* ctx, const char* input, size_t length) {
    if (!ctx || !input) return -1;
    
    size_t total_tokens = 0;
    size_t pos = 0;
    
    while (pos < length) {
        bool found_match = false;
        size_t best_match_length = 0;
        RegexComposition* best_regex = NULL;
        
        /* Try all patterns at current position */
        for (size_t i = 0; i < ctx->pattern_count; i++) {
            RegexComposition* regex = ctx->regex_patterns[i];
            if (!regex) continue;
            
            /* Try progressively longer matches */
            for (size_t end = pos + 1; end <= length; end++) {
                if (rift_regex_match(regex, input + pos, end - pos)) {
                    size_t match_length = end - pos;
                    if (match_length > best_match_length) {
                        best_match_length = match_length;
                        best_regex = regex;
                        found_match = true;
                    }
                }
            }
        }
        
        if (found_match && total_tokens < ctx->token_capacity) {
            /* Create token for best match */
            ctx->tokens[total_tokens] = rift_token_create(
                rift_dfa_get_token_type(best_regex->start_state),
                (uint16_t)pos,
                (uint8_t)best_regex->flags
            );
            total_tokens++;
            pos += best_match_length;
        } else {
            /* No match found, advance by one character */
            if (total_tokens < ctx->token_capacity) {
                ctx->tokens[total_tokens] = rift_token_create(
                    TOKEN_UNKNOWN,
                    (uint16_t)pos,
                    0
                );
                total_tokens++;
            }
            pos++;
        }
        
        /* Update position tracking */
        if (input[pos - 1] == '\n') {
            ctx->line_number++;
            ctx->column_number = 1;
        } else {
            ctx->column_number++;
        }
    }
    
    ctx->token_count = total_tokens;
    return (ssize_t)total_tokens;
}

size_t rift_rules_get_count(const TokenizerContext* ctx) {
    return ctx ? ctx->pattern_count : 0;
}

bool rift_rules_clear_all(TokenizerContext* ctx) {
    if (!ctx) return false;
    
    /* Destroy all patterns */
    for (size_t i = 0; i < ctx->pattern_count; i++) {
        if (ctx->regex_patterns[i]) {
            rift_regex_destroy(ctx->regex_patterns[i]);
            ctx->regex_patterns[i] = NULL;
        }
    }
    
    ctx->pattern_count = 0;
    ctx->stats.regex_patterns = 0;
    
    return true;
}

/* =================================================================
 * ERROR HANDLING IMPLEMENTATION
 * =================================================================
 */

const char* rift_rules_get_last_error(const TokenizerContext* ctx) {
    if (!ctx || !ctx->has_error) return NULL;
    return ctx->error_message;
}

TokenizerErrorCode rift_rules_get_last_error_code(const TokenizerContext* ctx) {
    if (!ctx) return RIFT_TOKENIZER_ERROR_NULL_CONTEXT;
    return ctx->error_code;
}

void rift_rules_clear_error(TokenizerContext* ctx) {
    if (!ctx) return;
    
    ctx->error_message[0] = '\0';
    ctx->error_code = RIFT_TOKENIZER_SUCCESS;
    ctx->has_error = false;
}

/* =================================================================
 * INTERNAL HELPER FUNCTIONS
 * =================================================================
 */

static bool _compile_simple_pattern(RegexComposition* regex, const char* pattern) {
    if (!regex || !pattern) return false;
    
    /*
     * Minimal regex compiler supporting character classes, wildcards and
     * basic quantifiers (*, +, ?).  The implementation is intentionally
     * conservative but is sufficient for the patterns used in the unit
     * tests.  We avoid full-blown NFA→DFA conversion by restricting the
     * grammar to concatenated elements with optional postfix modifiers.
     */

    typedef struct SkipNode {
        DFAState* from;
        struct SkipNode* next;
    } SkipNode;

    SkipNode* skip_list = NULL;
    DFAState* current = regex->start_state;
    uint32_t state_id = 1;
    size_t i = 0;

    /* Skip anchors if present */
    if (pattern[0] == '^') {
        i++;
    }
    size_t pattern_len = strlen(pattern);
    if (pattern_len > 0 && pattern[pattern_len - 1] == '$') {
        pattern_len--; /* Ignore trailing '$' */
    }

    while (i < pattern_len) {
        bool char_set[256] = {false};
        bool negate = false;

        /* ------------------- Parse single unit ------------------- */
        if (pattern[i] == '\\' && i + 1 < pattern_len) {
            /* Escaped literal character */
            char_set[(unsigned char)pattern[i + 1]] = true;
            i += 2;
        } else if (pattern[i] == '[') {
            /* Character class */
            i++; /* skip '[' */
            if (pattern[i] == '^') {
                negate = true;
                i++;
            }
            bool temp_set[256] = {false};
            while (i < pattern_len && pattern[i] != ']') {
                unsigned char c1 = (unsigned char)pattern[i];
                if (c1 == '\\' && i + 1 < pattern_len) {
                    c1 = (unsigned char)pattern[i + 1];
                    i += 2;
                } else {
                    i++;
                }

                if (i < pattern_len - 1 && pattern[i] == '-' && pattern[i + 1] != ']') {
                    unsigned char c2 = (unsigned char)pattern[i + 1];
                    if (c2 == '\\' && i + 2 < pattern_len) {
                        c2 = (unsigned char)pattern[i + 2];
                        i += 2;
                    } else {
                        i += 1;
                    }
                    for (unsigned char ch = c1; ch <= c2; ch++) {
                        temp_set[ch] = true;
                    }
                    i++; /* consume end char */
                } else {
                    temp_set[c1] = true;
                }
            }
            if (i < pattern_len && pattern[i] == ']') {
                i++; /* skip ']' */
            }

            if (negate) {
                for (int ch = 0; ch < 256; ch++) {
                    char_set[ch] = !temp_set[ch];
                }
            } else {
                memcpy(char_set, temp_set, sizeof(temp_set));
            }
        } else if (pattern[i] == '.') {
            /* Wildcard */
            for (int ch = 0; ch < 256; ch++) {
                char_set[ch] = true;
            }
            i++;
        } else {
            /* Literal character */
            char_set[(unsigned char)pattern[i]] = true;
            i++;
        }

        /* Look ahead for quantifier */
        char quant = '\0';
        if (i < pattern_len && (pattern[i] == '*' || pattern[i] == '+' || pattern[i] == '?')) {
            quant = pattern[i];
            i++;
        }

        /* Create next DFA state */
        DFAState* next = rift_dfa_create_state(state_id++, false);
        if (!next) {
            while (skip_list) {
                SkipNode* tmp = skip_list->next;
                free(skip_list);
                skip_list = tmp;
            }
            return false;
        }

        /* Helper to add transitions for a character set */
        for (int ch = 0; ch < 256; ch++) {
            if (char_set[ch]) {
                if (!rift_dfa_add_transition(current, next, (char)ch)) {
                    rift_dfa_destroy_states(next);
                    while (skip_list) {
                        SkipNode* tmp = skip_list->next;
                        free(skip_list);
                        skip_list = tmp;
                    }
                    return false;
                }
                for (SkipNode* sk = skip_list; sk; sk = sk->next) {
                    rift_dfa_add_transition(sk->from, next, (char)ch);
                }
                if (quant == '*' || quant == '+') {
                    rift_dfa_add_transition(next, next, (char)ch);
                }
            }
        }

        /* Update skip list for optional segments */
        if (quant == '*' || quant == '?') {
            SkipNode* node = malloc(sizeof(SkipNode));
            if (!node) {
                rift_dfa_destroy_states(next);
                while (skip_list) {
                    SkipNode* tmp = skip_list->next;
                    free(skip_list);
                    skip_list = tmp;
                }
                return false;
            }
            node->from = current;
            node->next = skip_list;
            skip_list = node;
        } else {
            while (skip_list) {
                SkipNode* tmp = skip_list->next;
                free(skip_list);
                skip_list = tmp;
            }
        }

        current = next;
    }

    /* Mark accept states */
    current->is_final = true;
    regex->accept_states[regex->accept_count++] = current;
    while (skip_list) {
        skip_list->from->is_final = true;
        regex->accept_states[regex->accept_count++] = skip_list->from;
        SkipNode* tmp = skip_list->next;
        free(skip_list);
        skip_list = tmp;
    }

    return true;
}