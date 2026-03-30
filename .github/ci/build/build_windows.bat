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

REM If sdk_url has a value, replace the URL in install.ps1
if not "%sdk_url%"=="" (
    if not "%sdk_url%"=="none" (
        echo "Replacing SDK URL in install.ps1"
        powershell -Command "(Get-Content windows\APIExample\install.ps1) -replace '\$agora_sdk = ''.*''', ('$agora_sdk = ''' + '%sdk_url%' + '''') | Set-Content windows\APIExample\install.ps1"
    )
)

REM Check compress_apiexample parameter
if "%compress_apiexample%"=="" set compress_apiexample=false
echo compress_apiexample: %compress_apiexample%

REM Check compile_project parameter
if "%compile_project%"=="" set compile_project=false
echo compile_project: %compile_project%

REM Package APIExample with SDK (only when compress_apiexample=true)
REM Run before compile so package content is not affected by compile
set result_zip=APIExample_result.zip
set des_path=%WORKSPACE%\Agora_Native_SDK_for_Windows_v%SDK_VER%_APIExample_%BUILD_NUMBER%.zip
if "%compress_apiexample%"=="true" (
    echo "Packaging APIExample with SDK..."

    set "sdk_package_url=%sdk_url%"
    if "!sdk_package_url!"=="" (
        for /f "tokens=*" %%a in ('powershell -Command "(Get-Content 'windows\APIExample\install.ps1' -Raw) -match '\$agora_sdk = ''([^'']+)''' | Out-Null; $matches[1]"') do set "sdk_package_url=%%a"
    )
    if /I "!sdk_package_url!"=="none" (
        for /f "tokens=*" %%a in ('powershell -Command "(Get-Content 'windows\APIExample\install.ps1' -Raw) -match '\$agora_sdk = ''([^'']+)''' | Out-Null; $matches[1]"') do set "sdk_package_url=%%a"
    )
    if "!sdk_package_url!"=="" (
        echo SDK package URL is empty!
        exit /b 1
    )

    for /f "tokens=*" %%a in ('powershell -Command "[System.IO.Path]::GetFileName(''!sdk_package_url!'')"') do set "sdk_zip_name=%%a"

    set "sdk_extract_dir=Agora_Native_SDK_for_Windows_FULL"
    if exist "!sdk_extract_dir!" rmdir /S /Q "!sdk_extract_dir!"
    del /F /Q "!sdk_zip_name!" 2>nul
    del /F /Q %result_zip% 2>nul

    echo "Downloading SDK package: !sdk_package_url!"
    curl "!sdk_package_url!" -o "!sdk_zip_name!"
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

    mkdir "!sdk_extract_dir!\samples"
    mkdir "!sdk_extract_dir!\samples\API-example"

    del /F /Q windows\APIExample\ci.py 2>nul

    xcopy /Y /E windows\APIExample "!sdk_extract_dir!\samples\API-example"
    xcopy /Y windows\README.md "!sdk_extract_dir!\samples\API-example\"
    xcopy /Y windows\README.zh.md "!sdk_extract_dir!\samples\API-example\"

    if exist "!sdk_extract_dir!\samples\API-example\APIExample\APIExample" (
        rmdir /S /Q "!sdk_extract_dir!\samples\API-example\APIExample\APIExample"
    )

    echo "Compressing APIExample package with SDK..."
    7z a -tzip %result_zip% -r "!sdk_extract_dir!"
    if errorlevel 1 (
        echo 7z compression failed!
        exit /b 1
    )

    REM Copy to WORKSPACE with new naming format
    echo "Copying %result_zip% to %des_path%"
    copy %result_zip% %des_path%
    if errorlevel 1 (
        echo copy failed!
        exit /b 1
    )
    
    REM Clean up temporary files in repo root
    del /F %result_zip%
    del /F /Q "!sdk_zip_name!" 2>nul
    if exist "!sdk_extract_dir!" rmdir /S /Q "!sdk_extract_dir!"

    echo "Complete: APIExample package with SDK created"
    dir %WORKSPACE%\
) else (
    echo "Skipping APIExample packaging (compress_apiexample=false)"
)

REM Compile project to generate executable (only when compile_project=true)
if "%compile_project%"=="true" (
    echo "Compiling project to generate executable..."
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
