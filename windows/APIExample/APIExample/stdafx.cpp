
// stdafx.cpp : source file that includes just the standard includes
// APIExample.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include "CConfig.h"
wchar_t commonGroupDoc[INFO_LEN]		= { 0 };
wchar_t commonDocumentWebsite[INFO_LEN] = { 0 };
wchar_t commonFAQWebsite[INFO_LEN]		= { 0 };
wchar_t commonRegisterWebsite[INFO_LEN] = { 0 };
wchar_t commonDemoWebsite[INFO_LEN]		= { 0 };
wchar_t commonBasicScene[INFO_LEN]		= { 0 };
wchar_t commonAdvanceScene[INFO_LEN]	= { 0 };
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
wchar_t advancedCustomEncrypt[INFO_LEN] = { 0 };

wchar_t advancedScreenCap[INFO_LEN]				= { 0 };
wchar_t advancedBeauty[INFO_LEN]				= { 0 };
wchar_t advancedBeautyAudio[INFO_LEN]			= { 0 };
wchar_t advancedAudioProfile[INFO_LEN]			= { 0 };
wchar_t advancedAudioMixing[INFO_LEN]			= { 0 };
wchar_t advancedCustomVideoCapture[INFO_LEN]	= { 0 };
wchar_t advancedOriginalVideo[INFO_LEN]			= { 0 };
wchar_t advancedCustomAudioCapture[INFO_LEN]	= { 0 };
wchar_t advancedOriginalAudio[INFO_LEN]			= { 0 };
wchar_t advancedMediaPlayer[INFO_LEN]			= { 0 };
wchar_t advancedMultiChannel[INFO_LEN]			= { 0 };
wchar_t advancedMultiVideoSource[INFO_LEN]		= { 0 };
wchar_t advancedAudioEffect[INFO_LEN] = { 0 };
wchar_t advancedPerCallTest[INFO_LEN] = { 0 };
wchar_t advancedAudioVolume[INFO_LEN] = { 0 };
wchar_t advancedReportInCall[INFO_LEN] = { 0 };
wchar_t advancedRegionConn[INFO_LEN] = { 0 };
wchar_t advancedCrossChannel[INFO_LEN] = { 0 };
wchar_t advancedMediaEncrypt[INFO_LEN] = { 0 };


//live broadcasting
wchar_t liveCtrlPersons[INFO_LEN] = { 0 };

//rtmp streaming
wchar_t rtmpStreamingCtrlPublishUrl[INFO_LEN]	= { 0 };
wchar_t rtmpStreamingCtrlTransCoding[INFO_LEN]	= { 0 };
wchar_t rtmpStreamingCtrlAdd[INFO_LEN]			= { 0 };
wchar_t rtmpStreamingCtrlRemove[INFO_LEN]		= { 0 };
wchar_t rtmpStreamingCtrlRemoveAll[INFO_LEN]	= { 0 };
wchar_t agoraRtmpStateIdle[INFO_LEN]			= { 0 };
wchar_t agoraRtmpStateConnecting[INFO_LEN]		= { 0 };
wchar_t agoraRtmpStateRunning[INFO_LEN]			= { 0 };
wchar_t agoraRtmpStateRunningSuccess[INFO_LEN]	= { 0 };
wchar_t agoraRtmpStateRecovering[INFO_LEN]		= { 0 };
wchar_t agoraRtmpStateInvalidArg[INFO_LEN]		= { 0 };
wchar_t agoraRtmpStateEncrypted[INFO_LEN]		= { 0 };
wchar_t agoraRtmpStateConnTimeout[INFO_LEN]		= { 0 };
wchar_t agoraRtmpStateInrealErr[INFO_LEN]		= { 0 };
wchar_t agoraRtmpStateServerErr[INFO_LEN]		= { 0 };
wchar_t agoraRtmpStateTooOften[INFO_LEN]		= { 0 };
wchar_t agoraRtmpStateReachLimit[INFO_LEN]		= { 0 };
wchar_t agoraRtmpStateNotAuth[INFO_LEN]			= { 0 };
wchar_t agoraRtmpStateNotFound[INFO_LEN]		= { 0 };
wchar_t agoraRtmpStateNotSupported[INFO_LEN]	= { 0 };

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
wchar_t videoSEIInformation[INFO_LEN]	= { 0 };
wchar_t metadataCtrlSendSEI[INFO_LEN]	= { 0 };
wchar_t metadataCtrlBtnSend[INFO_LEN]	= { 0 };
wchar_t metadataCtrlBtnClear[INFO_LEN]	= { 0 };

