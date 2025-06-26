# Master Makefile for RIFT Concurrency POC
# Aegis Development Team - Phase 1: Shared Library Foundation

CC=gcc
CFLAGS=-Wall -Wextra -Werror -z noexecstack -fstack-protector-strong -fpie -pthread -O2 -g
LDFLAGS=-pie -Wl,-z,relro,-z,now -pthread
BUILD_DIR=build
LIB_DIR=$(BUILD_DIR)/lib

# Shared libraries
SHARED_LIBS = librift_common.so librift_memory.so librift_http.so
TRUE_CONCURRENCY_LIB = librift_true_parallel.so
SIMULATED_CONCURRENCY_LIB = librift_simulated.so

.PHONY: all shared_libs true_concurrency simulated_concurrency clean test

all: shared_libs true_concurrency simulated_concurrency

shared_libs:
	@echo "Building shared libraries..."
	$(MAKE) -C shared/rift_common
	$(MAKE) -C shared/rift_memory  
	$(MAKE) -C shared/rift_http

true_concurrency: shared_libs
	@echo "Building true concurrency module..."
	$(MAKE) -C true_concurrency

simulated_concurrency: shared_libs
	@echo "Building simulated concurrency module..."
	$(MAKE) -C simulated_concurrency

clean:
	@echo "Cleaning all modules..."
	$(MAKE) -C shared/rift_common clean
	$(MAKE) -C shared/rift_memory clean
	$(MAKE) -C shared/rift_http clean
	$(MAKE) -C true_concurrency clean
	$(MAKE) -C simulated_concurrency clean
	rm -rf $(BUILD_DIR)/*

test: all
	@echo "Running Phase 1 tests..."
	$(MAKE) -C tests/thread_safety test
	$(MAKE) -C tests/policy_enforcement test
	$(MAKE) -C tests/integration test

install: all
	@echo "Installing RIFT libraries..."
	install -d /usr/local/lib
	install -m 755 $(LIB_DIR)/*.so /usr/local/lib/
	ldconfig

debug: CFLAGS += -DDEBUG -g3
debug: all

release: CFLAGS += -DNDEBUG -O3
release: all

help:
	@echo "RIFT Concurrency POC Build System"
	@echo "Available targets:"
	@echo "  all              - Build all modules"
	@echo "  shared_libs      - Build shared libraries only"
	@echo "  true_concurrency - Build true concurrency module"
	@echo "  simulated_concurrency - Build simulated concurrency module"
	@echo "  clean            - Clean all build artifacts"
	@echo "  test             - Run all tests"
	@echo "  install          - Install libraries system-wide"
	@echo "  debug            - Build with debug symbols"
	@echo "  release          - Build optimized release"
