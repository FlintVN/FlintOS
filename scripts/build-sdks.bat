@echo off
setlocal EnableExtensions

set "ROOT=%~dp0.."
for %%I in ("%ROOT%") do set "ROOT=%%~fI"
set "JDK_ROOT=%ROOT%\flintjdk"
set "UI_ROOT=%ROOT%\flintui"
set "MIDP_ROOT=%ROOT%\FlintJ2ME"
set "LIBRARY_ROOT=%ROOT%\files\lib"
set "UI_OUTPUT=%ROOT%\build\sdk\flint.ui"
set "DEVICE_OUTPUT=%ROOT%\build\sdk\flintos.device"

rem ---- JDK 8 rt.jar location (required for FlintMIDP Java 8 compilation) ----
if not defined JDK8_RT (
    if exist "C:\Program Files\Eclipse Adoptium\jdk-8.0.492.9-hotspot\jre\lib\rt.jar" (
        set "JDK8_RT=C:\Program Files\Eclipse Adoptium\jdk-8.0.492.9-hotspot\jre\lib\rt.jar"
    ) else if exist "C:\Program Files\Java\jdk1.8.0_202\jre\lib\rt.jar" (
        set "JDK8_RT=C:\Program Files\Java\jdk1.8.0_202\jre\lib\rt.jar"
    )
)

rem ---- Prerequisite checks ----
where javac >nul 2>nul || (echo javac was not found in PATH. Install JDK 17 or newer.>&2 & exit /b 1)
where jar >nul 2>nul || (echo jar was not found in PATH. Install JDK 17 or newer.>&2 & exit /b 1)
if not exist "%JDK_ROOT%\src\" (echo Missing FlintJDK submodule source. Run git submodule update --init --recursive.>&2 & exit /b 1)
if not exist "%UI_ROOT%\src\" (echo Missing FlintUI submodule source. Run git submodule update --init --recursive.>&2 & exit /b 1)
if not exist "%MIDP_ROOT%\src\" (echo Missing FlintMIDP submodule source. Run git submodule update --init --recursive.>&2 & exit /b 1)
if not exist "%ROOT%\java\src\" (echo Missing FlintOS device API source.>&2 & exit /b 1)
if not defined JDK8_RT (
    echo JDK 8 rt.jar not found. Set JDK8_RT environment variable to the path of rt.jar.>&2
    echo Example: set JDK8_RT=C:\Program Files\Eclipse Adoptium\jdk-8.0.492.9-hotspot\jre\lib\rt.jar>&2
    exit /b 1
)
if not exist "%JDK8_RT%" (
    echo JDK 8 rt.jar not found at: %JDK8_RT%>&2
    exit /b 1
)

if not exist "%LIBRARY_ROOT%" mkdir "%LIBRARY_ROOT%"

rem ---- FlintJDK ----
echo Building FlintJDK (java.base, flint.io, flint.net, flint.drawing)...
set "JDK_BUILD_DIR=%JDK_ROOT%\bin\run"
if exist "%JDK_BUILD_DIR%" rmdir /s /q "%JDK_BUILD_DIR%"
for %%m in (java.base flint.io flint.net flint.drawing) do (
    echo   Compiling module %%m...
    javac -Xlint:all -XDstringConcat=inline --release 17 -encoding UTF-8 -d "%JDK_BUILD_DIR%" --module %%m --module-source-path "%JDK_ROOT%\src"
    if errorlevel 1 exit /b 1
    jar --create --file "%JDK_BUILD_DIR%\%%m.jar" --manifest "%JDK_ROOT%\META-INF\MANIFEST.MF" -0 -C "%JDK_BUILD_DIR%\%%m" .
    if errorlevel 1 exit /b 1
)
copy /Y "%JDK_BUILD_DIR%\*.jar" "%LIBRARY_ROOT%\" >nul
if errorlevel 1 exit /b 1
echo FlintJDK build complete.

