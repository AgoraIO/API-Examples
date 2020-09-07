// pch.cpp: 与预编译标头对应的源文件

#include "pch.h"

#include "CConfig.h"
// 当使用预编译的头时，需要使用此源文件，编译才能成功。
std::string appid = "";
wchar_t commonGroupDoc[INFO_LEN] = { 0 };
wchar_t commonDocumentWebsite[INFO_LEN] = { 0 };
wchar_t commonFAQWebsite[INFO_LEN] = { 0 };
wchar_t commonRegisterWebsite[INFO_LEN] = { 0 };
wchar_t commonDemoWebsite[INFO_LEN] = { 0 };
wchar_t commonBasicScene[INFO_LEN] = { 0 };
wchar_t commonAdvanceScene[INFO_LEN] = { 0 };
//agora enum
wchar_t agoraRoleBroadcaster[INFO_LEN] = { 0 };
wchar_t agoraRoleAudience[INFO_LEN] = { 0 };
//common ctrl
wchar_t commonCtrlChannel[INFO_LEN] = { 0 };
wchar_t commonCtrlJoinChannel[INFO_LEN] = { 0 };
wchar_t commonCtrlLeaveChannel[INFO_LEN] = { 0 };
wchar_t commonCtrlClientRole[INFO_LEN] = { 0 };
//scene list
wchar_t basicLiveBroadcasting[INFO_LEN] = { 0 };
wchar_t advancedRtmpInject[INFO_LEN] = { 0 };
wchar_t advancedRtmpStreaming[INFO_LEN] = { 0 };
wchar_t advancedVideoMetadata[INFO_LEN] = { 0 };
wchar_t advancedCustomEncrypt[INFO_LEN] = { 0 };

wchar_t advancedScreenCap[INFO_LEN] = { 0 };
wchar_t advancedBeauty[INFO_LEN] = { 0 };
wchar_t advancedBeautyAudio[INFO_LEN] = { 0 };
wchar_t advancedAudioProfile[INFO_LEN] = { 0 };
wchar_t advancedAudioMixing[INFO_LEN] = { 0 };
wchar_t advancedCustomVideoCapture[INFO_LEN] = { 0 };
wchar_t advancedOriginalVideo[INFO_LEN] = { 0 };
wchar_t advancedCustomAudioCapture[INFO_LEN] = { 0 };
wchar_t advancedOriginalAudio[INFO_LEN] = { 0 };
wchar_t advancedMediaPlayer[INFO_LEN] = { 0 };


//live broadcasting
wchar_t liveCtrlPersons[INFO_LEN] = { 0 };

//rtmp streaming
wchar_t rtmpStreamingCtrlPublishUrl[INFO_LEN] = { 0 };
wchar_t rtmpStreamingCtrlTransCoding[INFO_LEN] = { 0 };
wchar_t rtmpStreamingCtrlAdd[INFO_LEN] = { 0 };
wchar_t rtmpStreamingCtrlRemove[INFO_LEN] = { 0 };
wchar_t rtmpStreamingCtrlRemoveAll[INFO_LEN] = { 0 };
wchar_t agoraRtmpStateIdle[INFO_LEN] = { 0 };
wchar_t agoraRtmpStateConnecting[INFO_LEN] = { 0 };
wchar_t agoraRtmpStateRunning[INFO_LEN] = { 0 };
wchar_t agoraRtmpStateRunningSuccess[INFO_LEN] = { 0 };
wchar_t agoraRtmpStateRecovering[INFO_LEN] = { 0 };
wchar_t agoraRtmpStateInvalidArg[INFO_LEN] = { 0 };
wchar_t agoraRtmpStateEncrypted[INFO_LEN] = { 0 };
wchar_t agoraRtmpStateConnTimeout[INFO_LEN] = { 0 };
wchar_t agoraRtmpStateInrealErr[INFO_LEN] = { 0 };
wchar_t agoraRtmpStateServerErr[INFO_LEN] = { 0 };
wchar_t agoraRtmpStateTooOften[INFO_LEN] = { 0 };
wchar_t agoraRtmpStateReachLimit[INFO_LEN] = { 0 };
wchar_t agoraRtmpStateNotAuth[INFO_LEN] = { 0 };
wchar_t agoraRtmpStateNotFound[INFO_LEN] = { 0 };
wchar_t agoraRtmpStateNotSupported[INFO_LEN] = { 0 };

