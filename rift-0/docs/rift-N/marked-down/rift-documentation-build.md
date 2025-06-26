# RIFT Documentation Build System

## Overview

The RIFT Documentation Build System provides a robust, extensible mechanism for generating comprehensive technical documentation for the RIFT Ecosystem components. This system integrates with Git submodules to ensure documentation remains synchronized across multiple repositories while maintaining version consistency.

The build system consists of two primary components:
1. `RIFT-Documentation-Build.ps1` - Core documentation generation pipeline
2. `RIFT-Submodule-Manager.ps1` - Submodule integration and management utility

## System Requirements

- PowerShell 5.1 or higher
- Git (accessible in PATH)
- Pandoc 2.11 or higher
- LaTeX distribution (MiKTeX or TeX Live) with the following packages:
  - fontspec, unicode-math, lm-math, amsmath
  - geometry, hyperref, fancyhdr, tabulary
  - titlesec, graphicx, xcolor, booktabs
  - pgf, tikz (for diagrams)

## Core Features

- Unicode-compliant box-drawing character preservation
- Technical diagram and mathematical notation support
- Git submodule integration for multi-repository documentation
- Custom LaTeX template application
- API documentation generation
- Flexible PDF engine selection (pdfLaTeX, XeLaTeX, LuaLaTeX)

## Basic Usage

### Generate Documentation from a Single Markdown File

```powershell
.\RIFT-Documentation-Build.ps1 -InputFile "source/manual.md" -OutputFile "output/manual.pdf"
```

### Generate Documentation with Submodule Content

```powershell
.\RIFT-Documentation-Build.ps1 -InputFile "source/manual.md" -IncludeSubmodules
```

### Generate with API Documentation

```powershell
.\RIFT-Documentation-Build.ps1 -InputFile "source/manual.md" -GenerateApiDocs
```

## Advanced Configuration

### Parameter Reference

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| InputFile | String | (Required) | Source Markdown file path |
| OutputFile | String | [InputFile].pdf | Target PDF output path |
| PdfEngine | String | "xelatex" | PDF generation engine ("pdflatex", "xelatex", "lualatex") |
| PreserveBoxChars | Switch | $true | Preserve Unicode box-drawing characters |
| EnableDiagrams | Switch | $true | Enable technical diagram support |
| EnableMath | Switch | $true | Enable mathematical notation support |
| IncludeSubmodules | Switch | $false | Integrate documentation from submodules |
| GenerateApiDocs | Switch | $false | Generate API documentation |
| TemplateDir | String | "./templates" | Template directory path |

### Example: Complete Build with All Features

```powershell
.\RIFT-Documentation-Build.ps1 `
    -InputFile "docs/RIFT-specification.md" `
    -OutputFile "build/RIFT-specification.pdf" `
    -PdfEngine "xelatex" `
    -PreserveBoxChars `
    -EnableDiagrams `
    -EnableMath `
    -IncludeSubmodules `
    -GenerateApiDocs `
    -TemplateDir "templates/corporate"
```

## Submodule Integration

The documentation build system can incorporate content from Git submodules to produce cohesive, unified documentation.

### Prerequisites

1. Properly configured Git submodules in your repository
2. Submodules must contain Markdown documentation (typically in `docs/` or `README.md`)

### Submodule Configuration

Create a `rift-submodules.json` file in your repository root with the following structure:

```json
{
  "submodules": [
    {
      "path": "lib/librift",
      "description": "Core RIFT library for flexible translation",
      "docsPath": "docs"
    },
    {
      "path": "lib/riftlang",
      "description": "RIFT language implementation",
      "docsPath": "documentation"
    },
    {
      "path": "lib/gosilang",
      "description": "Gossip language implementation",
      "docsPath": ""
    }
  ]
}
```

### Managing Submodules with RIFT-Submodule-Manager.ps1

Initialize all submodules:
```powershell
.\RIFT-Submodule-Manager.ps1 -Action Init -Recursive
```

Update specific submodule to latest version:
```powershell
.\RIFT-Submodule-Manager.ps1 -Action Update -Target "lib/librift"
```

Check submodule status:
```powershell
.\RIFT-Submodule-Manager.ps1 -Action Status
```

Extract documentation from submodules:
```powershell
.\RIFT-Submodule-Manager.ps1 -Action ExtractDocs -TargetDir "temp/submodule-docs"
```

Switch a submodule to a specific branch:
```powershell
.\RIFT-Submodule-Manager.ps1 -Action SwitchBranch -Target "lib/riftlang" -Branch "develop"
```

List available branches for a submodule:
```powershell
.\RIFT-Submodule-Manager.ps1 -Action ListBranches -Target "lib/gosilang"
```

## Implementation Note

As discussed with Nnamdi Okpala during the RIFT implementation planning, this documentation system follows the established waterfall methodology and ensures systematic integration of all ecosystem components. The approach prioritizes a solid foundation for future expansions while maintaining backward compatibility with existing documentation formats.

## Template Customization

The build system supports custom LaTeX templates for consistent corporate styling:

1. Create a template file in your template directory (default: `./templates/rift-default.tex`)
2. The template should include LaTeX packages and styling directives
3. The build system will automatically apply this template during PDF generation

Example template structure:
```latex
% RIFT Documentation Template
\usepackage{fancyhdr}
\usepackage{titlesec}
\usepackage{xcolor}

\definecolor{riftblue}{RGB}{0, 102, 204}

\pagestyle{fancy}
\fancyhf{}
\fancyhead[L]{OBINexus RIFT Documentation}
\fancyhead[R]{\thepage}
\fancyfoot[C]{¸ OBINexus Computing \the\year}

\titleformat{\chapter}{\normalfont\huge\bfseries\color{riftblue}}{\thechapter}{1em}{}
```

## Troubleshooting

### Common Issues

1. **Missing LaTeX Packages**
   - Error message: "LaTeX Error: File 'fontspec.sty' not found"
   - Resolution: Install the missing package through your TeX distribution's package manager

2. **Submodule Path Issues**
   - Error message: "Submodule 'path' not found"
   - Resolution: Verify submodule paths in `.gitmodules` file and ensure they match those in `rift-submodules.json`

3. **Unicode Character Rendering Issues**
   - Symptom: Box-drawing characters appear as placeholders or squares
   - Resolution: Use XeLaTeX or LuaLaTeX engine with `-PreserveBoxChars` switch

### Logging

Both scripts maintain detailed logs in the `logs` directory:
- `logs/rift-docs-build.log` - Documentation build process log
- `logs/rift-submodule-manager.log` - Submodule operations log

Review these logs to diagnose issues during the build process.

## Integration with CI/CD Pipelines

The build system can be integrated into continuous integration workflows:

```yaml
# Example GitHub Actions workflow
steps:
  - uses: actions/checkout@v3
    with:
      submodules: true
  
  - name: Set up build environment
    run: |
      choco install pandoc
      choco install miktex
      
  - name: Build documentation
    run: |
      .\RIFT-Documentation-Build.ps1 -InputFile "docs/specification.md" -OutputFile "build/specification.pdf" -IncludeSubmodules -GenerateApiDocs
```

## Future Development

In accordance with the established development roadmap for the RIFT Ecosystem, the documentation build system will be enhanced to support:

1. Multi-language documentation generation
2. Interactive element embedding in PDF outputs
3. Automated validation of code examples against the current codebase
4. Integration with the RIFT Zero-Trust Policy Framework for sensitive documentation

For any questions or technical support regarding the RIFT Documentation Build System, please contact the OBINexus Computing development team.