//beauty
wchar_t beautyCtrlLighteningContrastLevel[INFO_LEN] = { 0 };
wchar_t beautyCtrlLightening[INFO_LEN]				= { 0 };
wchar_t beautyCtrlSmoothness[INFO_LEN]				= { 0 };
wchar_t beautyCtrlRedness[INFO_LEN]					= { 0 };
wchar_t beautyCtrlEnable[INFO_LEN]					= { 0 };

//beauty audio
wchar_t beautyAudioCtrlSetAudioChange[INFO_LEN]		= { 0 };
wchar_t beautyAudioCtrlUnSetAudioChange[INFO_LEN]	= { 0 };
wchar_t beautyAudioCtrlChange[INFO_LEN]				= { 0 };

wchar_t beautyAudioCtrlPreSet[INFO_LEN] = { 0 };
wchar_t beautyAudioCtrlParam1[INFO_LEN] = { 0 };
wchar_t beautyAudioCtrlParam2[INFO_LEN] = { 0 };


//set audio profile
wchar_t audioProfileCtrlProfile[INFO_LEN]			= { 0 };
wchar_t audioProfileCtrlScenario[INFO_LEN]			= { 0 };
wchar_t audioProfileCtrlSetAudioProfile[INFO_LEN]	= { 0 };
wchar_t audioProfileCtrlUnSetAudioProfile[INFO_LEN] = { 0 };

//audio mixing
wchar_t audioMixingCtrlMixingPath[INFO_LEN]			= { 0 };
wchar_t audioMixingCtrlRepeatTimes[INFO_LEN]		= { 0 };
wchar_t audioMixingCtrlSetAudioMixing[INFO_LEN]		= { 0 };
wchar_t audioMixingCtrlUnSetAudioMixing[INFO_LEN]	= { 0 };
wchar_t audioMixingCtrlOnlyLocal[INFO_LEN]			= { 0 };
wchar_t audioMixingCtrlReplaceMicroPhone[INFO_LEN]	= { 0 };

//screen share
wchar_t screenShareCtrlScreenCap[INFO_LEN]				= { 0 };
wchar_t screenShareCtrlStartCap[INFO_LEN]				= { 0 };
wchar_t screenShareCtrlEndCap[INFO_LEN]					= { 0 };
wchar_t screenShareCtrlScreen[INFO_LEN]					= { 0 };
wchar_t screenShareCtrlShareSCreen[INFO_LEN]			= { 0 };
wchar_t screenShareCtrlStopShare[INFO_LEN]				= { 0 };
wchar_t screenShareCtrlGeneralSettings[INFO_LEN]		= { 0 };
wchar_t screenShareCtrlFPS[INFO_LEN]					= { 0 };
wchar_t screenShareCtrlBitrate[INFO_LEN]				= { 0 };
wchar_t screenShareCtrlShareCursor[INFO_LEN]			= { 0 };
wchar_t screenShareCtrlUpdateCaptureParam[INFO_LEN]		= { 0 };

wchar_t screenCtrlRectInfo[INFO_LEN]					= { 0 };
wchar_t virtualScreenCtrlRectInfo[INFO_LEN]				= { 0 };
wchar_t screenCtrlCustomRectInfo[INFO_LEN]				= { 0 };

//custom video capture
wchar_t customVideoCaptureCtrlCaptureVideoDevice[INFO_LEN]		= { 0 };
wchar_t customVideoCaptureCtrlSetExternlCapture[INFO_LEN]		= { 0 };
wchar_t customVideoCaptureCtrlCancelExternlCapture[INFO_LEN]	= { 0 };

//original video process
wchar_t OriginalVideoCtrlProc[INFO_LEN]			= { 0 };
wchar_t OriginalVideoCtrlSetProc[INFO_LEN]		= { 0 };
wchar_t OriginalVideoCtrlUnSetProc[INFO_LEN]	= { 0 };

//custom audio capture
wchar_t customAudioCaptureCtrlCaptureAudioDeivce[INFO_LEN]		= { 0 };
wchar_t customAudioCaptureCtrlSetExternlCapture[INFO_LEN]		= { 0 };
wchar_t customAudioCaptureCtrlCancelExternlCapture[INFO_LEN]	= { 0 };