//rtmp Inject
wchar_t rtmpInjectCtrlUrl[INFO_LEN] = { 0 };
wchar_t rtmpInjectCtrlInject[INFO_LEN] = { 0 };
wchar_t rtmpInjectCtrlRemove[INFO_LEN] = { 0 };

wchar_t agoraInjectStartSucc[INFO_LEN] = { 0 };
wchar_t agoraInjectExist[INFO_LEN] = { 0 };
wchar_t agoraInjectStartUnAuth[INFO_LEN] = { 0 };
wchar_t agoraInjectStartTimeout[INFO_LEN] = { 0 };
wchar_t agoraInjectStartFailed[INFO_LEN] = { 0 };
wchar_t agoraInjectStopSuccess[INFO_LEN] = { 0 };
wchar_t agoraInjectNotFound[INFO_LEN] = { 0 };
wchar_t agoraInjectStopUnAuth[INFO_LEN] = { 0 };
wchar_t agoraInjectStopTimeout[INFO_LEN] = { 0 };
wchar_t agoraInjectStopFailed[INFO_LEN] = { 0 };
wchar_t agoraInjectBroken[INFO_LEN] = { 0 };
//video SEI
wchar_t videoSEIInformation[INFO_LEN] = { 0 };
wchar_t metadataCtrlSendSEI[INFO_LEN] = { 0 };
wchar_t metadataCtrlBtnSend[INFO_LEN] = { 0 };
wchar_t metadataCtrlBtnClear[INFO_LEN] = { 0 };

//beauty
wchar_t beautyCtrlLighteningContrastLevel[INFO_LEN] = { 0 };
wchar_t beautyCtrlLightening[INFO_LEN] = { 0 };
wchar_t beautyCtrlSmoothness[INFO_LEN] = { 0 };
wchar_t beautyCtrlRedness[INFO_LEN] = { 0 };
wchar_t beautyCtrlEnable[INFO_LEN] = { 0 };

//beauty audio
wchar_t beautyAudioCtrlSetAudioChange[INFO_LEN] = { 0 };
wchar_t beautyAudioCtrlUnSetAudioChange[INFO_LEN] = { 0 };
wchar_t beautyAudioCtrlChange[INFO_LEN] = { 0 };

//set audio profile
wchar_t audioProfileCtrlProfile[INFO_LEN] = { 0 };
wchar_t audioProfileCtrlScenario[INFO_LEN] = { 0 };
wchar_t audioProfileCtrlSetAudioProfile[INFO_LEN] = { 0 };
wchar_t audioProfileCtrlUnSetAudioProfile[INFO_LEN] = { 0 };

//audio mixing
wchar_t audioMixingCtrlMixingPath[INFO_LEN] = { 0 };
wchar_t audioMixingCtrlRepeatTimes[INFO_LEN] = { 0 };
wchar_t audioMixingCtrlSetAudioMixing[INFO_LEN] = { 0 };
wchar_t audioMixingCtrlUnSetAudioMixing[INFO_LEN] = { 0 };
wchar_t audioMixingCtrlOnlyLocal[INFO_LEN] = { 0 };
wchar_t audioMixingCtrlReplaceMicroPhone[INFO_LEN] = { 0 };

