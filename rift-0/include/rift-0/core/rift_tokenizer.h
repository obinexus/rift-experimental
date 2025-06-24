#ifndef RIFT_TOKENIZER_INTEGRATION_H
#define RIFT_TOKENIZER_INTEGRATION_H

#include "tokenizer.h"

#ifdef __cplusplus
extern "C" {
#endif

/* RIFT language-specific token types */
typedef enum rift_lang_token_type {
    RIFT_LANG_TOKEN_MODULE = RIFT_TOKEN_USER_DEFINED,
    RIFT_LANG_TOKEN_IMPORT,
    RIFT_LANG_TOKEN_EXPORT,
    RIFT_LANG_TOKEN_ASYNC,
    RIFT_LANG_TOKEN_AWAIT,
    RIFT_LANG_TOKEN_CHANNEL,
    RIFT_LANG_TOKEN_SELECT,
    RIFT_LANG_TOKEN_GOSILANG_BLOCK,
    RIFT_LANG_TOKEN_AEGIS_ANNOTATION
} rift_lang_token_type_t;

/* OBINexus toolchain integration */
typedef struct rift_toolchain_config {
    const char *riftlang_path;      /* Path to riftlang.exe */
    const char *output_format;       /* .so.a generation settings */
    const char *gosilang_interface;  /* Gosilang integration config */
    uint32_t    optimization_level;
    bool        debug_symbols;
} rift_toolchain_config_t;

/* Polybuild integration hooks */
typedef struct rift_build_hooks {
    int (*pre_tokenize)(void *context, const char *source);
    int (*post_tokenize)(void *context, const rift_token_buffer_t *tokens);
    int (*on_error)(void *context, const rift_tokenizer_error_t *error);
    void *hook_context;
} rift_build_hooks_t;

/* High-level RIFT tokenizer interface */
typedef struct rift_tokenizer {
    rift_tokenizer_context_t    base;
    rift_toolchain_config_t     toolchain;
    rift_build_hooks_t          build_hooks;
    /* Language-specific rule set */
    rift_rule_set_t            *lang_rules;
} rift_tokenizer_t;

/* RIFT tokenizer initialization */
int rift_tokenizer_create(rift_tokenizer_t **tokenizer,
                          const rift_toolchain_config_t *config);
int rift_tokenizer_load_default_rules(rift_tokenizer_t *tokenizer);
int rift_tokenizer_load_custom_rules(rift_tokenizer_t *tokenizer,
                                     const char *rules_file);
void rift_tokenizer_free(rift_tokenizer_t *tokenizer);

/* Toolchain integration */
int rift_tokenizer_process_file(rift_tokenizer_t *tokenizer,
                                const char *input_file,
                                const char *output_file);
int rift_tokenizer_integrate_nlink(rift_tokenizer_t *tokenizer,
                                   void *nlink_context);
int rift_tokenizer_export_tokens_soa(const rift_tokenizer_t *tokenizer,
                                     void **soa_buffer, size_t *size);

/* Gosilang interop */
int rift_tokenizer_enable_gosilang(rift_tokenizer_t *tokenizer);
int rift_tokenizer_parse_gosilang_block(rift_tokenizer_t *tokenizer,
                                        const rift_token_t *start_token,
                                        rift_token_buffer_t *output);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_TOKENIZER_INTEGRATION_H */