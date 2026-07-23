[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

$root = Split-Path -Parent $PSScriptRoot
$jdkRoot = Join-Path $root 'flintjdk'
$uiRoot = Join-Path $root 'flintui'
$midpRoot = Join-Path $root 'FlintJ2ME'
$libraryRoot = Join-Path $root 'files/lib'
$uiOutput = Join-Path $root 'build/sdk/flint.ui'
$deviceOutput = Join-Path $root 'build/sdk/flintos.device'

# ---- JDK 8 rt.jar location (required for FlintMIDP Java 8 compilation) ----
$jdk8Rt = $env:JDK8_RT
if (-not $jdk8Rt) {
    $candidates = @(
        'C:\Program Files\Eclipse Adoptium\jdk-8.0.492.9-hotspot\jre\lib\rt.jar',
        'C:\Program Files\Java\jdk1.8.0_202\jre\lib\rt.jar'
    )
    foreach ($c in $candidates) {
        if (Test-Path -LiteralPath $c -PathType Leaf) {
            $jdk8Rt = $c
            break
        }
    }
}

# ---- Prerequisite checks ----
foreach ($tool in @('javac', 'jar')) {
    if (-not (Get-Command $tool -ErrorAction SilentlyContinue)) {
        throw "$tool was not found in PATH. Install JDK 17 or newer."
    }
}
foreach ($dir in @("$jdkRoot/src", "$uiRoot/src", "$midpRoot/src", (Join-Path $root 'java/src'))) {
    if (-not (Test-Path -LiteralPath $dir -PathType Container)) {
        throw "Missing source directory: $dir. Run git submodule update --init --recursive."
    }
}
if (-not $jdk8Rt -or -not (Test-Path -LiteralPath $jdk8Rt -PathType Leaf)) {
    throw "JDK 8 rt.jar not found. Set `$env:JDK8_RT to the path of rt.jar.`nExample: `$env:JDK8_RT = 'C:\Program Files\Eclipse Adoptium\jdk-8.0.492.9-hotspot\jre\lib\rt.jar'"
}

New-Item -ItemType Directory -Force -Path $libraryRoot | Out-Null

# ---- FlintJDK ----
Write-Host 'Building FlintJDK (java.base, flint.io, flint.net, flint.drawing)...'
$jdkBuildDir = Join-Path $jdkRoot 'bin/run'
if (Test-Path -LiteralPath $jdkBuildDir) { Remove-Item -LiteralPath $jdkBuildDir -Recurse -Force }
foreach ($mod in @('java.base', 'flint.io', 'flint.net', 'flint.drawing')) {
    Write-Host "  Compiling module $mod..."
    & javac -Xlint:all '-XDstringConcat=inline' --release 17 -encoding UTF-8 `
        -d $jdkBuildDir --module $mod --module-source-path (Join-Path $jdkRoot 'src')
    if ($LASTEXITCODE -ne 0) { throw "FlintJDK module $mod compilation failed." }
    & jar --create --file (Join-Path $jdkBuildDir "$mod.jar") `
        --manifest (Join-Path $jdkRoot 'META-INF/MANIFEST.MF') -0 -C (Join-Path $jdkBuildDir $mod) .
    if ($LASTEXITCODE -ne 0) { throw "Failed to package $mod.jar." }
}
Get-ChildItem -LiteralPath $jdkBuildDir -Filter '*.jar' -File |
    Copy-Item -Destination $libraryRoot -Force
Write-Host 'FlintJDK build complete.'

# ---- FlintUI ----
Write-Host 'Building FlintUI from its dedicated submodule...'
if (-not (Test-Path -LiteralPath (Join-Path $uiRoot 'src/flint.ui/module-info.java') -PathType Leaf)) {
    throw "Missing FlintUI submodule. Run git submodule update --init --recursive."
}
if (Test-Path -LiteralPath $uiOutput) { Remove-Item -LiteralPath $uiOutput -Recurse -Force }
New-Item -ItemType Directory -Force -Path $uiOutput | Out-Null
& javac -Xlint:all '-XDstringConcat=inline' --release 17 -encoding UTF-8 -d $uiOutput `
    --module-path $libraryRoot --module-source-path (Join-Path $uiRoot 'src') --module flint.ui
if ($LASTEXITCODE -ne 0) { throw 'FlintUI compilation failed.' }
& jar cf0m (Join-Path $libraryRoot 'flint.ui.jar') (Join-Path $root 'java/META-INF/flint.ui.MF') `
    -C (Join-Path $uiOutput 'flint.ui') .
if ($LASTEXITCODE -ne 0) { throw 'Failed to package flint.ui.jar.' }
Write-Host 'FlintUI build complete.'

# ---- FlintOS device API ----
Write-Host 'Building FlintOS device API...'
if (Test-Path -LiteralPath $deviceOutput) { Remove-Item -LiteralPath $deviceOutput -Recurse -Force }
New-Item -ItemType Directory -Force -Path $deviceOutput | Out-Null
& javac -Xlint:all '-XDstringConcat=inline' --release 17 -encoding UTF-8 -d $deviceOutput `
    --module-path $libraryRoot --module-source-path (Join-Path $root 'java/src') --module flintos.device
if ($LASTEXITCODE -ne 0) { throw 'FlintOS device API compilation failed.' }
& jar cf0m (Join-Path $libraryRoot 'flintos.device.jar') (Join-Path $root 'java/META-INF/MANIFEST.MF') `
    -C (Join-Path $deviceOutput 'flintos.device') .
if ($LASTEXITCODE -ne 0) { throw 'Failed to package flintos.device.jar.' }
Write-Host 'FlintOS device API build complete.'

# ---- FlintMIDP ----
Write-Host 'Building FlintMIDP...'
$midpToolsDir = Join-Path $midpRoot 'bin/tools'
$midpRunDir = Join-Path $midpRoot 'bin/run'

if (Test-Path -LiteralPath $midpRunDir) { Remove-Item -LiteralPath $midpRunDir -Recurse -Force }
if (Test-Path -LiteralPath $midpToolsDir) { Remove-Item -LiteralPath $midpToolsDir -Recurse -Force }
New-Item -ItemType Directory -Force -Path (Join-Path $midpRunDir 'midp') | Out-Null
New-Item -ItemType Directory -Force -Path $midpToolsDir | Out-Null

# Build classpath from already-built JARs in files/lib
$midpCp = "$libraryRoot\flint.drawing.jar;$libraryRoot\flintos.device.jar"
if (Test-Path -LiteralPath (Join-Path $libraryRoot 'midp.jar') -PathType Leaf) {
    $midpCp = "$midpCp;$libraryRoot\midp.jar"
}

Write-Host '  Compiling MIDP sources (Java 8 target)...'
$midpSources = Get-ChildItem -LiteralPath (Join-Path $midpRoot 'src') -Filter '*.java' -Recurse -File |
    Sort-Object FullName | ForEach-Object { $_.FullName }

& javac -source 8 -target 8 '-XDstringConcat=inline' `
    -bootclasspath $jdk8Rt `
    -Xlint:all,-serial,-options -encoding UTF-8 `
    -classpath $midpCp `
    -d (Join-Path $midpRunDir 'midp') $midpSources
if ($LASTEXITCODE -ne 0) { throw 'FlintMIDP compilation failed.' }

Write-Host '  Packaging MIDP JARs...'
& javac --release 17 -encoding UTF-8 -d $midpToolsDir (Join-Path $midpRoot 'tools/MidpJarPackager.java')
if ($LASTEXITCODE -ne 0) { throw 'MidpJarPackager compilation failed.' }
& java -cp $midpToolsDir MidpJarPackager `
    (Join-Path $midpRunDir 'midp') (Join-Path $midpRoot 'src') `
    (Join-Path $midpRoot 'META-INF/MANIFEST.MF') `
    (Join-Path $midpRoot 'config/jsr118-api-classes.txt') `
    $midpRunDir run
if ($LASTEXITCODE -ne 0) { throw 'MidpJarPackager execution failed.' }

foreach ($artifact in @('midp.jar', 'flintos.midp.jar', 'm3g.jar')) {
    Copy-Item -LiteralPath (Join-Path $midpRunDir $artifact) -Destination $libraryRoot -Force
}
Write-Host 'FlintMIDP build complete.'

# ---- Validate ----
& java (Join-Path $PSScriptRoot 'ValidateJarManifests.java') $libraryRoot
if ($LASTEXITCODE -ne 0) { throw 'SDK JAR manifest validation failed.' }

Write-Host "SDK libraries are ready in: $libraryRoot"
