# =================================================================
# RIFT AEGIS Build System - Optimized Production Implementation
# OBINexus Computing Framework - Technical Lead: Nnamdi Michael Okpala
# =================================================================

RIFT_VERSION := 1.0.0
CC := gcc
AR := ar
RANLIB := ranlib

# Directory structure
SRC_DIR := rift/src
INCLUDE_DIR := rift/include
LIB_DIR := lib
BIN_DIR := bin
OBJ_DIR := obj

# AEGIS Compiler flags with dependency tracking
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -MMD -MP
CFLAGS += -fstack-protector-strong -D_FORTIFY_SOURCE=2
CFLAGS += -DRIFT_AEGIS_COMPLIANCE=1 -DRIFT_VERSION_STRING=\"$(RIFT_VERSION)\"
CFLAGS += -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/rift/core

LDFLAGS := -Wl,-z,relro -Wl,-z,now
LIBS := -lssl -lcrypto -lpthread

# Stage configuration
STAGES := 0 1 2 3 4 5 6
STAGE_NAMES := tokenizer parser semantic validator bytecode verifier emitter

# CORRECTED: Proper library naming with librift-*.a convention
STATIC_LIBS := $(addprefix $(LIB_DIR)/lib,$(addsuffix .a,$(addprefix rift-,$(STAGES))))
EXECUTABLES := $(addprefix $(BIN_DIR)/,$(addprefix rift-,$(STAGES)))

# Color codes
GREEN := \033[0;32m
BLUE := \033[0;34m
NC := \033[0m

.PHONY: all setup clean build-static-libs build-executables

all: setup build-static-libs build-executables

setup:
	@echo -e "$(BLUE)[SETUP]$(NC) Creating AEGIS directory structure..."
	@mkdir -p $(LIB_DIR) $(BIN_DIR) $(OBJ_DIR)
	@for stage in $(STAGES); do \
		mkdir -p $(OBJ_DIR)/rift-$$stage; \
		mkdir -p $(SRC_DIR)/core/stage-$$stage; \
		mkdir -p $(INCLUDE_DIR)/rift/core/stage-$$stage; \
	done

build-static-libs: $(STATIC_LIBS)

build-executables: $(EXECUTABLES)

# Pattern rule for static libraries with corrected naming
$(LIB_DIR)/librift-%.a: $(OBJ_DIR)/rift-%/$(word $(shell expr $* + 1),$(STAGE_NAMES)).o
	@echo -e "$(BLUE)[LIB]$(NC) Building librift-$*.a..."
	@$(AR) rcs $@ $<
	@$(RANLIB) $@
	@echo -e "$(GREEN)[SUCCESS]$(NC) librift-$*.a created"

# Object file compilation with dependency tracking
$(OBJ_DIR)/rift-%/%.o: $(SRC_DIR)/core/stage-%/%.c
	@echo -e "$(BLUE)[COMPILE]$(NC) Building $@..."
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -MF $(@:.o=.d) -c $< -o $@

# Executable generation with corrected linking
$(BIN_DIR)/rift-%: $(SRC_DIR)/cli/stage-%/main.c $(LIB_DIR)/librift-%.a
	@echo -e "$(BLUE)[EXEC]$(NC) Building rift-$*..."
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< -L$(LIB_DIR) -l:librift-$*.a $(LIBS)

clean:
	@echo -e "$(BLUE)[CLEAN]$(NC) Removing build artifacts..."
	@rm -rf $(OBJ_DIR) $(LIB_DIR)/*.a $(BIN_DIR)/rift-*
	@find . -name "*.d" -delete

# Generate source files if missing
$(SRC_DIR)/core/stage-%/%.c:
	@mkdir -p $(dir $@)
	@mkdir -p $(INCLUDE_DIR)/rift/core/stage-$*
	@echo "Generating minimal $@ implementation..."
	@cat > $@ << 'EOFINNER'
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
EOFINNER

$(SRC_DIR)/cli/stage-%/main.c:
	@mkdir -p $(dir $@)
	@echo "Generating CLI for stage $*..."
	@cat > $@ << 'EOFINNER'
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
EOFINNER

# Include dependency files
-include $(shell find $(OBJ_DIR) -name "*.d" 2>/dev/null)
