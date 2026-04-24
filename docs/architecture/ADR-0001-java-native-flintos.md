# ADR-0001: FlintOS uses Java on FlintJVM with native C/C++ services

## Status

Proposed

## Context

FlintOS is part of the FlintVN ecosystem. Related projects include FlintJVM, FlintESPJVM, FlintJDK, FlintJVMDebug, FlintToolHub, and FlintExample.

The goal is to build an embedded operating environment for ESP32-class devices. While MicroPythonOS provides useful product inspiration, FlintOS should not be implemented as a Python/MicroPython runtime.

## Decision

FlintOS will use:

- Native C/C++ / ESP-IDF for boot, HAL, drivers, system services, storage, networking, display, input, permissions, OTA, and the FlintJVM host.
- FlintJVM / FlintESPJVM as the application runtime.
- Java applications as the primary app model.
- FlintJDK modules as the public API layer for apps.

MicroPythonOS is used only as UX/product inspiration, not as implementation architecture.

## Consequences

### Positive

- Aligns FlintOS with FlintVN's existing JVM ecosystem.
- Allows Java/Kotlin-style app development on constrained devices.
- Keeps hardware-critical operations in native C/C++.
- Enables reuse of FlintJDK modules such as `flint.io` and `flint.net`.
- Opens path to FlintJVMDebug and FlintToolHub integration.

### Negative / Trade-offs

- More complex than a pure script-based runtime.
- Requires careful native/JVM bridge design.
- Requires memory discipline due to JVM overhead on MCU hardware.
- App isolation is limited and must be enforced at API boundaries.

## Follow-up

- Define `flint.app`, `flint.os`, and `flint.ui` Java modules.
- Define native service boundaries and permission checks.
- Build minimal native boot → FlintJVM → Java launcher flow.
