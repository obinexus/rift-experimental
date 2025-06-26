// ===== src/core/rift_ast.c =====
#include "rift1/core/rift_ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

RiftASTNode *rift_ast_node_create(RiftASTNodeType type, const char *value) {
  RiftASTNode *node = calloc(1, sizeof(RiftASTNode));
  if (!node)
    return NULL;

  node->node_type = type;
  if (value) {
    node->value = strdup(value);
    if (!node->value) {
      free(node);
      return NULL;
    }
  }

  node->child_capacity = 4;
  node->children = calloc(node->child_capacity, sizeof(RiftASTNode *));
  if (!node->children) {
    free(node->value);
    free(node);
    return NULL;
  }

  return node;
}

void rift_ast_node_destroy(RiftASTNode *node) {
  if (!node)
    return;

  // Destroy all children
  for (size_t i = 0; i < node->child_count; ++i) {
    rift_ast_node_destroy(node->children[i]);
  }

  free(node->children);
  free(node->value);
  free(node);
}

RiftResult rift_ast_node_add_child(RiftASTNode *parent, RiftASTNode *child) {
  if (!parent || !child)
    return RIFT_ERROR_NULL_POINTER;

  // Resize children array if needed
  if (parent->child_count >= parent->child_capacity) {
    size_t new_capacity = parent->child_capacity * 2;
    RiftASTNode **new_children =
        realloc(parent->children, new_capacity * sizeof(RiftASTNode *));
    if (!new_children)
      return RIFT_ERROR_MEMORY_ALLOCATION;

    parent->children = new_children;
    parent->child_capacity = new_capacity;
  }

  parent->children[parent->child_count++] = child;
  child->parent = parent;
  child->depth = parent->depth + 1;

  return RIFT_SUCCESS;
}

RiftResult rift_ast_node_remove_child(RiftASTNode *parent, size_t index) {
  if (!parent || index >= parent->child_count)
    return RIFT_ERROR_INVALID_INPUT;

  // Destroy the child
  rift_ast_node_destroy(parent->children[index]);

  // Shift remaining children
  for (size_t i = index; i < parent->child_count - 1; ++i) {
    parent->children[i] = parent->children[i + 1];
  }

  parent->child_count--;
  return RIFT_SUCCESS;
}

void rift_ast_print(RiftASTNode *root, int indent) {
  if (!root)
    return;

  // Print indentation
  for (int i = 0; i < indent; ++i) {
    printf("  ");
  }

  // Print node info
  printf("AST[%d]: %s\n", root->node_type,
         root->value ? root->value : "(null)");

  // Print children
  for (size_t i = 0; i < root->child_count; ++i) {
    rift_ast_print(root->children[i], indent + 1);
  }
}

// ===== src/core/rift_automaton.c =====
#include "rift1/core/rift_automaton.h"
#include <stdlib.h>
#include <string.h>

RiftAutomaton *rift_automaton_create(void) {
  RiftAutomaton *automaton = calloc(1, sizeof(RiftAutomaton));
  if (!automaton)
    return NULL;

  automaton->state_capacity = 16;
  automaton->states = calloc(automaton->state_capacity, sizeof(RiftState *));
  if (!automaton->states) {
    free(automaton);
    return NULL;
  }

  return automaton;
}

void rift_automaton_destroy(RiftAutomaton *automaton) {
  if (!automaton)
    return;

  for (size_t i = 0; i < automaton->state_count; ++i) {
    rift_state_destroy(automaton->states[i]);
  }

  free(automaton->states);
  free(automaton);
}

RiftResult rift_automaton_add_state(RiftAutomaton *automaton,
                                    RiftState *state) {
  if (!automaton || !state)
    return RIFT_ERROR_NULL_POINTER;

  // Resize if needed
  if (automaton->state_count >= automaton->state_capacity) {
    size_t new_capacity = automaton->state_capacity * 2;
    RiftState **new_states =
        realloc(automaton->states, new_capacity * sizeof(RiftState *));
    if (!new_states)
      return RIFT_ERROR_MEMORY_ALLOCATION;

    automaton->states = new_states;
    automaton->state_capacity = new_capacity;
  }

  automaton->states[automaton->state_count++] = state;
  state->state_id = automaton->state_count - 1;

  if (state->is_initial) {
    automaton->initial_state = state;
    automaton->current_state = state;
  }

  return RIFT_SUCCESS;
}

