$ThirdPartysrc = 'https://agora-adc-artifacts.oss-cn-beijing.aliyuncs.com/libs/ThirdParty.zip'
$ThirdPartydes = 'ThirdParty.zip'
$agora_sdk = 'https://download.agora.io/sdk/release/Agora_Native_SDK_for_Windows_v3_7_0_FULL.zip'
$agora_des = 'Agora_Native_SDK_for_Windows.zip'
$MediaPlayerSDK = 'https://download.agora.io/sdk/release/Agora_Media_Player_for_Windows_x86_rel.v1.3.0_63393_ffmpeg_player_lite_20210727_1117.zip'
$MediaPlayerSDK64 = 'https://download.agora.io/sdk/release/Agora_Media_Player_for_Windows_x64_rel.v1.3.0_63392_ffmpeg_player_lite_20210727_1117.zip'
$MediaPlayerDes = 'MediaPlayerPartSave.zip'
$MediaPlayerDes64 = 'MediaPlayerPartSave.zip'

$arch = $args[0]

if (-not (Test-Path ThirdParty)){
	echo "download $ThirdPartydes"
	mkdir ThirdParty
	(New-Object System.Net.WebClient).DownloadFile($ThirdPartySrc,$ThirdPartyDes)
	Unblock-File $ThirdPartyDes
	Expand-Archive -Path $ThirdPartyDes -DestinationPath 'ThirdParty' -Force 
	Remove-Item $ThirdPartyDes -Recurse
}


if (-not (Test-Path libs)){
	echo "download $agora_des"
	mkdir libs
	(New-Object System.Net.WebClient).DownloadFile($agora_sdk,$agora_des)
	Unblock-File $agora_des
	Expand-Archive -Path $agora_des -DestinationPath . -Force 
	Move-Item Agora_Native_SDK_for_Windows_FULL\libs\* libs
	Remove-Item  $agora_des -Recurse
	Remove-Item  Agora_Native_SDK_for_Windows_FULL -Recurse
}

if($arch -eq 'x64'){
    if (-not (Test-Path MediaPlayerPart64)){
	    echo "download $MediaPlayerSDK64"
		mkdir MediaPlayerPart64
		(New-Object System.Net.WebClient).DownloadFile($MediaPlayerSDK64,$MediaPlayerDes)
		Unblock-File $MediaPlayerDes
		Expand-Archive -Path $MediaPlayerDes -DestinationPath . -Force 
		Move-Item Agora_Media_Player_for_Windows_x64_rel.v1.3.0_63392_ffmpeg_player_lite_20210727_1117\sdk\* MediaPlayerPart64
		Remove-Item $MediaPlayerDes -Recurse
		Remove-Item Agora_Media_Player_for_Windows_x64_rel.v1.3.0_63392_ffmpeg_player_lite_20210727_1117 -Recurse
	}
}else{
    if (-not (Test-Path MediaPlayerPart)){
		echo "download $MediaPlayerSDK"
		mkdir MediaPlayerPart
		(New-Object System.Net.WebClient).DownloadFile($MediaPlayerSDK,$MediaPlayerDes)
		Unblock-File $MediaPlayerDes
		Expand-Archive -Path $MediaPlayerDes -DestinationPath . -Force 
		Move-Item Agora_Media_Player_for_Windows_x86_rel.v1.3.0_63393_ffmpeg_player_lite_20210727_1117\sdk\* MediaPlayerPart
		Remove-Item $MediaPlayerDes -Recurse
		Remove-Item Agora_Media_Player_for_Windows_x86_rel.v1.3.0_63393_ffmpeg_player_lite_20210727_1117 -Recurse
	}
}