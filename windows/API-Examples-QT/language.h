#pragma once
#define INFO_LEN 255
#include <QString>

extern QString commonGroupDoc;
extern QString commonDocumentWebsite;
extern QString commonFAQWebsite;
extern QString commonRegisterWebsite;
extern QString commonDemoWebsite;

extern QString commonBasicScene;
extern QString commonAdvanceScene;

//common ctrl
extern  QString commonCtrlChannel;
extern  QString commonCtrlJoinChannel;
extern  QString commonCtrlLeaveChannel;
extern  QString commonCtrlClientRole;

extern  QString basicLiveBroadcasting;
extern  QString basicScreenCap;
//screen share
extern  QString screenShareCtrlScreenCap;
extern  QString screenShareCtrlStartCap;
extern  QString screenShareCtrlEndCap;

extern  QString  screenShareCtrlScreen;
extern  QString  screenShareCtrlShareSCreen;
extern  QString  screenShareCtrlStopShare;
extern  QString  screenShareCtrlGeneralSettings;
extern  QString  screenShareCtrlFPS;
extern  QString  screenShareCtrlBitrate;
extern  QString  screenShareCtrlShareCursor;
extern  QString  screenShareCtrlUpdateCaptureParam;



extern  QString screenCtrlRectInfo;
extern  QString virtualScreenCtrlRectInfo;
extern  QString screenCtrlCustomRectInfo;

extern  QString emptyChannelName;

