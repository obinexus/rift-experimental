# RIFT Stage 0 Makefile
# OBINexus Computing - AEGIS Framework

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Iinclude -g -O2
LDFLAGS = -lm

SRCDIR = src
INCDIR = include
BUILDDIR = build
TESTDIR = tests

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
TARGET = $(BUILDDIR)/rift0

# Test sources
TEST_SOURCES = $(wildcard $(TESTDIR)/qa_mocks/*.c)
TEST_OBJECTS = $(filter-out $(BUILDDIR)/main.o, $(OBJECTS))
TEST_TARGET = $(BUILDDIR)/test_rift0

.PHONY: all clean test init

all: init $(TARGET)

init:
	@mkdir -p $(BUILDDIR)

$(TARGET): $(OBJECTS)
	@echo "🔗 Linking RIFT Stage 0..."
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "✅ Build complete: $@"

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@echo "🔨 Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

test: init $(TEST_TARGET)
	@echo "🧪 Running QA tests..."
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJECTS) $(BUILDDIR)/test_automaton.o
	@echo "🔗 Linking test suite..."
	$(CC) $(TEST_OBJECTS) $(BUILDDIR)/test_automaton.o -o $@ $(LDFLAGS)

$(BUILDDIR)/test_automaton.o: $(TESTDIR)/qa_mocks/test_automaton.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "🧹 Cleaning build artifacts..."
	rm -rf $(BUILDDIR)

# RIFT Zero Trust pipeline integration (future)
zero-trust-init:
	@echo "🔐 Zero Trust cryptographic initialization..."
	@echo "TODO: Implement cryptographic key generation"

verify-keys:
	@echo "🔍 Verifying cryptographic key integrity..."
	@echo "TODO: Implement key verification"

# nlink integration (future)
nlink-build:
	@echo "📦 nlink package build..."
	@echo "TODO: Implement nlink orchestration"

.DEFAULT_GOAL := all
