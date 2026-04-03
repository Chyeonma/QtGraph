param(
    [string]$BuildDir = "build",
    [string]$Configuration = "Release",
    [string]$AppName = "QtGraph",
    [string]$OutputRoot = "dist"
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Resolve-ExePath {
    param(
        [string]$SearchBuildDir,
        [string]$SearchConfiguration,
        [string]$SearchAppName
    )

    $candidates = @(
        (Join-Path $SearchBuildDir "$SearchAppName.exe"),
        (Join-Path (Join-Path $SearchBuildDir $SearchConfiguration) "$SearchAppName.exe")
    )

    foreach ($candidate in $candidates) {
        if (Test-Path $candidate) {
            return (Resolve-Path $candidate).Path
        }
    }

    $discoveredExe = Get-ChildItem -Path $SearchBuildDir -Filter "$SearchAppName.exe" -Recurse -File | Select-Object -First 1
    if ($null -ne $discoveredExe) {
        return $discoveredExe.FullName
    }

    throw "Could not find $SearchAppName.exe inside '$SearchBuildDir'. Build the project in Release mode first."
}

function Resolve-WindowsDeployQt {
    if ($env:QT_ROOT_DIR) {
        $qtDeployPath = Join-Path $env:QT_ROOT_DIR "bin\windeployqt.exe"
        if (Test-Path $qtDeployPath) {
            return (Resolve-Path $qtDeployPath).Path
        }
    }

    $command = Get-Command "windeployqt.exe" -ErrorAction SilentlyContinue
    if ($null -ne $command) {
        return $command.Source
    }

    throw "Could not find windeployqt.exe. Make sure Qt is installed and available in QT_ROOT_DIR or PATH."
}

$resolvedBuildDir = (Resolve-Path $BuildDir).Path
$exePath = Resolve-ExePath -SearchBuildDir $resolvedBuildDir -SearchConfiguration $Configuration -SearchAppName $AppName
$windeployqtPath = Resolve-WindowsDeployQt

$outputRootPath = Join-Path (Get-Location) $OutputRoot
$deployDir = Join-Path $outputRootPath "$AppName-windows-portable"
$zipPath = Join-Path $outputRootPath "$AppName-windows-portable.zip"

if (Test-Path $deployDir) {
    Remove-Item -Path $deployDir -Recurse -Force
}

if (Test-Path $zipPath) {
    Remove-Item -Path $zipPath -Force
}

New-Item -ItemType Directory -Path $deployDir -Force | Out-Null
Copy-Item -Path $exePath -Destination $deployDir -Force

& $windeployqtPath --release --compiler-runtime (Join-Path $deployDir "$AppName.exe")

Compress-Archive -Path $deployDir -DestinationPath $zipPath -Force

Write-Host "Portable package created at: $zipPath"
