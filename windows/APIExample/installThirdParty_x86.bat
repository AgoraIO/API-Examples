cd /d %~dp0

if not exist .\libs (
    if exist ..\..\..\libs (
        echo d | xcopy ..\..\..\libs .\libs /Y /E /Q
    )
)

powershell.exe -command ^
  "& {set-executionpolicy Remotesigned -Scope Process; ./'install.ps1' 'x86'}"
