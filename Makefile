# =================================================================
# RIFT AEGIS Build System - Production Implementation with Cross-Platform Support
# OBINexus Computing Framework - Technical Lead: Nnamdi Michael Okpala
# VERSION: 1.2.0-PRODUCTION
# FIXES: Syntax errors, dependency tracking, pkg-config enforcement, hash lookup
# =================================================================

RIFT_VERSION := 1.2.0
BUILD_HASH_TABLE := build/stage_lookup.json

# Cross-platform compiler detection with pkg-config enforcement
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    CC := gcc
    PKG_CONFIG := pkg-config
    PLATFORM := linux
    LIB_EXT := .so
endif
ifeq ($(UNAME_S),Darwin)
    CC := clang
    PKG_CONFIG := pkg-config
    PLATFORM := macos
    LIB_EXT := .dylib
endif
ifeq ($(OS),Windows_NT)
    CC := gcc
    PKG_CONFIG := pkg-config.exe
    PLATFORM := windows
    LIB_EXT := .dll
endif

AR := ar
RANLIB := ranlib

# Directory structure with hash table optimization
SRC_DIR := rift/src
INCLUDE_DIR := rift/include
LIB_DIR := lib
BIN_DIR := bin
OBJ_DIR := obj
BUILD_DIR := build
HASH_DIR := $(BUILD_DIR)/hash_lookup

# AEGIS Compiler flags with dependency tracking and cross-platform optimization
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -MMD -MP
CFLAGS += -fstack-protector-strong -D_FORTIFY_SOURCE=2
CFLAGS += -DRIFT_AEGIS_COMPLIANCE=1 -DRIFT_VERSION_STRING=\"$(RIFT_VERSION)\"
CFLAGS += -DRIFT_PLATFORM=\"$(PLATFORM)\" -DRIFT_HASH_TABLE_ENABLED=1
CFLAGS += -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/rift/core

# Cross-platform pkg-config enforcement
ifeq ($(shell $(PKG_CONFIG) --version 2>/dev/null),)
    $(error pkg-config not found. Install pkg-config for your platform.)
endif

# Dynamic dependency detection via pkg-config
ifneq ($(shell $(PKG_CONFIG) --exists openssl && echo yes),yes)
    $(warning OpenSSL not found via pkg-config, using fallback)
    LIBS := -lssl -lcrypto -lpthread
else
    CFLAGS += $(shell $(PKG_CONFIG) --cflags openssl)
    LIBS := $(shell $(PKG_CONFIG) --libs openssl) -lpthread
endif

LDFLAGS := -Wl,-z,relro -Wl,-z,now

# Stage configuration with O(1) hash table lookup
STAGES := 0 1 2 3 4 5 6
STAGE_NAMES := tokenizer parser semantic validator bytecode verifier emitter

# CORRECTED: Proper library naming with librift-*.a convention
STATIC_LIBS := $(addprefix $(LIB_DIR)/lib,$(addsuffix .a,$(addprefix rift-,$(STAGES))))
EXECUTABLES := $(addprefix $(BIN_DIR)/,$(addprefix rift-,$(STAGES)))

# Hash table files for O(1) stage lookup
HASH_LOOKUP_FILES := $(addprefix $(HASH_DIR)/stage_,$(addsuffix .hash,$(STAGES)))