/*

//scene list

extern  QString advancedRtmpInject;
extern  QString advancedRtmpStreaming;
extern  QString advancedVideoMetadata;
extern  QString advancedCustomEncrypt;

extern  QString basicScreenCap;
extern  QString advancedAudioProfile;
extern  QString advancedAudioMixing;
extern  QString advancedBeauty;
extern  QString advancedBeautyAudio;
extern  QString advancedCustomVideoCapture;
extern  QString advancedOriginalVideo;
extern  QString advancedCustomAudioCapture;
extern  QString advancedOriginalAudio;
extern  QString advancedMediaPlayer;
extern  QString advancedMultiChannel;
extern  QString advancedMultiVideoSource;
extern  QString advancedAudioEffect;
extern  QString advancedPerCallTest;
extern  QString advancedAudioVolume;
extern  QString advancedReportInCall;
extern  QString advancedRegionConn;
extern  QString advancedCrossChannel;
extern  QString advancedMediaEncrypt;



//live broadcasting
extern  QString liveCtrlPersons;
//rtmp streaming
extern  QString rtmpStreamingCtrlPublishUrl;
extern  QString rtmpStreamingCtrlAdd;
extern  QString rtmpStreamingCtrlRemove;
extern  QString rtmpStreamingCtrlTransCoding;
extern  QString rtmpStreamingCtrlRemoveAll;
//rtmp Inject
extern  QString rtmpInjectCtrlUrl;
extern  QString rtmpInjectCtrlInject;
extern  QString rtmpInjectCtrlRemove;
//rtmp stream state changed
extern  QString agoraRtmpStateIdle;
extern  QString agoraRtmpStateConnecting;
extern  QString agoraRtmpStateRunning;
extern  QString agoraRtmpStateRunningSuccess;
extern  QString agoraRtmpStateRecovering;
extern  QString agoraRtmpStateInvalidArg;
extern  QString agoraRtmpStateEncrypted;
extern  QString agoraRtmpStateConnTimeout;
extern  QString agoraRtmpStateInrealErr;
extern  QString agoraRtmpStateServerErr;
extern  QString agoraRtmpStateTooOften;
extern  QString agoraRtmpStateReachLimit;
extern  QString agoraRtmpStateNotAuth;
extern  QString agoraRtmpStateNotFound;
extern  QString agoraRtmpStateNotSupported;

// inject status
extern  QString agoraInjectStartSucc;
extern  QString agoraInjectExist;
extern  QString agoraInjectStartUnAuth;
extern  QString agoraInjectStartTimeout;
extern  QString agoraInjectStartFailed;
extern  QString agoraInjectStopSuccess;
extern  QString agoraInjectNotFound;
extern  QString agoraInjectStopUnAuth;

extern  QString agoraInjectStopTimeout;
extern  QString agoraInjectStopFailed;
extern  QString agoraInjectBroken;

//video SEI
extern  QString videoSEIInformation;
extern  QString metadataCtrlSendSEI;
extern  QString metadataCtrlBtnSend;
extern  QString metadataCtrlBtnClear;

//custom encrypt
extern  QString customEncryptCtrlEncrypt;
extern  QString customEncryptCtrlSetEncrypt;
extern  QString customEncryptCtrlCancelEncrypt;




//media encrypt
extern  QString mediaEncryptCtrlMode;
extern  QString mediaEncryptCtrlSecret;
extern  QString mediaEncryptCtrlSetEncrypt;



//beauty
extern  QString beautyCtrlLighteningContrastLevel;
extern  QString beautyCtrlLightening;
extern  QString beautyCtrlSmoothness;
extern  QString beautyCtrlRedness;
extern  QString beautyCtrlEnable;

//beauty audio
extern  QString beautyAudioCtrlSetAudioChange;
extern  QString beautyAudioCtrlUnSetAudioChange;
extern  QString beautyAudioCtrlChange;

//audio effect
extern  QString AudioEffectCtrlEffectPath[INFO_LEN] ;
extern  QString AudioEffectCtrlEffect[INFO_LEN] ;
extern  QString AudioEffectCtrlLoops;
extern  QString AudioEffectCtrlGain;
extern  QString AudioEffectCtrlPitch[INFO_LEN] ;
extern  QString AudioEffectCtrlPan[INFO_LEN] ;
extern  QString AudioEffectCtrlPublish;
extern  QString AudioEffectCtrlAddEffect[INFO_LEN] ;
extern  QString AudioEffectCtrlRemoveEffect;
extern  QString AudioEffectCtrlPreLoad;
extern  QString AudioEffectCtrlUnPreload[INFO_LEN] ;
extern  QString AudioEffectCtrlPauseEffect;
extern  QString AudioEffectCtrlPlayEffect;
extern  QString AudioEffectCtrlPauseAllEffect[INFO_LEN] ;
extern  QString AudioEffectCtrlResumeEffect;
extern  QString AudioEffectCtrlResumeAllEffect;
extern  QString AudioEffectCtrlStopAllEffect[INFO_LEN] ;
extern  QString AudioEffectCtrlStopEffect[INFO_LEN] ;
extern  QString AudioEffectCtrlVolume[INFO_LEN] ;


//set audio profile
extern  QString audioProfileCtrlProfile;
extern  QString audioProfileCtrlScenario;
extern  QString audioProfileCtrlSetAudioProfile;
extern  QString audioProfileCtrlUnSetAudioProfile;

//audio mixing
extern  QString audioMixingCtrlMixingPath; 
extern  QString audioMixingCtrlRepeatTimes;
extern  QString audioMixingCtrlSetAudioMixing;
extern  QString audioMixingCtrlUnSetAudioMixing;
extern  QString audioMixingCtrlOnlyLocal; 
extern  QString audioMixingCtrlReplaceMicroPhone;


//custom video capture
extern  QString customVideoCaptureCtrlCaptureVideoDevice;
extern  QString customVideoCaptureCtrlSetExternlCapture;
extern  QString customVideoCaptureCtrlCancelExternlCapture;


//original video process
extern  QString OriginalVideoCtrlProc;
extern  QString OriginalVideoCtrlSetProc;
extern  QString OriginalVideoCtrlUnSetProc;


//custom audio capture
extern  QString customAudioCaptureCtrlCaptureAudioDeivce;
extern  QString customAudioCaptureCtrlSetExternlCapture;
extern  QString customAudioCaptureCtrlCancelExternlCapture;

//original audio process
extern  QString OriginalAudioCtrlProc;
extern  QString OriginalAudioCtrlSetProc;
extern  QString OriginalAudioCtrlUnSetProc;

//media player
extern  QString MeidaPlayerCtrlVideoSource;
extern  QString MeidaPlayerCtrlOpen;
extern  QString MeidaPlayerCtrlClose;
extern  QString MeidaPlayerCtrlPause;
extern  QString MeidaPlayerCtrlPlay;
extern  QString MeidaPlayerCtrlAttachPlayer;
extern  QString MeidaPlayerCtrlDettachPlayer;
extern  QString MeidaPlayerCtrlPublishVideo;
extern  QString MeidaPlayerCtrlUnPublishVideo;
extern  QString MeidaPlayerCtrlPublishAudio;
extern  QString MeidaPlayerCtrlUnPublishAudio;


//multi channel
extern  QString MultiChannelCtrlChannelList;

//multi video source
extern  QString MultiVideoSourceCtrlVideoSource;
extern  QString MultiVideoSourceCtrlPublish;
extern  QString MultiVideoSourceCtrlUnPublish;

//audio effect
extern  QString AudioEffectCtrlEffectPath;
extern  QString AudioEffectCtrlEffect;
extern  QString AudioEffectCtrlLoops;
extern  QString AudioEffectCtrlGain;
extern  QString AudioEffectCtrlPitch;
extern  QString AudioEffectCtrlPan;
extern  QString AudioEffectCtrlPublish;
extern  QString AudioEffectCtrlAddEffect;
extern  QString AudioEffectCtrlRemoveEffect;
extern  QString AudioEffectCtrlPreLoad;
extern  QString AudioEffectCtrlUnPreload;
extern  QString AudioEffectCtrlPauseEffect;
extern  QString AudioEffectCtrlPlayEffect;
extern  QString AudioEffectCtrlPauseAllEffect;
extern  QString AudioEffectCtrlResumeEffect;
extern  QString AudioEffectCtrlResumeAllEffect;
extern  QString AudioEffectCtrlStopAllEffect;
extern  QString AudioEffectCtrlStopEffect;
extern  QString AudioEffectCtrlVolume;

//per call test
extern  QString PerCallTestCtrlAudioInput;
extern  QString PerCallTestCtrlAudioOutput;
extern  QString PerCallTestCtrlAudioVol;
extern  QString PerCallTestCtrlCamera;
extern  QString PerCallTestCtrlStartTest;
extern  QString PerCallTestCtrlStopTest;

//audio volume
extern  QString AudioVolumeCtrlCapVol;
extern  QString AudioVolumeCtrlCapSigVol;
extern  QString AudioVolumeCtrlPlaybackVol;
extern  QString AudioVolumeCtrlPlaybackSigVol;



//report in call
extern  QString ReportInCallCtrlGopTotal;
extern  QString ReportInCallCtrlGopRemoteVideo;
extern  QString ReportInCallCtrlGopRemoteAudio;
extern  QString ReportInCallCtrlTotalUpDownLink;
extern  QString ReportInCallCtrlTotalBytes;
extern  QString ReportInCallCtrlTotalBitrate;
extern  QString ReportInCallCtrlVideoNetWorkDelay;
extern  QString ReportInCallCtrlVideoBytes;
extern  QString ReportInCallCtrlVideoBitrate;
extern  QString ReportInCallCtrlAudioNetWorkDelay;
extern  QString ReportInCallCtrlAudioBytes;
extern  QString ReportInCallCtrlAudioBitrate;
extern  QString ReportInCallCtrlLocalResoultion;
extern  QString ReportInCallCtrlLocalFPS;*/


//area code
extern void InitKeyInfomation();
