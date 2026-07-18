[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

$root = Split-Path -Parent $PSScriptRoot
$jdkRoot = Join-Path $root 'flintjdk'
$uiRoot = Join-Path $root 'flintui'
$midpRoot = Join-Path $root 'flintmidp'
$libraryRoot = Join-Path $root 'files/lib'
$uiOutput = Join-Path $root 'build/sdk/flint.ui'
$deviceOutput = Join-Path $root 'build/sdk/flintos.device'

foreach($tool in @('javac', 'jar')) {
    if(-not (Get-Command $tool -ErrorAction SilentlyContinue)) {
        throw "$tool was not found in PATH. Install JDK 17 or newer."
    }
}
$jdkBuild = Join-Path $jdkRoot 'scripts/build.ps1'
$midpBuild = Join-Path $midpRoot 'scripts/build.ps1'
foreach($script in @($jdkBuild, $midpBuild)) {
    if(-not (Test-Path -LiteralPath $script -PathType Leaf)) {
        throw "Missing submodule build script: $script. Run git submodule update --init --recursive."
    }
}

New-Item -ItemType Directory -Force -Path $libraryRoot | Out-Null

Write-Host 'Delegating FlintJDK build to its submodule...'
& $jdkBuild
if($LASTEXITCODE -ne 0) { throw 'FlintJDK submodule build failed.' }
Get-ChildItem -LiteralPath (Join-Path $jdkRoot 'bin/run') -Filter '*.jar' -File |
    Copy-Item -Destination $libraryRoot -Force

Write-Host 'Building FlintUI from its dedicated submodule...'
if(-not (Test-Path -LiteralPath (Join-Path $uiRoot 'src/flint.ui/module-info.java') -PathType Leaf)) {
    throw "Missing FlintUI submodule. Run git submodule update --init --recursive."
}
if(Test-Path -LiteralPath $uiOutput) { Remove-Item -LiteralPath $uiOutput -Recurse -Force }
New-Item -ItemType Directory -Force -Path $uiOutput | Out-Null
& javac -Xlint:all '-XDstringConcat=inline' --release 17 -encoding UTF-8 -d $uiOutput `
    --module-path $libraryRoot --module-source-path (Join-Path $uiRoot 'src') --module flint.ui
if($LASTEXITCODE -ne 0) { throw 'FlintUI compilation failed.' }
& jar cf0m (Join-Path $libraryRoot 'flint.ui.jar') (Join-Path $jdkRoot 'META-INF/MANIFEST.MF') `
    -C (Join-Path $uiOutput 'flint.ui') .
if($LASTEXITCODE -ne 0) { throw 'Failed to package flint.ui.jar.' }

Write-Host 'Building FlintOS device API...'
if(Test-Path -LiteralPath $deviceOutput) {
    Remove-Item -LiteralPath $deviceOutput -Recurse -Force
}
New-Item -ItemType Directory -Force -Path $deviceOutput | Out-Null
& javac -Xlint:all '-XDstringConcat=inline' --release 17 -encoding UTF-8 -d $deviceOutput `
    --module-path $libraryRoot --module-source-path (Join-Path $root 'java/src') --module flintos.device
if($LASTEXITCODE -ne 0) { throw 'FlintOS device API compilation failed.' }
& jar cf0m (Join-Path $libraryRoot 'flintos.device.jar') (Join-Path $root 'java/META-INF/MANIFEST.MF') `
    -C (Join-Path $deviceOutput 'flintos.device') .
if($LASTEXITCODE -ne 0) { throw 'Failed to package flintos.device.jar.' }

Write-Host 'Delegating FlintMIDP build to its submodule...'
& $midpBuild -LibraryRoot $libraryRoot
if($LASTEXITCODE -ne 0) { throw 'FlintMIDP submodule build failed.' }
foreach($artifact in @('midp.jar', 'flintos.midp.jar', 'm3g.jar')) {
    Copy-Item -LiteralPath (Join-Path $midpRoot "bin/run/$artifact") -Destination $libraryRoot -Force
}

Write-Host "SDK libraries are ready in: $libraryRoot"
