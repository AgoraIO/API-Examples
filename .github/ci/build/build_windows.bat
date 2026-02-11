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

REM Package APIExample code (only when compress_apiexample=true)
REM Run before compile so package content is not affected by compile
set result_zip=APIExample_result.zip
set des_path=%WORKSPACE%\Agora_Native_SDK_for_Windows_v%SDK_VER%_APIExample_%BUILD_NUMBER%.zip
if "%compress_apiexample%"=="true" (
    echo "Packaging APIExample code..."
    
    REM Compress windows\APIExample (code only) to zip
    echo "Compressing APIExample code package..."
    del /F /Q %result_zip% 2>nul
    7z a -tzip %result_zip% -r windows\APIExample
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
    
    REM Clean up temporary zip in repo root
    del /F %result_zip%
    
    echo "Complete: APIExample code package created"
    dir %WORKSPACE%\
) else (
    echo "Skipping APIExample code packaging (compress_apiexample=false)"
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