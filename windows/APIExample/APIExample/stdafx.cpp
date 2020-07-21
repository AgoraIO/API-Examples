
// stdafx.cpp : source file that includes just the standard includes
// APIExample.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include "CConfig.h"
wchar_t commonGroupDoc[INFO_LEN]        = {0};
wchar_t commonDocumentWebsite[INFO_LEN] = {0};
wchar_t commonFAQWebsite[INFO_LEN]      = {0};
wchar_t commonRegisterWebsite[INFO_LEN] = {0};
wchar_t commonDemoWebsite[INFO_LEN]     = {0};
wchar_t commonBasicScene[INFO_LEN]      = {0};
wchar_t commonAdvanceScene[INFO_LEN]    = {0};
//agora enum
wchar_t agoraRoleBroadcaster[INFO_LEN] = { 0 };
wchar_t agoraRoleAudience[INFO_LEN]    = { 0 };
//common ctrl
wchar_t commonCtrlChannel[INFO_LEN]      = { 0 };
wchar_t commonCtrlJoinChannel[INFO_LEN]  = { 0 };
wchar_t commonCtrlLeaveChannel[INFO_LEN] = { 0 };
wchar_t commonCtrlClientRole[INFO_LEN]   = { 0 };
//scene list
wchar_t basicLiveBroadcasting[INFO_LEN] = { 0 };
wchar_t advancedRtmpInject[INFO_LEN]    = { 0 };
wchar_t advancedRtmpStreaming[INFO_LEN] = { 0 };
wchar_t advancedVideoMetadata[INFO_LEN] = { 0 };

wchar_t advancedScreenCap[INFO_LEN] = { 0 };
wchar_t advancedBeauty[INFO_LEN] = { 0 };
wchar_t advancedBeautyAudio[INFO_LEN] = { 0 };
wchar_t advancedAudioProfile[INFO_LEN] = { 0 };
wchar_t advancedAudioMixing[INFO_LEN] = { 0 };
wchar_t advancedCustomVideoCapture[INFO_LEN] = { 0 };
wchar_t advancedCustomAudioCapture[INFO_LEN] = { 0 };



//live broadcasting
wchar_t liveCtrlPersons[INFO_LEN] = { 0 };

//rtmp streaming
wchar_t rtmpStreamingCtrlPublishUrl[INFO_LEN] = { 0 };
wchar_t rtmpStreamingCtrlAdd[INFO_LEN]        = { 0 };
wchar_t rtmpStreamingCtrlRemove[INFO_LEN]     = { 0 };
wchar_t rtmpStreamingCtrlRemoveAll[INFO_LEN]  = { 0 };
wchar_t agoraRtmpStateIdle[INFO_LEN]          = { 0 };
wchar_t agoraRtmpStateConnecting[INFO_LEN]    = { 0 };
wchar_t agoraRtmpStateRunning[INFO_LEN]       = { 0 };
wchar_t agoraRtmpStateRunningSuccess[INFO_LEN] = { 0 };
wchar_t agoraRtmpStateRecovering[INFO_LEN]     = { 0 };
wchar_t agoraRtmpStateInvalidArg[INFO_LEN]     = { 0 };
wchar_t agoraRtmpStateEncrypted[INFO_LEN]      = { 0 };
wchar_t agoraRtmpStateConnTimeout[INFO_LEN] = { 0 };
wchar_t agoraRtmpStateInrealErr[INFO_LEN]   = { 0 };
wchar_t agoraRtmpStateServerErr[INFO_LEN]   = { 0 };
wchar_t agoraRtmpStateTooOften[INFO_LEN]    = { 0 };
wchar_t agoraRtmpStateReachLimit[INFO_LEN]  = { 0 };
wchar_t agoraRtmpStateNotAuth[INFO_LEN]     = { 0 };
wchar_t agoraRtmpStateNotFound[INFO_LEN]    = { 0 };
wchar_t agoraRtmpStateNotSupported[INFO_LEN] = { 0 };

//rtmp Inject
wchar_t rtmpInjectCtrlUrl[INFO_LEN]    = { 0 };
wchar_t rtmpInjectCtrlInject[INFO_LEN] = { 0 };
wchar_t rtmpInjectCtrlRemove[INFO_LEN] = { 0 };

wchar_t agoraInjectStartSucc[INFO_LEN] = { 0 };
wchar_t agoraInjectExist[INFO_LEN]     = { 0 };
wchar_t agoraInjectStartUnAuth[INFO_LEN]  = { 0 };
wchar_t agoraInjectStartTimeout[INFO_LEN] = { 0 };
wchar_t agoraInjectStartFailed[INFO_LEN]  = { 0 };
wchar_t agoraInjectStopSuccess[INFO_LEN]  = { 0 };
wchar_t agoraInjectNotFound[INFO_LEN]     = { 0 };
wchar_t agoraInjectStopUnAuth[INFO_LEN]   = { 0 };
wchar_t agoraInjectStopTimeout[INFO_LEN]  = { 0 };
wchar_t agoraInjectStopFailed[INFO_LEN]   = { 0 };
wchar_t agoraInjectBroken[INFO_LEN]       = { 0 };
//video SEI
wchar_t videoSEIInformation[INFO_LEN]     = { 0 };
wchar_t metadataCtrlSendSEI[INFO_LEN] = { 0 };
wchar_t metadataCtrlBtnSend[INFO_LEN] = { 0 };
wchar_t metadataCtrlBtnClear[INFO_LEN] = { 0 };

