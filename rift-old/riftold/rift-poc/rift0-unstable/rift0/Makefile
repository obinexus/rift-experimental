# RIFT Stage 0 Enhanced Makefile
# OBINexus Computing - AEGIS Framework

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Werror -Iinclude -g -O2 -D_GNU_SOURCE
LDFLAGS = -lm

SRCDIR = src
CLIDIR = cli
INCDIR = include
BUILDDIR = build
BINDIR = bin
TESTDIR = tests

# Core sources (excluding main)
CORE_SOURCES = $(filter-out $(SRCDIR)/main.c, $(wildcard $(SRCDIR)/*.c))
CORE_OBJECTS = $(CORE_SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

# CLI sources
CLI_SOURCES = $(wildcard $(CLIDIR)/*.c)
CLI_OBJECTS = $(CLI_SOURCES:$(CLIDIR)/%.c=$(BUILDDIR)/cli_%.o)

# Test sources
TEST_SOURCES = $(wildcard $(TESTDIR)/qa_mocks/*.c)
TEST_OBJECTS = $(TEST_SOURCES:$(TESTDIR)/qa_mocks/%.c=$(BUILDDIR)/test_%.o)

# Targets
CLI_TARGET = $(BINDIR)/rift0.exe
DEMO_TARGET = $(BUILDDIR)/rift0_demo
TEST_TARGET = $(BUILDDIR)/test_rift0

.PHONY: all clean test init cli demo install

all: init cli demo

cli: init $(CLI_TARGET)

demo: init $(DEMO_TARGET)

init:
	@mkdir -p $(BUILDDIR) $(BINDIR)
	@echo "🏗️  Initialized build directories"

$(CLI_TARGET): $(CORE_OBJECTS) $(CLI_OBJECTS)
	@echo "🔗 Linking RIFT Stage 0 CLI..."
	$(CC) $(CORE_OBJECTS) $(CLI_OBJECTS) -o $@ $(LDFLAGS)
	@echo "✅ CLI executable ready: $@"

$(DEMO_TARGET): $(CORE_OBJECTS) $(BUILDDIR)/main.o
	@echo "🔗 Linking RIFT Stage 0 Demo..."
	$(CC) $(CORE_OBJECTS) $(BUILDDIR)/main.o -o $@ $(LDFLAGS)
	@echo "✅ Demo executable ready: $@"

# Core object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@echo "🔨 Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# CLI object files
$(BUILDDIR)/cli_%.o: $(CLIDIR)/%.c
	@echo "🔨 Compiling CLI $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Test object files
$(BUILDDIR)/test_%.o: $(TESTDIR)/qa_mocks/%.c
	@echo "🔨 Compiling test $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Demo main object
$(BUILDDIR)/main.o: $(SRCDIR)/main.c
	@echo "🔨 Compiling demo main..."
	$(CC) $(CFLAGS) -c $< -o $@

test: init $(TEST_TARGET)
	@echo "🧪 Running QA tests..."
	./$(TEST_TARGET)

$(TEST_TARGET): $(CORE_OBJECTS) $(TEST_OBJECTS)
	@echo "🔗 Linking test suite..."
	$(CC) $(CORE_OBJECTS) $(TEST_OBJECTS) -o $@ $(LDFLAGS)

clean:
	@echo "🧹 Cleaning build artifacts..."
	rm -rf $(BUILDDIR) $(BINDIR)

install: cli
	@echo "📦 Installing RIFT Stage 0..."
	cp $(CLI_TARGET) /usr/local/bin/rift0 2>/dev/null || echo "⚠️  Manual install required: sudo cp $(CLI_TARGET) /usr/local/bin/"

# Configuration file generation
config:
	@echo "⚙️  Generating default .riftrc..."
	@echo "# RIFT Configuration File" > .riftrc
	@echo "output_dir=./output" >> .riftrc
	@echo "debug_mode=false" >> .riftrc
	@echo "verbose=true" >> .riftrc
	@echo "log_level=INFO" >> .riftrc
	@echo "✅ Default configuration created: .riftrc"

# RIFT Zero Trust pipeline integration (future)
zero-trust-init:
	@echo "🔐 Zero Trust cryptographic initialization..."
	@mkdir -p keys
	@echo "TODO: Implement RSA-4096 key generation"

verify-keys:
	@echo "🔍 Verifying cryptographic key integrity..."
	@echo "TODO: Implement key verification"

# nlink integration (future)
nlink-build:
	@echo "📦 nlink package build..."
	@echo "TODO: Implement nlink orchestration"

.DEFAULT_GOAL := all
