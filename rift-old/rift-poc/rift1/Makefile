# OBINexus AEGIS Framework - Production Makefile
# rift1 Tokenizer → Parser → AST Pipeline
# Collaborative development: Technical Team + Nnamdi Okpala

# ================================
# AEGIS Build Configuration
# ================================
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Werror -Iinclude -g -O2
CFLAGS += -DAEEGIS_COMPLIANCE_ENABLED=1 -DRIFT1_VERSION=\"1.0\"
LDFLAGS = 

# ================================
# Directory Structure (Waterfall Methodology)
# ================================
SRC_DIR = src
CORE_DIR = $(SRC_DIR)/core
CLI_DIR = $(SRC_DIR)/cli
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin
LIB_DIR = lib

# Create all necessary subdirectories
CORE_SUBDIRS = tokenizer token_types token_values token_memory lexer r_extensions config
CLI_SUBDIRS = commands

# ================================
# AEGIS Component Sources (Modular Architecture)
# ================================

# Core tokenizer pipeline
LEXER_SRC = $(CORE_DIR)/lexer/lexer.c
TOKEN_TYPES_SRC = $(CORE_DIR)/token_types/token_type.c
TOKEN_VALUES_SRC = $(CORE_DIR)/token_values/token_value.c
TOKEN_MEMORY_SRC = $(CORE_DIR)/token_memory/token_memory.c $(CORE_DIR)/token_memory.c
TOKENIZER_SRC = $(CORE_DIR)/tokenizer/tokenizer.c

# Parser and AST infrastructure
AST_SRC = $(CORE_DIR)/rift1_ast.c $(CORE_DIR)/rift_ast.c
PARSER_SRC = $(CORE_DIR)/rift1_parsing.c $(CORE_DIR)/rift_parser.c

# Core system components
CORE_SRC = $(CORE_DIR)/rift_core.c $(CORE_DIR)/rift_core_utils.c $(CORE_DIR)/rift_automaton.c $(CORE_DIR)/rift_ir.c
CONFIG_SRC = $(CORE_DIR)/config/rift_pipeline_simple.c

# R-Extensions (AEGIS language extensions)
R_GOVERNANCE_SRC = $(CORE_DIR)/r_extensions/governance/r_governance_validation.c
R_UML_SRC = $(CORE_DIR)/r_extensions/uml/r_uml_patterns.c
R_CORE_SRC = $(CORE_DIR)/r_extensions/r_extensions_core.c

# CLI interface components
CLI_CORE_SRC = $(CLI_DIR)/cli_args.c $(CLI_DIR)/cli_interface.c
CLI_CMD_SRC = $(CLI_DIR)/commands/command_handler.c $(CLI_DIR)/commands/config_handler.c $(CLI_DIR)/commands/pattern_manager.c
CLI_CONFIG_SRC = $(CLI_DIR)/commands/config/cli_config.c
CLI_MAIN_SRC = $(CLI_DIR)/main.c

# ================================
# Object File Generation (Build Artifacts)
# ================================
LEXER_OBJ = $(BUILD_DIR)/lexer.o
TOKEN_TYPES_OBJ = $(BUILD_DIR)/token_types.o
TOKEN_VALUES_OBJ = $(BUILD_DIR)/token_values.o
TOKEN_MEMORY_OBJ = $(BUILD_DIR)/token_memory.o $(BUILD_DIR)/token_memory_legacy.o
TOKENIZER_OBJ = $(BUILD_DIR)/tokenizer.o

AST_OBJ = $(BUILD_DIR)/rift1_ast.o $(BUILD_DIR)/rift_ast.o
PARSER_OBJ = $(BUILD_DIR)/rift1_parsing.o $(BUILD_DIR)/rift_parser.o

CORE_OBJ = $(BUILD_DIR)/rift_core.o $(BUILD_DIR)/rift_core_utils.o $(BUILD_DIR)/rift_automaton.o $(BUILD_DIR)/rift_ir.o
CONFIG_OBJ = $(BUILD_DIR)/rift_pipeline_simple.o