//original audio process
wchar_t OriginalAudioCtrlProc[INFO_LEN]			= { 0 };
wchar_t OriginalAudioCtrlSetProc[INFO_LEN]		= { 0 };
wchar_t OriginalAudioCtrlUnSetProc[INFO_LEN]	= { 0 };


//custom encrypt
wchar_t customEncryptCtrlEncrypt[INFO_LEN] = {0};
wchar_t customEncryptCtrlSetEncrypt[INFO_LEN] = {0};
wchar_t customEncryptCtrlCancelEncrypt[INFO_LEN] = {0};

//media encrypt
wchar_t mediaEncryptCtrlMode[INFO_LEN];
wchar_t mediaEncryptCtrlSecret[INFO_LEN];
wchar_t mediaEncryptCtrlSetEncrypt[INFO_LEN];


//media player
wchar_t MeidaPlayerCtrlVideoSource[INFO_LEN]	= { 0 };
wchar_t MeidaPlayerCtrlOpen[INFO_LEN]			= { 0 };
wchar_t MeidaPlayerCtrlClose[INFO_LEN]			= { 0 };
wchar_t MeidaPlayerCtrlPause[INFO_LEN]			= { 0 };
wchar_t MeidaPlayerCtrlPlay[INFO_LEN]			= { 0 };
wchar_t MeidaPlayerCtrlAttachPlayer[INFO_LEN]	= { 0 };
wchar_t MeidaPlayerCtrlDettachPlayer[INFO_LEN]	= { 0 };
wchar_t MeidaPlayerCtrlPublishVideo[INFO_LEN]	= { 0 };
wchar_t MeidaPlayerCtrlUnPublishVideo[INFO_LEN] = { 0 };
wchar_t MeidaPlayerCtrlPublishAudio[INFO_LEN]	= { 0 };
wchar_t MeidaPlayerCtrlUnPublishAudio[INFO_LEN] = { 0 };

//multi channel
wchar_t MultiChannelCtrlChannelList[INFO_LEN] = { 0 };


//multi video source
wchar_t MultiVideoSourceCtrlVideoSource[INFO_LEN]	= { 0 };
wchar_t MultiVideoSourceCtrlPublish[INFO_LEN]		= { 0 };
wchar_t MultiVideoSourceCtrlUnPublish[INFO_LEN]		= { 0 };


//audio effect
wchar_t AudioEffectCtrlEffectPath[INFO_LEN] = { 0 };
wchar_t AudioEffectCtrlEffect[INFO_LEN] = { 0 };
wchar_t AudioEffectCtrlLoops[INFO_LEN] = { 0 };
wchar_t AudioEffectCtrlGain[INFO_LEN] = { 0 };
wchar_t AudioEffectCtrlPitch[INFO_LEN] = { 0 };
wchar_t AudioEffectCtrlPan[INFO_LEN] = { 0 };
wchar_t AudioEffectCtrlPublish[INFO_LEN] = { 0 };
wchar_t AudioEffectCtrlAddEffect[INFO_LEN] = { 0 };
wchar_t AudioEffectCtrlRemoveEffect[INFO_LEN] = { 0 };
wchar_t AudioEffectCtrlPreLoad[INFO_LEN] = { 0 };
wchar_t AudioEffectCtrlUnPreload[INFO_LEN] = { 0 };
wchar_t AudioEffectCtrlPauseEffect[INFO_LEN] = { 0 };
wchar_t AudioEffectCtrlPlayEffect[INFO_LEN] = { 0 };
wchar_t AudioEffectCtrlPauseAllEffect[INFO_LEN] = { 0 };
wchar_t AudioEffectCtrlResumeEffect[INFO_LEN] = { 0 };
wchar_t AudioEffectCtrlResumeAllEffect[INFO_LEN] = { 0 };
wchar_t AudioEffectCtrlStopAllEffect[INFO_LEN] = { 0 };
wchar_t AudioEffectCtrlStopEffect[INFO_LEN] = { 0 };
wchar_t AudioEffectCtrlVolume[INFO_LEN] = { 0 };

//per call test
wchar_t PerCallTestCtrlAudioInput[INFO_LEN] = { 0 };
wchar_t PerCallTestCtrlAudioOutput[INFO_LEN] = { 0 };
wchar_t PerCallTestCtrlAudioVol[INFO_LEN] = { 0 };
wchar_t PerCallTestCtrlCamera[INFO_LEN] = { 0 };
wchar_t PerCallTestCtrlStartTest[INFO_LEN] = { 0 };
wchar_t PerCallTestCtrlStopTest[INFO_LEN] = { 0 };


