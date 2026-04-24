# FlintOS System Design

## Goal

FlintOS is a lightweight operating environment for embedded devices built around **Java applications on FlintJVM** and **native C/C++ system services**.

FlintOS is **not** a Python/MicroPython runtime. MicroPythonOS is only a product/design reference for user experience ideas such as launcher, app model, app store, OTA, touch UI, and multi-device support. FlintOS should implement those ideas using the FlintVN stack:

- **Native C/C++** for boot, HAL, drivers, scheduler integration, storage, networking, display, input, and runtime services.
- **FlintJVM / FlintESPJVM** for running Java apps on ESP32-class hardware.
- **FlintJDK** for the Java-side API surface exposed to apps.
- **FlintJVMDebug** for developer debugging.
- **FlintToolHub / FlintTool** for desktop tooling, flashing, packaging, deployment, and device management.

## FlintVN reference modules

Relevant local repositories:

```text
D:/Workspace/Project/FlintVN/FlintESPJVM
D:/Workspace/Project/FlintVN/FlintJDK
D:/Workspace/Project/FlintVN/FlintJVMDebug
D:/Workspace/Project/FlintVN/FlintExample
D:/Workspace/Project/FlintVN/FlintToolHub
```

Important observations:

- `FlintESPJVM` brings FlintJVM to ESP-IDF and ESP32 devices.
- `FlintESPJVM` already uses a submodule path `ESPJVM/FlintJVM` pointing to `https://github.com/FlintVN/FlintJVM.git`.
- `FlintJDK` provides modules such as `java.base`, `flint.io`, and `flint.net`.
- `flint.io` already contains embedded IO APIs such as GPIO, ADC, DAC, I2C, I2S, SPI, serial, one-wire, and touch pad.
- `flint.net` already contains WiFi and socket-related classes.

These modules should be treated as the base substrate for FlintOS rather than replacing them with Python APIs.

## Design Principles

1. **Native core, Java apps**
   - Keep device-critical functionality in C/C++.
   - Run user/system apps as Java bytecode on FlintJVM.

2. **Use FlintJDK as the public API layer**
   - Apps should import Java APIs, not call native internals directly.
   - Native services are exposed through FlintJDK bindings.

3. **Board abstraction first**
   - Hardware differences belong in native board profiles and HAL drivers.
   - Java apps should see stable APIs.

4. **App lifecycle over ad-hoc execution**
   - Apps should have manifests, permissions, lifecycle hooks, and controlled launch/stop.

5. **Safe embedded operations**
   - Partition changes, OTA, flash writes, and device reset need explicit flows.
   - Preserve rollback/recovery path.

6. **Agent-friendly architecture**
   - Keep manifests, APIs, build commands, and logs structured so AI agents can reason about the system safely.

## Proposed Architecture

```text
ESP-IDF boot / native main
  ↓
flintos_native_core/                 C/C++
  kernel_services/
    boot_manager
    service_registry
    event_loop_bridge
    app_manager_native
    package_manager
    settings_store
    permission_manager
    ota_update_manager
    logger
  hal/
    board_profile
    display
    input_touch_keyboard
    storage_flash_sd
    network_wifi
    power
    sensors_camera_imu
  jvm_bridge/
    flintjvm_host
    native_method_bindings
    app_classloader
    exception_bridge
  ↓
FlintJVM / FlintESPJVM
  ↓
FlintJDK modules
  java.base
  flint.io
  flint.net
  flint.os              proposed
  flint.ui              proposed
  flint.app             proposed
  ↓
System Java apps
  Launcher
  Settings
  FileManager
  AppStore
  Diagnostics
  Updater
  ↓
User Java apps
```

## Recommended repository layout

```text
FlintOS/
  CMakeLists.txt
  sdkconfig.defaults
  partitions/
    partitions.csv
  main/
    CMakeLists.txt
    flintos_main.cpp
  native/
    core/
      boot_manager.cpp/.hpp
      service_registry.cpp/.hpp
      app_manager_native.cpp/.hpp
      settings_store.cpp/.hpp
      permission_manager.cpp/.hpp
      package_manager.cpp/.hpp
      ota_update_manager.cpp/.hpp
      logger.cpp/.hpp
    hal/
      board_profile.hpp
      display_driver.hpp
      input_driver.hpp
      storage_driver.hpp
      network_driver.hpp
      power_driver.hpp
    jvm_bridge/
      flintjvm_host.cpp/.hpp
      native_bindings.cpp/.hpp
  java/
    modules/
      flint.os/
      flint.ui/
      flint.app/
    system_apps/
      launcher/
      settings/
      file_manager/
      app_store/
      diagnostics/
  tools/
    package_app/
    install_app/
    image_builder/
  docs/
    architecture/
```

## Boot Flow

```text
1. ESP-IDF starts native `app_main()`.
2. Native boot manager initializes logging, storage, board profile, display/input/network drivers.
3. Native service registry starts settings, package manager, permission manager, OTA manager.
4. FlintJVM host initializes VM heap, classpath, native bindings, and system modules.
5. App manager loads installed app registry and system app manifests.
6. Launcher Java app starts as the first system app.
7. Input/display/events flow through native services into Java UI/app APIs.
```

## App Model

FlintOS apps are Java packages/classes plus a manifest.

Example manifest:

```json
{
  "id": "flint.settings",
  "name": "Settings",
  "version": "0.1.0",
  "type": "system",
  "runtime": "flintjvm",
  "mainClass": "flintos.settings.SettingsApp",
  "permissions": ["network.manage", "settings.write"],
  "minOs": "0.1.0",
  "minJdk": "0.1.0"
}
```

## Java app lifecycle

Proposed Java API:

```java
package flint.app;

public interface FlintApp {
    void onStart(AppContext context);
    void onEvent(AppEvent event);
    void onStop();
}
```