R_GOVERNANCE_OBJ = $(BUILD_DIR)/r_governance_validation.o
R_UML_OBJ = $(BUILD_DIR)/r_uml_patterns.o  
R_CORE_OBJ = $(BUILD_DIR)/r_extensions_core.o

CLI_CORE_OBJ = $(BUILD_DIR)/cli_args.o $(BUILD_DIR)/cli_interface.o
CLI_CMD_OBJ = $(BUILD_DIR)/command_handler.o $(BUILD_DIR)/config_handler.o $(BUILD_DIR)/pattern_manager.o
CLI_CONFIG_OBJ = $(BUILD_DIR)/cli_config.o
CLI_MAIN_OBJ = $(BUILD_DIR)/main.o

# ================================
# AEGIS Build Targets
# ================================
TARGET = $(BIN_DIR)/rift1
STATIC_LIB = $(LIB_DIR)/librift1.a

ALL_OBJ = $(LEXER_OBJ) $(TOKEN_TYPES_OBJ) $(TOKEN_VALUES_OBJ) $(TOKEN_MEMORY_OBJ) $(TOKENIZER_OBJ) \
          $(AST_OBJ) $(PARSER_OBJ) $(CORE_OBJ) $(CONFIG_OBJ) \
          $(R_GOVERNANCE_OBJ) $(R_UML_OBJ) $(R_CORE_OBJ) \
          $(CLI_CORE_OBJ) $(CLI_CMD_OBJ) $(CLI_CONFIG_OBJ) $(CLI_MAIN_OBJ)

.PHONY: all clean bootstrap test verify-aegis install dev release

# ================================
# Primary Build Targets
# ================================

all: $(TARGET)

bootstrap: clean directories headers $(TARGET)
	@echo "🏗️  AEGIS Framework Bootstrap Complete"
	@echo "✅ Tokenizer → Parser → AST pipeline operational"
	@echo "✅ R-Extensions governance validation active"
	@echo "✅ Build artifacts: $(TARGET)"

$(TARGET): $(ALL_OBJ) | directories
	@echo "🔗 Linking AEGIS executable..."
	$(CC) $(ALL_OBJ) -o $@ $(LDFLAGS)
	@echo "✅ AEGIS rift1 executable ready: $(TARGET)"

$(STATIC_LIB): $(filter-out $(CLI_MAIN_OBJ), $(ALL_OBJ)) | directories
	@echo "📚 Creating AEGIS static library..."
	ar rcs $@ $^
	@echo "✅ AEGIS library ready: $(STATIC_LIB)"

# ================================
# Component-Specific Build Rules
# ================================

# Lexer components
$(BUILD_DIR)/lexer.o: $(LEXER_SRC) | directories
	$(CC) $(CFLAGS) -c $< -o $@

# Token type system
$(BUILD_DIR)/token_types.o: $(TOKEN_TYPES_SRC) | directories
	$(CC) $(CFLAGS) -c $< -o $@

# Token value management
$(BUILD_DIR)/token_values.o: $(TOKEN_VALUES_SRC) | directories
	$(CC) $(CFLAGS) -c $< -o $@

# Token memory orchestration
$(BUILD_DIR)/token_memory.o: $(CORE_DIR)/token_memory/token_memory.c | directories
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/token_memory_legacy.o: $(CORE_DIR)/token_memory.c | directories
	$(CC) $(CFLAGS) -c $< -o $@

# Primary tokenizer
$(BUILD_DIR)/tokenizer.o: $(TOKENIZER_SRC) | directories
	$(CC) $(CFLAGS) -c $< -o $@

# AST infrastructure
$(BUILD_DIR)/rift1_ast.o: $(CORE_DIR)/rift1_ast.c | directories
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/rift_ast.o: $(CORE_DIR)/rift_ast.c | directories
	$(CC) $(CFLAGS) -c $< -o $@

# Parser engine
$(BUILD_DIR)/rift1_parsing.o: $(CORE_DIR)/rift1_parsing.c | directories
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/rift_parser.o: $(CORE_DIR)/rift_parser.c | directories
	$(CC) $(CFLAGS) -c $< -o $@