//screen share
wchar_t screenShareCtrlScreenCap[INFO_LEN] = { 0 };
wchar_t screenShareCtrlStartCap[INFO_LEN] = { 0 };
wchar_t screenShareCtrlEndCap[INFO_LEN] = { 0 };
wchar_t screenShareCtrlScreen[INFO_LEN] = { 0 };
wchar_t screenShareCtrlShareSCreen[INFO_LEN] = { 0 };
wchar_t screenShareCtrlStopShare[INFO_LEN] = { 0 };
wchar_t screenShareCtrlGeneralSettings[INFO_LEN] = { 0 };
wchar_t screenShareCtrlFPS[INFO_LEN] = { 0 };
wchar_t screenShareCtrlBitrate[INFO_LEN] = { 0 };
wchar_t screenShareCtrlShareCursor[INFO_LEN] = { 0 };
wchar_t screenShareCtrlUpdateCaptureParam[INFO_LEN] = { 0 };

wchar_t screenCtrlRectInfo[INFO_LEN] = { 0 };
wchar_t virtualScreenCtrlRectInfo[INFO_LEN] = { 0 };
wchar_t screenCtrlCustomRectInfo[INFO_LEN] = { 0 };

//custom video capture
wchar_t customVideoCaptureCtrlCaptureVideoDevice[INFO_LEN] = { 0 };
wchar_t customVideoCaptureCtrlSetExternlCapture[INFO_LEN] = { 0 };
wchar_t customVideoCaptureCtrlCancelExternlCapture[INFO_LEN] = { 0 };

//original video process
wchar_t OriginalVideoCtrlProc[INFO_LEN] = { 0 };
wchar_t OriginalVideoCtrlSetProc[INFO_LEN] = { 0 };
wchar_t OriginalVideoCtrlUnSetProc[INFO_LEN] = { 0 };

//custom audio capture
wchar_t customAudioCaptureCtrlCaptureAudioDeivce[INFO_LEN] = { 0 };
wchar_t customAudioCaptureCtrlSetExternlCapture[INFO_LEN] = { 0 };
wchar_t customAudioCaptureCtrlCancelExternlCapture[INFO_LEN] = { 0 };

//original audio process
wchar_t OriginalAudioCtrlProc[INFO_LEN] = { 0 };
wchar_t OriginalAudioCtrlSetProc[INFO_LEN] = { 0 };
wchar_t OriginalAudioCtrlUnSetProc[INFO_LEN] = { 0 };


//custom encrypt
wchar_t customEncryptCtrlEncrypt[INFO_LEN] = { 0 };
wchar_t customEncryptCtrlSetEncrypt[INFO_LEN] = { 0 };
wchar_t customEncryptCtrlCancelEncrypt[INFO_LEN] = { 0 };


//media player
wchar_t MeidaPlayerCtrlVideoSource[INFO_LEN] = { 0 };
wchar_t MeidaPlayerCtrlOpen[INFO_LEN] = { 0 };
wchar_t MeidaPlayerCtrlClose[INFO_LEN] = { 0 };
wchar_t MeidaPlayerCtrlPause[INFO_LEN] = { 0 };
wchar_t MeidaPlayerCtrlPlay[INFO_LEN] = { 0 };
wchar_t MeidaPlayerCtrlAttachPlayer[INFO_LEN] = { 0 };
wchar_t MeidaPlayerCtrlDettachPlayer[INFO_LEN] = { 0 };
wchar_t MeidaPlayerCtrlPublishVideo[INFO_LEN] = { 0 };
wchar_t MeidaPlayerCtrlUnPublishVideo[INFO_LEN] = { 0 };
wchar_t MeidaPlayerCtrlPublishAudio[INFO_LEN] = { 0 };
wchar_t MeidaPlayerCtrlUnPublishAudio[INFO_LEN] = { 0 };


//Multi camera
wchar_t MultiCameara[INFO_LEN] = { 0 };
wchar_t ScreenCapturer[INFO_LEN] = { 0 };

std::string cs2utf8(CString str)
{
    char szBuf[2 * MAX_PATH] = { 0 };
    WideCharToMultiByte(CP_UTF8, 0, str.GetBuffer(0), str.GetLength(), szBuf, 2 * MAX_PATH, NULL, NULL);
    return szBuf;
}

