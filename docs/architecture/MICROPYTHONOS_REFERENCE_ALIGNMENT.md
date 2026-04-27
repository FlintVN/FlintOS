# MicroPythonOS Reference Alignment

FlintOS uses MicroPythonOS documentation as a product and UX reference only. FlintOS remains a Java/native embedded OS: ESP-IDF + native C/C++ services + FlintJVM + FlintJDK APIs.

## Reference points reviewed

- MicroPythonOS architecture overview: thin OS, everything is an app, launcher, app store, OTA, touch UI.
- App lifecycle: Android-style create/start/resume/pause/stop/destroy, foreground-aware updates, activity stack.
- Intent routing: explicit/implicit intents, extras, flags, result flow, handler resolution.
- TaskManager: centralized background task scheduling, delay, timeout, safe UI update patterns.
- AppStore: remote catalog, download/install app extension model.

## FlintOS implementation mapping

| MicroPythonOS concept | FlintOS implementation |
| --- | --- |
| Thin OS / everything is an app | Java system apps: Launcher, Settings, Diagnostics, App Store, Updater, File Manager |
| Activity lifecycle | `flint.app.Activity` wrapper over `FlintApp`, with create/start/resume/pause/stop/destroy hooks |
| Intent routing | `flint.app.Intent` and `AppManager.startActivity/queryIntentActivities` API surface |
| Start activity for result | `AppResult` and `Activity.startActivityForResult` API contract, native routing pending |
| TaskManager | `flint.os.TaskManager` native API stub for scheduled/background work |
| DownloadManager | `flint.os.DownloadManager` native API stub for App Store/catalog downloads |
| App Store/catalog | `flintos.appstore.AppStoreApp`, `.flintapp` packaging, PackageManager/AppRegistry |
| OTA update app | `flintos.updater.UpdaterApp` + native `OTAUpdateManager` |
| Touch UI widgets | `flint.ui` widgets: Label, Button, ListView, Dialog, TextInput, VirtualKeyboard, events |
| HIL/test markers | `FLINTOS_TEST_*` boot marker flow and serial monitor parser |

## Boundaries

- Do not copy MicroPython runtime or Python app model into FlintOS.
- Python remains tooling only: build scripts, package scripts, HIL scripts, CI checks.
- Native services remain responsible for storage, package install, permissions, OTA, power, input, and network.
- Java apps interact through FlintJDK APIs only.

## Current implementation slice

Implemented host-checkable APIs and flows:

- Java system apps compile and package.
- Multi-app runtime image generation.
- App manifest validation for all system apps.
- Package checks for all system apps.
- Activity/Intent/TaskManager/DownloadManager Java API contracts.
- Native binding logs for new API surfaces.
- CI workflow for Java/runtime/package/HIL marker checks.

## Next native work

1. Implement real native `AppManager` activity stack and intent resolver.
2. Persist intent handlers from app manifests.
3. Implement native `TaskManager` on FreeRTOS/ESP timers.
4. Implement `DownloadManager` over ESP HTTP client.
5. Wire App Store catalog downloads into `PackageManager.installPackage`.
6. Add UI event dispatch from native input HAL to Java `AppEvent` / `TouchEvent` / `KeyEvent`.
