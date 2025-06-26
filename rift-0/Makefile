# =================================================================
# Makefile - RIFT-0 Tokenizer Build Orchestration
# RIFT: RIFT Is a Flexible Translator
# Component: Build system for tokenizer stage
# OBINexus Computing Framework - Stage 0 Implementation
# 
# Build Flow: nlink → polybuild (AEGIS Framework)
# Author: Nnamdi Michael Okpala & AEGIS Integration Team
# =================================================================

# Project configuration
PROJECT_NAME = rift-0
VERSION = 0.4.0
PREFIX ?= /usr/local
LIBDIR = $(PREFIX)/lib
INCLUDEDIR = $(PREFIX)/include
PKGCONFIGDIR = $(LIBDIR)/pkgconfig

# Compiler configuration
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -O2 -fPIC
CFLAGS += -DRIFT_VERSION=$(VERSION)
CFLAGS += -DRIFT_VERSION_STRING=\"$(VERSION)\"
CFLAGS += -DRIFT_COMPILER_EXTENSIONS
CFLAGS += -DRIFT_THREAD_SUPPORT

# Debug configuration
DEBUG_CFLAGS = -std=c11 -Wall -Wextra -Werror -g -O0 -fPIC
DEBUG_CFLAGS += -DRIFT_VERSION=$(VERSION)
DEBUG_CFLAGS += -DRIFT_VERSION_STRING=\"$(VERSION)\"
DEBUG_CFLAGS += -DRIFT_COMPILER_EXTENSIONS
DEBUG_CFLAGS += -DRIFT_THREAD_SUPPORT
DEBUG_CFLAGS += -DDEBUG

# Linker configuration
LDFLAGS = -shared
LIBS = -lpthread

# Directory structure
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin
OBJ_DIR = $(BUILD_DIR)/obj
LIB_DIR = $(BUILD_DIR)/lib