//beauty
wchar_t beautyCtrlLighteningContrastLevel[INFO_LEN] = { 0 };
wchar_t beautyCtrlLightening[INFO_LEN] = {0};
wchar_t beautyCtrlSmoothness[INFO_LEN] = {0};
wchar_t beautyCtrlRedness[INFO_LEN] = {0};
wchar_t beautyCtrlEnable[INFO_LEN];

//beauty audio
wchar_t beautyAudioCtrlSetAudioChange[INFO_LEN] = {0};
wchar_t beautyAudioCtrlUnSetAudioChange[INFO_LEN] = {0};
wchar_t beautyAudioCtrlChange[INFO_LEN] = {0};

//set audio profile
wchar_t audioProfileCtrlProfile[INFO_LEN] = { 0 };
wchar_t audioProfileCtrlScenario[INFO_LEN] = { 0 };
wchar_t audioProfileCtrlSetAudioProfile[INFO_LEN] = { 0 };
wchar_t audioProfileCtrlUnSetAudioProfile[INFO_LEN] = {0};

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

//custom video capture
wchar_t customVideoCaptureCtrlCaptureVideoDevice[INFO_LEN] = { 0 };
wchar_t customVideoCaptureCtrlSetExternlCapture[INFO_LEN] = { 0 };
wchar_t customVideoCaptureCtrlCancelExternlCapture[INFO_LEN] = { 0 };

//custom audio capture
wchar_t customAudioCaptureCtrlCaptureAudioDeivce[INFO_LEN] = { 0 };
wchar_t customAudioCaptureCtrlSetExternlCapture[INFO_LEN] = { 0 };
wchar_t customAudioCaptureCtrlCancelExternlCapture[INFO_LEN] = { 0 };



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

    _tcscpy_s(advancedScreenCap, INFO_LEN, Str(_T("Advanced.ScreenCap")));
	_tcscpy_s(advancedBeauty, INFO_LEN, Str(_T("Advanced.Beauty")));
	_tcscpy_s(advancedBeautyAudio, INFO_LEN, Str(_T("Advanced.BeautyAudio")));

	_tcscpy_s(advancedAudioProfile, INFO_LEN, Str(_T("Advanced.AudioProfile")));
	_tcscpy_s(advancedAudioMixing, INFO_LEN, Str(_T("Advanced.AudioMixing")));

    _tcscpy_s(advancedCustomVideoCapture, INFO_LEN, Str(_T("Advanced.CustomVideoCapture")));
    _tcscpy_s(advancedCustomAudioCapture, INFO_LEN, Str(_T("Advanced.CustomAudioCapture")));


    //agora
    _tcscpy_s(agoraRoleBroadcaster, INFO_LEN, Str(_T("Agora.ClientRole.Broadcaster")));
    _tcscpy_s(agoraRoleAudience, INFO_LEN, Str(_T("Agora.ClientRole.Audienc")));


   //rtmp streaming
    _tcscpy_s(rtmpStreamingCtrlPublishUrl, INFO_LEN, Str(_T("RtmpStreaming.Ctrl.PublishUrl")));
    _tcscpy_s(rtmpStreamingCtrlAdd, INFO_LEN, Str(_T("RtmpStreaming.Ctrl.Add")));
    _tcscpy_s(rtmpStreamingCtrlRemove, INFO_LEN, Str(_T("RtmpStreaming.Ctrl.Remove")));
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

    //custom audio capture
    _tcscpy_s(customAudioCaptureCtrlCaptureAudioDeivce, INFO_LEN, Str(_T("CustomAudioCapture.Ctrl.CaptureAudio")));
    _tcscpy_s(customAudioCaptureCtrlSetExternlCapture, INFO_LEN, Str(_T("CustomAudioCapture.Ctrl.SetExternlCap")));
    _tcscpy_s(customAudioCaptureCtrlCancelExternlCapture, INFO_LEN, Str(_T("CustomAudioCapture.Ctrl.CancelExternlCap")));
    
 /*   
 
   
    _tcscpy_s(, INFO_LEN, Str(_T("")));

 _tcscpy_s(, INFO_LEN, Str(_T("")));
    _tcscpy_s(, INFO_LEN, Str(_T("")));
    _tcscpy_s(, INFO_LEN, Str(_T("")));
    _tcscpy_s(, INFO_LEN, Str(_T("")));
    _tcscpy_s(, INFO_LEN, Str(_T("")));
    _tcscpy_s(, INFO_LEN, Str(_T("")));
    _tcscpy_s(, INFO_LEN, Str(_T("")));
    _tcscpy_s(, INFO_LEN, Str(_T("")));
    _tcscpy_s(, INFO_LEN, Str(_T("")));
    _tcscpy_s(, INFO_LEN, Str(_T("")));
    _tcscpy_s(, INFO_LEN, Str(_T("")));
    _tcscpy_s(, INFO_LEN, Str(_T("")));
*/
}
