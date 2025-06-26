/*
 * main.c - RIFT Compiler Main Entry Point
 * RIFT: RIFT Is a Flexible Translator
 * OBINexus Computing Framework
 */

#include "rift/cli/cli.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
    rift_cli_options_t options;
    
    int parse_result = rift_cli_parse_args(argc, argv, &options);
    if (parse_result != 0) {
        return parse_result > 0 ? 0 : 1;
    }
    
    int result = rift_cli_execute(&options);
    
    // Cleanup
    free(options.input_file);
    free(options.output_file);
    free(options.config_file);
    free(options.architecture);
    
    return result;
}
