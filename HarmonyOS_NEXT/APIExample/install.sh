#!/bin/bash
target_dir=$(cd $(dirname $0); pwd)

mv /Users/xcz/Workspaces/media_sdk_script/rte_sdk/proj.hmos/AgoraRtcSdk/build/default/outputs/default/AgoraRtcSdk.har /Users/xcz/Workspaces/API-Examples-latest/HarmonyOS_NEXT/APIExample/entry/libs/AgoraRtcSdk.har
/Applications/DevEco-Studio.app/Contents/tools/node/bin/node /Applications/DevEco-Studio.app/Contents/tools/hvigor/bin/hvigorw.js -p product=default clean --analyze=normal --parallel --incremental --daemon
/Applications/DevEco-Studio.app/Contents/tools/ohpm/bin/ohpm install --all --registry https://repo.harmonyos.com/ohpm/ --strict_ssl true
/Applications/DevEco-Studio.app/Contents/tools/node/bin/node /Applications/DevEco-Studio.app/Contents/tools/hvigor/bin/hvigorw.js --mode module -p module=entry@default -p product=default -p buildMode=debug -p requiredDeviceType=phone assembleHap --analyze=normal --parallel --incremental --daemon
hdc shell aa force-stop io.agora.hmos.apiexample
hdc uninstall io.agora.hmos.apiexample
hdc shell mkdir data/local/tmp/4930bedbd75444de95be151cdb8f6adf
hdc file send /Users/xcz/Workspaces/API-Examples-latest/HarmonyOS_NEXT/APIExample/entry/build/default/outputs/default/entry-default-signed.hap "data/local/tmp/4930bedbd75444de95be151cdb8f6adf"
hdc shell bm install -p data/local/tmp/4930bedbd75444de95be151cdb8f6adf
hdc shell rm -rf data/local/tmp/4930bedbd75444de95be151cdb8f6adf
hdc shell aa start -a EntryAbility -b io.agora.hmos.apiexample