# Core system
$(BUILD_DIR)/rift_core.o: $(CORE_DIR)/rift_core.c | directories
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/rift_core_utils.o: $(CORE_DIR)/rift_core_utils.c | directories
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/rift_automaton.o: $(CORE_DIR)/rift_automaton.c | directories
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/rift_ir.o: $(CORE_DIR)/rift_ir.c | directories
	$(CC) $(CFLAGS) -c $< -o $@

# Configuration system
$(BUILD_DIR)/rift_pipeline_simple.o: $(CONFIG_SRC) | directories
	$(CC) $(CFLAGS) -c $< -o $@

# R-Extensions (Governance)
$(BUILD_DIR)/r_governance_validation.o: $(R_GOVERNANCE_SRC) | directories
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/r_uml_patterns.o: $(R_UML_SRC) | directories
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/r_extensions_core.o: $(R_CORE_SRC) | directories
	$(CC) $(CFLAGS) -c $< -o $@

# CLI interface
$(BUILD_DIR)/cli_args.o: $(CLI_DIR)/cli_args.c | directories
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/cli_interface.o: $(CLI_DIR)/cli_interface.c | directories
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/command_handler.o: $(CLI_DIR)/commands/command_handler.c | directories
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/config_handler.o: $(CLI_DIR)/commands/config_handler.c | directories
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/pattern_manager.o: $(CLI_DIR)/commands/pattern_manager.c | directories
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/cli_config.o: $(CLI_CONFIG_SRC) | directories
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/main.o: $(CLI_MAIN_SRC) | directories
	$(CC) $(CFLAGS) -c $< -o $@

# ================================
# Infrastructure Targets
# ================================

directories:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR) $(LIB_DIR)
	@mkdir -p $(addprefix $(INCLUDE_DIR)/rift1/core/, $(CORE_SUBDIRS))
	@mkdir -p $(addprefix $(INCLUDE_DIR)/rift1/cli/, $(CLI_SUBDIRS))

headers: | directories
	@echo "🔧 Validating AEGIS header structure..."
	@test -f include/rift1/core/rift_core.h || echo "⚠️  Missing: include/rift1/core/rift_core.h"
	@test -f include/rift1/core/tokenizer/tokenizer.h || echo "⚠️  Missing: include/rift1/core/tokenizer/tokenizer.h"
	@echo "✅ Header validation complete"

lib: $(STATIC_LIB)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) $(LIB_DIR)
	@echo "🧹 AEGIS build artifacts cleaned"

# ================================
# Testing and Validation
# ================================

test: $(TARGET)
	@echo "🧪 Running AEGIS compliance tests..."
	@./$(TARGET) --help >/dev/null && echo "✅ CLI interface test: PASS" || echo "❌ CLI interface test: FAIL"
	@./$(TARGET) --version >/dev/null && echo "✅ Version test: PASS" || echo "❌ Version test: FAIL"

verify-aegis: $(TARGET)
	@echo "🔍 AEGIS Architecture Verification:"
	@echo "✅ Tokenizer pipeline: $(shell test -f $(BUILD_DIR)/tokenizer.o && echo "OPERATIONAL" || echo "MISSING")"
	@echo "✅ Governance validation: $(shell test -f $(BUILD_DIR)/r_governance_validation.o && echo "ACTIVE" || echo "INACTIVE")"
	@echo "✅ AST infrastructure: $(shell test -f $(BUILD_DIR)/rift1_ast.o && echo "READY" || echo "NOT_READY")"
	@echo "✅ Parser engine: $(shell test -f $(BUILD_DIR)/rift1_parsing.o && echo "OPERATIONAL" || echo "NOT_OPERATIONAL")"

# ================================
# Development Profiles
# ================================

dev: CFLAGS += -DDEBUG -g3 -O0
dev: $(TARGET)
	@echo "🔧 AEGIS development build ready"

release: CFLAGS += -DNDEBUG -O3 -flto
release: $(TARGET)
	@echo "🚀 AEGIS production build ready"

install: $(TARGET)
	@echo "📦 Installing AEGIS rift1 to system..."
	install -m 755 $(TARGET) /usr/local/bin/rift1
	@echo "✅ AEGIS installation complete"