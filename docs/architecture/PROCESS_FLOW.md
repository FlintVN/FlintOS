# FlintOS Process Flow

This document captures the execution flows for FlintOS as a Java/native embedded operating environment.

See also `MICROPYTHONOS_REFERENCE_ALIGNMENT.md` for the product-reference mapping from MicroPythonOS docs into FlintOS Java/native flows.

FlintOS uses MicroPythonOS only as product/UX inspiration for a thin app-centric OS, launcher, app store/catalog, OTA, touch UI, multi-device feel, and lightweight embedded experience. FlintOS does **not** use Python or MicroPython as runtime, app runtime, system API layer, service layer, or boot implementation.

## 1. Boot flow: ESP-IDF to Java Launcher

```mermaid
flowchart TD
    A["ESP32 Reset / Power On"] --> B["ESP-IDF Bootloader"]
    B --> C["Partition Table: Factory or OTA App"]
    C --> D["ESP-IDF app_main()"]

    D --> E["FlintOS Logger Init"]
    E --> F["BootManager.initialize()"]

    F --> G["Board Profile Init"]
    G --> H["NVS / Storage Init"]
    H --> I["Display HAL Init"]
    I --> J["Input HAL Init"]
    J --> K["Network HAL Init"]
    K --> L["Power / Watchdog Policy Init"]

    L --> M["ServiceRegistry Init"]

    M --> M1["SettingsStore"]
    M --> M2["PermissionManager"]
    M --> M3["AppRegistry"]
    M --> M4["PackageManager"]
    M --> M5["LoggerService"]
    M --> M6["OTAUpdateManager"]

    M1 --> N["BootManager.start()"]
    M2 --> N
    M3 --> N
    M4 --> N
    M5 --> N
    M6 --> N

    N --> O["FlintJvmHost.initialize()"]
    O --> P["Configure JVM Heap / Stack / Classpath"]
    P --> Q["Register Native Bindings"]

    Q --> Q1["flint.os.SystemInfo"]
    Q --> Q2["flint.os.Logger"]
    Q --> Q3["flint.ui.Screen"]
    Q --> Q4["flint.app.AppManager"]
    Q --> Q5["flint.io / flint.net bindings"]

    Q1 --> R["Load System App Registry"]
    Q2 --> R
    Q3 --> R
    Q4 --> R
    Q5 --> R

    R --> S["Find flintos.launcher app.json"]
    S --> T["Permission Check: app.launch + system.info"]

    T -->|allowed| U["FlintJvmHost.launchSystemApp()"]
    T -->|denied| X["Boot Fail / Recovery UI / Serial Error"]

    U --> V["Load flintos.launcher.LauncherApp"]
    V --> W["Instantiate FlintApp"]
    W --> Y["Call LauncherApp.onStart(AppContext)"]

    Y --> Z["Launcher UI Active"]
```

## 2. App launch flow

```mermaid
sequenceDiagram
    participant User
    participant Launcher as Java LauncherApp
    participant AppManager as flint.app.AppManager
    participant JVM as FlintJvmHost
    participant Registry as Native AppRegistry
    participant Permission as Native PermissionManager
    participant Services as Native Services
    participant App as Java User/System App

    User->>Launcher: Touch / button select app
    Launcher->>AppManager: launch(appId)
    AppManager->>JVM: native launch request

    JVM->>Registry: findById(appId)
    Registry-->>JVM: AppManifest / AppInfo

    JVM->>Permission: validate caller + requested app permissions
    Permission-->>JVM: allowed / denied

    alt permission denied
        JVM-->>AppManager: false / SecurityException
        AppManager-->>Launcher: launch failed
        Launcher->>Launcher: show error / log
    else permission allowed
        JVM->>Services: prepare app context
        Services-->>JVM: AppContext handle
        JVM->>App: load mainClass
        JVM->>App: instantiate FlintApp
        JVM->>App: onStart(AppContext)
        App-->>JVM: started
        JVM-->>AppManager: true
        AppManager-->>Launcher: launch success
    end
```

## 3. Java native API call flow

```mermaid
flowchart TD
    A["Java App"] --> B["FlintJDK / FlintOS Java API"]

    B --> B1["flint.os.SystemInfo"]
    B --> B2["flint.ui.Screen"]
    B --> B3["flint.net.WiFi"]
    B --> B4["flint.io.Pin / I2C / SPI"]
    B --> B5["flint.os.Settings"]
    B --> B6["flint.os.Storage"]

    B1 --> C["Native Method Boundary"]
    B2 --> C
    B3 --> C
    B4 --> C
    B5 --> C
    B6 --> C

    C --> D["Caller Context Resolve"]
    D --> E["PermissionManager Check"]

    E -->|denied| F["Throw SecurityException / return error"]
    E -->|allowed| G["Dispatch to Native Service"]

    G --> G1["SystemInfoService"]
    G --> G2["DisplayService"]
    G --> G3["NetworkService"]
    G --> G4["IoService / HAL"]
    G --> G5["SettingsStore"]
    G --> G6["StorageService"]

    G1 --> H["ESP-IDF / FreeRTOS / HAL"]
    G2 --> H
    G3 --> H
    G4 --> H
    G5 --> H
    G6 --> H

    H --> I["Return Result / Error Code"]
    I --> J["Bridge Native Result to Java"]
    J --> K["Java App Continues"]
```