# Color codes
GREEN := \033[0;32m
BLUE := \033[0;34m
YELLOW := \033[1;33m
RED := \033[0;31m
PURPLE := \033[0;35m
NC := \033[0m

.PHONY: all setup clean build-static-libs build-executables validate hash-table-init git-lfs-setup

all: git-lfs-setup setup hash-table-init build-static-libs build-executables validate

git-lfs-setup:
	@echo -e "$(PURPLE)[GIT-LFS]$(NC) Initializing Git LFS for large file system..."
	@if ! command -v git-lfs >/dev/null 2>&1; then \
		echo -e "$(RED)[ERROR]$(NC) Git LFS not installed. Please install git-lfs"; \
		exit 1; \
	fi
	@git lfs track "*.a" "*.so" "*.dylib" "*.dll" "*.exe" "docs/**/*.pdf" "*.bin" "*.obj"
	@git add .gitattributes || true
	@echo -e "$(GREEN)[SUCCESS]$(NC) Git LFS configured for binary artifacts"

setup:
	@echo -e "$(BLUE)[SETUP]$(NC) Creating AEGIS directory structure..."
	@mkdir -p $(LIB_DIR) $(BIN_DIR) $(OBJ_DIR) $(BUILD_DIR) $(HASH_DIR)
	@for stage in $(STAGES); do \
		mkdir -p $(OBJ_DIR)/rift-$$stage; \
		mkdir -p $(SRC_DIR)/core/stage-$$stage; \
		mkdir -p $(INCLUDE_DIR)/rift/core/stage-$$stage; \
	done
	@echo -e "$(GREEN)[SUCCESS]$(NC) Directory structure created"

hash-table-init: $(HASH_LOOKUP_FILES)
	@echo -e "$(PURPLE)[HASH-TABLE]$(NC) Generating O(1) stage lookup table..."
	@echo "{" > $(BUILD_HASH_TABLE)
	@echo "  \"version\": \"$(RIFT_VERSION)\"," >> $(BUILD_HASH_TABLE)
	@echo "  \"platform\": \"$(PLATFORM)\"," >> $(BUILD_HASH_TABLE)
	@echo "  \"stages\": {" >> $(BUILD_HASH_TABLE)
	@for i in $(STAGES); do \
		stage_name=$$(echo $(STAGE_NAMES) | cut -d' ' -f$$(($$i + 1))); \
		echo "    \"$$i\": {" >> $(BUILD_HASH_TABLE); \
		echo "      \"name\": \"$$stage_name\"," >> $(BUILD_HASH_TABLE); \
		echo "      \"lib\": \"$(LIB_DIR)/librift-$$i.a\"," >> $(BUILD_HASH_TABLE); \
		echo "      \"bin\": \"$(BIN_DIR)/rift-$$i\"," >> $(BUILD_HASH_TABLE); \
		echo "      \"hash_file\": \"$(HASH_DIR)/stage_$$i.hash\"" >> $(BUILD_HASH_TABLE); \
		if [ $$i -eq 6 ]; then \
			echo "    }" >> $(BUILD_HASH_TABLE); \
		else \
			echo "    }," >> $(BUILD_HASH_TABLE); \
		fi; \
	done
	@echo "  }" >> $(BUILD_HASH_TABLE)
	@echo "}" >> $(BUILD_HASH_TABLE)
	@echo -e "$(GREEN)[SUCCESS]$(NC) Hash table generated: $(BUILD_HASH_TABLE)"

$(HASH_DIR)/stage_%.hash:
	@mkdir -p $(HASH_DIR)
	@stage_name=$$(echo $(STAGE_NAMES) | cut -d' ' -f$$(($$* + 1))); \
	echo "stage_id:$*" > $@; \
	echo "stage_name:$$stage_name" >> $@; \
	echo "timestamp:$$(date +%s)" >> $@; \
	echo "hash:$$(echo "$*$$stage_name" | sha256sum | cut -d' ' -f1)" >> $@

build-static-libs: $(STATIC_LIBS)

build-executables: $(EXECUTABLES)

# Pattern rule for static libraries with corrected naming
$(LIB_DIR)/librift-%.a: $(OBJ_DIR)/rift-%/stage%.o
	@echo -e "$(BLUE)[LIB]$(NC) Building librift-$*.a..."
	@$(AR) rcs $@ $<
	@$(RANLIB) $@
	@echo -e "$(GREEN)[SUCCESS]$(NC) librift-$*.a created"

# Object file compilation with dependency tracking
$(OBJ_DIR)/rift-%/stage%.o: $(SRC_DIR)/core/stage-%/stage%.c
	@echo -e "$(BLUE)[COMPILE]$(NC) Building stage $* object..."
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -MF $(@:.o=.d) -c $< -o $@

# Executable generation with corrected linking
$(BIN_DIR)/rift-%: $(SRC_DIR)/cli/stage-%/main.c $(LIB_DIR)/librift-%.a
	@echo -e "$(BLUE)[EXEC]$(NC) Building rift-$*..."
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< -L$(LIB_DIR) -lrift-$* $(LIBS)

validate:
	@echo -e "$(BLUE)[VALIDATE]$(NC) Running AEGIS build validation..."
	@echo -e "$(PURPLE)[PLATFORM]$(NC) Target: $(PLATFORM)"
	@echo -e "$(PURPLE)[PKG-CONFIG]$(NC) Version: $$($(PKG_CONFIG) --version)"
	@for stage in $(STAGES); do \
		if [ -f "$(LIB_DIR)/librift-$$stage.a" ]; then \
			echo -e "$(GREEN)✓$(NC) librift-$$stage.a"; \
		else \
			echo -e "$(RED)✗$(NC) librift-$$stage.a missing"; \
		fi; \
		if [ -f "$(BIN_DIR)/rift-$$stage" ]; then \
			echo -e "$(GREEN)✓$(NC) rift-$$stage"; \
		else \
			echo -e "$(RED)✗$(NC) rift-$$stage missing"; \
		fi; \
	done
	@if [ -f "$(BUILD_HASH_TABLE)" ]; then \
		echo -e "$(GREEN)✓$(NC) Hash table: $(BUILD_HASH_TABLE)"; \
	else \
		echo -e "$(RED)✗$(NC) Hash table missing"; \
	fi

clean:
	@echo -e "$(BLUE)[CLEAN]$(NC) Removing build artifacts..."
	@rm -rf $(OBJ_DIR) $(LIB_DIR)/*.a $(BIN_DIR)/rift-* $(BUILD_DIR)
	@find . -name "*.d" -delete

# Generate source files if missing (with corrected naming)
$(SRC_DIR)/core/stage-%/stage%.c:
	@mkdir -p $(dir $@)
	@mkdir -p $(INCLUDE_DIR)/rift/core/stage-$*
	@echo "Generating stage $* implementation..."
	@cat > $@ << 'EOF'
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int rift_stage_$*_init(void) {
    printf("RIFT Stage $* - Initialized\n");
    return 0;
}

int rift_stage_$*_process(const char* input, char** output) {
    if (!input || !output) return -1;
    size_t len = strlen(input) + 32;
    *output = malloc(len);
    if (!*output) return -1;
    snprintf(*output, len, "Stage-$*-processed: %s", input);
    return 0;
}

void rift_stage_$*_cleanup(void) {
    printf("RIFT Stage $* - Cleanup complete\n");
}
EOF

$(SRC_DIR)/cli/stage-%/main.c:
	@mkdir -p $(dir $@)
	@echo "Generating CLI for stage $*..."
	@cat > $@ << 'EOF'
#include <stdio.h>
#include <stdlib.h>

extern int rift_stage_$*_init(void);
extern int rift_stage_$*_process(const char* input, char** output);
extern void rift_stage_$*_cleanup(void);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input>\n", argv[0]);
        return 1;
    }
    
    rift_stage_$*_init();
    char* output = NULL;
    int result = rift_stage_$*_process(argv[1], &output);
    
    if (result == 0 && output) {
        printf("Output: %s\n", output);
        free(output);
    }
    
    rift_stage_$*_cleanup();
    return result;
}
EOF

# Include dependency files
-include $(shell find $(OBJ_DIR) -name "*.d" 2>/dev/null)

# AEGIS Status Report
status:
	@echo -e "$(BLUE)=================================================================="
	@echo -e "RIFT AEGIS Build System Status"
	@echo -e "Version: $(RIFT_VERSION)"
	@echo -e "Platform: $(PLATFORM)"
	@echo -e "Compliance: AEGIS Waterfall Methodology"
	@echo -e "Libraries: $(words $(STATIC_LIBS)) stages"
	@echo -e "Executables: $(words $(EXECUTABLES)) binaries"
	@echo -e "Hash Table: O(1) lookup enabled"
	@echo -e "Git LFS: Binary artifact tracking enabled"
	@echo -e "==================================================================$(NC)"

help:
	@echo -e "$(BLUE)RIFT AEGIS Build System$(NC)"
	@echo "Available targets:"
	@echo "  all         - Complete AEGIS build (default)"
	@echo "  git-lfs-setup - Configure Git LFS for large files"
	@echo "  setup       - Create directory structure"
	@echo "  hash-table-init - Generate O(1) stage lookup"
	@echo "  build-static-libs - Build stage libraries"
	@echo "  build-executables - Build stage executables"
	@echo "  validate    - Validate build artifacts"
	@echo "  clean       - Remove build artifacts"
	@echo "  status      - Show build system status"
	@echo "  help        - Show this help message"
	@echo ""
	@echo "Technical Implementation: OBINexus Computing Framework"
	@echo "Methodology: AEGIS Waterfall with Zero Trust Governance"
