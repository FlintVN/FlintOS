# FlintOS Product Backlog

## Product goal

Build FlintOS as a Java + native C/C++ embedded operating environment for ESP32-class devices using FlintJVM, FlintESPJVM, and FlintJDK.

## Epics

### Epic 1: Native boot foundation

- As a developer, I can build an ESP-IDF FlintOS skeleton.
- As a system, FlintOS initializes native logging and boot manager.
- As a runtime, FlintOS can initialize a FlintJVM host placeholder.

### Epic 2: Java app runtime

- As an app developer, I can implement a Java app with `FlintApp` lifecycle methods.
- As the OS, FlintOS can launch a Java system app through FlintJVM.
- As the OS, FlintOS exposes `flint.app`, `flint.os`, and `flint.ui` modules.

### Epic 3: App registry and launcher

- As a user, I can see installed system/user apps in a launcher.
- As a developer, I can define app metadata through `app.json`.
- As the OS, FlintOS can validate app manifests before launch.

### Epic 4: Native services and HAL

- As the OS, FlintOS abstracts display, input, storage, network, and power through native HAL.
- As Java apps, APIs remain stable across board profiles.

### Epic 5: Tooling and deployment

- As a developer, I can package a Java app into `.flintapp`.
- As a developer, I can install apps via desktop tooling.
- As a developer, I can inspect logs and debug apps.

## Current sprint candidate

Focus: replace Python prototype with Java/native skeleton and align architecture docs.

Stories:

1. Remove Python prototype from active project structure.
2. Add native ESP-IDF boot skeleton.
3. Add FlintJVM host bridge placeholder.
4. Add Java API module stubs.
5. Add Java launcher app stub.
6. Add ADR and roadmap.
