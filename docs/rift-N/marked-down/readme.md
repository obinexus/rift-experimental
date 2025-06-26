# RIFTLang Project Structure

This project follows the RIFTLang Implementation Specification v1.0 with waterfall methodology.

## Directory Structure

### Foundation Track (FT1)
- src/core/ - Core RIFTLang implementation
- src/parser/ - Token parsing and AST generation  
- src/memory/ - Memory governance and alignment
- src/threading/ - Thread safety and concurrency
- src/policy/ - Policy validation engine
- src/nlink/ - NLINK integration components

### Aspirational Track (AS2)
- src/advanced/ - Advanced parser capabilities
- src/security/ - Zero-trust security implementation
- src/platform/ - Platform extensions

### Documentation
- docs/specification/ - Technical specifications
- docs/api/ - API documentation
- docs/developer/ - Developer resources
- docs/certification/ - Certification requirements

### Testing
- 	tests/unit/ - Unit tests
- 	tests/integration/ - Integration tests
- 	tests/policy/ - Policy validation tests
- 	tests/concurrency/ - Thread safety tests

### Governance
- governance/thread_safety/ - Thread safety governance
- governance/memory_contracts/ - Memory governance contracts
- governance/zero_trust/ - Zero-trust policy framework

## Build Instructions

See build/ directory for platform-specific build configurations.

## Security Notice

All files follow RIFTLang governance principles and zero-trust architecture.
