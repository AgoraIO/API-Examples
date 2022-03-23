#pragma once
#define INFO_LEN 255
//main dialog
extern wchar_t commonGroupDoc[INFO_LEN];
extern wchar_t commonDocumentWebsite[INFO_LEN];
extern wchar_t commonFAQWebsite[INFO_LEN];
extern wchar_t commonRegisterWebsite[INFO_LEN];
extern wchar_t commonDemoWebsite[INFO_LEN];

extern wchar_t commonBasicScene[INFO_LEN];
extern wchar_t commonAdvanceScene[INFO_LEN];
//agora enum
extern wchar_t agoraRoleBroadcaster[INFO_LEN];
extern wchar_t agoraRoleAudience[INFO_LEN];


//common ctrl
extern wchar_t commonCtrlChannel[INFO_LEN];
extern wchar_t commonCtrlJoinChannel[INFO_LEN];
extern wchar_t commonCtrlLeaveChannel[INFO_LEN];
extern wchar_t commonCtrlClientRole[INFO_LEN];
//scene list
extern wchar_t basicLiveBroadcasting[INFO_LEN];
extern wchar_t advancedRtmpStreaming[INFO_LEN];
extern wchar_t advancedVideoMetadata[INFO_LEN];
extern wchar_t advancedCustomEncrypt[INFO_LEN];
extern wchar_t advancedMediaEncrypt[INFO_LEN];
extern wchar_t advancedScreenCap[INFO_LEN];
extern wchar_t advancedVideoProfile[INFO_LEN];
extern wchar_t advancedAudioProfile[INFO_LEN];
extern wchar_t advancedAudioMixing[INFO_LEN];
extern wchar_t advancedBeauty[INFO_LEN];
extern wchar_t advancedBeautyAudio[INFO_LEN];
extern wchar_t advancedCustomVideoCapture[INFO_LEN];
extern wchar_t advancedMediaIOCustomVideoCapture[INFO_LEN];
extern wchar_t advancedOriginalVideo[INFO_LEN];
extern wchar_t advancedMediaAudioCapture[INFO_LEN];
extern wchar_t advancedCustomAudioCapture[INFO_LEN];
extern wchar_t advancedOriginalAudio[INFO_LEN];
extern wchar_t advancedMediaPlayer[INFO_LEN];
extern wchar_t advancedAudioEffect[INFO_LEN];
extern wchar_t advancedMultiChannel[INFO_LEN];
extern wchar_t advancedPerCallTest[INFO_LEN];
extern wchar_t advancedAudioVolume[INFO_LEN];
extern wchar_t advancedReportInCall[INFO_LEN];
extern wchar_t advancedRegionConn[INFO_LEN];
extern wchar_t advancedCrossChannel[INFO_LEN];


//live broadcasting
extern wchar_t liveCtrlPersons[INFO_LEN];
extern wchar_t liveCtrlLoopbackDevice[INFO_LEN];
extern wchar_t liveCtrlLoopbackVolume[INFO_LEN];
extern wchar_t liveCtrlLoopbackEnable[INFO_LEN];
extern wchar_t liveCtrlAudienceLatency[INFO_LEN];
extern wchar_t liveCtrlAudienceLowLatency[INFO_LEN];
extern wchar_t liveCtrlAudienceUltraLowLatency[INFO_LEN];
//rtmp streaming
extern wchar_t rtmpStreamingCtrlPublishUrl[INFO_LEN];
extern wchar_t rtmpStreamingCtrlAdd[INFO_LEN];
extern wchar_t rtmpStreamingCtrlRemove[INFO_LEN];
extern wchar_t rtmpStreamingCtrlTransCoding[INFO_LEN];
extern wchar_t rtmpStreamingCtrlRemoveAll[INFO_LEN];
//rtmp stream state changed
extern wchar_t agoraRtmpStateIdle[INFO_LEN];
extern wchar_t agoraRtmpStateConnecting[INFO_LEN];
extern wchar_t agoraRtmpStateRunning[INFO_LEN];
extern wchar_t agoraRtmpStateRunningSuccess[INFO_LEN];
extern wchar_t agoraRtmpStateRecovering[INFO_LEN];
extern wchar_t agoraRtmpStateInvalidArg[INFO_LEN];
extern wchar_t agoraRtmpStateEncrypted[INFO_LEN];
extern wchar_t agoraRtmpStateConnTimeout[INFO_LEN];
extern wchar_t agoraRtmpStateInrealErr[INFO_LEN];
extern wchar_t agoraRtmpStateServerErr[INFO_LEN];
extern wchar_t agoraRtmpStateTooOften[INFO_LEN];
extern wchar_t agoraRtmpStateReachLimit[INFO_LEN];
extern wchar_t agoraRtmpStateNotAuth[INFO_LEN];
extern wchar_t agoraRtmpStateNotFound[INFO_LEN];
extern wchar_t agoraRtmpStateNotSupported[INFO_LEN];

