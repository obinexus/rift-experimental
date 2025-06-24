CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -O2 -Iinclude
LDFLAGS = -lm -lbc

SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=build/%.o)
TARGET = build/stage3_ast

STAGE_INPUT = ../stage1_parsing/build/output.rift.1
STAGE_OUTPUT = build/output.rift.3

.PHONY: all clean validate-and-build validate-input build validate-output auraseal-verify tennis-fsm-validate

all: validate-and-build

validate-and-build: validate-input build validate-output tennis-fsm-validate

validate-input:
	@echo "=== Stage 3: Validating input ==="
	@./scripts/validate_stage1_output.sh $(STAGE_INPUT)
	@echo "Input validation passed"

build: $(TARGET)

$(TARGET): $(OBJECTS) | build
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

build/%.o: $(SRCDIR)/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

validate-output: $(TARGET)
	@echo "=== Stage 3: Validating AST output ==="
	@$(TARGET) $(STAGE_INPUT) $(STAGE_OUTPUT) --zero-trust
	@./scripts/validate_ast_structure.sh $(STAGE_OUTPUT)
	@echo "AST output validation passed"

tennis-fsm-validate: validate-output
	@echo "=== Stage 3: Tennis FSM Validation ==="
	@./scripts/tennis_fsm_validation.sh $(STAGE_OUTPUT)
	@echo "Tennis FSM validation passed"

auraseal-verify:
	@echo "=== Stage 3: AuraSeal verification ==="
	@./scripts/auraseal_sign.sh $(STAGE_OUTPUT)
	@echo "AuraSeal verification passed"

clean:
	rm -rf build
