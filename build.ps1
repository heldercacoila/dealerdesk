<#
.SYNOPSIS
    Build and test automation script for DealerDesk on Windows.
.PARAMETER Test
    Build and execute the test suite (test_calc and test_dbfile).
.PARAMETER Clean
    Remove all build artifacts and test binaries.
#>
[CmdletBinding()]
param(
    [switch]$Test,
    [switch]$Clean
)

$ErrorActionPreference = "Stop"

function Initialize-CompilerEnvironment {
    if (Get-Command cl.exe -ErrorAction SilentlyContinue) {
        return
    }

    $vswherePath = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (-not (Test-Path $vswherePath)) {
        if (Get-Command gcc.exe -ErrorAction SilentlyContinue) {
            return
        }
        throw "Neither cl.exe nor vswhere.exe was found. Please install Visual Studio C++ Build Tools."
    }

    $installPath = & $vswherePath -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if (-not $installPath) {
        throw "Visual Studio with C++ tools component not detected."
    }

    $vcvarsPath = Join-Path $installPath "VC\Auxiliary\Build\vcvars64.bat"
    if (-not (Test-Path $vcvarsPath)) {
        throw "vcvars64.bat not found at '$vcvarsPath'."
    }

    Write-Host "Initializing MSVC build environment..." -ForegroundColor Cyan
    $envOutput = cmd.exe /c "call `"$vcvarsPath`" >nul && set"
    foreach ($line in $envOutput) {
        if ($line -match "^([^=]+)=(.*)$") {
            $name = $matches[1]
            $val = $matches[2]
            [System.Environment]::SetEnvironmentVariable($name, $val, [System.EnvironmentVariableTarget]::Process)
        }
    }

    if (-not (Get-Command cl.exe -ErrorAction SilentlyContinue)) {
        throw "Failed to locate cl.exe after initializing MSVC environment."
    }
}

function Invoke-Clean {
    Write-Host "Cleaning build artifacts..." -ForegroundColor Yellow
    $targets = @("dealerdesk.exe", "test_calc.exe", "test_dbfile.exe", "*.obj", "*.ilk", "*.pdb")
    foreach ($t in $targets) {
        Remove-Item -Path $t -Force -ErrorAction SilentlyContinue
    }
    Write-Host "Clean completed." -ForegroundColor Green
}

if ($Clean) {
    Invoke-Clean
    exit 0
}

Initialize-CompilerEnvironment

$commonFlags = @("/W3", "/D_CRT_SECURE_NO_WARNINGS", "/nologo")

if ($Test) {
    Write-Host "`n===> Building and running test_calc..." -ForegroundColor Cyan
    & cl.exe @commonFlags /Fe:test_calc.exe tests\test_calc.c calc.c
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Failed to compile test_calc.c"
        exit $LASTEXITCODE
    }

    & .\test_calc.exe
    if ($LASTEXITCODE -ne 0) {
        Write-Error "test_calc failed!"
        exit $LASTEXITCODE
    }

    Write-Host "`n===> Building and running test_dbfile..." -ForegroundColor Cyan
    & cl.exe @commonFlags /Fe:test_dbfile.exe tests\test_dbfile.c dbfile.c
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Failed to compile test_dbfile.c"
        exit $LASTEXITCODE
    }

    & .\test_dbfile.exe
    if ($LASTEXITCODE -ne 0) {
        Write-Error "test_dbfile failed!"
        exit $LASTEXITCODE
    }

    Remove-Item -Path test_calc.exe, test_dbfile.exe, *.obj -Force -ErrorAction SilentlyContinue
    Write-Host "`n===> All characterization tests passed successfully (100% Green)!" -ForegroundColor Green
    exit 0
}

# Normal application build
Write-Host "Building DealerDesk application..." -ForegroundColor Cyan
& cl.exe @commonFlags /Fe:dealerdesk.exe main.c calc.c dbfile.c screen.c
if ($LASTEXITCODE -ne 0) {
    Write-Error "Build failed!"
    exit $LASTEXITCODE
}

Remove-Item -Path *.obj -Force -ErrorAction SilentlyContinue
Write-Host "Build succeeded: dealerdesk.exe" -ForegroundColor Green
