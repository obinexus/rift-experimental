<#
.SYNOPSIS
    RiftLang Development Environment Setup Script
.DESCRIPTION
    Establishes the complete RiftLang development environment with Docker integration,
    Git LFS submodule tracking, and Pomodoro-based workflow components.
.NOTES
    Author: OBINexus Computing Team
    Version: 0.1.0
    Requirements:
    - PowerShell 5.1+
    - Docker Desktop
    - Git with LFS extension
#>

# Configuration Parameters
$CONFIG = @{
    # Base directory structures
    BaseDirectory = "C:\Dev\OBINexus\RIFTLang"
    DockerDirectory = "C:\Dev\OBINexus\RIFTLang\riftlang_mvp_docker"
    
    # Repository configurations
    GitHubOrg = "obinexus"
    Repositories = @(
        @{Name = "RIFTLang"; Drive = "R:"; Size = "89.42GB"; Type = "NTFS"},
        @{Name = "RIFT"; Drive = "S:"; Size = "94.40GB"; Type = "NTFS"},
        @{Name = "NLINK"; Drive = "T:"; Size = "80.48GB"; Type = "NTFS"},
        @{Name = "GOSILang"; Drive = "U:"; Size = "89.39GB"; Type = "NTFS"},
        @{Name = "RIFTersWay"; Drive = "V:"; Size = "93.41GB"; Type = "NTFS"}
    )
    
    # Docker configuration
    DockerImage = "riftlang-dev"
    DockerfileContent = @"
FROM debian:bullseye
RUN apt-get update && \
    apt-get install -y build-essential valgrind
WORKDIR /app
COPY . .
CMD ["bash"]
"@

    # Workflow configuration
    PomodoroSettings = @{
        WorkDuration = 25
        BreakDuration = 5
        LongBreakDuration = 15
        CyclesBeforeLongBreak = 4
    }
}

# Initialization Phase
function Initialize-Environment {
    Write-Host "Phase 1: Environment Initialization" -ForegroundColor Blue

    # Create base directories if they don't exist
    if (-not (Test-Path $CONFIG.BaseDirectory)) {
        New-Item -Path $CONFIG.BaseDirectory -ItemType Directory -Force
        Write-Host "  Created base directory: $($CONFIG.BaseDirectory)" -ForegroundColor Green
    }
    
    if (-not (Test-Path $CONFIG.DockerDirectory)) {
        New-Item -Path $CONFIG.DockerDirectory -ItemType Directory -Force
        Write-Host "  Created Docker directory: $($CONFIG.DockerDirectory)" -ForegroundColor Green
    }
    
    # Verify required tools
    $tools = @{
        "Docker" = { docker --version }
        "Git" = { git --version }
        "Git LFS" = { git lfs version }
    }
    
    foreach ($tool in $tools.Keys) {
        try {
            Invoke-Command -ScriptBlock $tools[$tool] | Out-Null
            Write-Host "  ✓ $tool detected" -ForegroundColor Green
        }
        catch {
            Write-Host "  ✗ $tool not found. Please install $tool to continue." -ForegroundColor Red
            exit 1
        }
    }
}

# Docker Configuration Phase
function Setup-DockerEnvironment {
    Write-Host "Phase 2: Docker Environment Configuration" -ForegroundColor Blue
    
    # Create Dockerfile
    $dockerfilePath = Join-Path $CONFIG.DockerDirectory "Dockerfile"
    $CONFIG.DockerfileContent | Out-File -FilePath $dockerfilePath -Encoding utf8 -Force
    Write-Host "  Created Dockerfile at: $dockerfilePath" -ForegroundColor Green
    
    # Build Docker image
    Set-Location $CONFIG.DockerDirectory
    try {
        Write-Host "  Building Docker image: $($CONFIG.DockerImage)..." -ForegroundColor Yellow
        docker build -t $CONFIG.DockerImage .
        Write-Host "  ✓ Docker image built successfully" -ForegroundColor Green
    }
    catch {
        Write-Host "  ✗ Failed to build Docker image: $_" -ForegroundColor Red
        exit 1
    }
    
    # Create docker-compose.yml for multi-container environment
    $dockerComposePath = Join-Path $CONFIG.DockerDirectory "docker-compose.yml"
    @"
version: '3.8'
services:
  riftlang-dev:
    image: $($CONFIG.DockerImage)
    volumes:
      - $($CONFIG.BaseDirectory):/app
    tty: true
"@ | Out-File -FilePath $dockerComposePath -Encoding utf8 -Force
    Write-Host "  Created docker-compose.yml at: $dockerComposePath" -ForegroundColor Green
}

