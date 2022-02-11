$ThirdPartysrc = 'https://agora-adc-artifacts.oss-cn-beijing.aliyuncs.com/libs/ThirdParty.zip'
$ThirdPartydes = 'ThirdParty.zip'
$agora_sdk = 'https://download.agora.io/sdk/release/Agora_Native_SDK_for_Windows_x86_rel.v3.7.201_55354_20211231_1714_full.zip'
$agora_des = 'Agora_Native_SDK_for_Windows_x86_rel.v3.7.201_55354_20211231_1714_full.zip'

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
	Move-Item Agora_Native_SDK_for_Windows_x86_rel.v3.7.201_55354_20211231_1714_full\rtc\sdk\*  sdk
	Remove-Item  $agora_des -Recurse
	Remove-Item  Agora_Native_SDK_for_Windows_x86_rel.v3.7.201_55354_20211231_1714_full -Recurse
}



