$ThirdPartysrc = 'https://agora-adc-artifacts.oss-cn-beijing.aliyuncs.com/libs/ThirdParty.zip'
$ThirdPartydes = 'ThirdParty.zip'
$agora_sdk = 'https://github.com/AgoraIO/API-Examples/releases/download/3.4.5/3.6.200_x86_windows_full.zip'
$agora_des = '3.6.200_x86_windows_full.zip'

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
	(New-Object System.Net.WebClient).DownloadFile($agora_sdk,$agora_des)
	Unblock-File $agora_des
	Expand-Archive -Path $agora_des -DestinationPath . -Force 
	Move-Item 3.6.200_x86_windows_full\sdk\*  sdk
	Remove-Item  $agora_des -Recurse
	Remove-Item  3.6.200_x86_windows_full -Recurse
}



