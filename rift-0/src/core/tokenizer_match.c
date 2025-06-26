#include "rift-0/core/tokenizer_rules.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#ifndef HAVE_STRDUP
static char* dup_string(const char* s) {
    size_t l = strlen(s) + 1;
    char* n = malloc(l);
    if (n) memcpy(n, s, l);
    return n;
}
#define strdup dup_string
#endif

/* Simple DFA state machine for basic R pattern checks */
struct DFAStateMachine {
    int state;
};

#define DFA_STATE_INIT 0
#define DFA_STATE_R 1
#define DFA_STATE_QUOTE 2
#define DFA_STATE_CONTENT 3
#define DFA_STATE_ACCEPT 4
#define DFA_STATE_ERROR -1

DFAStateMachine* dfa_create(void) {
    DFAStateMachine* m = malloc(sizeof(DFAStateMachine));
    if (m) m->state = DFA_STATE_INIT;
    return m;
}

bool dfa_process_char(DFAStateMachine* dfa, char c) {
    if (!dfa || dfa->state == DFA_STATE_ERROR) return false;
    switch (dfa->state) {
        case DFA_STATE_INIT:
            if (c == 'R') dfa->state = DFA_STATE_R;
            else dfa->state = DFA_STATE_ERROR;
            break;
        case DFA_STATE_R:
            if (c == '"' || c == '\'') { dfa->state = DFA_STATE_QUOTE; }
            else { dfa->state = DFA_STATE_ERROR; }
            break;
        case DFA_STATE_QUOTE:
            if (c == '"' || c == '\'') { dfa->state = DFA_STATE_ACCEPT; }
            else { dfa->state = DFA_STATE_CONTENT; }
            break;
        case DFA_STATE_CONTENT:
            if (c == '"' || c == '\'') dfa->state = DFA_STATE_ACCEPT;
            break;
        case DFA_STATE_ACCEPT:
            dfa->state = DFA_STATE_ERROR;
            break;
        default:
            return false;
    }
    return dfa->state != DFA_STATE_ERROR;
}

bool dfa_is_accepting(DFAStateMachine* dfa) {
    return dfa && dfa->state == DFA_STATE_ACCEPT;
}

void dfa_reset(DFAStateMachine* dfa) {
    if (dfa) dfa->state = DFA_STATE_INIT;
}

void dfa_destroy(DFAStateMachine* dfa) {
    free(dfa);
}

/* ------------------------------------------------------------------ */

TokenTriplet R_encode_safe(TokenType type, uint16_t mem_ptr, uint8_t value) {
    return rift_token_create((uint8_t)type, mem_ptr, value);
}

bool validate_token_separation(const TokenTriplet* token) {
    if (!token) return false;
    if (token->type >= TOKEN_ERROR) return false;
    return true;
}

TokenType classify_null_nil_semantic(const char* text, size_t len) {
    if (!text) return TOKEN_UNKNOWN;
    if (len == 4 && strncasecmp(text, "NULL", 4) == 0) return TOKEN_NULL_KEYWORD;
    if (len == 3 && strncasecmp(text, "nil", 3) == 0) return TOKEN_NIL_KEYWORD;
    return TOKEN_UNKNOWN;
}


static int simple_match(const char* src, TokenTriplet* token) {
    if (!src || !token) return -1;
    const char* p = src;
    if (isalpha((unsigned char)*p) || *p == '_') {
        size_t len = 1;
        while (isalnum((unsigned char)p[len]) || p[len] == '_') len++;
        *token = rift_token_create(TOKEN_IDENTIFIER, 0, (uint8_t)len);
        if (classify_null_nil_semantic(p, len) != TOKEN_UNKNOWN)
            token->type = classify_null_nil_semantic(p, len);
        return (int)len;
    } else if (isdigit((unsigned char)*p)) {
        size_t len = 1;
        while (isdigit((unsigned char)p[len])) len++;
        *token = rift_token_create(TOKEN_LITERAL_NUMBER, 0, (uint8_t)len);
        return (int)len;
    } else if (*p == '"') {
        size_t len = 1;
        while (p[len] && p[len] != '"') len++;
        if (p[len] == '"') len++;
        *token = rift_token_create(TOKEN_LITERAL_STRING, 0, (uint8_t)len);
        return (int)len;
    } else if (*p == 'R' && (p[1] == '"' || p[1] == '\'')) {
        char q = p[1];
        size_t i = 2;
        while (p[i] && p[i] != q) i++;
        if (p[i] == q) i++;
        *token = rift_token_create(TOKEN_R_PATTERN, 0, (uint8_t)i);
        return (int)i;
    } else if (strchr("+-*/=<>!", *p)) {
        *token = rift_token_create(TOKEN_OPERATOR, 0, 1);
        return 1;
    } else if (strchr("(){}[]", *p)) {
        *token = rift_token_create(TOKEN_DELIMITER, 0, 1);
        return 1;
    }
    *token = rift_token_create(TOKEN_UNKNOWN, 0, 1);
    return 1;
}

int match_token_pattern(const char* src, TokenTriplet* out_token) {
    if (!src || !out_token) return -1;
    return simple_match(src, out_token);
}

int match_token_pattern_ex(const char* src, const char* pattern, uint32_t flags, PatternMatchResult* result) {
    if (!src || !result) return -1;
    TokenTriplet t;
    int len = simple_match(src, &t);
    result->token = t;
    result->match_length = len;
    result->success = len > 0;
    result->error_msg = NULL;
    (void)pattern;
    (void)flags;
    return (len > 0) ? 0 : -1;
}

TokenizationResult tokenize_source(const char* src, size_t length) {
    TokenizationResult res = {0};
    if (!src || length == 0) {
        res.success = false;
        res.error_message = strdup("empty input");
        return res;
    }
    res.tokens = malloc(length * sizeof(TokenTriplet));
    if (!res.tokens) {
        res.success = false;
        res.error_message = strdup("alloc fail");
        return res;
    }
    size_t pos = 0;
    while (pos < length) {
        TokenTriplet t;
        int consumed = simple_match(src + pos, &t);
        if (consumed <= 0) break;
        t.mem_ptr = (uint16_t)pos;
        res.tokens[res.count++] = t;
        pos += consumed;
    }
    res.success = true;
    return res;
}

void free_tokenization_result(TokenizationResult* result) {
    if (!result) return;
    free(result->tokens);
    free(result->error_message);
    result->tokens = NULL;
    result->error_message = NULL;
    result->count = 0;
    result->success = false;
}

int tokenize_source_into(const char* src, TokenTriplet* tokens, size_t max_tokens, size_t* token_count) {
    if (!src || !tokens || !token_count) return -1;
    size_t len = strlen(src);
    size_t pos = 0; size_t count = 0;
    while (pos < len && count < max_tokens) {
        TokenTriplet t; int c = simple_match(src + pos, &t);
        if (c <= 0) break;
        t.mem_ptr = (uint16_t)pos;
        tokens[count++] = t;
        pos += c;
    }
    *token_count = count;
    return 0;
}

int init_tokenizer_rules(void) { return 0; }
void cleanup_tokenizer_rules(void) {}