## 4. App registry and manifest validation flow

```mermaid
flowchart TD
    A["Boot / Package Install / Registry Refresh"] --> B["Scan System App Manifests"]
    B --> C["Scan User App Manifests"]
    C --> D["Read app.json"]

    D --> E["Schema Validation"]
    E --> E1["id"]
    E --> E2["name"]
    E --> E3["version"]
    E --> E4["type: system/user"]
    E --> E5["runtime: flintjvm"]
    E --> E6["mainClass"]
    E --> E7["permissions"]
    E --> E8["minOs / minJdk"]

    E1 --> F["Semantic Validation"]
    E2 --> F
    E3 --> F
    E4 --> F
    E5 --> F
    E6 --> F
    E7 --> F
    E8 --> F

    F --> G{"Valid?"}

    G -->|No| H["Reject App"]
    H --> H1["Log reason"]
    H1 --> H2["Do not expose in launcher"]

    G -->|Yes| I["Compute checksum / trust state"]
    I --> J["Register AppInfo"]
    J --> K["Persist Registry"]
    K --> L["Expose to flint.app.AppManager"]
    L --> M["Launcher displays installed apps"]
```

## 5. Package install flow

```mermaid
flowchart TD
    A["Developer builds .flintapp"] --> B["FlintToolHub / FlintTool"]
    B --> C["Send package to device"]

    C --> D["Native PackageManager receives package"]
    D --> E["Write to staging area"]

    E --> F["Verify package structure"]
    F --> F1["app.json exists"]
    F --> F2["classes/modules exist"]
    F --> F3["assets optional"]
    F --> F4["checksum/signature"]

    F1 --> G["Manifest Validation"]
    F2 --> G
    F3 --> G
    F4 --> G

    G --> H{"Valid package?"}

    H -->|No| I["Delete staging package"]
    I --> J["Return install failure"]

    H -->|Yes| K["Check permissions policy"]
    K --> L{"Allowed permissions?"}

    L -->|No| M["Reject install / require approval"]
    L -->|Yes| N["Install to app storage"]

    N --> O["Atomic registry update"]
    O --> P["Flush storage"]
    P --> Q["Notify Launcher / AppRegistry refresh"]
    Q --> R["App visible in Launcher"]
```

## 6. OTA update flow

```mermaid
flowchart TD
    A["Updater Java System App"] --> B["flint.os.UpdateManager"]
    B --> C["Native OTAUpdateManager"]

    C --> D["Check update metadata"]
    D --> E["Download firmware package"]
    E --> F["Verify checksum/signature"]
    F --> G{"Valid update?"}

    G -->|No| H["Abort OTA"]
    H --> I["Log failure / keep current firmware"]

    G -->|Yes| J["Write to OTA partition via ESP-IDF OTA API"]
    J --> K["Mark pending boot partition"]
    K --> L["Reboot device"]

    L --> M["Boot new firmware"]
    M --> N["Boot health check"]

    N --> O{"Healthy?"}

    O -->|Yes| P["Mark OTA valid"]
    P --> Q["Continue normal boot"]

    O -->|No| R["Rollback to previous firmware"]
    R --> S["Log rollback reason"]
```

## 7. Testing, CI, and HIL flow

```mermaid
flowchart TD
    A["Pull Request / Local Check"] --> B["Static Checks"]

    B --> B1["Forbidden Python runtime scan"]
    B --> B2["Manifest schema validation"]
    B --> B3["CMake structure check"]
    B --> B4["Java module compile"]
    B --> B5["C/C++ warnings / formatting"]

    B1 --> C["Host Tests"]
    B2 --> C
    B3 --> C
    B4 --> C
    B5 --> C

    C --> C1["PermissionManager tests"]
    C --> C2["AppRegistry tests"]
    C --> C3["Manifest parser tests"]
    C --> C4["SettingsStore tests"]
    C --> C5["Launcher fake Screen tests"]

    C1 --> D["ESP-IDF Build"]
    C2 --> D
    C3 --> D
    C4 --> D
    C5 --> D

    D --> E{"IDF environment active?"}

    E -->|No| F["Mark build blocked with reason"]
    E -->|Yes| G["idf.py build"]

    G --> H{"Build passed?"}

    H -->|No| I["Fix compile/link/config errors"]
    H -->|Yes| J["Artifact ready"]

    J --> K{"HIL runner available?"}

    K -->|No| L["Skip HIL / keep artifact"]
    K -->|Yes| M["Flash approved test device"]

    M --> N["Serial monitor"]
    N --> O["Wait for markers"]

    O --> O1["FLINTOS_TEST_START"]
    O --> O2["FLINTOS_TEST_PASS"]
    O --> O3["FLINTOS_TEST_FAIL"]
    O --> O4["FLINTOS_TEST_END"]

    O2 --> P["HIL pass"]
    O3 --> Q["HIL fail + logs"]
```

