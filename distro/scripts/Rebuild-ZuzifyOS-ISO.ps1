# Rebuild the official ZuzifyOS Windows Preview ISO from GitHub Release parts.
# Download every .partNNN file and the .sha256 file into this script's folder.
# This script only reads those parts and creates one ISO; it never modifies disks.
$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$baseName = "ZuzifyOS-Windows-Preview.iso"
$directory = Split-Path -Parent $MyInvocation.MyCommand.Path
$destination = Join-Path $directory $baseName
$checksumFile = Join-Path $directory "$baseName.sha256"

if (-not (Test-Path -LiteralPath $checksumFile -PathType Leaf)) {
    throw "Missing $baseName.sha256. Download all release assets into the same folder."
}
$parts = @(Get-ChildItem -LiteralPath $directory -File |
    Where-Object { $_.Name -match '^ZuzifyOS-Windows-Preview\.iso\.part[0-9]{3}$' } |
    Sort-Object Name)
if ($parts.Count -eq 0) {
    throw "No ISO parts found. Download every ZuzifyOS-Windows-Preview.iso.partNNN file."
}
for ($i = 0; $i -lt $parts.Count; $i++) {
    $expectedName = "{0}.part{1:D3}" -f $baseName, $i
    if ($parts[$i].Name -cne $expectedName) {
        throw "ISO part missing: $expectedName. Download every part."
    }
}
if (Test-Path -LiteralPath $destination) {
    throw "$baseName already exists. Move or remove the old ISO before rebuilding."
}

Write-Host "Reconstructing $baseName from $($parts.Count) parts..."
$output = [System.IO.File]::Open($destination, [System.IO.FileMode]::CreateNew,
                                 [System.IO.FileAccess]::Write, [System.IO.FileShare]::None)
try {
    foreach ($part in $parts) {
        Write-Host "Adding $($part.Name)..."
        $input = [System.IO.File]::OpenRead($part.FullName)
        try {
            $input.CopyTo($output)
        }
        finally {
            $input.Dispose()
        }
    }
}
finally {
    $output.Dispose()
}

Write-Host "Verifying SHA256 checksum..."
$expectedHash = ((Get-Content -LiteralPath $checksumFile -TotalCount 1) -split '\s+')[0]
$actualHash = (Get-FileHash -LiteralPath $destination -Algorithm SHA256).Hash
if ($actualHash -ine $expectedHash) {
    Remove-Item -LiteralPath $destination -Force
    throw "Checksum mismatch: one or more parts are missing or damaged. Re-download and retry."
}
Write-Host "Verified: $destination"
Write-Host "Attach this ISO to a VirtualBox Debian 64-bit virtual machine."
