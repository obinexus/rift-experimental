# RIFT-Documentation-Build.ps1
# Version: 1.3.2
# Last Updated: 2025-05-20
# Author: OBINexus Computing

[CmdletBinding()]
param(
    [Parameter(Mandatory=$true, Position=0, HelpMessage="Input Markdown file path")]
    [ValidateScript({Test-Path $_ -PathType Leaf})]
    [string]$InputFile,
    
    [Parameter(Mandatory=$false, HelpMessage="Output PDF file path")]
    [string]$OutputFile = [System.IO.Path]::ChangeExtension($InputFile, "pdf"),
    
    [Parameter(Mandatory=$false, HelpMessage="PDF engine selection")]
    [ValidateSet("pdflatex", "xelatex", "lualatex")]
    [string]$PdfEngine = "xelatex",
    
    [Parameter(Mandatory=$false, HelpMessage="Preserve Unicode box characters")]
    [switch]$PreserveBoxChars = $true,
    
    [Parameter(Mandatory=$false, HelpMessage="Add technical diagram support")]
    [switch]$EnableDiagrams = $true,
    
    [Parameter(Mandatory=$false, HelpMessage="Add mathematical notation support")]
    [switch]$EnableMath = $true,
    
    [Parameter(Mandatory=$false, HelpMessage="Submodule content integration")]
    [switch]$IncludeSubmodules = $false,
    
    [Parameter(Mandatory=$false, HelpMessage="Generate API documentation")]
    [switch]$GenerateApiDocs = $false,
    
    [Parameter(Mandatory=$false, HelpMessage="Template directory")]
    [string]$TemplateDir = (Join-Path $PSScriptRoot "templates")
)

# Configuration variables
$TempDir = Join-Path ([System.IO.Path]::GetTempPath()) ([System.Guid]::NewGuid().ToString())
$TempFile = Join-Path $TempDir "processed_content.md"
$LogFile = Join-Path $PSScriptRoot "logs/rift-docs-build.log"

# Ensure log and temp directories
$LogDir = Split-Path $LogFile -Parent
if (-not (Test-Path $LogDir)) {
    New-Item -Path $LogDir -ItemType Directory -Force | Out-Null
}
if (-not (Test-Path $TempDir)) {
    New-Item -Path $TempDir -ItemType Directory -Force | Out-Null
}

function Write-Log {
    param(
        [string]$Message,
        [string]$Level = "INFO",
        [switch]$NoConsole
    )
    $Timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    $LogMessage = "[$Timestamp] [$Level] $Message"
    Add-Content -Path $LogFile -Value $LogMessage
    if (-not $NoConsole) {
        switch ($Level) {
            "ERROR" { Write-Host $Message -ForegroundColor Red }
            "WARNING" { Write-Host $Message -ForegroundColor Yellow }
            "SUCCESS" { Write-Host $Message -ForegroundColor Green }
            default { Write-Host $Message }
        }
    }
}

function Test-CommandAvailable {
    param([string]$Command)
    $CommandPath = Get-Command $Command -ErrorAction SilentlyContinue
    return ($null -ne $CommandPath)
}

function Initialize-BuildEnvironment {
    Write-Log "Verifying build environment..."
    $MissingDeps = @()
    if (-not (Test-CommandAvailable "pandoc")) {
        $MissingDeps += "pandoc"
    }
    if (-not (Test-CommandAvailable $PdfEngine)) {
        $MissingDeps += $PdfEngine
    }
    if ($EnableDiagrams -and -not (Test-CommandAvailable "pandoc-diagrams")) {
        Write-Log "pandoc-diagrams not found. Diagram support will be disabled." -Level "WARNING"
        $script:EnableDiagrams = $false
    }
    if ($MissingDeps.Count -gt 0) {
        Write-Log "Missing required dependencies: $($MissingDeps -join ', ')" -Level "ERROR"
        return $false
    }
    if (-not (Test-Path $TemplateDir)) {
        Write-Log "Creating template directory: $TemplateDir"
        New-Item -Path $TemplateDir -ItemType Directory -Force | Out-Null
    }

    $DefaultTemplate = Join-Path $TemplateDir "rift-default.tex"
    if (-not (Test-Path $DefaultTemplate)) {
        Write-Log "Creating default template..."
        @"
% RIFT Default LaTeX Template
\usepackage{fancyhdr}
\usepackage{titlesec}
\usepackage{xcolor}
\usepackage{booktabs}
\usepackage{graphicx}
\usepackage{hyperref}

\definecolor{riftblue}{RGB}{0, 102, 204}
\definecolor{riftgray}{RGB}{60, 60, 60}

\hypersetup{
    colorlinks=true,
    linkcolor=riftblue,
    urlcolor=riftblue
}

\pagestyle{fancy}
\fancyhf{}
\fancyhead[L]{OBINexus RIFT Documentation}
\fancyhead[R]{\thepage}
\fancyfoot[C]{\textcopyright~OBINexus Computing \the\year}

\titleformat{\chapter}{\normalfont\huge\bfseries\color{riftblue}}{\thechapter}{1em}{}
\titleformat{\section}{\normalfont\Large\bfseries\color{riftblue}}{\thesection}{1em}{}
"@ | Out-File -FilePath $DefaultTemplate -Encoding UTF8
    }
    Write-Log "Build environment initialized successfully." -Level "SUCCESS"
    return $true
}

# Begin main execution
try {
    Write-Log "=========================================================="
    Write-Log "RIFT Documentation Build: Processing '$InputFile'..."
    Write-Log "Engine: $PdfEngine, PreserveBoxChars: $PreserveBoxChars"
    if (-not (Initialize-BuildEnvironment)) {
        Write-Log "Failed to initialize build environment. Build aborted." -Level "ERROR"
        exit 1
    }
    # (additional logic goes here, such as markdown processing and PDF generation)
}
catch {
    Write-Log "Unhandled exception: $_" -Level "ERROR"
    exit 1
}
finally {
    if (Test-Path $TempDir) {
        Remove-Item -Path $TempDir -Recurse -Force -ErrorAction SilentlyContinue
    }
}

Write-Log "Build process completed."
