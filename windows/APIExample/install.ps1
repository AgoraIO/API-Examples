$agora_sdk = 'https://download.agora.io/sdk/release/Agora_Native_SDK_for_Windows_v4.5.0_FULL.zip'
$ThirdPartysrc = 'https://fullapp.oss-cn-beijing.aliyuncs.com/API-Examples/ThirdParty.zip'
$ThirdPartydes = 'ThirdParty.zip'
$agora_des = 'AgoraSdk.zip'
$agora_local_sdk = '../../sdk'

if (-not (Test-Path ThirdParty)){
	echo "download $ThirdPartydes"
	mkdir ThirdParty
	(New-Object System.Net.WebClient).DownloadFile($ThirdPartySrc,$ThirdPartyDes)
	Unblock-File $ThirdPartyDes
	Expand-Archive -Path $ThirdPartyDes -DestinationPath 'ThirdParty' -Force 
	Remove-Item $ThirdPartyDes -Recurse
}



if (-not (Test-Path sdk)){
    if (Test-Path $agora_local_sdk){
	    mkdir sdk
	    mkdir sdk\x64
	    mkdir sdk\high_level_api
	    mkdir sdk\high_level_api\include
	    Copy-Item $agora_local_sdk\x86\*  sdk -Recurse
	    Copy-Item $agora_local_sdk\x86_64\*  sdk\x64 -Recurse
	    Copy-Item $agora_local_sdk\high_level_api\include\*  sdk\high_level_api\include -Recurse
    }else{
        echo "download $agora_des"
	    mkdir sdk
	    mkdir sdk\x64
	    mkdir sdk\high_level_api
	    mkdir sdk\high_level_api\include
	    (New-Object System.Net.WebClient).DownloadFile($agora_sdk,$agora_des)
	    Unblock-File $agora_des
	    Expand-Archive -Path $agora_des -DestinationPath . -Force 
	    Move-Item Agora_Native_SDK_for_Windows_FULL\sdk\x86\*  sdk
	    Move-Item Agora_Native_SDK_for_Windows_FULL\sdk\x86_64\*  sdk\x64
	    Move-Item Agora_Native_SDK_for_Windows_FULL\sdk\high_level_api\include\*  sdk\high_level_api\include
	    Remove-Item  $agora_des -Recurse -Force
	    Remove-Item  Agora_Native_SDK_for_Windows_FULL -Recurse -Force
    }
}