# Repository Configuration Phase
function Setup-Repositories {
    Write-Host "Phase 3: Repository Configuration" -ForegroundColor Blue
    
    # Clone or initialize repositories
    foreach ($repo in $CONFIG.Repositories) {
        $repoPath = Join-Path $CONFIG.BaseDirectory $repo.Name
        
        if (-not (Test-Path $repoPath)) {
            Write-Host "  Initializing repository: $($repo.Name)" -ForegroundColor Yellow
            New-Item -Path $repoPath -ItemType Directory -Force | Out-Null
            
            Set-Location $repoPath
            git init
            git lfs install
            
            # Create .gitattributes for LFS
            @"
*.rift.so filter=lfs diff=lfs merge=lfs -text
*.rift.a filter=lfs diff=lfs merge=lfs -text
*.RIFT.2 filter=lfs diff=lfs merge=lfs -text
*.RIFT.3 filter=lfs diff=lfs merge=lfs -text
"@ | Out-File -FilePath (Join-Path $repoPath ".gitattributes") -Encoding utf8 -Force
            
            # Create basic README
            @"
# $($repo.Name)

Part of the RiftLang ecosystem by OBINexus Computing.

## Directory Structure
- Drive Mapping: $($repo.Drive)
- Allocated Size: $($repo.Size)
- Filesystem: $($repo.Type)

## Overview
This repository is part of the thread-safe RiftLang compiler ecosystem.
"@ | Out-File -FilePath (Join-Path $repoPath "README.md") -Encoding utf8 -Force
            
            # Initial commit
            git add .
            git commit -m "Initial repository setup for $($repo.Name)"
            
            # Create remote if needed
            # git remote add origin "https://github.com/$($CONFIG.GitHubOrg)/$($repo.Name).git"
            
            Write-Host "  ✓ Repository initialized: $($repo.Name)" -ForegroundColor Green
        }
        else {
            Write-Host "  Repository already exists: $($repo.Name)" -ForegroundColor Yellow
        }
    }
}

# Workflow Tools Phase
function Setup-WorkflowTools {
    Write-Host "Phase 4: Workflow Tools Configuration" -ForegroundColor Blue
    
    # Create Pomodoro timer script for development cycle management
    $pomodoroScriptPath = Join-Path $CONFIG.BaseDirectory "Start-PomodoroSession.ps1"
    
    @"
<#
.SYNOPSIS
    RiftLang Development Pomodoro Timer
.DESCRIPTION
    Implements the Pomodoro technique for focused development cycles 
    with automatic commit reminders and workflow checkpoints.
#>

# Pomodoro Configuration
`$workDuration = $($CONFIG.PomodoroSettings.WorkDuration)
`$breakDuration = $($CONFIG.PomodoroSettings.BreakDuration)
`$longBreakDuration = $($CONFIG.PomodoroSettings.LongBreakDuration)
`$cyclesBeforeLongBreak = $($CONFIG.PomodoroSettings.CyclesBeforeLongBreak)

function Start-PomodoroTimer {
    param (
        [int]`$Duration,
        [string]`$Type
    )
    
    `$endTime = (Get-Date).AddMinutes(`$Duration)
    
    # Display message
    if (`$Type -eq "Work") {
        Write-Host "`nStarting `$Duration-minute work session..." -ForegroundColor Green
        Write-Host "Focus on a single task in the RiftLang ecosystem." -ForegroundColor Green
    }
    else {
        Write-Host "`nStarting `$Duration-minute break..." -ForegroundColor Cyan
        Write-Host "Step away from the keyboard and rest." -ForegroundColor Cyan
    }
    
    # Timer loop
    while ((Get-Date) -lt `$endTime) {
        `$timeLeft = `$endTime - (Get-Date)
        `$minutesLeft = [math]::Floor(`$timeLeft.TotalMinutes)
        `$secondsLeft = `$timeLeft.Seconds
        
        Write-Host "`r`$Type session: `$minutesLeft minutes `$secondsLeft seconds remaining..." -NoNewline -ForegroundColor Yellow
        Start-Sleep -Seconds 1
    }
    
    # Session completed
    if (`$Type -eq "Work") {
        Write-Host "`r`nWork session completed!" -ForegroundColor Green
        
        # Prompt for git commit
        `$commitChanges = Read-Host "Would you like to commit your changes? (y/n)"
        if (`$commitChanges -eq "y") {
            `$commitMessage = Read-Host "Enter a commit message"
            git add .
            git commit -m "`$commitMessage"
            Write-Host "Changes committed." -ForegroundColor Green
        }
    }
    else {
        Write-Host "`r`nBreak completed! Ready to start the next session?" -ForegroundColor Cyan
        Read-Host "Press Enter to continue"
    }
}

