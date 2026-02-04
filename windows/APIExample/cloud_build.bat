@echo off
setlocal enabledelayedexpansion

echo "compile start..."

REM --- 1. Install Dependencies ---
powershell.exe -ExecutionPolicy Bypass -File "%~dp0install.ps1"

REM --- 2. Find MSBuild ---
set "MSBuildPath="
set "vswhere=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if exist "!vswhere!" (
    for /f "usebackq tokens=*" %%i in (`"!vswhere!" -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do set "MSBuildPath=%%i"
)

if not defined MSBuildPath (
    for %%p in (
        "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
        "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
        "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
    ) do if exist %%p set "MSBuildPath=%%p"
)

if not defined MSBuildPath (
    echo Error: MSBuild not found.
    exit /b 1
)

echo Using MSBuild at: "!MSBuildPath!"

REM --- 3. Build ---
"!MSBuildPath!" "APIExample.sln" /p:platform="Win32" /p:configuration="Release"
if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    exit /b %ERRORLEVEL%
)

echo "compile done."

REM --- 4. Package (Clean Output) ---
echo Preparing clean output directory...
rmdir /S /Q Output 2>nul
mkdir Output

REM Copy executables and DLLs from Release
xcopy /Y /S Release\*.exe Output\ >nul 2>&1
xcopy /Y /S Release\*.dll Output\ >nul 2>&1

REM Copy resources
xcopy /Y /S /I APIExample\res Output\res >nul 2>&1
xcopy /Y APIExample\*.ini Output\ >nul 2>&1

REM Copy SDK and ThirdParty DLLs (Agora uses sdk\x86 for Win32)
xcopy /Y sdk\x86\*.dll Output\ >nul 2>&1
xcopy /Y ThirdParty\libFFmpeg\*.dll Output\ >nul 2>&1

echo Packaging...
set "result_zip=result.zip"
del /F /Q !result_zip! 2>nul

REM Try 7z, fallback to PowerShell
where 7z >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    7z a -tzip !result_zip! .\Output\* >nul
) else (
    "C:\Program Files\7-Zip\7z.exe" a -tzip !result_zip! .\Output\* >nul 2>nul
    if %ERRORLEVEL% NEQ 0 (
        powershell -command "Compress-Archive -Path '.\Output\*' -DestinationPath '!result_zip!' -Force"
    )
)

REM Copy to WORKSPACE with original naming format
set h=%time:~0,2%
set h=%h: =0%
if not defined WORKSPACE set WORKSPACE=%~dp0
copy !result_zip! "%WORKSPACE%\\APIExample_windows_%BUILD_NUMBER%_%date:~4,2%%date:~7,2%%h%%time:~3,2%_Release_exe.zip"
del /F /Q !result_zip! 2>nul
rmdir /S /Q Output 2>nul

echo Build and Package done.