RiftState *rift_state_create(const char *name, const char *pattern) {
  RiftState *state = calloc(1, sizeof(RiftState));
  if (!state)
    return NULL;

  if (name) {
    state->name = strdup(name);
    if (!state->name) {
      free(state);
      return NULL;
    }
  }

  if (pattern) {
    state->pattern = strdup(pattern);
    if (!state->pattern) {
      free(state->name);
      free(state);
      return NULL;
    }
  }

  return state;
}

void rift_state_destroy(RiftState *state) {
  if (!state)
    return;

  free(state->name);
  free(state->pattern);
  free(state->transitions);
  free(state);
}

RiftPattern *rift_pattern_create(const char *pattern, RiftTokenType type) {
  RiftPattern *pattern_obj = calloc(1, sizeof(RiftPattern));
  if (!pattern_obj)
    return NULL;

  if (pattern) {
    pattern_obj->pattern_string = strdup(pattern);
    if (!pattern_obj->pattern_string) {
      free(pattern_obj);
      return NULL;
    }

    // Compile regex
    int result = regcomp(&pattern_obj->compiled_regex, pattern,
                         REG_EXTENDED | REG_NOSUB);
    if (result != 0) {
      free(pattern_obj->pattern_string);
      free(pattern_obj);
      return NULL;
    }
  }

  pattern_obj->target_type = type;
  return pattern_obj;
}

void rift_pattern_destroy(RiftPattern *pattern) {
  if (!pattern)
    return;

  free(pattern->pattern_string);
  regfree(&pattern->compiled_regex);
  free(pattern);
}

// ===== src/core/rift_token_memory.c =====
#include "rift1/core/rift_token_memory.h"
#include <stdlib.h>
#include <string.h>

RiftTokenMemory *rift_token_memory_create(void) {
  RiftTokenMemory *memory = calloc(1, sizeof(RiftTokenMemory));
  if (!memory)
    return NULL;

  memory->token_capacity = 64;
  memory->tokens = calloc(memory->token_capacity, sizeof(RiftToken *));
  if (!memory->tokens) {
    free(memory);
    return NULL;
  }

  return memory;
}

void rift_token_memory_destroy(RiftTokenMemory *memory) {
  if (!memory)
    return;

  for (size_t i = 0; i < memory->token_count; ++i) {
    rift_token_destroy(memory->tokens[i]);
  }

  free(memory->tokens);
  free(memory->source_file);
  free(memory->stage_identifier);
  free(memory);
}

RiftResult rift_token_memory_add_token(RiftTokenMemory *memory,
                                       RiftToken *token) {
  if (!memory || !token)
    return RIFT_ERROR_NULL_POINTER;

  // Resize if needed
  if (memory->token_count >= memory->token_capacity) {
    size_t new_capacity = memory->token_capacity * 2;
    RiftToken **new_tokens =
        realloc(memory->tokens, new_capacity * sizeof(RiftToken *));
    if (!new_tokens)
      return RIFT_ERROR_MEMORY_ALLOCATION;

    memory->tokens = new_tokens;
    memory->token_capacity = new_capacity;
  }

  memory->tokens[memory->token_count++] = token;
  return RIFT_SUCCESS;
}

RiftToken *rift_token_create(const char *type, const char *value,
                             const char *lexeme) {
  RiftToken *token = calloc(1, sizeof(RiftToken));
  if (!token)
    return NULL;

  if (type) {
    token->type = strdup(type);
    if (!token->type)
      goto error;
  }

  if (value) {
    token->value = strdup(value);
    if (!token->value)
      goto error;
  }

  if (lexeme) {
    token->lexeme = strdup(lexeme);
    if (!token->lexeme)
      goto error;
  }

  token->type_enum = RIFT_TOKEN_UNKNOWN;
  return token;

error:
  rift_token_destroy(token);
  return NULL;
}

