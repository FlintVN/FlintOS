# FlintOS Testing Strategy

## Purpose

Define the execution and testing strategy for FlintOS as a Java + native C/C++ embedded OS environment.

This strategy is informed by embedded CI/HIL practices from `andrempmattos/tests-embedded-systems` and general embedded systems engineering skill requirements: C/C++, microcontrollers, peripherals, RTOS concepts, hardware interfaces, communication protocols, validation, and technical documentation.

## Testing pyramid

```text
Static checks
  ↓
Host unit tests
  ↓
Java module tests
  ↓
Native integration tests
  ↓
Emulator/simulator tests where possible
  ↓
Hardware-in-the-loop tests
  ↓
Long-run stability tests
```

## Test categories

### 1. Static checks

Run before build or review:

- C/C++ formatting and warnings
- Java modular compile
- manifest schema validation
- CMake structure check
- forbidden runtime checks, e.g. no Python runtime files in active tree

Suggested checks:

```bash
# Java module compile
javac -d build/java-check --module-source-path "java/modules;java/system_apps/launcher/src" --module flint.app,flint.os,flint.ui,flintos.launcher

# ESP-IDF build, when environment is active
idf.py build
```

### 2. Host unit tests

Goal: test logic without hardware.

Targets:

- app manifest parser
- settings store
- permission checks
- package manager
- app registry
- logger formatting
- native/JVM bridge argument validation

Prefer host-testable C++ modules with hardware dependencies behind interfaces.

### 3. Java module tests

Goal: validate Java APIs and system apps before device deployment.

Targets:

- `flint.app` lifecycle contracts
- `flint.os` system API behavior
- `flint.ui` API surface
- Launcher app behavior with fake `Screen` backend

Java modules should compile in CI even before native binding is complete.

### 4. Native integration tests

Goal: test native services together under ESP-IDF.

Targets:

- boot manager initialization order
- service registry
- settings store read/write
- storage mount
- permission manager
- JVM host lifecycle placeholder

### 5. Hardware-in-the-loop tests

Inspired by `tests-embedded-systems`, HIL should use a self-hosted runner with connected device.

Flow:

```text
build
→ flash device
→ monitor UART/serial logs
→ wait for pass/fail markers
→ collect logs/artifacts
→ reset device for next test
```

Expected serial markers:

```text
FLINTOS_TEST_START <test_name>
FLINTOS_TEST_PASS <test_name>
FLINTOS_TEST_FAIL <test_name> <reason>
FLINTOS_TEST_END <test_name>
```

### 6. Long-run stability tests

Targets:

- boot loop stability
- heap/stack exhaustion
- watchdog resets
- network reconnects
- display/input event loops
- app launch/stop cycles

Durations:

```text
short: 1 hour
mid: 24 hours
long: 7 days, when hardware lab exists
```

## HIL runner requirements

A self-hosted runner should have:

- ESP-IDF installed and activated
- target board connected by USB/JTAG/UART
- known serial port
- permission to access serial port
- stable power supply
- optional relay/power switch for hard reset
- scripts for detecting connected devices
- log artifact upload

## Test folder layout

Recommended layout:

```text
tests/
  host/
    native/
    java/
  hil/
    boot/
    display/
    input/
    storage/
    network/
    jvm/
  fixtures/
    manifests/
    packages/
  scripts/
    detect_devices.py
    serial_monitor.py
    hil_runner.py
```

## HIL JSON matrix

Use JSON to describe hardware tests and generate CI matrices:

```json
{
  "include": [
    {
      "name": "boot_smoke",
      "type": "hil_boot",
      "target": "esp32s3",
      "port": "COM3",
      "baud": 115200,
      "timeout_seconds": 30,
      "pass_pattern": "FLINTOS_TEST_PASS boot_smoke"
    }
  ]
}
```

## CI workflow stages

### Pull request CI

No hardware required:

1. Java module compile
2. C/C++ static structure check
3. host tests
4. docs/ADR checks

### Main branch CI

If ESP-IDF is available:

1. ESP-IDF build
2. package Java modules
3. artifact upload

### HIL CI

Manual or scheduled:

1. detect device
2. build firmware
3. flash firmware
4. serial monitor checks pass/fail marker
5. upload serial logs

## Definition of Done for embedded code

A story touching firmware/runtime code is done only when:

- Java modules compile if Java APIs are touched.
- ESP-IDF build is run, or blocker is documented.
- No Python runtime dependency is introduced.
- Native hardware calls are behind HAL/service boundaries.
- Flash/monitor operations are not performed without explicit permission.
- Tests or test plan are updated.
- Relevant docs/ADR are updated when architecture changes.

## Engineering skill checklist

FlintOS implementation should maintain competence in:

- C/C++ embedded development
- Java module/API design for constrained runtime
- microcontroller architecture
- ESP-IDF and FreeRTOS concepts
- peripherals: GPIO, ADC/DAC, I2C, SPI, I2S, UART, touch
- communication: WiFi, sockets, HTTP/MQTT where needed
- memory/stack/heap analysis
- hardware debugging through serial logs
- CI/HIL testing
- technical documentation and design reviews

## Immediate actions

1. Add Java module compile command to CI.
2. Add `tests/scripts/serial_monitor.py` later for HIL.
3. Add boot smoke serial markers once native boot is connected to UART logs.
4. Add manifest schema tests once app registry exists.
5. Add HIL workflow only after a stable self-hosted runner is available.