App context:

```java
public interface AppContext {
    flint.ui.Screen screen();
    flint.ui.Input input();
    flint.os.Settings settings();
    flint.os.Storage storage();
    flint.os.Logger logger();
    flint.net.WiFi wifi();
}
```

## Proposed FlintJDK modules

Add new Java modules on top of existing `java.base`, `flint.io`, and `flint.net`.

```text
flint.os
  AppManager
  Settings
  Storage
  Logger
  SystemInfo
  Power
  UpdateManager

flint.app
  FlintApp
  AppContext
  AppEvent
  AppManifest
  Permission

flint.ui
  Screen
  Color
  Font
  Widget
  Label
  Button
  ListView
  Dialog
  TextInput
  VirtualKeyboard
  TouchEvent
  KeyEvent
```

Native bindings implement the backend for these APIs.

## Native/JVM boundary

Use a narrow, stable bridge:

```text
Java API call
  ↓
FlintJDK native method / internal VM hook
  ↓
FlintJVM native binding
  ↓
C/C++ FlintOS service
  ↓
ESP-IDF / driver
```

Examples:

```java
Screen.clear();
Screen.drawText("Hello", 0, 0);
Settings.getString("wifi.ssid");
Storage.open("/data/app/file.txt");
```

Native side should validate permissions and arguments before performing operations.

## UI model

Inspired by MicroPythonOS, but implemented in Java + native display drivers.

### Phase 1

- basic framebuffer/display API
- text drawing
- simple launcher list
- key/touch navigation

### Phase 2

- Java widget toolkit
- focus management
- virtual keyboard
- dialogs/settings screens

### Phase 3

- touch gestures
- app switcher
- animations if device supports it
- app store UI

## Package format

Proposed `.flintapp` package:

```text
my-app.flintapp  (zip)
  app.json
  classes/
    ... compiled Java classes ...
  lib/
    optional jars/modules
  assets/
    icon.raw
    images/
```

Installation flow:

```text
validate manifest
check permissions
check minOs/minJdk
copy package to app storage
update app registry atomically
make app visible in launcher
```

## Storage model

Suggested native-managed paths:

```text
/flintos/system/apps/<app_id>/
/flintos/user/apps/<app_id>/
/flintos/data/<app_id>/
/flintos/config/os.json
/flintos/config/apps.json
/flintos/logs/system.log
/flintos/tmp/
```

Use atomic writes for registry/settings:

```text
write tmp → fsync/flush if available → rename
```

## Permissions

Soft permissions are still valuable on embedded devices:

```text
network
network.manage
storage.read
storage.write
settings.read
settings.write
device.info
device.power
io.gpio
io.i2c
io.spi
io.serial
ota.update
```

Native services enforce permissions at API boundaries.

## OTA and update design

OTA should be native-controlled, not app-controlled.

Flow:

```text
1. Check OS/app catalog manifest.
2. Download update to staging partition/storage.
3. Verify checksum/signature.
4. Apply update.
5. Reboot into new image or reload app registry.
6. Roll back if boot health check fails.
```

For early milestones, implement app package update before full firmware OTA.

## Development workflow

Native side:

```bash
idf.py set-target esp32s3
idf.py build
idf.py -p <port> flash
idf.py -p <port> monitor
```

Java side:

```bash
# In FlintJDK
make

# Build app classes/jars with FlintJDK modules
javac --system=none --module-source-path ...
jar --create ...
```

FlintOS should eventually provide tooling:

```bash
flintos package ./java/system_apps/launcher
flintos install --port COMx launcher.flintapp
flintos logs --port COMx
```

## Milestones

### M0: Correct direction and architecture

- Replace Python/MicroPython assumptions with Java + native C/C++ architecture.
- Define app manifest, lifecycle, module boundaries, and native/JVM bridge.
- Document relationship with FlintESPJVM and FlintJDK.

### M1: ESP-IDF native skeleton

- Add ESP-IDF project skeleton.
- Add native boot manager and logger.
- Initialize FlintJVM host from native app.
- Boot a minimal Java Launcher class.

### M2: FlintJDK OS modules

- Add `flint.app`, `flint.os`, and minimal `flint.ui` APIs.
- Bind display/input/settings/logging to native services.

### M3: App registry and launcher

- Implement manifest parser native-side or Java-side.
- Load system app manifests.
- Show launcher list.
- Launch Java apps by main class.

### M4: Storage/settings/permissions

- Add atomic settings store.
- Add app data directories.
- Add permission checks at native bridge.

### M5: Packaging and tool support

- Define `.flintapp` package.
- Add desktop package/install tooling.
- Integrate FlintToolHub/FlintTool.

### M6: OTA/app store

- Add app catalog.
- Add app update flow.
- Add firmware OTA with rollback.

## Immediate next tasks

1. Remove or quarantine Python prototype files if they are not part of the Java/native direction.
2. Create ESP-IDF native skeleton in `main/` and `native/`.
3. Add Java module stubs for `flint.app`, `flint.os`, `flint.ui`.
4. Define `app.json` schema for Java apps.
5. Prototype native boot → FlintJVM init → Java Launcher main class.
6. Add a design ADR documenting: "FlintOS uses Java on FlintJVM + native C/C++, not Python/MicroPython."

## Notes from MicroPythonOS reference

MicroPythonOS is useful for product inspiration only:

- launcher/app UX
- app store concept
- OTA updates
- multi-device hardware support
- touch UI and gestures
- lightweight embedded OS feel

Implementation should remain FlintVN-native:

```text
MicroPythonOS inspiration → FlintOS UX ideas
FlintJVM + FlintESPJVM + FlintJDK → FlintOS implementation substrate
ESP-IDF C/C++ → FlintOS native services and HAL
```
