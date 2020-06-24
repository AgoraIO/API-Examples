param($SDKURL, $LocalFileName)
Invoke-WebRequest -Uri "$SDKURL" -OutFile "$LocalFileName" -TimeoutSec 10;