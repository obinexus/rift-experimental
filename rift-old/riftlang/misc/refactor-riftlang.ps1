# RIFTLang Project Directory Structure Refactoring Script
# Author: Aegis Development Team
# Version: 1.0.0
# Purpose: Secure reorganization of RIFTLang ecosystem before R&D PoC milestone

param(
    [Parameter(Mandatory=$true)]
    [string]$SourcePath,
    
    [Parameter(Mandatory=$true)]
    [string]$DestinationPath,
    
    [switch]$Backup = $true,
    [switch]$Validate = $true
)

# Error handling and logging
$ErrorActionPreference = "Stop"
$VerbosePreference = "Continue"

# Logging function
function Write-AegisLog {
    param([string]$Message, [string]$Level = "INFO")
    $timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    $logMessage = "[$timestamp] [$Level] $Message"
    Write-Host $logMessage
    
    # Ensure destination directory exists before logging
    if ($DestinationPath -and -not (Test-Path $DestinationPath)) {
        New-Item -Path $DestinationPath -ItemType Directory -Force | Out-Null
    }
    
    if ($DestinationPath) {
        Add-Content -Path "$DestinationPath\refactor.log" -Value $logMessage
    }
}

# Validate environment and prerequisites
function Test-Environment {
    Write-AegisLog "Validating environment for RIFTLang refactoring"
    
    if (-not (Test-Path $SourcePath)) {
        throw "Source path does not exist: $SourcePath"
    }
    
    # CRITICAL: Prevent recursive copy operations
    $resolvedSource = Resolve-Path $SourcePath
    $resolvedDestination = if (Test-Path $DestinationPath) { 
        Resolve-Path $DestinationPath 
    } else { 
        [System.IO.Path]::GetFullPath($DestinationPath) 
    }
    
    if ($resolvedSource.Path -eq $resolvedDestination) {
        throw "FATAL: Source and destination paths cannot be identical. This would cause recursive copying."
    }
    
    # Ensure destination directory exists
    if (-not (Test-Path $DestinationPath -PathType Container)) {
        Write-AegisLog "Creating destination directory: $DestinationPath"
        New-Item -Path $DestinationPath -ItemType Directory -Force | Out-Null
    }
    
    # Check PowerShell version for advanced features
    if ($PSVersionTable.PSVersion.Major -lt 5) {
        Write-AegisLog "Warning: PowerShell 5.0+ recommended for optimal functionality" "WARN"
    }
    
    Write-AegisLog "Environment validation completed"
}

# Create backup of existing structure
function Invoke-Backup {
    if ($Backup) {
        $backupPath = "$DestinationPath\backup_$(Get-Date -Format 'yyyyMMdd_HHmmss')"
        Write-AegisLog "Creating backup at: $backupPath"
        
        if (Test-Path $SourcePath) {
            Copy-Item -Path $SourcePath -Destination $backupPath -Recurse -Force
            Write-AegisLog "Backup completed successfully"
        }
    }
}

# Define RIFTLang directory structure per specification
function Initialize-ProjectStructure {
    Write-AegisLog "Initializing RIFTLang project structure per waterfall methodology"
    
    $directories = @(
        # Foundation Track (FT1) Structure
        "src\core",
        "src\parser",
        "src\memory",
        "src\threading",
        "src\policy",
        "src\nlink",
        
        # Aspirational Track (AS2) Structure  
        "src\advanced",
        "src\security",
        "src\platform",
        
        # Build and Distribution
        "build\debug",
        "build\release",
        "dist\windows",
        "dist\linux",
        "dist\macos",
        
        # Documentation Structure
        "docs\specification",
        "docs\api",
        "docs\developer",
        "docs\certification",
        
        # Testing Infrastructure
        "tests\unit",
        "tests\integration", 
        "tests\policy",
        "tests\concurrency",
        "tests\performance",
        
        # Configuration and Policy
        "config\policies",
        "config\environments",
        
        # Research and Development
        "research\poc",
        "research\experiments",
        "research\formal_methods",
        
        # External Dependencies
        "lib\librift",
        "lib\nlink", 
        "lib\gosilang",
        
        # Utilities and Tools
        "tools\generators",
        "tools\validators",
        "tools\profilers",
        
        # Governance and Compliance
        "governance\thread_safety",
        "governance\memory_contracts",
        "governance\zero_trust"
    )
    
    foreach ($dir in $directories) {
        $fullPath = Join-Path $DestinationPath $dir
        if (-not (Test-Path $fullPath)) {
            New-Item -Path $fullPath -ItemType Directory -Force
            Write-AegisLog "Created directory: $dir"
        }
    }
}

