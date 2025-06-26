#!/bin/bash

# Generate sample RIFT files for testing
# OBINexus Computing - AEGIS Framework

EXAMPLES_DIR="examples"
mkdir -p "$EXAMPLES_DIR"

echo "🎮 Generating sample RIFT programs..."

# Basic sample
cat > "$EXAMPLES_DIR/sample.rift" << 'SAMPLE_EOF'
// Example .rift file for Stage 1 testing
// This demonstrates RIFT tokenization

// Sample code with various tokens
function hello(name) {
    let greeting = "Hello, " + name + "!";
    console.log(greeting);
    return greeting.length;
}

// Numbers and operators
let count = 42;
let pi = 3.14159;
let result = count * pi;

// Conditionals and loops
if (result > 100) {
    for (let i = 0; i < count; i++) {
        process(data[i]);
    }
}

// Regular expressions (to test parsing)
let pattern = /[A-Za-z0-9]+/gi;
let match = text.match(pattern);
SAMPLE_EOF

echo "✅ Generated examples/sample.rift"

# Advanced sample
cat > "$EXAMPLES_DIR/advanced.rift" << 'ADVANCED_EOF'
// Advanced RIFT sample demonstrating complex patterns

class DataProcessor {
    constructor(config) {
        this.config = config;
        this.patterns = [
            /^\d{4}-\d{2}-\d{2}$/,
            /^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$/
        ];
    }
    
    async process(data) {
        try {
            const results = await Promise.all(
                data.map(item => this.validate(item))
            );
            return results.filter(Boolean);
        } catch (error) {
            console.error("Processing failed:", error.message);
            throw error;
        }
    }
    
    validate(item) {
        return this.patterns.some(pattern => pattern.test(item));
    }
}

export default DataProcessor;
ADVANCED_EOF

echo "✅ Generated examples/advanced.rift"
echo "🎉 Sample generation complete"
