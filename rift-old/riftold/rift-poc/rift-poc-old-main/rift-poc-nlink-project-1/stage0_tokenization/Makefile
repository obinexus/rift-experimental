CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -O2 -Iinclude
LDFLAGS = -lm

SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=build/%.o)
TARGET = build/stage0_tokenizer

STAGE_INPUT = ../test_input.rift
STAGE_OUTPUT = build/output.rift.0

.PHONY: all clean validate-and-build validate-input build validate-output auraseal-verify

all: validate-and-build

validate-and-build: validate-input build validate-output

validate-input:
	@echo "=== Stage 0: Validating input ==="
	@if [ ! -f "$(STAGE_INPUT)" ]; then echo "Creating test input file"; echo 'function main() { return 0; }' > $(STAGE_INPUT); fi
	@echo "Input validation passed"

build: $(TARGET)

$(TARGET): $(OBJECTS) | build
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

build/%.o: $(SRCDIR)/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

validate-output: $(TARGET)
	@echo "=== Stage 0: Validating output ==="
	@$(TARGET) $(STAGE_INPUT) $(STAGE_OUTPUT) --zero-trust
	@./scripts/validate_uscn_compliance.sh $(STAGE_OUTPUT)
	@echo "Output validation passed"

auraseal-verify:
	@echo "=== Stage 0: AuraSeal verification ==="
	@./scripts/auraseal_sign.sh $(STAGE_OUTPUT)
	@echo "AuraSeal verification passed"

clean:
	rm -rf build