//audio volume
wchar_t AudioVolumeCtrlCapVol[INFO_LEN] = { 0 };
wchar_t AudioVolumeCtrlCapSigVol[INFO_LEN] = { 0 };
wchar_t AudioVolumeCtrlPlaybackVol[INFO_LEN] = { 0 };


wchar_t AudioVolumeCtrlPlaybackSigVol[INFO_LEN] = { 0 };


//report in call
wchar_t ReportInCallCtrlGopTotal[INFO_LEN];
wchar_t ReportInCallCtrlGopRemoteVideo[INFO_LEN];
wchar_t ReportInCallCtrlGopRemoteAudio[INFO_LEN];
wchar_t ReportInCallCtrlTotalUpDownLink[INFO_LEN];
wchar_t ReportInCallCtrlTotalBytes[INFO_LEN];
wchar_t ReportInCallCtrlTotalBitrate[INFO_LEN];
wchar_t ReportInCallCtrlVideoNetWorkDelay[INFO_LEN];
wchar_t ReportInCallCtrlVideoBytes[INFO_LEN];
wchar_t ReportInCallCtrlVideoBitrate[INFO_LEN];
wchar_t ReportInCallCtrlAudioNetWorkDelay[INFO_LEN];
wchar_t ReportInCallCtrlAudioBytes[INFO_LEN];
wchar_t ReportInCallCtrlAudioBitrate[INFO_LEN];
wchar_t ReportInCallCtrlLocalResoultion[INFO_LEN];
wchar_t ReportInCallCtrlLocalFPS[INFO_LEN];

//area code
wchar_t RegionConnCtrlAreaCode[INFO_LEN];


//Multi camera
wchar_t MultiCameara[INFO_LEN] = { 0 };
wchar_t MultiCamearaPublishCamera2[INFO_LEN] = { 0 };
wchar_t MultiCamearaStopPublishCamera2[INFO_LEN] = { 0 };
wchar_t MultiCamearaCamera1[INFO_LEN] = { 0 };
wchar_t MultiCamearaCamera2[INFO_LEN] = { 0 };
wchar_t MultiCameraStartCapture[INFO_LEN] = { 0 };
wchar_t MultiCameraStopCapture[INFO_LEN] = { 0 };

wchar_t AdvancedLocalVideoTranscoding[INFO_LEN] = { 0 };

//media player
wchar_t mediaPlayerCtrlVideoSource[INFO_LEN] = { 0 };
wchar_t mediaPlayerCtrlOpen[INFO_LEN] = { 0 };
wchar_t mediaPlayerCtrlClose[INFO_LEN] = { 0 };
wchar_t mediaPlayerCtrlPause[INFO_LEN] = { 0 };
wchar_t mediaPlayerCtrlPlay[INFO_LEN] = { 0 };
wchar_t mediaPlayerCtrlAttachPlayer[INFO_LEN] = { 0 };
wchar_t mediaPlayerCtrlDettachPlayer[INFO_LEN] = { 0 };
wchar_t mediaPlayerCtrlPublishVideo[INFO_LEN] = { 0 };
wchar_t mediaPlayerCtrlUnPublishVideo[INFO_LEN] = { 0 };
wchar_t mediaPlayerCtrlPublishAudio[INFO_LEN] = { 0 };
wchar_t mediaPlayerCtrlUnPublishAudio[INFO_LEN] = { 0 };


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

CString GetExePath()
{
	TCHAR szPath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szPath, MAX_PATH);

	CString strPath = szPath;
	int pos = strPath.ReverseFind(_T('\\'));
	return strPath.Mid(0, pos );
}

