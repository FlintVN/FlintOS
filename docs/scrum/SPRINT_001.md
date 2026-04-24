# Sprint 001: Java/native FlintOS foundation

## Sprint goal

Establish the correct FlintOS foundation: native C/C++ ESP-IDF boot plus Java app model on FlintJVM. Remove Python/MicroPython assumptions from active implementation.

## Scope

- Remove active Python prototype files.
- Add ESP-IDF skeleton files.
- Add native boot manager/logger/JVM host placeholders.
- Add Java module stubs for `flint.app`, `flint.os`, `flint.ui`.
- Add Java launcher system app stub.
- Add architecture docs and ADR.

## Definition of Done

- No active Python runtime files remain in the root app structure.
- Project has ESP-IDF `CMakeLists.txt` and `main/` entrypoint.
- Native files compile conceptually against ESP-IDF headers.
- Java API stubs are present and organized by module.
- Java module compile check passes.
- Architecture docs state Java + native C/C++ direction clearly.
- Testing strategy covers static, host, Java module, native integration, HIL, and long-run stability levels.
- No flashing or hardware operation is performed.

## Risks

- FlintJVM native API integration points need confirmation from FlintESPJVM internals.
- Java module compilation may need alignment with FlintJDK build tooling.
- ESP-IDF build cannot be verified unless ESP-IDF environment is active.
- HIL tests require a self-hosted runner, stable board power, serial access, and clear pass/fail log markers.
