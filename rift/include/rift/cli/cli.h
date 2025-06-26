/*
 * cli.h - RIFT Command Line Interface
 * RIFT: RIFT Is a Flexible Translator
 * OBINexus Computing Framework
 */

#ifndef RIFT_CLI_H
#define RIFT_CLI_H

#include "rift/rift.h"

#ifdef __cplusplus
extern "C" {
#endif

/* CLI option structure */
typedef struct rift_cli_options {
    char *input_file;
    char *output_file;
    char *config_file;
    bool verbose;
    bool debug;
    bool bottom_up;
    bool top_down;
    uint32_t threads;
    char *architecture;
} rift_cli_options_t;

/* CLI functions */
int rift_cli_parse_args(int argc, char **argv, rift_cli_options_t *options);
int rift_cli_execute(rift_cli_options_t *options);
void rift_cli_print_help(void);
void rift_cli_print_version(void);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CLI_H */
