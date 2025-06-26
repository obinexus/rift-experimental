<#
.SYNOPSIS
    Creates and maintains a search index for RIFTLang repository files.

.DESCRIPTION
    This script indexes the content of all text-based files in the RIFTLang repository,
    enabling fast searches across code and documentation.

.PARAMETER Path
    The root directory to index. Defaults to the current directory.

.PARAMETER Rebuild
    Forces a complete rebuild of the index instead of incremental updates.

.PARAMETER Query
    A search term to look for in the indexed content.
#>

param(
    [string]$Path = ".",
    [switch]$Rebuild,
    [string]$Query
)

# Define index file location
$indexFile = Join-Path $Path ".rift-search-index.xml"

# File types to index
$fileExtensions = @(".md", ".c", ".h", ".ps1", ".rift", ".gs", ".txt")

# Class to store file metadata and content
class IndexEntry {
    [string]$FilePath
    [datetime]$LastModified
    [string]$Content
    [hashtable]$Keywords
    
    IndexEntry([string]$path, [datetime]$modified, [string]$content) {
        $this.FilePath = $path
        $this.LastModified = $modified
        $this.Content = $content
        $this.Keywords = @{}
        
        # Extract keywords and their positions
        $words = $content -split '\W+' | Where-Object { $_.Length -gt 3 } | Select-Object -Unique
        foreach ($word in $words) {
            if ($word.Length -gt 0) {
                $this.Keywords[$word.ToLower()] = $true
            }
        }
    }
}

function Initialize-Index {
    [CmdletBinding()]
    param()
    
    Write-Host "Initializing search index..." -ForegroundColor Cyan
    return @{}
}

