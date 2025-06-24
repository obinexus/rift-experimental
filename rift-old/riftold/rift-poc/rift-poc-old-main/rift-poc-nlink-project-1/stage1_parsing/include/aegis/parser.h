#ifndef AEGIS_PARSER_H
#define AEGIS_PARSER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct ParserContext ParserContext;

ParserContext* parser_create_context(bool zero_trust_mode);
bool parser_process_file(ParserContext* ctx, const char* input_file, const char* output_file);
void parser_free_context(ParserContext* ctx);

#endif
