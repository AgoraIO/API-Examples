echo "compile start..."
call installThirdParty.bat
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\MSBuild\Current\Bin\MSBuild.exe" "APIExample.sln" /p:platform="Win32" /p:configuration="Release"
7z a -tzip result.zip -r Release

set h=%time:~0,2%
set h=%h: =0%
copy result.zip %WORKSPACE%\\APIExample_windows_%BUILD_NUMBER%_%date:~4,2%%date:~7,2%%h%%time:~3,2%_Release_exe.zip
del /F result.zip
echo "compile done."