# Main Pomodoro loop
`$cycleCount = 0
while (`$true) {
    `$cycleCount++
    
    # Work session
    Start-PomodoroTimer -Duration `$workDuration -Type "Work"
    
    # Determine break type
    if (`$cycleCount % `$cyclesBeforeLongBreak -eq 0) {
        Start-PomodoroTimer -Duration `$longBreakDuration -Type "Long Break"
    }
    else {
        Start-PomodoroTimer -Duration `$breakDuration -Type "Short Break"
    }
    
    # Ask if user wants to continue
    `$continue = Read-Host "Continue with another Pomodoro cycle? (y/n)"
    if (`$continue -ne "y") {
        break
    }
}
"@ | Out-File -FilePath $pomodoroScriptPath -Encoding utf8 -Force
    
    Write-Host "  ✓ Created Pomodoro workflow script: $pomodoroScriptPath" -ForegroundColor Green
    
    # Create ZetScript tracking for RiftersWay
    $zetScriptPath = Join-Path (Join-Path $CONFIG.BaseDirectory "RIFTersWay") "Track-ZetScript.ps1"
    
    @"
<#
.SYNOPSIS
    ZetScript Tracking for RiftersWay Ecosystem
.DESCRIPTION
    Manages and tracks Zettelkasten-style documentation for the V:\RiftersWay ecosystem.
#>

# Configuration
`$zetClassPath = "V:\RiftersWay\ZetClass"
`$ebookStructurePath = "V:\RiftersWay\EbookStructure"

