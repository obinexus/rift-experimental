CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -O2 -Iinclude
LDFLAGS = -lm

SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=build/%.o)
TARGET = build/stage1_parser

STAGE_INPUT = ../stage0_tokenization/build/output.rift.0
STAGE_OUTPUT = build/output.rift.1

.PHONY: all clean validate-and-build validate-input build validate-output auraseal-verify

all: validate-and-build

validate-and-build: validate-input build validate-output

validate-input:
	@echo "=== Stage 1: Validating input ==="
	@./scripts/validate_stage0_output.sh $(STAGE_INPUT)
	@echo "Input validation passed"

build: $(TARGET)

$(TARGET): $(OBJECTS) | build
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

build/%.o: $(SRCDIR)/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

validate-output: $(TARGET)
	@echo "=== Stage 1: Validating output ==="
	@$(TARGET) $(STAGE_INPUT) $(STAGE_OUTPUT) --zero-trust
	@./scripts/validate_parse_tree.sh $(STAGE_OUTPUT)
	@echo "Output validation passed"

auraseal-verify:
	@echo "=== Stage 1: AuraSeal verification ==="
	@./scripts/auraseal_sign.sh $(STAGE_OUTPUT)
	@echo "AuraSeal verification passed"

clean:
	rm -rf build