# File classification and movement logic
function Move-ProjectFiles {
    Write-AegisLog "Classifying and moving project files"
    
    # Get all files from source
    $sourceFiles = Get-ChildItem -Path $SourcePath -Recurse -File
    
    foreach ($file in $sourceFiles) {
        $targetPath = Get-FileDestination -File $file
        
        if ($targetPath) {
            $destinationFile = Join-Path $DestinationPath $targetPath
            $destinationDir = Split-Path $destinationFile -Parent
            
            # CRITICAL: Prevent self-overwrite operations
            if ($file.FullName -eq $destinationFile) {
                Write-AegisLog "Skipping self-referential copy: $($file.Name)" "WARN"
                continue
            }
            
            if (-not (Test-Path $destinationDir)) {
                New-Item -Path $destinationDir -ItemType Directory -Force
            }
            
            try {
                Copy-Item -Path $file.FullName -Destination $destinationFile -Force
                Write-AegisLog "Moved: $($file.Name) -> $targetPath"
            }
            catch {
                Write-AegisLog "Failed to move $($file.Name): $($_.Exception.Message)" "ERROR"
            }
        }
    }
}

# File classification logic based on RIFTLang specification
function Get-FileDestination {
    param([System.IO.FileInfo]$File)
    
    $fileName = $File.Name.ToLower()
    $extension = $File.Extension.ToLower()
    
    # Core RIFTLang files (per specification)
    switch -Regex ($fileName) {
        "^riftlang\.c$" { return "src\core\riftlang.c" }
        "^riftlang\.h$" { return "src\core\riftlang.h" }
        "^riftlang\.rift$" { return "src\core\riftlang.rift" }
        "^.*\.dll$" { return "dist\windows\$fileName" }
        "^.*\.exe$" { return "dist\windows\$fileName" }
        "^.*\.so$" { return "dist\linux\$fileName" }
        "^.*\.a$" { return "dist\linux\$fileName" }
        
        # PoC and Concurrency files
        "concurrency.*poc" { return "research\poc\concurrency\$fileName" }
        "rift.*concurrency" { return "src\threading\$fileName" }
        
        # Memory and governance
        "memory.*governance" { return "governance\memory_contracts\$fileName" }
        "memory.*alignment" { return "src\memory\$fileName" }
        
        # Policy files
        "policy" { return "config\policies\$fileName" }
        "governance" { return "governance\$fileName" }
        
        # Specification documents
        "specification" { return "docs\specification\$fileName" }
        "implementation.*spec" { return "docs\specification\$fileName" }
        
        # Formal methods and theory
        "formal.*language" { return "research\formal_methods\$fileName" }
        "chomsky|grammar" { return "research\formal_methods\$fileName" }
        "game.*theory" { return "research\formal_methods\$fileName" }
        
        # White papers and philosophy
        "rifter.*way|manifesto" { return "docs\developer\$fileName" }
        "whitepaper|ecosystem" { return "docs\specification\$fileName" }
        
        # LibRIFT components
        "librift" { return "lib\librift\$fileName" }
        
        # Build artifacts
        "makefile|cmake" { return "build\$fileName" }
        "\.vcxproj|\.sln" { return "build\windows\$fileName" }
        
        default {
            # Classify by extension
            switch ($extension) {
                ".c" { return "src\core\$fileName" }
                ".h" { return "src\core\$fileName" }
                ".rift" { return "src\core\$fileName" }
                ".md" { return "docs\$fileName" }
                ".pdf" { return "docs\specification\$fileName" }
                ".txt" { return "research\poc\$fileName" }
                ".json" { return "config\$fileName" }
                ".xml" { return "config\$fileName" }
                default { return "misc\$fileName" }
            }
        }
    }
}