# Create directory structure if it doesn't exist
if (-not (Test-Path `$zetClassPath)) {
    New-Item -Path `$zetClassPath -ItemType Directory -Force
}

if (-not (Test-Path `$ebookStructurePath)) {
    New-Item -Path `$ebookStructurePath -ItemType Directory -Force
}

# Initialize Zettelkasten index
`$indexPath = Join-Path `$zetClassPath "index.md"
if (-not (Test-Path `$indexPath)) {
    @"
# RiftersWay Zettelkasten Index

This document serves as the main hub for the RiftersWay knowledge base.

## Core Concepts
- [[thread-safety]]
- [[single-pass-systems]]
- [[automaton-architecture]]

## Implementation Guides
- [[docker-integration]]
- [[valgrind-memory-checks]]
- [[librift-implementation]]

## Development Methodology
- [[pomodoro-workflow]]
- [[thread-based-security]]
"@ | Out-File -FilePath `$indexPath -Encoding utf8 -Force
}

# Function to create a new zettel note
function New-ZettelNote {
    param (
        [Parameter(Mandatory=$true)]
        [string]$Title,
        
        [Parameter(Mandatory=$true)]
        [string]$Tags,
        [Parameter(Mandatory=$false)]
        [string]$Content = ""
    )
    
    # Create unique ID based on timestamp
    `$dateStamp = Get-Date -Format "yyyyMMddHHmmss"
    `$fileName = "`$dateStamp-`$(`$Title -replace ' ','-').md"
    `$filePath = Join-Path `$zetClassPath `$fileName
    
    # Create the note content
    @"
# `$Title
Date: $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")
Tags: `$Tags

`$Content

## References
- 

## Related Notes
- 
"@ | Out-File -FilePath `$filePath -Encoding utf8 -Force
    
    Write-Host "Created new note: `$filePath" -ForegroundColor Green
    return `$fileName
}

# Main menu
function Show-ZetMenu {
    Write-Host "===== RiftersWay ZetScript Tracking =====" -ForegroundColor Cyan
    Write-Host "1. Create new note"
    Write-Host "2. List all notes"
    Write-Host "3. Search notes by tag"
    Write-Host "4. Generate ebook structure"
    Write-Host "5. Exit"
    
    `$choice = Read-Host "Select an option"
    
    switch (`$choice) {
        "1" {
            `$title = Read-Host "Enter note title"
            `$tags = Read-Host "Enter tags (comma-separated)"
            New-ZettelNote -Title `$title -Tags `$tags
            Show-ZetMenu
        }
        "2" {
            Get-ChildItem -Path `$zetClassPath -Filter "*.md" | ForEach-Object {
                if (`$_.Name -ne "index.md") {
                    Write-Host `$_.Name
                }
            }
            Read-Host "Press Enter to continue"
            Show-ZetMenu
        }
        "3" {
            `$searchTag = Read-Host "Enter tag to search for"
            Get-ChildItem -Path `$zetClassPath -Filter "*.md" | ForEach-Object {
                `$content = Get-Content `$_.FullName -Raw
                if (`$content -match "Tags:.*`$searchTag") {
                    Write-Host `$_.Name
                }
            }
            Read-Host "Press Enter to continue"
            Show-ZetMenu
        }
        "4" {
            # Generate ebook structure from notes
            `$ebookOutlinePath = Join-Path `$ebookStructurePath "outline.md"
            @"
# RiftersWay: The Thread-Safe Programming Manifesto

## Table of Contents

"@ | Out-File -FilePath `$ebookOutlinePath -Encoding utf8 -Force
            
            # Group notes by tags and create chapters
            `$tags = @{}
            Get-ChildItem -Path `$zetClassPath -Filter "*.md" | ForEach-Object {
                if (`$_.Name -ne "index.md") {
                    `$content = Get-Content `$_.FullName -Raw
                    if (`$content -match "Tags: (.*)") {
                        `$noteTags = `$matches[1] -split ','
                        foreach (`$tag in `$noteTags) {
                            `$tag = `$tag.Trim()
                            if (-not `$tags.ContainsKey(`$tag)) {
                                `$tags[`$tag] = @()
                            }
                            `$tags[`$tag] += `$_.Name
                        }
                    }
                }
            }
            
            # Write chapters based on tags
            foreach (`$tag in `$tags.Keys) {
                Add-Content -Path `$ebookOutlinePath -Value "### Chapter: `$tag"
                foreach (`$note in `$tags[`$tag]) {
                    `$content = Get-Content (Join-Path `$zetClassPath `$note) -Raw
                    if (`$content -match "# (.*)") {
                        `$title = `$matches[1]
                        Add-Content -Path `$ebookOutlinePath -Value "- `$title"
                    }
                }
                Add-Content -Path `$ebookOutlinePath -Value ""
            }
            
            Write-Host "Generated ebook structure at: `$ebookOutlinePath" -ForegroundColor Green
            Read-Host "Press Enter to continue"
            Show-ZetMenu
        }
        "5" {
            return
        }
        default {
            Write-Host "Invalid option. Please try again." -ForegroundColor Red
            Show-ZetMenu
        }
    }
}

# Start the application
Show-ZetMenu
"@ | Out-File -FilePath $zetScriptPath -Encoding utf8 -Force
    
    Write-Host "  ✓ Created ZetScript tracking for RiftersWay: $zetScriptPath" -ForegroundColor Green
}

# Main script execution
function Start-Setup {
    Write-Host "RiftLang Development Environment Setup" -ForegroundColor Cyan
    Write-Host "=======================================" -ForegroundColor Cyan
    
    Initialize-Environment
    Setup-DockerEnvironment
    Setup-Repositories
    Setup-WorkflowTools
    
    Write-Host "`nSetup completed successfully!" -ForegroundColor Green
    Write-Host "To start development with Pomodoro technique, run:" -ForegroundColor Yellow
    Write-Host "  & '$($CONFIG.BaseDirectory)\Start-PomodoroSession.ps1'" -ForegroundColor Yellow
    
    Write-Host "`nTo track ZetScript for RiftersWay, run:" -ForegroundColor Yellow
    Write-Host "  & '$($CONFIG.BaseDirectory)\RIFTersWay\Track-ZetScript.ps1'" -ForegroundColor Yellow
    
    Write-Host "`nTo run the Docker development environment, run:" -ForegroundColor Yellow
    Write-Host "  cd '$($CONFIG.DockerDirectory)'" -ForegroundColor Yellow
    Write-Host "  docker-compose up -d" -ForegroundColor Yellow
    Write-Host "  docker exec -it riftlang-mvp-docker_riftlang-dev_1 bash" -ForegroundColor Yellow
}

# Execute the setup
Start-Setup