void rift_token_destroy(RiftToken *token) {
  if (!token)
    return;

  free(token->type);
  free(token->value);
  free(token->lexeme);
  free(token);
}

RiftStackFrame *rift_stack_frame_create(void) {
  RiftStackFrame *frame = calloc(1, sizeof(RiftStackFrame));
  return frame;
}

void rift_stack_frame_destroy(RiftStackFrame *frame) {
  if (!frame)
    return;

  // Note: Don't destroy token/node here as they're managed elsewhere
  free(frame);
}

// ===== src/core/rift_ir.c =====
#include "rift1/core/rift_ir.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

RiftIR *rift_ir_create(const char *source_file) {
  RiftIR *ir = calloc(1, sizeof(RiftIR));
  if (!ir)
    return NULL;

  if (source_file) {
    ir->source_file = strdup(source_file);
    if (!ir->source_file) {
      free(ir);
      return NULL;
    }
  }

  return ir;
}

void rift_ir_destroy(RiftIR *ir) {
  if (!ir)
    return;

  free(ir->source_file);
  free(ir->stage);
  free(ir->metadata);

  // Note: tokens and AST are managed by other components
  free(ir);
}

RiftResult rift_ir_read_file(RiftIR *ir, const char *filename) {
  if (!ir || !filename)
    return RIFT_ERROR_NULL_POINTER;

  FILE *file = fopen(filename, "r");
  if (!file)
    return RIFT_ERROR_FILE_NOT_FOUND;

  // TODO: Implement actual IR file reading
  // For now, this is a stub that sets basic properties

  fclose(file);
  return RIFT_SUCCESS;
}

RiftResult rift_ir_write_file(RiftIR *ir, const char *filename) {
  if (!ir || !filename)
    return RIFT_ERROR_NULL_POINTER;

  FILE *file = fopen(filename, "w");
  if (!file)
    return RIFT_ERROR_FILE_NOT_FOUND;

  // Write IR header
  fprintf(file, "# RIFT IR File\n");
  fprintf(file, "# Stage: %s\n", ir->stage ? ir->stage : "unknown");
  fprintf(file, "# Source: %s\n",
          ir->source_file ? ir->source_file : "unknown");
  fprintf(file, "\n");

  // Write tokens if available
  if (ir->tokens && ir->token_count > 0) {
    fprintf(file, "[TOKENS]\n");
    for (size_t i = 0; i < ir->token_count; ++i) {
      RiftToken *token = ir->tokens[i];
      fprintf(file, "%s|%s|%s\n", token->type ? token->type : "",
              token->value ? token->value : "",
              token->lexeme ? token->lexeme : "");
    }
    fprintf(file, "\n");
  }

  // Write AST if available
  if (ir->ast_root) {
    fprintf(file, "[AST]\n");
    rift_ir_write_ast_node(file, ir->ast_root, 0);
    fprintf(file, "\n");
  }

  fclose(file);
  return RIFT_SUCCESS;
}

void rift_ir_write_ast_node(FILE *file, RiftASTNode *node, int indent) {
  if (!file || !node)
    return;

  // Write indentation
  for (int i = 0; i < indent; ++i) {
    fprintf(file, "  ");
  }

  // Write node
  fprintf(file, "node:%d:%s\n", node->node_type,
          node->value ? node->value : "");

  // Write children
  for (size_t i = 0; i < node->child_count; ++i) {
    rift_ir_write_ast_node(file, node->children[i], indent + 1);
  }
}

// ===== src/core/rift_core_utils.c =====
#include "rift1/core/rift_types.h"
#include <stdio.h>
#include <string.h>

