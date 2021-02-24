$ThirdPartysrc = 'https://agora-adc-artifacts.oss-cn-beijing.aliyuncs.com/libs/ThirdParty.zip'
$ThirdPartydes = 'ThirdParty.zip'
$MediaPlayerSDK = 'https://download.agora.io/sdk/release/Agora_Media_Player_for_Windows_x86_rel.v1.1.0.16486_20200507_1537.zip'
$MediaPlayerDes = 'MediaPlayerPartSave.zip'


if (-not (Test-Path ThirdParty)){
	echo "download $ThirdPartydes"
	mkdir ThirdParty
	Invoke-WebRequest -uri $ThirdPartySrc -OutFile $ThirdPartyDes -TimeoutSec 10;
	Unblock-File $ThirdPartyDes
	Expand-Archive -Path $ThirdPartyDes -DestinationPath 'ThirdParty' -Force 
	Remove-Item $ThirdPartyDes -Recurse
}

if (-not (Test-Path MediaPlayerPart)){
	echo "download $MediaPlayerSDK"
	mkdir MediaPlayerPart
	Invoke-WebRequest -uri $MediaPlayerSDK -OutFile $MediaPlayerDes -TimeoutSec 10;
	Unblock-File $MediaPlayerDes
	Expand-Archive -Path $MediaPlayerDes -DestinationPath . -Force 
	Move-Item Agora_Media_Player_for_Windows_x86_rel.v1.1.0.16486_20200507_1537\sdk\* MediaPlayerPart
	Remove-Item $MediaPlayerDes -Recurse
	Remove-Item Agora_Media_Player_for_Windows_x86_rel.v1.1.0.16486_20200507_1537 -Recurse
}



