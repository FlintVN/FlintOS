# FlintOS

FlintOS is a lightweight embedded operating environment for ESP32-class devices built with **native C/C++ system services** and **Java applications running on FlintJVM**.

FlintOS does **not** use Python or MicroPython as its implementation runtime. MicroPythonOS is used only as product inspiration for launcher, app store, OTA, touch UI, and lightweight embedded OS concepts.

## Architecture

```text
ESP-IDF native boot
  -> FlintOS native core / HAL / services
  -> FlintJVM host bridge
  -> FlintJDK APIs
  -> Java system apps and user apps
```

## Repository layout

```text
main/                         ESP-IDF entrypoint
native/                       C/C++ native services, HAL, JVM bridge
java/modules/                 FlintOS Java API modules
java/system_apps/             Java system apps
docs/architecture/            Architecture docs and ADRs
docs/scrum/                   Scrum-aligned backlog and sprint docs
```

## Related FlintVN projects

- FlintESPJVM: ESP-IDF integration for FlintJVM on ESP32 devices
- FlintJDK: Java core libraries and embedded APIs for FlintJVM
- FlintJVMDebug: VS Code debugging support
- FlintToolHub / FlintTool: future desktop tooling and deployment support

## Initial build direction

Native side will use ESP-IDF:

```bash
idf.py set-target esp32s3
idf.py build
```

Java side will align with FlintJDK module tooling.

## Current status

This repository is being reset from an early Python prototype into the Java/native FlintOS architecture.

See:

- `docs/architecture/FlintOS-system-design.md`
- `docs/architecture/ADR-0001-java-native-flintos.md`
- `docs/architecture/TESTING_STRATEGY.md`
- `docs/scrum/PRODUCT_BACKLOG.md`
- `docs/scrum/SPRINT_001.md`
