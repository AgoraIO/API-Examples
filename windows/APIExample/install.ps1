$agora_sdk = 'https://download.agora.io/sdk/release/Agora_Native_SDK_for_Windows_v4.5.3_FULL.zip'
$ThirdPartysrc = 'https://demo-app-download.agora.io/api_example/ThirdParty.zip'
$ThirdPartydes = 'ThirdParty.zip'
$agora_des = 'AgoraSdk.zip'
$agora_local_sdk = '../../sdk'

if (-not (Test-Path ThirdParty)){
	echo "download $ThirdPartydes"
	mkdir ThirdParty
	(New-Object System.Net.WebClient).DownloadFile($ThirdPartysrc,$ThirdPartydes)
	Unblock-File $ThirdPartydes
	Expand-Archive -Path $ThirdPartydes -DestinationPath 'ThirdParty' -Force 
	Remove-Item $ThirdPartydes -Recurse
}

if (-not (Test-Path sdk)){
    if (Test-Path $agora_local_sdk){
	    mkdir sdk
	    mkdir sdk\x86
	    mkdir sdk\x64
	    mkdir sdk\high_level_api
	    mkdir sdk\high_level_api\include
	    Copy-Item $agora_local_sdk\x86\*  sdk\x86 -Recurse
	    Copy-Item $agora_local_sdk\x86_64\*  sdk\x64 -Recurse
	    Copy-Item $agora_local_sdk\high_level_api\include\*  sdk\high_level_api\include -Recurse
    }else{
        echo "download $agora_des"
		echo "Downloading SDK from: $agora_sdk"

	    mkdir sdk
	    mkdir sdk\x86
	    mkdir sdk\x64
	    mkdir sdk\high_level_api
	    mkdir sdk\high_level_api\include
	    
	    # Use system temporary directory for extraction to avoid path length issues
	    $tempBasePath = [System.IO.Path]::GetTempPath()
	    $tempExtractPath = Join-Path $tempBasePath "RtcSdkExtract_$(Get-Random)"
	    $tempZipPath = Join-Path $tempBasePath $agora_des
	    
	    try {
	        (New-Object System.Net.WebClient).DownloadFile($agora_sdk, $tempZipPath)
	        Unblock-File $tempZipPath
	        Expand-Archive -Path $tempZipPath -DestinationPath $tempExtractPath -Force
	        
	        $extractedSdkPath = (Get-ChildItem -Path $tempExtractPath -Directory | Select-Object -First 1).FullName
	        Move-Item (Join-Path $extractedSdkPath "sdk\x86\*") sdk\x86 -Force
	        Move-Item (Join-Path $extractedSdkPath "sdk\x86_64\*") sdk\x64 -Force
	        Move-Item (Join-Path $extractedSdkPath "sdk\high_level_api\include\*") sdk\high_level_api\include -Force
	    }
	    finally {
	        # Clean up temporary files
	        if (Test-Path $tempZipPath) { Remove-Item $tempZipPath -Force -ErrorAction SilentlyContinue }
	        if (Test-Path $tempExtractPath) { Remove-Item $tempExtractPath -Recurse -Force -ErrorAction SilentlyContinue }
	    }
    }
}

# Validation: check each folder has at least one direct child (non-empty), output to console only
$allOk = $true
foreach ($dir in @("ThirdParty", "sdk\x86", "sdk\x64", "sdk\high_level_api\include")) {
    $p = Join-Path $PSScriptRoot $dir
    $n = 0
    if (Test-Path $p -PathType Container) {
        try { $n = (Get-ChildItem $p -ErrorAction SilentlyContinue).Count } catch { Write-Host "[FAIL] $dir | $_" -ForegroundColor Red; $allOk = $false; continue }
    }
    $ok = $n -ge 1
    if (-not $ok) { $allOk = $false }
    if ($ok) { Write-Host "[PASS] $dir (items=$n)" } else { Write-Host "[FAIL] $dir (items=$n)" }
}
if ($allOk) {
    Write-Host "Result: All passed."
    Write-Host "Dependencies are installed; you can proceed to the next step." -ForegroundColor Green
} else {
    Write-Host "Result: Validation failed." -ForegroundColor Red
    Write-Host "Please check network and download, ensure ThirdParty and sdk are not empty, then retry." -ForegroundColor Red
    exit 1
}