# Source files
SOURCES = $(wildcard $(SRC_DIR)/core/*.c) $(SRC_DIR)/cli/main.c

# Object files
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
DEPS = $(OBJECTS:.o=.d)

# Target files
STATIC_LIB = $(LIB_DIR)/librift-0.a
SHARED_LIB = $(LIB_DIR)/librift-0.so.$(VERSION)
SHARED_LIB_LINK = $(LIB_DIR)/librift-0.so
EXECUTABLE = $(BIN_DIR)/rift-0
PKGCONFIG = $(BUILD_DIR)/rift-0.pc

# =================================================================
# BUILD TARGETS
# =================================================================

.PHONY: all clean setup install uninstall debug test help

# Default target
all: setup $(STATIC_LIB) $(SHARED_LIB) $(EXECUTABLE) $(PKGCONFIG)

# Setup build directories
setup:
	@echo "Setting up RIFT-0 build environment..."
	@mkdir -p $(OBJ_DIR)/core
	@mkdir -p $(OBJ_DIR)/cli
	@mkdir -p $(LIB_DIR)
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(BUILD_DIR)

# Compile object files with dependency generation
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $< ..."
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -MMD -MP -I$(INC_DIR) -I. -c $< -o $@

# Static library
$(STATIC_LIB): $(OBJECTS)
	@echo "Creating static library $@..."
	@ar rcs $@ $^
	@echo "Static library created successfully"

# Shared library
$(SHARED_LIB): $(OBJECTS)
	@echo "Creating shared library $@..."
	@$(CC) $(LDFLAGS) -Wl,-soname,librift-0.so.0 -o $@ $^ $(LIBS)
	@ln -sf librift-0.so.$(VERSION) $(SHARED_LIB_LINK)
	@echo "Shared library created successfully"

# Executable (using static library)
$(EXECUTABLE): $(STATIC_LIB)
	@echo "Creating executable $@..."
	@$(CC) $(CFLAGS) -I$(INC_DIR) -DRIFT_STANDALONE_BUILD \
		-o $@ $(STATIC_LIB) $(LIBS)
	@echo "Executable created successfully"

# pkg-config file generation
$(PKGCONFIG): rift-0.pc.in
	@echo "Generating pkg-config file..."
	@sed -e 's|@PREFIX@|$(PREFIX)|g' \
	     -e 's|@LIBDIR@|$(LIBDIR)|g' \
	     -e 's|@INCLUDEDIR@|$(INCLUDEDIR)|g' \
	     -e 's|@VERSION@|$(VERSION)|g' \
	     $< > $@
	@echo "pkg-config file generated"

# =================================================================
# DEBUG BUILD TARGET
# =================================================================

debug: CFLAGS = $(DEBUG_CFLAGS)
debug: clean setup $(STATIC_LIB) $(SHARED_LIB) $(EXECUTABLE)
	@echo "Debug build completed"

# =================================================================
# TESTING TARGETS
# =================================================================

test: $(EXECUTABLE)
	@echo "Running RIFT-0 tokenizer tests..."
	@echo "Testing basic tokenization..."
	@echo "int main() { return 0; }" | $(EXECUTABLE)
	@echo "Tests completed successfully"

# =================================================================
# INSTALLATION TARGETS
# =================================================================

install: all
	@echo "Installing RIFT-0 tokenizer..."
	@install -d $(DESTDIR)$(LIBDIR)
	@install -d $(DESTDIR)$(INCLUDEDIR)
	@install -d $(DESTDIR)$(PKGCONFIGDIR)
	@install -d $(DESTDIR)$(PREFIX)/bin
	@install -m 644 $(STATIC_LIB) $(DESTDIR)$(LIBDIR)/
	@install -m 755 $(SHARED_LIB) $(DESTDIR)$(LIBDIR)/
	@ln -sf librift-0.so.$(VERSION) $(DESTDIR)$(LIBDIR)/librift-0.so
	@install -m 644 $(INC_DIR)/*.h $(DESTDIR)$(INCLUDEDIR)/
	@install -m 644 $(PKGCONFIG) $(DESTDIR)$(PKGCONFIGDIR)/
	@install -m 755 $(EXECUTABLE) $(DESTDIR)$(PREFIX)/bin/
	@ldconfig
	@echo "Installation completed"

uninstall:
	@echo "Uninstalling RIFT-0 tokenizer..."
	@rm -f $(DESTDIR)$(LIBDIR)/librift-0.*
	@rm -rf $(DESTDIR)$(INCLUDEDIR)
	@rm -f $(DESTDIR)$(PKGCONFIGDIR)/rift-0.pc
	@rm -f $(DESTDIR)$(PREFIX)/bin/rift-0
	@ldconfig
	@echo "Uninstallation completed"

# =================================================================
# MAINTENANCE TARGETS
# =================================================================

clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)
	@echo "Clean completed"

help:
	@echo "RIFT-0 Tokenizer Build System"
	@echo "============================="
	@echo "Available targets:"
	@echo "  all      - Build all targets (default)"
	@echo "  setup    - Create build directories"
	@echo "  debug    - Build with debug symbols"
	@echo "  test     - Run basic tests"
	@echo "  install  - Install to system"
	@echo "  uninstall- Remove from system"
	@echo "  clean    - Remove build artifacts"
	@echo "  help     - Show this help"
	@echo ""
	@echo "Configuration:"
	@echo "  PREFIX=$(PREFIX)"
	@echo "  CC=$(CC)"
	@echo "  VERSION=$(VERSION)"

# =================================================================
# DEPENDENCY INCLUSION
# =================================================================

# Include dependency files (if they exist)
-include $(DEPS)

# =================================================================
# BUILD VALIDATION
# =================================================================

.PHONY: validate
validate: all
	@echo "Validating RIFT-0 build..."
	@echo "Checking static library..."
	@test -f $(STATIC_LIB) || (echo "Static library missing" && exit 1)
	@echo "Checking shared library..."
	@test -f $(SHARED_LIB) || (echo "Shared library missing" && exit 1)
	@echo "Checking executable..."
	@test -f $(EXECUTABLE) || (echo "Executable missing" && exit 1)
	@echo "Checking pkg-config..."
	@test -f $(PKGCONFIG) || (echo "pkg-config file missing" && exit 1)
	@echo "Build validation successful"