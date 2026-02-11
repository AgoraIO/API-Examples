cd /d %~dp0

powershell.exe -Command "Unblock-File -Path '%~dp0install.ps1'"
powershell.exe -File "%~dp0install.ps1"

pause