const char *rift_result_to_string(RiftResult result) {
  switch (result) {
  case RIFT_SUCCESS:
    return "SUCCESS";
  case RIFT_ERROR_NULL_POINTER:
    return "ERROR_NULL_POINTER";
  case RIFT_ERROR_MEMORY_ALLOCATION:
    return "ERROR_MEMORY_ALLOCATION";
  case RIFT_ERROR_FILE_NOT_FOUND:
    return "ERROR_FILE_NOT_FOUND";
  case RIFT_ERROR_INVALID_INPUT:
    return "ERROR_INVALID_INPUT";
  case RIFT_ERROR_PARSE_FAILED:
    return "ERROR_PARSE_FAILED";
  case RIFT_ERROR_PARSE_INCOMPLETE:
    return "ERROR_PARSE_INCOMPLETE";
  case RIFT_ERROR_PATTERN_INVALID:
    return "ERROR_PATTERN_INVALID";
  case RIFT_ERROR_INVALID_PARSE_MODE:
    return "ERROR_INVALID_PARSE_MODE";
  case RIFT_ERROR_PARSING_INCOMPLETE:
    return "ERROR_PARSING_INCOMPLETE";
  default:
    return "UNKNOWN_ERROR";
  }
}

const char *rift_token_type_to_string(RiftTokenType type) {
  switch (type) {
  case RIFT_TOKEN_UNKNOWN:
    return "UNKNOWN";
  case RIFT_TOKEN_IDENTIFIER:
    return "IDENTIFIER";
  case RIFT_TOKEN_INTEGER:
    return "INTEGER";
  case RIFT_TOKEN_FLOAT:
    return "FLOAT";
  case RIFT_TOKEN_STRING:
    return "STRING";
  case RIFT_TOKEN_CHAR:
    return "CHAR";
  case RIFT_TOKEN_OPERATOR:
    return "OPERATOR";
  case RIFT_TOKEN_DELIMITER:
    return "DELIMITER";
  case RIFT_TOKEN_SEPARATOR:
    return "SEPARATOR";
  case RIFT_TOKEN_KEYWORD:
    return "KEYWORD";
  case RIFT_TOKEN_WHITESPACE:
    return "WHITESPACE";
  case RIFT_TOKEN_EOF:
    return "EOF";
  default:
    return "INVALID";
  }
}

void rift_debug_print_token(RiftToken *token) {
  if (!token) {
    printf("Token: (null)\n");
    return;
  }

  printf(
      "Token: type='%s' value='%s' lexeme='%s' type_enum=%s line=%zu col=%zu\n",
      token->type ? token->type : "(null)",
      token->value ? token->value : "(null)",
      token->lexeme ? token->lexeme : "(null)",
      rift_token_type_to_string(token->type_enum), token->line, token->column);
}

// ===== Missing Parser Strategy Function Stubs =====

RiftResult rift1_attempt_reductions(Rift1ParseEngine *engine) {
  if (!engine)
    return RIFT_ERROR_NULL_POINTER;

  // TODO: Implement reduction pattern matching
  // This is where bottom-up parser checks for grammar rule reductions

  return RIFT_SUCCESS;
}

RiftASTNode *rift1_parse_keyword_statement(Rift1ParseEngine *engine) {
  if (!engine || engine->current_position >= engine->token_count)
    return NULL;

  RiftToken *keyword_token = engine->input_tokens[engine->current_position++];

  // Create statement node based on keyword
  RiftASTNode *statement =
      rift_ast_node_create(AST_NODE_STATEMENT, keyword_token->lexeme);
  statement->source_token = keyword_token;

  return statement;
}

RiftASTNode *rift1_parse_assignment_or_call(Rift1ParseEngine *engine) {
  if (!engine || engine->current_position >= engine->token_count)
    return NULL;

  RiftToken *identifier_token =
      engine->input_tokens[engine->current_position++];

  // Create expression node for identifier
  RiftASTNode *expression =
      rift_ast_node_create(AST_NODE_IDENTIFIER, identifier_token->lexeme);
  expression->source_token = identifier_token;

  return expression;
}

RiftASTNode *rift1_parse_expression_statement(Rift1ParseEngine *engine) {
  if (!engine || engine->current_position >= engine->token_count)
    return NULL;

  RiftToken *token = engine->input_tokens[engine->current_position++];

  // Create terminal expression node
  RiftASTNode *expression =
      rift_ast_node_create(AST_NODE_EXPRESSION, token->lexeme);
  expression->source_token = token;

  return expression;
}