## 8. Overall architecture flow

```mermaid
flowchart TB
    subgraph Native["Native C/C++ / ESP-IDF / FreeRTOS"]
        N1["BootManager"]
        N2["ServiceRegistry"]
        N3["LoggerService"]
        N4["SettingsStore"]
        N5["PermissionManager"]
        N6["AppRegistry"]
        N7["PackageManager"]
        N8["OTAUpdateManager"]

        subgraph HAL["HAL / Board Profile"]
            H1["Display"]
            H2["Input"]
            H3["Storage"]
            H4["Network"]
            H5["Power"]
            H6["Sensors"]
        end
    end

    subgraph Bridge["JVM Bridge"]
        B1["FlintJvmHost"]
        B2["Native Bindings"]
        B3["Exception Bridge"]
        B4["Event Loop Bridge"]
    end

    subgraph Runtime["FlintJVM / FlintESPJVM"]
        R1["VM Heap"]
        R2["Class Loader"]
        R3["Thread Mapping"]
        R4["Native Method Dispatch"]
    end

    subgraph JDK["FlintJDK / FlintOS Java APIs"]
        J1["java.base"]
        J2["flint.io"]
        J3["flint.net"]
        J4["flint.app"]
        J5["flint.os"]
        J6["flint.ui"]
    end

    subgraph Apps["Java Apps"]
        A1["Launcher"]
        A2["Settings"]
        A3["Diagnostics"]
        A4["Updater"]
        A5["User Apps"]
    end

    N1 --> N2
    N2 --> N3
    N2 --> N4
    N2 --> N5
    N2 --> N6
    N2 --> N7
    N2 --> N8
    N2 --> HAL

    N2 --> B1
    HAL --> B2
    B1 --> Runtime
    B2 --> R4
    B3 --> Runtime
    B4 --> Runtime

    Runtime --> JDK
    JDK --> Apps

    Apps --> JDK
    JDK --> B2
    B2 --> N2
    N2 --> HAL
```

## 9. Sprint 002 runtime foundation flow

```mermaid
flowchart TD
    A["Sprint 002: App Runtime Foundation"] --> B["Repo Hygiene"]

    B --> B1["Resolve .claude/settings.json"]
    B --> B2["Confirm no Python runtime"]
    B --> B3["Record baseline status"]

    B1 --> C["Verify Sprint 001 DoD"]
    B2 --> C
    B3 --> C

    C --> C1["Java module compile"]
    C --> C2["ESP-IDF build if IDF active"]
    C --> C3["Forbidden runtime scan"]

    C1 --> D["Integrate FlintESPJVM"]
    C2 --> D
    C3 --> D

    D --> D1["Add sdkconfig.defaults"]
    D --> D2["Add partitions.csv"]
    D --> D3["Link FlintJVM sources/includes"]
    D --> D4["Define target board"]

    D1 --> E["Native ServiceRegistry MVP"]
    D2 --> E
    D3 --> E
    D4 --> E

    E --> E1["PermissionManager"]
    E --> E2["AppRegistry"]
    E --> E3["SettingsStore skeleton"]
    E --> E4["LoggerService"]

    E1 --> F["FlintJvmHost Real Contract"]
    E2 --> F
    E3 --> F
    E4 --> F

    F --> F1["initialize VM"]
    F --> F2["register native bindings"]
    F --> F3["launch Java class"]
    F --> F4["bridge errors"]

    F1 --> G["Java API Expansion"]
    F2 --> G
    F3 --> G
    F4 --> G

    G --> G1["AppManifest"]
    G --> G2["AppManager"]
    G --> G3["Logger"]
    G --> G4["SystemInfo"]
    G --> G5["Screen"]

    G1 --> H["Launcher MVP"]
    G2 --> H
    G3 --> H
    G4 --> H
    G5 --> H

    H --> H1["List installed apps"]
    H --> H2["Render app list"]
    H --> H3["Launch selected app"]

    H1 --> I["Tests / Evidence"]
    H2 --> I
    H3 --> I

    I --> I1["Host tests"]
    I --> I2["Java tests"]
    I --> I3["Native integration smoke"]
    I --> I4["HIL markers ready"]

    I1 --> J["Sprint 002 Done"]
    I2 --> J
    I3 --> J
    I4 --> J
```
