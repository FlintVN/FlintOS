# SDK build scripts

These scripts orchestrate the SDK build. FlintJDK and FlintMIDP are built by
their own scripts inside each submodule. FlintUI is built from the dedicated
`flintui` submodule tracking the `flint.ui` branch. FlintOS builds its local
`flintos.device` API and copies all runtime JARs into `files/lib`.

Requirements:

- JDK 17 or newer (`javac` and `jar` in `PATH`)
- initialized submodules: `git submodule update --init --recursive`

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
