.PHONY: all build test clean

BUILD_DIR := build

all: build

build:
	cmake -S . -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)

test:
	cmake --build $(BUILD_DIR) --target test

clean:
	rm -rf $(BUILD_DIR)