// inject status
extern wchar_t agoraInjectStartSucc[INFO_LEN];
extern wchar_t agoraInjectExist[INFO_LEN];
extern wchar_t agoraInjectStartUnAuth[INFO_LEN];
extern wchar_t agoraInjectStartTimeout[INFO_LEN];
extern wchar_t agoraInjectStartFailed[INFO_LEN];
extern wchar_t agoraInjectStopSuccess[INFO_LEN];
extern wchar_t agoraInjectNotFound[INFO_LEN];
extern wchar_t agoraInjectStopUnAuth[INFO_LEN];

extern wchar_t agoraInjectStopTimeout[INFO_LEN];
extern wchar_t agoraInjectStopFailed[INFO_LEN];
extern wchar_t agoraInjectBroken[INFO_LEN];

//video SEI
extern wchar_t videoSEIInformation[INFO_LEN];
extern wchar_t metadataCtrlSendSEI[INFO_LEN];
extern wchar_t metadataCtrlBtnSend[INFO_LEN];
extern wchar_t metadataCtrlBtnClear[INFO_LEN];

//media encrypt
extern wchar_t mediaEncryptCtrlMode[INFO_LEN];
extern wchar_t mediaEncryptCtrlSecret[INFO_LEN];
extern wchar_t mediaEncryptCtrlSetEncrypt[INFO_LEN];

//custom encrypt
extern wchar_t customEncryptCtrlEncrypt[INFO_LEN];
extern wchar_t customEncryptCtrlSetEncrypt[INFO_LEN];
extern wchar_t customEncryptCtrlCancelEncrypt[INFO_LEN];

//screen share
extern wchar_t screenShareCtrlScreenCap[INFO_LEN];
extern wchar_t screenShareCtrlStartCap[INFO_LEN];
extern wchar_t screenShareCtrlEndCap[INFO_LEN];

extern wchar_t  screenShareCtrlScreen[INFO_LEN];
extern wchar_t  screenShareCtrlShareSCreen[INFO_LEN];
extern wchar_t  screenShareCtrlStopShare[INFO_LEN];
extern wchar_t  screenShareCtrlGeneralSettings[INFO_LEN];
extern wchar_t  screenShareCtrlFPS[INFO_LEN];
extern wchar_t  screenShareCtrlBitrate[INFO_LEN];
extern wchar_t  screenShareCtrlShareCursor[INFO_LEN];
extern wchar_t  screenShareCtrlUpdateCaptureParam[INFO_LEN];
extern wchar_t  screenShareCtrlWindowFocus[INFO_LEN];
extern wchar_t  screenShareCtrlExcludeWindowList[INFO_LEN];






extern wchar_t screenCtrlRectInfo[INFO_LEN];
extern wchar_t virtualScreenCtrlRectInfo[INFO_LEN];
extern wchar_t screenCtrlCustomRectInfo[INFO_LEN];

//beauty
extern wchar_t beautyCtrlLighteningContrastLevel[INFO_LEN];
extern wchar_t beautyCtrlLightening[INFO_LEN];
extern wchar_t beautyCtrlSmoothness[INFO_LEN];
extern wchar_t beautyCtrlRedness[INFO_LEN];
extern wchar_t beautyCtrlEnable[INFO_LEN];