# Generate project metadata and configuration files
function New-ProjectMetadata {
    Write-AegisLog "Generating project metadata and configuration files"
    
    # Create .riftproject file
    $projectConfig = @{
        name = "RIFTLang"
        version = "1.0.0"
        ecosystem = "RIFT"
        author = "Nnamdi Michael Okpala"
        organization = "OBINexus Computing"
        methodology = "Waterfall"
        tracks = @("Foundation", "Aspirational")
        components = @("LibRIFT", "RIFTLang", "NLINK", "GosiLang")
        created = Get-Date -Format "yyyy-MM-dd"
    } | ConvertTo-Json -Depth 3
    
    Set-Content -Path "$DestinationPath\.riftproject" -Value $projectConfig
    
    # Create README.md with project structure
    $readmeContent = @"
# RIFTLang Project Structure

This project follows the RIFTLang Implementation Specification v1.0 with waterfall methodology.

## Directory Structure

### Foundation Track (FT1)
- `src/core/` - Core RIFTLang implementation
- `src/parser/` - Token parsing and AST generation  
- `src/memory/` - Memory governance and alignment
- `src/threading/` - Thread safety and concurrency
- `src/policy/` - Policy validation engine
- `src/nlink/` - NLINK integration components

### Aspirational Track (AS2)
- `src/advanced/` - Advanced parser capabilities
- `src/security/` - Zero-trust security implementation
- `src/platform/` - Platform extensions

### Documentation
- `docs/specification/` - Technical specifications
- `docs/api/` - API documentation
- `docs/developer/` - Developer resources
- `docs/certification/` - Certification requirements

### Testing
- `tests/unit/` - Unit tests
- `tests/integration/` - Integration tests
- `tests/policy/` - Policy validation tests
- `tests/concurrency/` - Thread safety tests

### Governance
- `governance/thread_safety/` - Thread safety governance
- `governance/memory_contracts/` - Memory governance contracts
- `governance/zero_trust/` - Zero-trust policy framework

## Build Instructions

See build/ directory for platform-specific build configurations.

## Security Notice

All files follow RIFTLang governance principles and zero-trust architecture.
"@
    
    Set-Content -Path "$DestinationPath\README.md" -Value $readmeContent
    
    # Create build configuration
    $buildConfig = @'
# RIFTLang Build Configuration
# Generated by Aegis Development Team

CC=gcc
CFLAGS=-Wall -Wextra -Werror -z noexecstack -fstack-protector-strong -fpie
LDFLAGS=-pie -Wl,-z,relro,-z,now

# Foundation Track targets
FT1_TARGETS=riftlang_core riftlang_parser riftlang_memory riftlang_threading

# Aspirational Track targets  
AS2_TARGETS=riftlang_advanced riftlang_security riftlang_platform

# Test targets
TEST_TARGETS=unit_tests integration_tests policy_tests concurrency_tests

.PHONY: all clean test foundation aspirational

all: foundation aspirational

foundation: $(FT1_TARGETS)

aspirational: $(AS2_TARGETS)

test: $(TEST_TARGETS)

clean:
	rm -rf build/debug/* build/release/*
'@
    
    Set-Content -Path "$DestinationPath\Makefile" -Value $buildConfig
}

# Validation of refactored structure
function Test-RefactoredStructure {
    if ($Validate) {
        Write-AegisLog "Validating refactored structure against specification"
        
        $requiredFiles = @(
            "src\core\riftlang.c",
            "src\core\riftlang.h", 
            "src\core\riftlang.rift",
            "docs\specification",
            "tests\unit",
            "governance\thread_safety"
        )
        
        $validationErrors = @()
        
        foreach ($requiredPath in $requiredFiles) {
            $fullPath = Join-Path $DestinationPath $requiredPath
            if (-not (Test-Path $fullPath)) {
                $validationErrors += "Missing required path: $requiredPath"
            }
        }
        
        if ($validationErrors.Count -eq 0) {
            Write-AegisLog "Structure validation passed"
        } else {
            Write-AegisLog "Structure validation failed with errors:" "ERROR"
            $validationErrors | ForEach-Object { Write-AegisLog $_ "ERROR" }
        }
    }
}

# Generate security manifests
function New-SecurityManifests {
    Write-AegisLog "Generating security manifests for zero-trust architecture"
    
    $securityManifest = @{
        framework = "RIFT Zero-Trust"
        policies = @(
            "Continuous Authentication and Authorization",
            "Least Privilege Access Model", 
            "Micro-Segmentation",
            "Always-On Encryption",
            "Continuous Monitoring and Validation"
        )
        governance = @{
            memory = "Memory as Governance Contract"
            threading = "Thread Safety through Policy Mutex"
            concurrency = "Policy-Driven Concurrency Model"
        }
        validation_threshold = 0.85
    } | ConvertTo-Json -Depth 3
    
    Set-Content -Path "$DestinationPath\governance\security_manifest.json" -Value $securityManifest
}

# Main execution flow
function Invoke-RIFTLangRefactor {
    try {
        Write-AegisLog "Starting RIFTLang directory structure refactoring"
        Write-AegisLog "Source: $SourcePath"
        Write-AegisLog "Destination: $DestinationPath"
        
        # Phase 1: Environment validation
        Test-Environment
        
        # Phase 2: Backup existing structure  
        Invoke-Backup
        
        # Phase 3: Initialize new structure
        Initialize-ProjectStructure
        
        # Phase 4: Move and classify files
        Move-ProjectFiles
        
        # Phase 5: Generate metadata and configurations
        New-ProjectMetadata
        
        # Phase 6: Generate security manifests
        New-SecurityManifests
        
        # Phase 7: Validate final structure
        Test-RefactoredStructure
        
        Write-AegisLog "RIFTLang refactoring completed successfully"
        Write-AegisLog "Project ready for R&D PoC milestone development"
        
    } catch {
        Write-AegisLog "Refactoring failed: $($_.Exception.Message)" "ERROR"
        throw
    }
}

# Execute main function
Invoke-RIFTLangRefactor