CString utf82cs(std::string utf8)
{
    TCHAR szBuf[2 * MAX_PATH] = { 0 };
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), 2 * MAX_PATH, szBuf, 2 * MAX_PATH);
    return szBuf;
}

CString getCurrentTime()
{
    SYSTEMTIME st = { 0 };
    GetLocalTime(&st);
    CString strTime;
    strTime.Format(_T("%02d:%02d:%02d %02d"), st.wHour, st.wHour, st.wMinute, st.wMilliseconds);
    return strTime;
}

void InitKeyInfomation()
{
    //common
    _tcscpy_s(commonGroupDoc, INFO_LEN, Str(_T("Common.Group.Doc")));
    _tcscpy_s(commonDocumentWebsite, INFO_LEN, Str(_T("Common.Document.Website")));
    _tcscpy_s(commonFAQWebsite, INFO_LEN, Str(_T("Common.FAQ.Website")));
    _tcscpy_s(commonRegisterWebsite, INFO_LEN, Str(_T("Common.Register.Website")));
    _tcscpy_s(commonDemoWebsite, INFO_LEN, Str(_T("Common.Demo.Website")));
    _tcscpy_s(commonBasicScene, INFO_LEN, Str(_T("Common.Basic.Scene")));
    _tcscpy_s(commonAdvanceScene, INFO_LEN, Str(_T("Common.Advanced.Scene")));

    _tcscpy_s(commonCtrlChannel, INFO_LEN, Str(_T("Common.Ctrl.ChannelName")));
    _tcscpy_s(commonCtrlJoinChannel, INFO_LEN, Str(_T("Common.Ctrl.JoinChannel")));
    _tcscpy_s(commonCtrlLeaveChannel, INFO_LEN, Str(_T("Common.Ctrl.LeaveChannel")));
    _tcscpy_s(commonCtrlClientRole, INFO_LEN, Str(_T("Common.Ctrl.ClientRole")));
    _tcscpy_s(liveCtrlPersons, INFO_LEN, Str(_T("LiveBroadcasting.Ctrl.Persons")));

    //basic scene list
    _tcscpy_s(basicLiveBroadcasting, INFO_LEN, Str(_T("Basic.LiveBroadcasting")));
    //advanced  scene list
    _tcscpy_s(advancedRtmpInject, INFO_LEN, Str(_T("Advanced.RtmpInject")));
    _tcscpy_s(advancedRtmpStreaming, INFO_LEN, Str(_T("Advanced.RtmpStreaming")));
    _tcscpy_s(advancedVideoMetadata, INFO_LEN, Str(_T("Advanced.Metadata")));
    _tcscpy_s(advancedCustomEncrypt, INFO_LEN, Str(_T("Advanced.CustomEncrypt")));

    _tcscpy_s(advancedScreenCap, INFO_LEN, Str(_T("Advanced.ScreenCap")));
    _tcscpy_s(advancedBeauty, INFO_LEN, Str(_T("Advanced.Beauty")));
    _tcscpy_s(advancedBeautyAudio, INFO_LEN, Str(_T("Advanced.BeautyAudio")));

    _tcscpy_s(advancedAudioProfile, INFO_LEN, Str(_T("Advanced.AudioProfile")));
    _tcscpy_s(advancedAudioMixing, INFO_LEN, Str(_T("Advanced.AudioMixing")));

    _tcscpy_s(advancedCustomVideoCapture, INFO_LEN, Str(_T("Advanced.CustomVideoCapture")));
    _tcscpy_s(advancedOriginalVideo, INFO_LEN, Str(_T("Advanced.OriginalVideo")));

    _tcscpy_s(advancedCustomAudioCapture, INFO_LEN, Str(_T("Advanced.CustomAudioCapture")));
    _tcscpy_s(advancedOriginalAudio, INFO_LEN, Str(_T("Advanced.OriginalAudio")));
    _tcscpy_s(advancedMediaPlayer, INFO_LEN, Str(_T("Advanced.MediaPlayer")));



    //agora
    _tcscpy_s(agoraRoleBroadcaster, INFO_LEN, Str(_T("Agora.ClientRole.Broadcaster")));
    _tcscpy_s(agoraRoleAudience, INFO_LEN, Str(_T("Agora.ClientRole.Audienc")));


    //rtmp streaming
    _tcscpy_s(rtmpStreamingCtrlPublishUrl, INFO_LEN, Str(_T("RtmpStreaming.Ctrl.PublishUrl")));
    _tcscpy_s(rtmpStreamingCtrlAdd, INFO_LEN, Str(_T("RtmpStreaming.Ctrl.Add")));
    _tcscpy_s(rtmpStreamingCtrlRemove, INFO_LEN, Str(_T("RtmpStreaming.Ctrl.Remove")));
    _tcscpy_s(rtmpStreamingCtrlTransCoding, INFO_LEN, Str(_T("RtmpStreaming.Ctrl.TransCoding")));
    _tcscpy_s(rtmpStreamingCtrlRemoveAll, INFO_LEN, Str(_T("RtmpStreaming.Ctrl.RemoveAll")));
    //rtmp inject
    _tcscpy_s(rtmpInjectCtrlUrl, INFO_LEN, Str(_T("RtmpInject.Ctrl.Url")));
    _tcscpy_s(rtmpInjectCtrlInject, INFO_LEN, Str(_T("RtmpInject.Ctrl.Inject")));
    _tcscpy_s(rtmpInjectCtrlRemove, INFO_LEN, Str(_T("RtmpInject.Ctrl.Remove")));
    //rtmp state changed
    _tcscpy_s(agoraRtmpStateIdle, INFO_LEN, Str(_T("Agora.RtmpStateChange.IDLE")));
    _tcscpy_s(agoraRtmpStateConnecting, INFO_LEN, Str(_T("Agora.RtmpStateChange.Connecting")));
    _tcscpy_s(agoraRtmpStateRunning, INFO_LEN, Str(_T("Agora.RtmpStateChange.Running")));
    _tcscpy_s(agoraRtmpStateRunningSuccess, INFO_LEN, Str(_T("Agora.RtmpStateChange.Running.Success")));
    _tcscpy_s(agoraRtmpStateRecovering, INFO_LEN, Str(_T("Agora.RtmpStateChange.Recovering")));
    _tcscpy_s(agoraRtmpStateInvalidArg, INFO_LEN, Str(_T("Agora.RtmpStateChange.InvalidArg")));
    _tcscpy_s(agoraRtmpStateEncrypted, INFO_LEN, Str(_T("Agora.RtmpStateChange.Encrypted")));
    _tcscpy_s(agoraRtmpStateConnTimeout, INFO_LEN, Str(_T("Agora.RtmpStateChange.ConnTimeout")));
    _tcscpy_s(agoraRtmpStateInrealErr, INFO_LEN, Str(_T("Agora.RtmpStateChange.InrealErr")));
    _tcscpy_s(agoraRtmpStateServerErr, INFO_LEN, Str(_T("Agora.RtmpStateChange.ServerErr")));
    _tcscpy_s(agoraRtmpStateTooOften, INFO_LEN, Str(_T("Agora.RtmpStateChange.TooOften")));
    _tcscpy_s(agoraRtmpStateReachLimit, INFO_LEN, Str(_T("Agora.RtmpStateChange.ReachLimit")));
    _tcscpy_s(agoraRtmpStateNotAuth, INFO_LEN, Str(_T("Agora.RtmpStateChange.NotAuth")));
    _tcscpy_s(agoraRtmpStateNotFound, INFO_LEN, Str(_T("Agora.RtmpStateChange.NotFound")));
    _tcscpy_s(agoraRtmpStateNotSupported, INFO_LEN, Str(_T("Agora.RtmpStateChange.NotSupported")));

    //inject status
    _tcscpy_s(agoraInjectStartSucc, INFO_LEN, Str(_T("Agora.InjectStatus.StartSuccess")));
    _tcscpy_s(agoraInjectExist, INFO_LEN, Str(_T("Agora.InjectStatus.Exist")));
    _tcscpy_s(agoraInjectStartUnAuth, INFO_LEN, Str(_T("Agora.InjectStatus.StartUnAuth")));
    _tcscpy_s(agoraInjectStartTimeout, INFO_LEN, Str(_T("Agora.InjectStatus.StartTimeout")));
    _tcscpy_s(agoraInjectStartFailed, INFO_LEN, Str(_T("Agora.InjectStatus.StartFailed")));
    _tcscpy_s(agoraInjectStopSuccess, INFO_LEN, Str(_T("Agora.InjectStatus.StopSuccess")));
    _tcscpy_s(agoraInjectNotFound, INFO_LEN, Str(_T("Agora.InjectStatus.NotFound")));
    _tcscpy_s(agoraInjectStopUnAuth, INFO_LEN, Str(_T("Agora.InjectStatus.StopUnAuth")));
    _tcscpy_s(agoraInjectStopTimeout, INFO_LEN, Str(_T("Agora.InjectStatus.StopTimeout")));
    _tcscpy_s(agoraInjectStopFailed, INFO_LEN, Str(_T("Agora.InjectStatus.StopFailed")));
    _tcscpy_s(agoraInjectBroken, INFO_LEN, Str(_T("Agora.InjectStatus.Broken")));


    _tcscpy_s(videoSEIInformation, INFO_LEN, Str(_T("MetaData.Info")));
    _tcscpy_s(metadataCtrlSendSEI, INFO_LEN, Str(_T("MetaData.Ctrl.SendSEI")));
    _tcscpy_s(metadataCtrlBtnSend, INFO_LEN, Str(_T("MetaData.Ctrl.Send")));
    _tcscpy_s(metadataCtrlBtnClear, INFO_LEN, Str(_T("MetaData.Ctrl.Clear")));


    //screen share
    _tcscpy_s(screenShareCtrlScreenCap, INFO_LEN, Str(_T("ScreenShare.Ctrl.ScreenCap")));
    _tcscpy_s(screenShareCtrlStartCap, INFO_LEN, Str(_T("ScreenShare.Ctrl.StartCap")));
    _tcscpy_s(screenShareCtrlEndCap, INFO_LEN, Str(_T("ScreenShare.Ctrl.EndCap")));
    _tcscpy_s(screenShareCtrlScreen, INFO_LEN, Str(_T("ScreenShare.Ctrl.Screen")));
    _tcscpy_s(screenShareCtrlShareSCreen, INFO_LEN, Str(_T("ScreenShare.Ctrl.ShareSCreen")));
    _tcscpy_s(screenShareCtrlStopShare, INFO_LEN, Str(_T("ScreenShare.Ctrl.StopShare")));
    _tcscpy_s(screenShareCtrlGeneralSettings, INFO_LEN, Str(_T("ScreenShare.Ctrl.GeneralSettings")));
    _tcscpy_s(screenShareCtrlFPS, INFO_LEN, Str(_T("ScreenShare.Ctrl.FPS")));
    _tcscpy_s(screenShareCtrlBitrate, INFO_LEN, Str(_T("ScreenShare.Ctrl.Bitrate")));
    _tcscpy_s(screenShareCtrlShareCursor, INFO_LEN, Str(_T("ScreenShare.Ctrl.ShareCursor")));
    _tcscpy_s(screenShareCtrlUpdateCaptureParam, INFO_LEN, Str(_T("ScreenShare.Ctrl.UpdateCaptureParam")));


    _tcscpy_s(screenCtrlRectInfo, INFO_LEN, Str(_T("Share.Ctrl.Screen.RectInfo")));
    _tcscpy_s(virtualScreenCtrlRectInfo, INFO_LEN, Str(_T("Share.Ctrl.VirtualScreen.RectInfo")));
    _tcscpy_s(screenCtrlCustomRectInfo, INFO_LEN, Str(_T("Share.Ctrl.Screen.CustomInfo")));

    //beauty
    _tcscpy_s(beautyCtrlLighteningContrastLevel, INFO_LEN, Str(_T("Beauty.Ctrl.LighteningContrastLevel")));
    _tcscpy_s(beautyCtrlLightening, INFO_LEN, Str(_T("Beauty.Ctrl.Lightening")));
    _tcscpy_s(beautyCtrlRedness, INFO_LEN, Str(_T("Beauty.Ctrl.Redness")));
    _tcscpy_s(beautyCtrlSmoothness, INFO_LEN, Str(_T("Beauty.Ctrl.Smoothness")));
    _tcscpy_s(beautyCtrlEnable, INFO_LEN, Str(_T("Beauty.Ctrl.Enable")));


    //beauty audio
    _tcscpy_s(beautyAudioCtrlChange, INFO_LEN, Str(_T("BeautyAudio.Ctrl.Change")));
    _tcscpy_s(beautyAudioCtrlSetAudioChange, INFO_LEN, Str(_T("BeautyAudio.Ctrl.SetAudioChange")));
    _tcscpy_s(beautyAudioCtrlUnSetAudioChange, INFO_LEN, Str(_T("BeautyAudio.Ctrl.UnSetAudioChange")));



    //audio profile
    _tcscpy_s(audioProfileCtrlSetAudioProfile, INFO_LEN, Str(_T("AudioProfile.Ctrl.SetAudioProfile")));
    _tcscpy_s(audioProfileCtrlProfile, INFO_LEN, Str(_T("AudioProfile.Ctrl.Profile")));
    _tcscpy_s(audioProfileCtrlScenario, INFO_LEN, Str(_T("AudioProfile.Ctrl.Scenario")));
    _tcscpy_s(audioProfileCtrlUnSetAudioProfile, INFO_LEN, Str(_T("AudioProfile.Ctrl.UnSetAudioProfile")));

    //audio mixing
    _tcscpy_s(audioMixingCtrlMixingPath, INFO_LEN, Str(_T("AudioMixing.Ctrl.MixingPath")));
    _tcscpy_s(audioMixingCtrlOnlyLocal, INFO_LEN, Str(_T("AudioMixing.Ctrl.OnlyLocal")));
    _tcscpy_s(audioMixingCtrlSetAudioMixing, INFO_LEN, Str(_T("AudioMixing.Ctrl.SetAudioMixing")));
    _tcscpy_s(audioMixingCtrlRepeatTimes, INFO_LEN, Str(_T("AudioMixing.Ctrl.RepeatTimes")));
    _tcscpy_s(audioMixingCtrlUnSetAudioMixing, INFO_LEN, Str(_T("AudioMixing.Ctrl.UnSetAudioMixing")));
    _tcscpy_s(audioMixingCtrlReplaceMicroPhone, INFO_LEN, Str(_T("AudioMixing.Ctrl.ReplaceMicroPhone")));

    //custom video capture
    _tcscpy_s(customVideoCaptureCtrlCaptureVideoDevice, INFO_LEN, Str(_T("CustomVideoCapture.Ctrl.CaptureVideo")));
    _tcscpy_s(customVideoCaptureCtrlSetExternlCapture, INFO_LEN, Str(_T("CustomVideoCapture.Ctrl.SetExternlCap")));
    _tcscpy_s(customVideoCaptureCtrlCancelExternlCapture, INFO_LEN, Str(_T("CustomVideoCapture.Ctrl.CancelExternlCap")));

    //original video process
    _tcscpy_s(OriginalVideoCtrlProc, INFO_LEN, Str(_T("OriginalVideo.Ctrl.Proc")));
    _tcscpy_s(OriginalVideoCtrlSetProc, INFO_LEN, Str(_T("OriginalVideo.Ctrl.SetProc")));
    _tcscpy_s(OriginalVideoCtrlUnSetProc, INFO_LEN, Str(_T("OriginalVideo.Ctrl.UnSetProc")));

    //custom audio capture
    _tcscpy_s(customAudioCaptureCtrlCaptureAudioDeivce, INFO_LEN, Str(_T("CustomAudioCapture.Ctrl.CaptureAudio")));
    _tcscpy_s(customAudioCaptureCtrlSetExternlCapture, INFO_LEN, Str(_T("CustomAudioCapture.Ctrl.SetExternlCap")));
    _tcscpy_s(customAudioCaptureCtrlCancelExternlCapture, INFO_LEN, Str(_T("CustomAudioCapture.Ctrl.CancelExternlCap")));

    //original video process
    _tcscpy_s(OriginalAudioCtrlProc, INFO_LEN, Str(_T("OriginalVideo.Ctrl.Proc")));
    _tcscpy_s(OriginalAudioCtrlSetProc, INFO_LEN, Str(_T("OriginalVideo.Ctrl.SetProc")));
    _tcscpy_s(OriginalAudioCtrlUnSetProc, INFO_LEN, Str(_T("OriginalVideo.Ctrl.UnSetProc")));

    //custom encrypt
    _tcscpy_s(customEncryptCtrlEncrypt, INFO_LEN, Str(_T("CustomEncrypt.Ctrl.Encrypt")));
    _tcscpy_s(customEncryptCtrlSetEncrypt, INFO_LEN, Str(_T("CustomEncrypt.Ctrl.SetEncrypt")));
    _tcscpy_s(customEncryptCtrlCancelEncrypt, INFO_LEN, Str(_T("CustomEncrypt.Ctrl.CancelEncrypt")));

    //media player
    _tcscpy_s(MeidaPlayerCtrlVideoSource, INFO_LEN, Str(_T("MeidaPlayer.Ctrl.VideoSource")));
    _tcscpy_s(MeidaPlayerCtrlOpen, INFO_LEN, Str(_T("MeidaPlayer.Ctrl.Open")));
    _tcscpy_s(MeidaPlayerCtrlClose, INFO_LEN, Str(_T("MeidaPlayer.Ctrl.Close")));
    _tcscpy_s(MeidaPlayerCtrlPause, INFO_LEN, Str(_T("MeidaPlayer.Ctrl.Pause")));
    _tcscpy_s(MeidaPlayerCtrlPlay, INFO_LEN, Str(_T("MeidaPlayer.Ctrl.Play")));
    _tcscpy_s(MeidaPlayerCtrlAttachPlayer, INFO_LEN, Str(_T("MeidaPlayer.Ctrl.AttachPlayer")));
    _tcscpy_s(MeidaPlayerCtrlDettachPlayer, INFO_LEN, Str(_T("MeidaPlayer.Ctrl.DettachPlayer")));
    _tcscpy_s(MeidaPlayerCtrlPublishVideo, INFO_LEN, Str(_T("MeidaPlayer.Ctrl.PublishVideo")));
    _tcscpy_s(MeidaPlayerCtrlUnPublishVideo, INFO_LEN, Str(_T("MeidaPlayer.Ctrl.UnPublishVideo")));
    _tcscpy_s(MeidaPlayerCtrlPublishAudio, INFO_LEN, Str(_T("MeidaPlayer.Ctrl.PublishAudio")));
    _tcscpy_s(MeidaPlayerCtrlUnPublishAudio, INFO_LEN, Str(_T("MeidaPlayer.Ctrl.UnPublishAudio")));

    _tcscpy_s(MultiCameara, INFO_LEN, Str(_T("Advanced.MultiCamera")));
	_tcscpy_s(ScreenCapturer, INFO_LEN, Str(_T("Advanced.ScreenCapturer")));
	
}

