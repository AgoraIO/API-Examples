$ThirdPartysrc = 'https://agora-adc-artifacts.oss-cn-beijing.aliyuncs.com/libs/ThirdParty.zip'
$ThirdPartydes = 'ThirdParty.zip'
$agora_sdk = 'https://download.agora.io/sdk/release/Agora_Native_SDK_for_Windows_v4.0.0-beta.2_FULL.zip'
$agora_des = 'AgoraSdk.zip'

if (-not (Test-Path ThirdParty)){
	echo "download $ThirdPartydes"
	mkdir ThirdParty
	(New-Object System.Net.WebClient).DownloadFile($ThirdPartySrc,$ThirdPartyDes)
	Unblock-File $ThirdPartyDes
	Expand-Archive -Path $ThirdPartyDes -DestinationPath 'ThirdParty' -Force 
	Remove-Item $ThirdPartyDes -Recurse
}


if (-not (Test-Path sdk)){
	echo "download $agora_des"
	mkdir sdk
	mkdir sdk/x86
	mkdir sdk/x64
	(New-Object System.Net.WebClient).DownloadFile($agora_sdk,$agora_des)
	Unblock-File $agora_des
	Expand-Archive -Path $agora_des -DestinationPath . -Force 
	Move-Item Agora_Native_SDK_for_Windows_FULL\x86\rtc\sdk\*  sdk\x86
	Move-Item Agora_Native_SDK_for_Windows_FULL\x86_64\rtc\sdk\*  sdk\x64
	Remove-Item  $agora_des -Recurse
	Remove-Item  Agora_Native_SDK_for_Windows_FULL -Recurse
}