//beauty audio
extern wchar_t beautyAudioCtrlSetAudioChange[INFO_LEN];
extern wchar_t beautyAudioCtrlUnSetAudioChange[INFO_LEN];
extern wchar_t beautyAudioCtrlChange[INFO_LEN];
extern wchar_t beautyAudioCtrlPreSet[INFO_LEN];
extern wchar_t beautyAudioCtrlParam1[INFO_LEN];
extern wchar_t beautyAudioCtrlParam2[INFO_LEN];

//set video profile
extern wchar_t videoProfileCtrlWidth[INFO_LEN];
extern wchar_t videoProfileCtrlHeight[INFO_LEN];
extern wchar_t videoProfileCtrlFPS[INFO_LEN];
extern wchar_t videoProfileCtrlBitrate[INFO_LEN];
extern wchar_t videoProfileCtrldegradationPreference[INFO_LEN];
extern wchar_t videoProfileCtrlSetVideoProfile[INFO_LEN];
extern wchar_t videoProfileCtrlUnSetVideoProfile[INFO_LEN];


//set audio profile
extern wchar_t audioProfileCtrlProfile[INFO_LEN];
extern wchar_t audioProfileCtrlScenario[INFO_LEN];
extern wchar_t audioProfileCtrlSetAudioProfile[INFO_LEN];
extern wchar_t audioProfileCtrlUnSetAudioProfile[INFO_LEN];

//audio mixing
extern wchar_t audioMixingCtrlMixingPath[INFO_LEN]; 
extern wchar_t audioMixingCtrlRepeatTimes[INFO_LEN];
extern wchar_t audioMixingCtrlSetAudioMixing[INFO_LEN];
extern wchar_t audioMixingCtrlUnSetAudioMixing[INFO_LEN];
extern wchar_t audioMixingCtrlOnlyLocal[INFO_LEN]; 
extern wchar_t audioMixingCtrlReplaceMicroPhone[INFO_LEN];
extern wchar_t audioMixingCtrlDuration[INFO_LEN];
extern wchar_t audioMixingCtrlSecond[INFO_LEN];

//audio effect
extern wchar_t AudioEffectCtrlEffectPath[INFO_LEN];
extern wchar_t AudioEffectCtrlEffect[INFO_LEN];
extern wchar_t AudioEffectCtrlLoops[INFO_LEN];
extern wchar_t AudioEffectCtrlGain[INFO_LEN];
extern wchar_t AudioEffectCtrlPitch[INFO_LEN];
extern wchar_t AudioEffectCtrlPan[INFO_LEN];
extern wchar_t AudioEffectCtrlPublish[INFO_LEN];
extern wchar_t AudioEffectCtrlAddEffect[INFO_LEN];
extern wchar_t AudioEffectCtrlRemoveEffect[INFO_LEN];
extern wchar_t AudioEffectCtrlPreLoad[INFO_LEN];
extern wchar_t AudioEffectCtrlUnPreload[INFO_LEN];
extern wchar_t AudioEffectCtrlPauseEffect[INFO_LEN];
extern wchar_t AudioEffectCtrlPlayEffect[INFO_LEN];
extern wchar_t AudioEffectCtrlPauseAllEffect[INFO_LEN];
extern wchar_t AudioEffectCtrlResumeEffect[INFO_LEN];
extern wchar_t AudioEffectCtrlResumeAllEffect[INFO_LEN];
extern wchar_t AudioEffectCtrlStopAllEffect[INFO_LEN];
extern wchar_t AudioEffectCtrlStopEffect[INFO_LEN];
extern wchar_t AudioEffectCtrlVolume[INFO_LEN];





//custom video capture
extern wchar_t customVideoCaptureCtrlCaptureVideoDevice[INFO_LEN];
extern wchar_t customVideoCaptureCtrlSetExternlCapture[INFO_LEN];
extern wchar_t customVideoCaptureCtrlCancelExternlCapture[INFO_LEN];


