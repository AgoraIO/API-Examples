REM ##################################
REM --- Guidelines: ---
REM
REM Common Environment Variable:
REM 'Package_Publish:boolean:true',
REM 'Clean_Clone:boolean:false',
REM 'is_tag_fetch:boolean:false',
REM 'is_offical_build:boolean:false',
REM 'repo:string',
REM 'base:string',
REM 'arch:string'
REM 'output:string'
REM 'build_date:string(yyyyMMdd)',
REM 'build_timestamp:string (yyyyMMdd_hhmm)',
REM 'platform: string',
REM 'BUILD_NUMBER: string',
REM 'WORKSPACE: string'
REM
REM --- Test Related: ---
REM For PR build, zip test related to test.zip
REM For Package build, zip package related to package.zip
REM --- Artifactory Related: ---
REM download artifactory:
REM python %WORKSPACE%\\artifactory_utils.py --action=download_file --file=ARTIFACTORY_URL
REM upload file to artifactory:
REM python %WORKSPACE%\\artifactory_utils.py --action=upload_file --file=FILEPATTERN --server_path=SERVERPATH --server_repo=SERVER_REPO --with_pattern
REM for example: python %WORKSPACE%\\artifactory_utils.py --action=upload_file --file=*.zip --server_path=windows/ --server_repo=ACCS_repo --with_pattern
REM upload folder to artifactory
REM python %WORKSPACE%\\artifactory_utils.py --action=upload_file --file=FILEPATTERN --server_path=SERVERPATH --server_repo=SERVER_REPO --with_folder
REM for example: python %WORKSPACE%\\artifactory_utils.py --action=upload_file --file=publish --server_path=windows/ --server_repo=ACCS_repo --with_folder
REM --- Input: ----
REM sourcePath: see jenkins console for details.
REM WORKSPACE: %WORKSPACE%
REM --- Output: ----
REM pr: output test.zip to workspace dir
REM others: Rename the zip package name yourself, But need copy it to workspace dir
REM ##################################

setlocal EnableDelayedExpansion

echo compile_project: %compile_project%
echo Package_Publish: %Package_Publish%
echo is_tag_fetch: %is_tag_fetch%
echo arch: %arch%
echo source_root: %source_root%
echo output: C:\\tmp\\%project%_out
echo build_date: %build_date%
echo build_time: %build_time%
echo pwd: %cd%
echo sdk_url: %sdk_url%

REM Version validation: branch name vs install.ps1 SDK version
for /f "tokens=*" %%a in ('powershell -Command "(Get-Content 'windows\APIExample\install.ps1' -Raw) -match '_v([0-9]+\.[0-9]+\.[0-9]+)' | Out-Null; $matches[1]"') do set SDK_VER=%%a
set "BRANCH_STRIP=%api_examples_branch:origin/=%"
if "%BRANCH_STRIP%"=="main" (
    echo Branch is main, skipping version validation ^(main branch is trusted^)
    goto :skip_version_validation
)
for /f "tokens=*" %%b in ('powershell -Command "'%api_examples_branch%' -match '([0-9]+\.[0-9]+\.[0-9]+)' | Out-Null; $matches[1]"') do set BRANCH_VER=%%b
if not "%SDK_VER%"=="%BRANCH_VER%" (
    echo ERROR: Version mismatch - Branch: %BRANCH_VER%, install.ps1: %SDK_VER%
    exit /b 1
)
echo Version validated: %BRANCH_VER%
:skip_version_validation

REM Keep install.ps1 unchanged. If sdk_url is provided, later prepare a local ../../sdk folder for compile.

REM Check compress_apiexample parameter
if "%compress_apiexample%"=="" set compress_apiexample=false
echo compress_apiexample: %compress_apiexample%

REM Check compile_project parameter
if "%compile_project%"=="" set compile_project=false
echo compile_project: %compile_project%

set "effective_sdk_url=%sdk_url%"
set result_zip=APIExample_result.zip
set des_path=%WORKSPACE%\Agora_Native_SDK_for_Windows_v%SDK_VER%_APIExample_%BUILD_NUMBER%.zip
set "need_sdk=false"

if "%compress_apiexample%"=="true" set "need_sdk=true"
if "%compile_project%"=="true" set "need_sdk=true"

if "%need_sdk%"=="true" (
    call :RESOLVE_SDK_URL
    if errorlevel 1 exit /b 1

    echo "Downloading SDK package: !effective_sdk_url!"
    call :DOWNLOAD_AND_EXTRACT_SDK "!effective_sdk_url!"
    if errorlevel 1 exit /b 1
) else (
    echo "Skipping SDK download (no compile/package requested)"
)

REM Package before compile so package contents are not affected by build outputs.
if "%compress_apiexample%"=="true" (
    echo "Packaging APIExample with SDK..."
    call :PACKAGE_APIEXAMPLE_WITH_SDK "!sdk_extract_dir!"
    if errorlevel 1 exit /b 1
    echo "Complete: APIExample package with SDK created"
    dir %WORKSPACE%\
) else (
    echo "Skipping APIExample packaging (compress_apiexample=false)"
)

