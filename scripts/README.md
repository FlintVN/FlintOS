# SDK build scripts

These scripts build the complete FlintOS SDK from source. All build logic lives
here in the FlintOS repository — no build scripts are required inside submodules.

## What gets built

| Component | Source | Output JARs |
|---|---|---|
| FlintJDK | `flintjdk/src/` (4 modules) | `java.base.jar`, `flint.io.jar`, `flint.net.jar`, `flint.drawing.jar` |
| FlintUI | `flintui/src/` (flint.ui module) | `flint.ui.jar` |
| FlintOS Device API | `java/src/` (flintos.device module) | `flintos.device.jar` |
| FlintMIDP | `flintmidp/src/` | `midp.jar`, `flintos.midp.jar`, `m3g.jar` |

All JARs are placed in `files/lib/` at the end of the build.

## Requirements

- **JDK 17 or newer** (`javac` and `jar` in `PATH`)
- **JDK 8** (for `rt.jar` — required by FlintMIDP which targets Java 8)
- **Initialized submodules**: `git submodule update --init --recursive`

### JDK 8 rt.jar

The FlintMIDP component compiles against the Java 8 runtime. Set the `JDK8_RT`
environment variable to the path of `rt.jar` before running the build:

```bat
rem Windows Command Prompt
set JDK8_RT=C:\Program Files\Eclipse Adoptium\jdk-8.0.492.9-hotspot\jre\lib\rt.jar
```

```powershell
# Windows PowerShell
$env:JDK8_RT = 'C:\Program Files\Eclipse Adoptium\jdk-8.0.492.9-hotspot\jre\lib\rt.jar'
```

```sh
# Linux / Git Bash
export JDK8_RT="C:/Program Files/Eclipse Adoptium/jdk-8.0.492.9-hotspot/jre/lib/rt.jar"
```

If `JDK8_RT` is not set, the script will try common default locations.

## Running the build

Run one script for your shell:

```sh
# Linux or Git Bash on Windows
./scripts/build-sdks.sh
```

```bat
rem Windows Command Prompt
scripts\build-sdks.bat
```

```powershell
# Windows PowerShell / PowerShell
.\scripts\build-sdks.ps1
```

Each entry point contains the complete build flow and does not invoke either
of the other platform scripts.

## Build order

1. **FlintJDK** — compiles 4 Java 17 modules, copies JARs to `files/lib/`
2. **FlintUI** — compiles `flint.ui` module against FlintJDK JARs
3. **FlintOS Device API** — compiles `flintos.device` module against FlintJDK JARs
4. **FlintMIDP** — compiles MIDP sources (Java 8 target) against all preceding JARs, then runs `MidpJarPackager` to split into `midp.jar`, `flintos.midp.jar`, and `m3g.jar`
5. **Validate** — checks all JAR manifests in `files/lib/`
