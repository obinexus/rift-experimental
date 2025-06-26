#ifndef RIFT_CLI_COMMANDS_H
#define RIFT_CLI_COMMANDS_H

#include "rift/core/common.h"

#ifdef __cplusplus
extern "C" {
#endif

// CLI command function prototypes - stubs for build validation
int rift_cli_cmd_tokenize(int argc, char* argv[]);
int rift_cli_cmd_parse(int argc, char* argv[]);
int rift_cli_cmd_analyze(int argc, char* argv[]);
int rift_cli_cmd_validate(int argc, char* argv[]);
int rift_cli_cmd_generate(int argc, char* argv[]);
int rift_cli_cmd_verify(int argc, char* argv[]);
int rift_cli_cmd_emit(int argc, char* argv[]);
int rift_cli_cmd_compile(int argc, char* argv[]);
int rift_cli_cmd_governance(int argc, char* argv[]);

#ifdef __cplusplus
}
#endif

#endif /* RIFT_CLI_COMMANDS_H */