BOOL PASCAL SaveResourceToFile(LPCTSTR lpResourceType, WORD wResourceID, LPCTSTR lpFilePath)
{
	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule == NULL)
		return FALSE;

	HRSRC hResrc = ::FindResource(hModule, MAKEINTRESOURCE(wResourceID), lpResourceType);
	if (hResrc == NULL)
		return FALSE;

	HGLOBAL	hGlobal = ::LoadResource(hModule, hResrc);
	if (hGlobal == NULL)
		return FALSE;

	LPBYTE lpPointer = (LPBYTE)::LockResource(hGlobal);
	DWORD dwResSize = ::SizeofResource(hModule, hResrc);

	HANDLE hFile = ::CreateFile(lpFilePath, GENERIC_ALL, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD dwBytesWritten = 0;
	::WriteFile(hFile, lpPointer, dwResSize, &dwBytesWritten, NULL);
	::CloseHandle(hFile);

	return (dwBytesWritten == dwResSize) ? TRUE : FALSE;
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
	_tcscpy_s(advancedMultiChannel, INFO_LEN, Str(_T("Advanced.MultiChannel")));
	_tcscpy_s(advancedMultiVideoSource, INFO_LEN, Str(_T("Advanced.MultiVideoSource")));
	
	_tcscpy_s(advancedAudioEffect, INFO_LEN, Str(_T("Advanced.AudioEffect")));
	_tcscpy_s(advancedPerCallTest, INFO_LEN, Str(_T("Advanced.PerCallTest")));
	_tcscpy_s(advancedAudioVolume, INFO_LEN, Str(_T("Advanced.AudioVolume")));
	_tcscpy_s(advancedReportInCall, INFO_LEN, Str(_T("Advanced.ReportInCall")));
	_tcscpy_s(advancedRegionConn, INFO_LEN, Str(_T("Advanced.RegionConn")));
	_tcscpy_s(advancedCrossChannel, INFO_LEN, Str(_T("Advanced.CrossChannel")));
	_tcscpy_s(advancedMediaEncrypt, INFO_LEN, Str(_T("Advanced.MediaEncrypt")));

    //agora
    _tcscpy_s(agoraRoleBroadcaster, INFO_LEN, Str(_T("Agora.ClientRole.Broadcaster")));
    _tcscpy_s(agoraRoleAudience, INFO_LEN, Str(_T("Agora.ClientRole.Audience")));


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

	//audio effect
	_tcscpy_s(AudioEffectCtrlEffectPath, INFO_LEN, Str(_T("AudioEffect.Ctrl.EffectPath")));
	_tcscpy_s(AudioEffectCtrlEffect, INFO_LEN, Str(_T("AudioEffect.Ctrl.Effect")));
	_tcscpy_s(AudioEffectCtrlLoops, INFO_LEN, Str(_T("AudioEffect.Ctrl.Loops")));
	_tcscpy_s(AudioEffectCtrlGain, INFO_LEN, Str(_T("AudioEffect.Ctrl.Gain")));
	_tcscpy_s(AudioEffectCtrlPitch, INFO_LEN, Str(_T("AudioEffect.Ctrl.Pitch")));
	_tcscpy_s(AudioEffectCtrlPan, INFO_LEN, Str(_T("AudioEffect.Ctrl.Pan")));
	_tcscpy_s(AudioEffectCtrlPublish, INFO_LEN, Str(_T("AudioEffect.Ctrl.Publish")));
	_tcscpy_s(AudioEffectCtrlAddEffect, INFO_LEN, Str(_T("AudioEffect.Ctrl.AddEffect")));
	_tcscpy_s(AudioEffectCtrlRemoveEffect, INFO_LEN, Str(_T("AudioEffect.Ctrl.RemoveEffect")));
	_tcscpy_s(AudioEffectCtrlPreLoad, INFO_LEN, Str(_T("AudioEffect.Ctrl.PreLoad")));
	_tcscpy_s(AudioEffectCtrlUnPreload, INFO_LEN, Str(_T("AudioEffect.Ctrl.UnPreload")));
	_tcscpy_s(AudioEffectCtrlPauseEffect, INFO_LEN, Str(_T("AudioEffect.Ctrl.PauseEffect")));
	_tcscpy_s(AudioEffectCtrlPlayEffect, INFO_LEN, Str(_T("AudioEffect.Ctrl.PlayEffect")));
	_tcscpy_s(AudioEffectCtrlPauseAllEffect, INFO_LEN, Str(_T("AudioEffect.Ctrl.PauseAllEffect")));
	_tcscpy_s(AudioEffectCtrlResumeEffect, INFO_LEN, Str(_T("AudioEffect.Ctrl.ResumeEffect")));
	_tcscpy_s(AudioEffectCtrlResumeAllEffect, INFO_LEN, Str(_T("AudioEffect.Ctrl.ResumeAllEffect")));
	_tcscpy_s(AudioEffectCtrlStopAllEffect, INFO_LEN, Str(_T("AudioEffect.Ctrl.StopAllEffect")));
	_tcscpy_s(AudioEffectCtrlStopEffect, INFO_LEN, Str(_T("AudioEffect.Ctrl.StopEffect")));
	//audio effect
	_tcscpy_s(AudioEffectCtrlEffectPath, INFO_LEN, Str(_T("AudioEffect.Ctrl.EffectPath")));
	_tcscpy_s(AudioEffectCtrlEffect, INFO_LEN, Str(_T("AudioEffect.Ctrl.Effect")));
	_tcscpy_s(AudioEffectCtrlLoops, INFO_LEN, Str(_T("AudioEffect.Ctrl.Loops")));
	_tcscpy_s(AudioEffectCtrlGain, INFO_LEN, Str(_T("AudioEffect.Ctrl.Gain")));
	_tcscpy_s(AudioEffectCtrlPitch, INFO_LEN, Str(_T("AudioEffect.Ctrl.Pitch")));
	_tcscpy_s(AudioEffectCtrlPan, INFO_LEN, Str(_T("AudioEffect.Ctrl.Pan")));
	_tcscpy_s(AudioEffectCtrlPublish, INFO_LEN, Str(_T("AudioEffect.Ctrl.Publish")));
	_tcscpy_s(AudioEffectCtrlAddEffect, INFO_LEN, Str(_T("AudioEffect.Ctrl.AddEffect")));
	_tcscpy_s(AudioEffectCtrlRemoveEffect, INFO_LEN, Str(_T("AudioEffect.Ctrl.RemoveEffect")));
	_tcscpy_s(AudioEffectCtrlPreLoad, INFO_LEN, Str(_T("AudioEffect.Ctrl.PreLoad")));
	_tcscpy_s(AudioEffectCtrlUnPreload, INFO_LEN, Str(_T("AudioEffect.Ctrl.UnPreload")));
	_tcscpy_s(AudioEffectCtrlPauseEffect, INFO_LEN, Str(_T("AudioEffect.Ctrl.PauseEffect")));
	_tcscpy_s(AudioEffectCtrlPlayEffect, INFO_LEN, Str(_T("AudioEffect.Ctrl.PlayEffect")));
	_tcscpy_s(AudioEffectCtrlPauseAllEffect, INFO_LEN, Str(_T("AudioEffect.Ctrl.PauseAllEffect")));
	_tcscpy_s(AudioEffectCtrlResumeEffect, INFO_LEN, Str(_T("AudioEffect.Ctrl.ResumeEffect")));
	_tcscpy_s(AudioEffectCtrlResumeAllEffect, INFO_LEN, Str(_T("AudioEffect.Ctrl.ResumeAllEffect")));
	_tcscpy_s(AudioEffectCtrlStopAllEffect, INFO_LEN, Str(_T("AudioEffect.Ctrl.StopAllEffect")));
	_tcscpy_s(AudioEffectCtrlStopEffect, INFO_LEN, Str(_T("AudioEffect.Ctrl.StopEffect")));
	_tcscpy_s(AudioEffectCtrlVolume, INFO_LEN, Str(_T("AudioEffect.Ctrl.Volume")));

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
	_tcscpy_s(OriginalAudioCtrlProc, INFO_LEN, Str(_T("OriginalAudio.Ctrl.Proc")));
	_tcscpy_s(OriginalAudioCtrlSetProc, INFO_LEN, Str(_T("OriginalAudio.Ctrl.SetProc")));
	_tcscpy_s(OriginalAudioCtrlUnSetProc, INFO_LEN, Str(_T("OriginalAudio.Ctrl.UnSetProc")));

	//custom encrypt
	_tcscpy_s(customEncryptCtrlEncrypt, INFO_LEN, Str(_T("CustomEncrypt.Ctrl.Encrypt")));
	_tcscpy_s(customEncryptCtrlSetEncrypt, INFO_LEN, Str(_T("CustomEncrypt.Ctrl.SetEncrypt")));
	_tcscpy_s(customEncryptCtrlCancelEncrypt, INFO_LEN, Str(_T("CustomEncrypt.Ctrl.CancelEncrypt")));

	_tcscpy_s(mediaEncryptCtrlMode, INFO_LEN, Str(_T("MediaEncrypt.Ctrl.Mode")));
	_tcscpy_s(mediaEncryptCtrlSecret, INFO_LEN, Str(_T("MediaEncrypt.Ctrl.Secret")));
	_tcscpy_s(mediaEncryptCtrlSetEncrypt, INFO_LEN, Str(_T("MediaEncrypt.Ctrl.SetEncrypt")));
	
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

	//multichannel
	_tcscpy_s(MultiChannelCtrlChannelList, INFO_LEN, Str(_T("MultiChannel.Ctrl.ChannelList")));


	//multi video source
	_tcscpy_s(MultiVideoSourceCtrlVideoSource, INFO_LEN, Str(_T("MultiVideoSource.Ctrl.VideoSource")));
	_tcscpy_s(MultiVideoSourceCtrlPublish, INFO_LEN, Str(_T("MultiVideoSource.Ctrl.Publish")));
	_tcscpy_s(MultiVideoSourceCtrlUnPublish, INFO_LEN, Str(_T("MultiVideoSource.Ctrl.UnPublish")));



	_tcscpy_s(PerCallTestCtrlAudioInput, INFO_LEN, Str(_T("PerCallTest.Ctrl.AudioInput")));
	_tcscpy_s(PerCallTestCtrlAudioOutput, INFO_LEN, Str(_T("PerCallTest.Ctrl.AudioOutput")));
	_tcscpy_s(PerCallTestCtrlAudioVol, INFO_LEN, Str(_T("PerCallTest.Ctrl.AudioVol")));
	_tcscpy_s(PerCallTestCtrlCamera, INFO_LEN, Str(_T("PerCallTest.Ctrl.Camera")));
	_tcscpy_s(PerCallTestCtrlStartTest, INFO_LEN, Str(_T("PerCallTest.Ctrl.StartTest")));
	_tcscpy_s(PerCallTestCtrlStopTest, INFO_LEN, Str(_T("PerCallTest.Ctrl.StopTest")));



	_tcscpy_s(AudioVolumeCtrlCapVol, INFO_LEN, Str(_T("AudioVolume.Ctrl.AudioCapVol")));
	_tcscpy_s(AudioVolumeCtrlCapSigVol, INFO_LEN, Str(_T("AudioVolume.Ctrl.AudioCapSigVol")));
	_tcscpy_s(AudioVolumeCtrlPlaybackVol, INFO_LEN, Str(_T("AudioVolume.Ctrl.AudioPlaybackVol")));
	_tcscpy_s(AudioVolumeCtrlPlaybackSigVol, INFO_LEN, Str(_T("AudioVolume.Ctrl.AudioPlaybackSigVol")));




	_tcscpy_s(ReportInCallCtrlLocalFPS, INFO_LEN, Str(_T("ReportInCall.Ctrl.LocalFPS")));
	_tcscpy_s(ReportInCallCtrlLocalResoultion, INFO_LEN, Str(_T("ReportInCall.Ctrl.LocaLResoultion")));
	_tcscpy_s(ReportInCallCtrlAudioBitrate, INFO_LEN, Str(_T("ReportInCall.Ctrl.AudioBitrate")));
	_tcscpy_s(ReportInCallCtrlAudioBytes, INFO_LEN, Str(_T("ReportInCall.Ctrl.AudioBytes")));
	_tcscpy_s(ReportInCallCtrlAudioNetWorkDelay, INFO_LEN, Str(_T("ReportInCall.Ctrl.AudioNetWorkDelay")));
	_tcscpy_s(ReportInCallCtrlGopRemoteAudio, INFO_LEN, Str(_T("ReportInCall.Ctrl.GopRemoteAudio")));
	_tcscpy_s(ReportInCallCtrlGopRemoteVideo, INFO_LEN, Str(_T("ReportInCall.Ctrl.GopRemoteVideo")));
	_tcscpy_s(ReportInCallCtrlGopTotal, INFO_LEN, Str(_T("ReportInCall.Ctrl.GopTotal")));
	_tcscpy_s(ReportInCallCtrlTotalBitrate, INFO_LEN, Str(_T("ReportInCall.Ctrl.TotalBitrate")));
	_tcscpy_s(ReportInCallCtrlTotalBytes, INFO_LEN, Str(_T("ReportInCall.Ctrl.TotalBytes")));
	_tcscpy_s(ReportInCallCtrlTotalUpDownLink, INFO_LEN, Str(_T("ReportInCall.Ctrl.TotalUpDownLink")));
	_tcscpy_s(ReportInCallCtrlVideoNetWorkDelay, INFO_LEN, Str(_T("ReportInCall.Ctrl.VideoNetWorkDelay")));
	_tcscpy_s(ReportInCallCtrlVideoBitrate, INFO_LEN, Str(_T("ReportInCall.Ctrl.VideoBitrate")));
	_tcscpy_s(ReportInCallCtrlVideoBytes, INFO_LEN, Str(_T("ReportInCall.Ctrl.VideoBytes")));


	_tcscpy_s(RegionConnCtrlAreaCode, INFO_LEN, Str(_T("RegionConn.Ctrl.AreaCode")));
	_tcscpy_s(MultiCameara, INFO_LEN, Str(_T("Advanced.MultiCamera")));

	_tcscpy_s(MultiCamearaPublishCamera2, INFO_LEN, Str(_T("MultiCamera.Publish")));
	_tcscpy_s(MultiCamearaStopPublishCamera2, INFO_LEN, Str(_T("MultiCamera.StopPublish")));

	_tcscpy_s(MultiCamearaCamera1, INFO_LEN, Str(_T("MultiCamera.Camera1")));
	_tcscpy_s(MultiCamearaCamera2, INFO_LEN, Str(_T("MultiCamera.Camera2")));
	_tcscpy_s(MultiCameraStartCapture, INFO_LEN, Str(_T("MultiCamera.StartCapture")));
	_tcscpy_s(MultiCameraStopCapture, INFO_LEN, Str(_T("MultiCamera.StopCapture")));

	_tcscpy_s(AdvancedLocalVideoTranscoding, INFO_LEN, Str(_T("Advanced.LocalVideoTranscoding")));
	
	//media player
	_tcscpy_s(mediaPlayerCtrlVideoSource, INFO_LEN, Str(_T("mediaPlayer.Ctrl.VideoSource")));
	_tcscpy_s(mediaPlayerCtrlOpen, INFO_LEN, Str(_T("mediaPlayer.Ctrl.Open")));
	_tcscpy_s(mediaPlayerCtrlClose, INFO_LEN, Str(_T("mediaPlayer.Ctrl.Close")));
	_tcscpy_s(mediaPlayerCtrlPause, INFO_LEN, Str(_T("mediaPlayer.Ctrl.Pause")));
	_tcscpy_s(mediaPlayerCtrlPlay, INFO_LEN, Str(_T("mediaPlayer.Ctrl.Play")));
	_tcscpy_s(mediaPlayerCtrlAttachPlayer, INFO_LEN, Str(_T("mediaPlayer.Ctrl.AttachPlayer")));
	_tcscpy_s(mediaPlayerCtrlDettachPlayer, INFO_LEN, Str(_T("mediaPlayer.Ctrl.DettachPlayer")));
	_tcscpy_s(mediaPlayerCtrlPublishVideo, INFO_LEN, Str(_T("mediaPlayer.Ctrl.PublishVideo")));
	_tcscpy_s(mediaPlayerCtrlUnPublishVideo, INFO_LEN, Str(_T("mediaPlayer.Ctrl.UnPublishVideo")));
	_tcscpy_s(mediaPlayerCtrlPublishAudio, INFO_LEN, Str(_T("mediaPlayer.Ctrl.PublishAudio")));
	_tcscpy_s(mediaPlayerCtrlUnPublishAudio, INFO_LEN, Str(_T("mediaPlayer.Ctrl.UnPublishAudio")));

	//beauty audio
	_tcscpy_s(beautyAudioCtrlChange, INFO_LEN, Str(_T("BeautyAudio.Ctrl.Change")));
	_tcscpy_s(beautyAudioCtrlSetAudioChange, INFO_LEN, Str(_T("BeautyAudio.Ctrl.SetAudioChange")));
	_tcscpy_s(beautyAudioCtrlUnSetAudioChange, INFO_LEN, Str(_T("BeautyAudio.Ctrl.UnSetAudioChange")));
	_tcscpy_s(beautyAudioCtrlPreSet, INFO_LEN, Str(_T("BeautyAudio.Ctrl.ReverbPreSet")));
	_tcscpy_s(beautyAudioCtrlParam1, INFO_LEN, Str(_T("BeautyAudio.Ctrl.BeautyAudioCtrlParam1")));
	_tcscpy_s(beautyAudioCtrlParam2, INFO_LEN, Str(_T("BeautyAudio.Ctrl.BeautyAudioCtrlParam2")));
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
*/
}
