@echo off
setlocal EnableExtensions

set "ROOT=%~dp0.."
for %%I in ("%ROOT%") do set "ROOT=%%~fI"
set "JDK_ROOT=%ROOT%\flintjdk"
set "MIDP_ROOT=%ROOT%\flintmidp"
set "LIBRARY_ROOT=%ROOT%\files\lib"
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

echo SDK libraries are ready in: %LIBRARY_ROOT%
endlocal