rem ---- FlintUI ----
echo Building FlintUI from its dedicated submodule...
if not exist "%UI_ROOT%\src\flint.ui\module-info.java" (echo Missing FlintUI submodule. Run git submodule update --init --recursive.>&2 & exit /b 1)
if exist "%UI_OUTPUT%" rmdir /s /q "%UI_OUTPUT%"
mkdir "%UI_OUTPUT%"
javac -Xlint:all -XDstringConcat=inline --release 17 -encoding UTF-8 -d "%UI_OUTPUT%" --module-path "%LIBRARY_ROOT%" --module-source-path "%UI_ROOT%\src" --module flint.ui
if errorlevel 1 exit /b 1
jar cf0m "%LIBRARY_ROOT%\flint.ui.jar" "%ROOT%\java\META-INF\flint.ui.MF" -C "%UI_OUTPUT%\flint.ui" .
if errorlevel 1 exit /b 1
echo FlintUI build complete.

rem ---- FlintOS device API ----
echo Building FlintOS device API...
if exist "%DEVICE_OUTPUT%" rmdir /s /q "%DEVICE_OUTPUT%"
mkdir "%DEVICE_OUTPUT%"
javac -Xlint:all -XDstringConcat=inline --release 17 -encoding UTF-8 -d "%DEVICE_OUTPUT%" --module-path "%LIBRARY_ROOT%" --module-source-path "%ROOT%\java\src" --module flintos.device
if errorlevel 1 exit /b 1
jar cf0m "%LIBRARY_ROOT%\flintos.device.jar" "%ROOT%\java\META-INF\MANIFEST.MF" -C "%DEVICE_OUTPUT%\flintos.device" .
if errorlevel 1 exit /b 1
echo FlintOS device API build complete.

rem ---- FlintMIDP ----
echo Building FlintMIDP...
set "MIDP_TOOLS_DIR=%MIDP_ROOT%\bin\tools"
set "MIDP_RUN_DIR=%MIDP_ROOT%\bin\run"

if exist "%MIDP_RUN_DIR%" rmdir /s /q "%MIDP_RUN_DIR%"
if exist "%MIDP_TOOLS_DIR%" rmdir /s /q "%MIDP_TOOLS_DIR%"
mkdir "%MIDP_RUN_DIR%\midp"
mkdir "%MIDP_TOOLS_DIR%"

rem Build classpath from already-built JARs in files/lib
set "MIDP_CP=%LIBRARY_ROOT%\flint.drawing.jar;%LIBRARY_ROOT%\flintos.device.jar"
if exist "%LIBRARY_ROOT%\midp.jar" set "MIDP_CP=%MIDP_CP%;%LIBRARY_ROOT%\midp.jar"

echo   Compiling MIDP sources ^(Java 8 target^)...
rem Collect all Java sources
set "SOURCES_FILE=%MIDP_RUN_DIR%\sources.txt"
dir /s /b "%MIDP_ROOT%\src\*.java" | sort > "%SOURCES_FILE%"

javac -source 8 -target 8 -XDstringConcat=inline -bootclasspath "%JDK8_RT%" -Xlint:all,-serial,-options -encoding UTF-8 -classpath "%MIDP_CP%" -d "%MIDP_RUN_DIR%\midp" @"%SOURCES_FILE%"
if errorlevel 1 exit /b 1

echo   Packaging MIDP JARs...
javac --release 17 -encoding UTF-8 -d "%MIDP_TOOLS_DIR%" "%MIDP_ROOT%\tools\MidpJarPackager.java"
if errorlevel 1 exit /b 1
java -cp "%MIDP_TOOLS_DIR%" MidpJarPackager "%MIDP_RUN_DIR%\midp" "%MIDP_ROOT%\src" "%MIDP_ROOT%\META-INF\MANIFEST.MF" "%MIDP_ROOT%\config\jsr118-api-classes.txt" "%MIDP_RUN_DIR%" run
if errorlevel 1 exit /b 1

copy /Y "%MIDP_RUN_DIR%\midp.jar" "%LIBRARY_ROOT%\midp.jar" >nul
if errorlevel 1 exit /b 1
copy /Y "%MIDP_RUN_DIR%\flintos.midp.jar" "%LIBRARY_ROOT%\flintos.midp.jar" >nul
if errorlevel 1 exit /b 1
copy /Y "%MIDP_RUN_DIR%\m3g.jar" "%LIBRARY_ROOT%\m3g.jar" >nul
if errorlevel 1 exit /b 1
echo FlintMIDP build complete.

rem ---- Validate ----
java "%~dp0ValidateJarManifests.java" "%LIBRARY_ROOT%"
if errorlevel 1 exit /b 1

echo SDK libraries are ready in: %LIBRARY_ROOT%
endlocal