//original video process
extern wchar_t OriginalVideoCtrlProc[INFO_LEN];
extern wchar_t OriginalVideoCtrlSetProc[INFO_LEN];
extern wchar_t OriginalVideoCtrlUnSetProc[INFO_LEN];


//custom audio capture
extern wchar_t customAudioCaptureCtrlCaptureAudioDeivce[INFO_LEN];
extern wchar_t customAudioCaptureCtrlSetExternlCapture[INFO_LEN];
extern wchar_t customAudioCaptureCtrlCancelExternlCapture[INFO_LEN];
extern wchar_t customAudioCaptureCtrlSetAudioRender[INFO_LEN];
extern wchar_t customAudioCaptureCtrlCancelAudioRender[INFO_LEN];



//original audio process
extern wchar_t OriginalAudioCtrlProc[INFO_LEN];
extern wchar_t OriginalAudioCtrlSetProc[INFO_LEN];
extern wchar_t OriginalAudioCtrlUnSetProc[INFO_LEN];

//media player
extern wchar_t mediaPlayerCtrlVideoSource[INFO_LEN];
extern wchar_t mediaPlayerCtrlOpen[INFO_LEN];
extern wchar_t mediaPlayerCtrlClose[INFO_LEN];
extern wchar_t mediaPlayerCtrlPause[INFO_LEN];
extern wchar_t mediaPlayerCtrlPlay[INFO_LEN];
extern wchar_t mediaPlayerCtrlAttachPlayer[INFO_LEN];
extern wchar_t mediaPlayerCtrlDettachPlayer[INFO_LEN];
extern wchar_t mediaPlayerCtrlPublishVideo[INFO_LEN];
extern wchar_t mediaPlayerCtrlUnPublishVideo[INFO_LEN];
extern wchar_t mediaPlayerCtrlPublishAudio[INFO_LEN];
extern wchar_t mediaPlayerCtrlUnPublishAudio[INFO_LEN];


//multi channel
extern wchar_t MultiChannelCtrlChannelList[INFO_LEN];


//per call test
extern wchar_t PerCallTestCtrlAudioInput[INFO_LEN];
extern wchar_t PerCallTestCtrlAudioOutput[INFO_LEN];
extern wchar_t PerCallTestCtrlAudioVol[INFO_LEN];
extern wchar_t PerCallTestCtrlCamera[INFO_LEN];
extern wchar_t PerCallTestCtrlStartTest[INFO_LEN];
extern wchar_t PerCallTestCtrlStopTest[INFO_LEN];
extern wchar_t PerCallTestCtrlStartEchoTest[INFO_LEN];
extern wchar_t PerCallTestCtrlStartEchoTest1[INFO_LEN];
extern wchar_t PerCallTestCtrlStopEchoTest[INFO_LEN];

//audio volume
extern wchar_t AudioVolumeCtrlCapVol[INFO_LEN];
extern wchar_t AudioVolumeCtrlCapSigVol[INFO_LEN];
extern wchar_t AudioVolumeCtrlPlaybackVol[INFO_LEN];
extern wchar_t AudioVolumeCtrlPlaybackSigVol[INFO_LEN];



//report in call
extern wchar_t ReportInCallCtrlGopTotal[INFO_LEN];
extern wchar_t ReportInCallCtrlGopRemoteVideo[INFO_LEN];
extern wchar_t ReportInCallCtrlGopRemoteAudio[INFO_LEN];
extern wchar_t ReportInCallCtrlTotalUpDownLink[INFO_LEN];
extern wchar_t ReportInCallCtrlTotalBytes[INFO_LEN];
extern wchar_t ReportInCallCtrlTotalBitrate[INFO_LEN];
extern wchar_t ReportInCallCtrlVideoNetWorkDelay[INFO_LEN];
extern wchar_t ReportInCallCtrlVideoBytes[INFO_LEN];
extern wchar_t ReportInCallCtrlVideoBitrate[INFO_LEN];
extern wchar_t ReportInCallCtrlAudioNetWorkDelay[INFO_LEN];
extern wchar_t ReportInCallCtrlAudioBytes[INFO_LEN];
extern wchar_t ReportInCallCtrlAudioBitrate[INFO_LEN];
extern wchar_t ReportInCallCtrlLocalResoultion[INFO_LEN];
extern wchar_t ReportInCallCtrlLocalFPS[INFO_LEN];