if "%compile_project%"=="true" (
    echo "Compiling project to generate executable..."
    call :COPY_EXTRACTED_SDK_TO_REPO_ROOT "!sdk_extract_dir!"
    if errorlevel 1 (
        echo Preparing repo root sdk folder failed!
        exit /b 1
    )
    cd windows\APIExample
    call cloud_build.bat
    if %ERRORLEVEL% NEQ 0 (
        echo Build failed!
        exit /b %ERRORLEVEL%
    )
    cd ..\..
) else (
    echo "Skipping project compilation (compile_project=false)"
)
goto :eof

:RESOLVE_SDK_URL
if not "!effective_sdk_url!"=="" (
    if /I not "!effective_sdk_url!"=="none" exit /b 0
)
for /f "tokens=*" %%a in ('powershell -Command "(Get-Content 'windows\APIExample\install.ps1' -Raw) -match '\$agora_sdk = ''([^'']+)''' | Out-Null; $matches[1]"') do set "effective_sdk_url=%%a"
if "!effective_sdk_url!"=="" (
    echo Effective SDK URL is empty!
    exit /b 1
)
exit /b 0

:DOWNLOAD_AND_EXTRACT_SDK
set "download_sdk_url=%~1"
set "DOWNLOAD_SDK_URL=%download_sdk_url%"
for /f "usebackq tokens=*" %%a in (`powershell -Command "[System.IO.Path]::GetFileName($env:DOWNLOAD_SDK_URL)"`) do set "sdk_zip_name=%%a"
if "!sdk_zip_name!"=="" (
    echo SDK zip file name parse failed!
    exit /b 1
)

set "sdk_extract_dir=Agora_Native_SDK_for_Windows_FULL"
if exist "!sdk_extract_dir!" rmdir /S /Q "!sdk_extract_dir!"
del /F /Q "!sdk_zip_name!" 2>nul
del /F /Q %result_zip% 2>nul

curl "!download_sdk_url!" -o "!sdk_zip_name!"
if errorlevel 1 (
    echo SDK download failed!
    exit /b 1
)

echo "Extracting SDK package..."
7z x ".\!sdk_zip_name!" -y
if errorlevel 1 (
    echo SDK extraction failed!
    exit /b 1
)

if not exist "!sdk_extract_dir!" (
    echo Extracted SDK folder "!sdk_extract_dir!" not found!
    exit /b 1
)

if exist "!sdk_extract_dir!\demo" rmdir /S /Q "!sdk_extract_dir!\demo"
if exist "!sdk_extract_dir!\commits" del /F /Q "!sdk_extract_dir!\commits"
if exist "!sdk_extract_dir!\package_size_report.txt" del /F /Q "!sdk_extract_dir!\package_size_report.txt"
exit /b 0

:PACKAGE_APIEXAMPLE_WITH_SDK
set "package_sdk_extract_dir=%~1"
if not exist "!package_sdk_extract_dir!\samples" mkdir "!package_sdk_extract_dir!\samples"
if exist "!package_sdk_extract_dir!\samples\API-example" rmdir /S /Q "!package_sdk_extract_dir!\samples\API-example"
mkdir "!package_sdk_extract_dir!\samples\API-example"

del /F /Q windows\APIExample\ci.py 2>nul

xcopy /Y /E windows\APIExample "!package_sdk_extract_dir!\samples\API-example"
xcopy /Y windows\README.md "!package_sdk_extract_dir!\samples\API-example\"
xcopy /Y windows\README.zh.md "!package_sdk_extract_dir!\samples\API-example\"

if exist "!package_sdk_extract_dir!\samples\API-example\APIExample\APIExample" (
    rmdir /S /Q "!package_sdk_extract_dir!\samples\API-example\APIExample\APIExample"
)

echo "Compressing APIExample package with SDK..."
7z a -tzip %result_zip% -r "!package_sdk_extract_dir!"
if errorlevel 1 (
    echo 7z compression failed!
    exit /b 1
)

echo "Copying %result_zip% to %des_path%"
copy %result_zip% %des_path%
if errorlevel 1 (
    echo copy failed!
    exit /b 1
)

del /F /Q %result_zip% 2>nul
exit /b 0

:COPY_EXTRACTED_SDK_TO_REPO_ROOT
set "copy_sdk_extract_dir=%~1"
if not exist "!copy_sdk_extract_dir!\sdk\x86" (
    echo Extracted SDK folder "!copy_sdk_extract_dir!" is invalid!
    exit /b 1
)

if exist "sdk" rmdir /S /Q sdk
mkdir sdk
xcopy /Y /E "!copy_sdk_extract_dir!\sdk\x86" "sdk\x86\"
xcopy /Y /E "!copy_sdk_extract_dir!\sdk\x86_64" "sdk\x86_64\"
xcopy /Y /E "!copy_sdk_extract_dir!\sdk\high_level_api" "sdk\high_level_api\"
if errorlevel 1 (
    echo Copying extracted SDK into repo root sdk folder failed!
    exit /b 1
)
exit /b 0
