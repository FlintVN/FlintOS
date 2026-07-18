@echo off
setlocal EnableExtensions

set "ROOT=%~dp0.."
for %%I in ("%ROOT%") do set "ROOT=%%~fI"
set "JDK_ROOT=%ROOT%\flintjdk"
set "UI_ROOT=%ROOT%\flintui"
set "MIDP_ROOT=%ROOT%\flintmidp"
set "LIBRARY_ROOT=%ROOT%\files\lib"
set "UI_OUTPUT=%ROOT%\build\sdk\flint.ui"
set "DEVICE_OUTPUT=%ROOT%\build\sdk\flintos.device"

where javac >nul 2>nul || (echo javac was not found in PATH. Install JDK 17 or newer.>&2 & exit /b 1)
where jar >nul 2>nul || (echo jar was not found in PATH. Install JDK 17 or newer.>&2 & exit /b 1)
if not exist "%JDK_ROOT%\scripts\build.bat" (echo Missing FlintJDK submodule build script.>&2 & exit /b 1)
if not exist "%MIDP_ROOT%\scripts\build.bat" (echo Missing FlintMIDP submodule build script.>&2 & exit /b 1)
if not exist "%LIBRARY_ROOT%" mkdir "%LIBRARY_ROOT%"

echo Delegating FlintJDK build to its submodule...
call "%JDK_ROOT%\scripts\build.bat"
if errorlevel 1 exit /b 1
copy /Y "%JDK_ROOT%\bin\run\*.jar" "%LIBRARY_ROOT%\" >nul
if errorlevel 1 exit /b 1

echo Building FlintUI from its dedicated submodule...
if not exist "%UI_ROOT%\src\flint.ui\module-info.java" (echo Missing FlintUI submodule. Run git submodule update --init --recursive.>&2 & exit /b 1)
if exist "%UI_OUTPUT%" rmdir /s /q "%UI_OUTPUT%"
mkdir "%UI_OUTPUT%"
javac -Xlint:all -XDstringConcat=inline --release 17 -encoding UTF-8 -d "%UI_OUTPUT%" --module-path "%LIBRARY_ROOT%" --module-source-path "%UI_ROOT%\src" --module flint.ui
if errorlevel 1 exit /b 1
jar cf0m "%LIBRARY_ROOT%\flint.ui.jar" "%ROOT%\java\META-INF\flint.ui.MF" -C "%UI_OUTPUT%\flint.ui" .
if errorlevel 1 exit /b 1

echo Building FlintOS device API...
if exist "%DEVICE_OUTPUT%" rmdir /s /q "%DEVICE_OUTPUT%"
mkdir "%DEVICE_OUTPUT%"
javac -Xlint:all -XDstringConcat=inline --release 17 -encoding UTF-8 -d "%DEVICE_OUTPUT%" --module-path "%LIBRARY_ROOT%" --module-source-path "%ROOT%\java\src" --module flintos.device
if errorlevel 1 exit /b 1
jar cf0m "%LIBRARY_ROOT%\flintos.device.jar" "%ROOT%\java\META-INF\MANIFEST.MF" -C "%DEVICE_OUTPUT%\flintos.device" .
if errorlevel 1 exit /b 1

echo Delegating FlintMIDP build to its submodule...
call "%MIDP_ROOT%\scripts\build.bat" --library-root "%LIBRARY_ROOT%"
if errorlevel 1 exit /b 1
copy /Y "%MIDP_ROOT%\bin\run\midp.jar" "%LIBRARY_ROOT%\midp.jar" >nul
if errorlevel 1 exit /b 1
copy /Y "%MIDP_ROOT%\bin\run\flintos.midp.jar" "%LIBRARY_ROOT%\flintos.midp.jar" >nul
if errorlevel 1 exit /b 1
copy /Y "%MIDP_ROOT%\bin\run\m3g.jar" "%LIBRARY_ROOT%\m3g.jar" >nul
if errorlevel 1 exit /b 1

java "%~dp0ValidateJarManifests.java" "%LIBRARY_ROOT%"
if errorlevel 1 exit /b 1

echo SDK libraries are ready in: %LIBRARY_ROOT%
endlocal