//area code
extern wchar_t RegionConnCtrlAreaCode[INFO_LEN];


//Cross Channel
extern wchar_t CrossChannelCtrlCrossChannel[INFO_LEN];
extern wchar_t CrossChannelCtrlToken[INFO_LEN];
extern wchar_t CrossChannelCtrlUid[INFO_LEN];
extern wchar_t CrossChannelCrossChannelList[INFO_LEN];
extern wchar_t CrossChannelAddChannel[INFO_LEN];
extern wchar_t CrossChannelRemoveChannel[INFO_LEN];
extern wchar_t CrossChannelStartMediaRelay[INFO_LEN];
extern wchar_t CrossChannelStopMediaRelay[INFO_LEN];
extern wchar_t CrossChannelUpdateMediaRelay[INFO_LEN];

//multi video source
extern wchar_t MultiVideoSourceCtrlVideoSource[INFO_LEN];
extern wchar_t MultiVideoSourceCtrlPublish[INFO_LEN];
extern wchar_t MultiVideoSourceCtrlUnPublish[INFO_LEN];
extern wchar_t advancedMultiVideoSource[INFO_LEN];

//mediaio
extern wchar_t mediaIOCaptureType[INFO_LEN];
extern wchar_t mediaIOCaptureTypeSDKCamera[INFO_LEN];
extern wchar_t mediaIOCaptureTypeSDKScreen[INFO_LEN];
extern wchar_t mediaIOCaptureCamera[INFO_LEN];
extern wchar_t mediaIOCaptureScreen[INFO_LEN];

extern wchar_t mediaIOScreenMotion[INFO_LEN];
extern wchar_t mediaIOScreenDetails[INFO_LEN];
extern wchar_t mediaIOScreenNone[INFO_LEN];
extern wchar_t mediaIOCaptureSDKCamera[INFO_LEN];

extern wchar_t adscUnknown[INFO_LEN];
extern wchar_t adscPlayback[INFO_LEN];
extern wchar_t adscCapturing[INFO_LEN];
extern wchar_t adscRenderer[INFO_LEN];
extern wchar_t adscCapturer[INFO_LEN];
extern wchar_t adscAPPPlayback[INFO_LEN];

extern wchar_t adscAcitve[INFO_LEN];
extern wchar_t adscDisabled[INFO_LEN];
extern wchar_t adscNoPresent[INFO_LEN];
extern wchar_t adscUnPlugined[INFO_LEN];
extern wchar_t adscUnRecommend[INFO_LEN];

extern wchar_t videoBackgroundSourceType[INFO_LEN];
extern wchar_t videoBackgroundSourceTypeNone[INFO_LEN];
extern wchar_t videoBackgroundSourceTypeColor[INFO_LEN];
extern wchar_t videoBackgroundSourceTypeImg[INFO_LEN];
extern wchar_t videoBackgroundSourceTypeBlur[INFO_LEN];
extern wchar_t videoBackgroundSourceTypeEnable[INFO_LEN];
extern wchar_t videoBackgroundSourceTypeRed[INFO_LEN];
extern wchar_t videoBackgroundSourceTypeBlue[INFO_LEN];
extern wchar_t videoBackgroundSourceTypeGreen[INFO_LEN];
extern wchar_t videoBackgroundSourceTypeImagePath[INFO_LEN];

extern wchar_t SpatialAudio[INFO_LEN];
extern wchar_t SpatialAudioInitInfo[INFO_LEN];

extern wchar_t SpatialAudioMoveInfo[INFO_LEN];
extern wchar_t SpatialAudioPrepareInfo[INFO_LEN];
extern void InitKeyInfomation();
extern CString GetExePath();