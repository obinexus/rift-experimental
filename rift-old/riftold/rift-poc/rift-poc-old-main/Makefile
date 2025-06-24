# OBINexus AST-Aware System with Zero Trust Pipeline
# Enhanced Makefile with cryptographic governance enforcement

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic -Werror -O2
SECURITY_FLAGS = -fstack-protector-strong -D_FORTIFY_SOURCE=2
INCLUDE_DIRS = -Iinclude -Iinclude/obinexus -Iinclude/aegis -Iinclude/pocrift -Iinclude/zero_trust
LDFLAGS = -lpthread -lregex -lm -lcrypto

BUILD_DIR = build
KEYS_DIR = keys
SRC_DIRS = src/core src/aegis src/pocrift src/librift_integration src/tennis_case_study src/core/zero_trust
SOURCES = $(shell find $(SRC_DIRS) -name '*.c')
OBJECTS = $(SOURCES:%.c=$(BUILD_DIR)/%.o)

# Zero Trust Configuration
PRIVATE_KEY = $(KEYS_DIR)/rift_signing_key.pem
PUBLIC_KEY = $(KEYS_DIR)/rift_signing_pub.pem
ZERO_TRUST_SCRIPT = scripts/rift-zero-trust-pipeline.sh

# Primary targets
.PHONY: all clean test validate install zero-trust-init rift-zero-trust-run

all: zero-trust-init rift.exe tennis_study.exe

# Zero Trust initialization
zero-trust-init:
	@echo "Initializing Zero Trust environment..."
	@mkdir -p $(KEYS_DIR)
	@if [ ! -f $(PRIVATE_KEY) ]; then \
		echo "Generating cryptographic keys..."; \
		openssl genrsa -out $(PRIVATE_KEY) 4096 2>/dev/null; \
		chmod 600 $(PRIVATE_KEY); \
		openssl rsa -in $(PRIVATE_KEY) -pubout -out $(PUBLIC_KEY) 2>/dev/null; \
		chmod 644 $(PUBLIC_KEY); \
		echo "Zero Trust keys generated successfully"; \
	else \
		echo "Using existing Zero Trust keys"; \
	fi

rift.exe: $(BUILD_DIR)/rift.exe
tennis_study.exe: $(BUILD_DIR)/tennis_study.exe

$(BUILD_DIR)/rift.exe: $(OBJECTS) $(BUILD_DIR)/src/rift_compiler/main.o
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(SECURITY_FLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/tennis_study.exe: $(OBJECTS) $(BUILD_DIR)/src/tennis_case_study/main.o
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(SECURITY_FLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(SECURITY_FLAGS) $(INCLUDE_DIRS) -c $< -o $@

# Zero Trust Rift compilation
rift-zero-trust-run: zero-trust-init rift.exe
	@if [ -z "$(FILE)" ]; then \
		echo "Usage: make rift-zero-trust-run FILE=myfile.rift"; \
		exit 1; \
	fi
	@echo "Running Zero Trust Rift Pipeline for: $(FILE)"
	@$(ZERO_TRUST_SCRIPT) $(FILE)

# Verification targets
verify-keys:
	@echo "Verifying cryptographic keys..."
	@openssl rsa -in $(PRIVATE_KEY) -check -noout 2>/dev/null && echo "Private key: VALID" || echo "Private key: INVALID"
	@openssl rsa -pubin -in $(PUBLIC_KEY) -noout 2>/dev/null && echo "Public key: VALID" || echo "Public key: INVALID"

clean:
	rm -rf $(BUILD_DIR)
	rm -f examples/*.rift.*

clean-keys:
	@echo "WARNING: This will remove all cryptographic keys!"
	@read -p "Are you sure? [y/N] " confirm && [ "$$confirm" = "y" ] && rm -rf $(KEYS_DIR) || echo "Cancelled"

test: all
	@echo "Running validation tests..."
	@$(MAKE) -C tests all
	@./tests/run_tests.sh

validate: all
	@echo "Running formal validation..."
	@./scripts/validate_compliance.sh
	@./scripts/validate_semantic_preservation.sh

install: all
	@echo "Installing OBINexus AST-Aware System..."
	@mkdir -p /usr/local/bin
	@cp $(BUILD_DIR)/rift.exe /usr/local/bin/rift
	@cp $(BUILD_DIR)/tennis_study.exe /usr/local/bin/tennis_study
	@cp $(ZERO_TRUST_SCRIPT) /usr/local/bin/rift-zero-trust

# Help target
help:
	@echo "OBINexus AST-Aware System with Zero Trust Pipeline"
	@echo ""
	@echo "Targets:"
	@echo "  all                    Build all components"
	@echo "  zero-trust-init        Initialize Zero Trust environment"
	@echo "  rift-zero-trust-run    Run Zero Trust Rift pipeline"
	@echo "                         Usage: make rift-zero-trust-run FILE=myfile.rift"
	@echo "  verify-keys            Verify cryptographic key integrity"
	@echo "  clean                  Clean build artifacts"
	@echo "  clean-keys             Remove cryptographic keys (WARNING)"
	@echo "  test                   Run test suite"
	@echo "  validate               Run compliance validation"
	@echo "  install                Install system components"
	@echo ""
	@echo "Examples:"
	@echo "  make rift-zero-trust-run FILE=examples/hello.rift"
	@echo "  make verify-keys"

.SECONDARY: $(OBJECTS)
