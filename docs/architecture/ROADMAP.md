# FlintOS Roadmap

## Direction

FlintOS is a Java + native C/C++ embedded OS environment:

```text
ESP-IDF native services + FlintJVM host + FlintJDK APIs + Java apps
```

It does not use Python/MicroPython as the implementation runtime. MicroPythonOS is used only as a UX/product reference.

## Short-term

- Document architecture and ADR for Java/native direction.
- Decide how to handle existing Python prototype files: remove, archive, or keep as legacy reference.
- Create ESP-IDF native project skeleton.
- Create native boot manager and logger.
- Initialize FlintJVM from native boot.
- Run a minimal Java Launcher app.

## Medium-term

- Add FlintJDK modules:
  - `flint.app`
  - `flint.os`
  - `flint.ui`
- Add native bindings for display, input, settings, storage, logging.
- Define app manifest schema.
- Add app registry and app launcher.
- Add permission checks at native/JVM boundary.

## Long-term

- Add `.flintapp` package format.
- Add app store/catalog.
- Add OTA and rollback.
- Integrate FlintToolHub/FlintTool for package install, logs, and device management.
- Integrate FlintJVMDebug for Java app debugging.
- Support multiple ESP32-family boards through native board profiles.

## First implementation slice

1. Add ADR-0001 for Java/native architecture.
2. Create `main/flintos_main.cpp` ESP-IDF entrypoint.
3. Create `native/core/logger.*` and `native/core/boot_manager.*`.
4. Create `native/jvm_bridge/flintjvm_host.*`.
5. Add Java module stubs for `flint.app`, `flint.os`, `flint.ui`.
6. Add `java/system_apps/launcher` as first Java system app.
7. Build path: native boot starts JVM and launches `flintos.launcher.LauncherApp`.
