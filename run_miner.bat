@echo off
setlocal enabledelayedexpansion

REM ==========================================
REM 0. CONFIGURATION - EDIT THIS PATH!
REM ==========================================
REM PASTE YOUR VCPKG FOLDER PATH BELOW (No trailing slash)
REM Example: SET VCPKG_ROOT=C:\vcpkg
SET VCPKG_ROOT=E:\Bin\vcpkg

REM ==========================================
REM 1. SETUP BUILD DIRECTORY
REM ==========================================
if not exist "build" (
    mkdir build
)
cd build

REM ==========================================
REM 2. RUN CMAKE CONFIGURATION
REM ==========================================
echo RUNNING CMAKE
REM We check if the toolchain file exists to prevent confusing errors
if not exist "%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" (
    echo [ERROR] Could not find vcpkg.cmake at:
    echo %VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake
    echo Please edit line 9 of this script to point to your vcpkg folder.
    pause
    exit /b 1
)

REM Adding -DCMAKE_TOOLCHAIN_FILE tells CMake where to find the libraries
cmake .. -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows

if %errorlevel% neq 0 (
    echo [ERROR] CMake Configuration failed.
    pause
    exit /b 1
)

REM ==========================================
REM 3. COMPILING BLOCKCHAIN
REM ==========================================
echo COMPILING BLOCKCHAIN
cmake --build . --target blockchain_miner --config Debug --parallel
if %errorlevel% neq 0 (
    echo [ERROR] Compilation failed.
    pause
    exit /b 1
)

REM ==========================================
REM 4. KILL OLD PROCESSES
REM ==========================================
echo KILLING ALL OLD BLOCKCHAIN PROCESSES
taskkill /F /IM blockchain_miner.exe /T >nul 2>&1

REM ==========================================
REM 5. LOCATE EXECUTABLE
REM ==========================================
REM Visual Studio generator usually puts the .exe inside 'Debug'
set EXE_PATH=Debug\blockchain_miner.exe

if not exist "%EXE_PATH%" (
    REM Fallback check just in case
    set EXE_PATH=blockchain_miner.exe
)

if not exist "%EXE_PATH%" (
    echo [ERROR] Could not find executable at %EXE_PATH%
    pause
    exit /b 1
)

REM ==========================================
REM 6. START NEW BLOCKCHAIN
REM ==========================================
echo STARTING NEW BLOCKCHAIN
REM Start minimized (/MIN), redirect logs
start /MIN "Blockchain Miner Service" cmd /c "mpiexec -n 4 %EXE_PATH% > miner.log 2>&1"

echo BLOCKCHAIN MINING SERVICE STARTED AT: 127.0.0.1:8081
echo Logs are being written to build/miner.log
timeout /t 3 >nul

REM ==========================================
REM 7. PREVENT WINDOW CLOSE
REM ==========================================
pause