function Update-Index {
    [CmdletBinding()]
    param(
        [hashtable]$Index,
        [string]$BasePath
    )
    
    $files = Get-ChildItem -Path $BasePath -Recurse -File | 
        Where-Object { $fileExtensions -contains $_.Extension }
    
    $totalFiles = $files.Count
    $processedFiles = 0
    
    foreach ($file in $files) {
        $processedFiles++
        $relativePath = $file.FullName.Substring($BasePath.Length).TrimStart('\', '/')
        $progress = [math]::Round(($processedFiles / $totalFiles) * 100)
        
        Write-Progress -Activity "Indexing files" -Status "$progress% Complete" -PercentComplete $progress -CurrentOperation $relativePath
        
        # Skip if file hasn't changed since last index
        if (-not $Rebuild -and 
            $Index.ContainsKey($relativePath) -and 
            $Index[$relativePath].LastModified -eq $file.LastWriteTime) {
            continue
        }
        
        try {
            # Only process text files
            $content = Get-Content -Path $file.FullName -Raw -ErrorAction SilentlyContinue
            if ($null -ne $content) {
                $Index[$relativePath] = [IndexEntry]::new($relativePath, $file.LastWriteTime, $content)
            }
        }
        catch {
            Write-Warning "Could not index file: $relativePath"
        }
    }
    
    Write-Progress -Activity "Indexing files" -Completed
    return $Index
}

function Save-Index {
    [CmdletBinding()]
    param(
        [hashtable]$Index,
        [string]$IndexPath
    )
    
    Write-Host "Saving index with $($Index.Count) entries..." -ForegroundColor Cyan
    $Index | Export-Clixml -Path $IndexPath -Force
}

function Load-Index {
    [CmdletBinding()]
    param(
        [string]$IndexPath
    )
    
    if (Test-Path $IndexPath) {
        Write-Host "Loading existing index..." -ForegroundColor Cyan
        try {
            return Import-Clixml -Path $IndexPath
        }
        catch {
            Write-Warning "Index file corrupted. Rebuilding..."
            return Initialize-Index
        }
    }
    else {
        return Initialize-Index
    }
}

function Search-Index {
    [CmdletBinding()]
    param(
        [hashtable]$Index,
        [string]$SearchTerm
    )
    
    Write-Host "Searching for: $SearchTerm" -ForegroundColor Green
    
    $searchTerms = $SearchTerm.ToLower() -split '\s+' | Where-Object { $_.Length -gt 1 }
    $results = @{}
    
    foreach ($entry in $Index.GetEnumerator()) {
        $score = 0
        $fileContent = $entry.Value.Content.ToLower()
        
        foreach ($term in $searchTerms) {
            if ($entry.Value.Keywords.ContainsKey($term)) {
                # Give higher score for keyword matches
                $score += 10
            }
            
            # Add score based on number of occurrences
            $occurrences = ([regex]::Matches($fileContent, [regex]::Escape($term))).Count
            $score += $occurrences
        }
        
        if ($score -gt 0) {
            $results[$entry.Key] = $score
        }
    }
    
    # Sort results by score
    $sortedResults = $results.GetEnumerator() | Sort-Object Value -Descending
    
    # Display results
    if ($sortedResults.Count -eq 0) {
        Write-Host "No matches found." -ForegroundColor Yellow
    }
    else {
        Write-Host "Found $($sortedResults.Count) results:" -ForegroundColor Cyan
        
        foreach ($result in $sortedResults) {
            $filePath = $result.Key
            $score = $result.Value
            $snippet = Get-SearchSnippet -Content $Index[$filePath].Content -SearchTerms $searchTerms
            
            Write-Host "`n[$score] $filePath" -ForegroundColor Yellow
            Write-Host $snippet -ForegroundColor Gray
        }
    }
}

function Get-SearchSnippet {
    param(
        [string]$Content,
        [string[]]$SearchTerms
    )
    
    $maxSnippetLength = 150
    $lowerContent = $Content.ToLower()
    $bestPosition = -1
    $highestScore = 0
    
    foreach ($term in $SearchTerms) {
        $positions = [regex]::Matches($lowerContent, [regex]::Escape($term))
        
        foreach ($pos in $positions) {
            $start = [Math]::Max(0, $pos.Index - 60)
            $end = [Math]::Min($lowerContent.Length, $pos.Index + $term.Length + 60)
            $snippet = $lowerContent.Substring($start, $end - $start)
            
            $score = 0
            foreach ($t in $SearchTerms) {
                $score += ([regex]::Matches($snippet, [regex]::Escape($t))).Count
            }
            
            if ($score -gt $highestScore) {
                $highestScore = $score
                $bestPosition = $start
            }
        }
    }
    
    if ($bestPosition -ge 0) {
        $start = [Math]::Max(0, $bestPosition)
        $length = [Math]::Min($maxSnippetLength, $Content.Length - $start)
        $snippet = $Content.Substring($start, $length)
        
        # Add ellipsis if needed
        if ($start -gt 0) { $snippet = "..." + $snippet }
        if (($start + $length) -lt $Content.Length) { $snippet = $snippet + "..." }
        
        # Highlight search terms (using simpler highlighting for PowerShell console)
        foreach ($term in $SearchTerms) {
            $pattern = [regex]::Escape($term)
            $snippet = $snippet -replace "(?i)($pattern)", "[$1]"
        }
        
        return $snippet
    }
    
    # Return the beginning of the content if no match found
    return $Content.Substring(0, [Math]::Min($maxSnippetLength, $Content.Length)) + "..."
}

# Main execution
$absolutePath = Resolve-Path $Path

if ($Query) {
    # Search mode
    $index = Load-Index -IndexPath $indexFile
    Search-Index -Index $index -SearchTerm $Query
}
else {
    # Index mode
    $index = $Rebuild ? (Initialize-Index) : (Load-Index -IndexPath $indexFile)
    $updatedIndex = Update-Index -Index $index -BasePath $absolutePath
    Save-Index -Index $updatedIndex -IndexPath $indexFile
    
    Write-Host "Index created successfully. Use -Query parameter to search." -ForegroundColor Green
    Write-Host "Example: .\search-index.ps1 -Query 'thread safety'" -ForegroundColor Cyan
}
