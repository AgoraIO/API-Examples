#include "Language.h"
#include "config.h"
QString commonGroupDoc;
QString commonDocumentWebsite;
QString commonFAQWebsite;
QString commonRegisterWebsite;
QString commonDemoWebsite;

QString commonBasicScene;
QString commonAdvanceScene;

QString commonCtrlChannel;
QString commonCtrlJoinChannel;
QString commonCtrlLeaveChannel;
QString commonCtrlClientRole;

QString basicLiveBroadcasting;

QString basicScreenCap;
//screen share
QString screenShareCtrlScreenCap;
QString screenShareCtrlStartCap;
QString screenShareCtrlEndCap;

QString  screenShareCtrlScreen;
QString  screenShareCtrlShareSCreen;
QString  screenShareCtrlStopShare;
QString  screenShareCtrlGeneralSettings;
QString  screenShareCtrlFPS;
QString  screenShareCtrlBitrate;
QString  screenShareCtrlShareCursor;
QString  screenShareCtrlUpdateCaptureParam;
QString emptyChannelName = "input channel name";

void InitKeyInfomation()
{
    commonGroupDoc = Str("Common.Group.Doc");
    commonDocumentWebsite = Str("Common.Document.Website");
    commonFAQWebsite = Str("Common.FAQ.Website");
    commonRegisterWebsite = Str("Common.Register.Website");
    commonDemoWebsite= Str("Common.Demo.Website");
    commonBasicScene = Str("Common.Basic.Scene");
    commonAdvanceScene = Str("Common.Advanced.Scene");
    commonCtrlChannel = Str("Common.Ctrl.ChannelName");
    commonCtrlJoinChannel = Str("Common.Ctrl.JoinChannel");
    commonCtrlLeaveChannel = Str("Common.Ctrl.LeaveChannel");
	commonCtrlClientRole = Str("Common.Ctrl.ClientRole");

	emptyChannelName = Str("Common.Ctrl.EmptyChannelName");

	basicScreenCap = Str("Advanced.ScreenCap");

	//screen share
	screenShareCtrlScreenCap = Str("ScreenShare.Ctrl.ScreenCap");
	screenShareCtrlStartCap = Str("ScreenShare.Ctrl.StartCap");
	screenShareCtrlEndCap = Str("ScreenShare.Ctrl.EndCap");
	screenShareCtrlScreen = Str("ScreenShare.Ctrl.Screen");

	screenShareCtrlShareSCreen = Str("ScreenShare.Ctrl.ShareSCreen");
	screenShareCtrlStopShare = Str("ScreenShare.Ctrl.StopShare");
	screenShareCtrlScreen = Str("ScreenShare.Ctrl.GeneralSettings");
	screenShareCtrlFPS = Str("ScreenShare.Ctrl.FPS");
	screenShareCtrlBitrate = Str("ScreenShare.Ctrl.Bitrate");
	screenShareCtrlShareCursor = Str("ScreenShare.Ctrl.ShareCursor");
	screenShareCtrlUpdateCaptureParam = Str("ScreenShare.Ctrl.UpdateCaptureParam");
}

