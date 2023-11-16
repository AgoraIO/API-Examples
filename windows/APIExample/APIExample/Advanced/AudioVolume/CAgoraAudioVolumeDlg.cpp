#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraAudioVolumeDlg.h"



IMPLEMENT_DYNAMIC(CAgoraAudioVolumeDlg, CDialogEx)

CAgoraAudioVolumeDlg::CAgoraAudioVolumeDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_VOLUME, pParent)
{

}

CAgoraAudioVolumeDlg::~CAgoraAudioVolumeDlg()
{
}



void CAgoraAudioVolumeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannel);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannel);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_STATIC_AUDIO_CAP_VOL, m_staCapVol);
	DDX_Control(pDX, IDC_SLIDER_CAP_VOLUME, m_sldCapVol);
	DDX_Control(pDX, IDC_STATIC_AUDIO_SIGNAL_VOL, m_staCapSigVol);
	DDX_Control(pDX, IDC_SLIDER_SIGNAL_VOLUME2, m_sldCapSigVol);
	DDX_Control(pDX, IDC_STATIC_PLAYBACK_VOL, m_staPlaybackVol);
	DDX_Control(pDX, IDC_SLIDER_PLAYBACK_VOLUME, m_sldPlaybackVol);
	DDX_Control(pDX, IDC_STATIC_PLAYBACK_VOL_SIGNAL, m_staPlaybackSigVol);
	DDX_Control(pDX, IDC_SLIDER_PLAYBACK_SIGNAL_VOLUME, m_sldPlaybackSigVol);
	DDX_Control(pDX, IDC_STATIC_DETAIL, m_details);
	DDX_Control(pDX, IDC_STATIC_SPEAKER_INFO, m_staSpeaker_Info);
}

//init ctrl text.
void CAgoraAudioVolumeDlg::InitCtrlText()
{
	m_staCapSigVol.SetWindowText(AudioVolumeCtrlCapSigVol);
	m_staCapVol.SetWindowText(AudioVolumeCtrlCapVol);
	m_staPlaybackVol.SetWindowText(AudioVolumeCtrlPlaybackVol);
	m_staPlaybackSigVol.SetWindowText(AudioVolumeCtrlPlaybackSigVol);
	m_staChannel.SetWindowText(commonCtrlChannel);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
}

//Initialize the Agora SDK
bool CAgoraAudioVolumeDlg::InitAgora()
{
	//create Agora RTC engine
	m_rtcEngine = createAgoraRtcEngine();
	if (!m_rtcEngine) {
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("createAgoraRtcEngine failed"));
		return false;
	}
	//set message notify receiver window
	m_eventHandler.SetMsgReceiver(m_hWnd);

	RtcEngineContext context;
	std::string strAppID = GET_APP_ID;
	context.appId = strAppID.c_str();
	context.eventHandler = &m_eventHandler;
	//set channel profile in the engine to the CHANNEL_PROFILE_LIVE_BROADCASTING.
	context.channelProfile = CHANNEL_PROFILE_LIVE_BROADCASTING;
	//initialize the Agora RTC engine context.
	int ret = m_rtcEngine->initialize(context);
	if (ret != 0) {
		m_initialize = false;
		CString strInfo;
		strInfo.Format(_T("initialize failed: %d"), ret);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
		return false;
	}
	else
		m_initialize = true;
	m_audioDeviceManager = new AAudioDeviceManager(m_rtcEngine);
	m_rtcEngine->enableAudioVolumeIndication(1000, 0, false);
	int vol;
	m_audioDeviceManager->get()->getRecordingDeviceVolume(&vol);
	m_sldCapVol.SetPos(vol);
	m_sldCapSigVol.SetPos(vol);
	m_audioDeviceManager->get()->getPlaybackDeviceVolume(&vol);
	m_sldPlaybackVol.SetPos(vol);
	m_sldPlaybackSigVol.SetPos(vol);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("initialize success"));
	//enable video in the engine.
	m_rtcEngine->enableVideo();
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("enable video"));
	
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("live broadcasting"));
	//set client role in the engine to the CLIENT_ROLE_BROADCASTER.
	m_rtcEngine->setClientRole(CLIENT_ROLE_BROADCASTER);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setClientRole broadcaster"));
	return true;
}

void CAgoraAudioVolumeDlg::UnInitAgora()
{
	if (m_rtcEngine) {
		if (m_joinChannel)
			//leave channel
			m_joinChannel = !m_rtcEngine->leaveChannel();
		delete m_audioDeviceManager;
		if (m_audioDeviceManager)
		{
			m_audioDeviceManager->release();
		}
		//stop preview in the engine.
		m_rtcEngine->stopPreview();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stopPreview"));
		//disable video in the engine.
		m_rtcEngine->disableVideo();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("disableVideo"));
		//release engine.
		if (m_initialize) {
			m_rtcEngine->release(true);
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release rtc engine"));
		}
		m_rtcEngine = NULL;
	}
}


//render local video from SDK local capture.
void CAgoraAudioVolumeDlg::RenderLocalVideo()
{
	if (m_rtcEngine) {
		//start preview in the engine.
		m_rtcEngine->startPreview();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("startPreview"));
		VideoCanvas canvas;
		canvas.renderMode =  media::base::RENDER_MODE_FIT;
		canvas.uid = 0;
		canvas.view = m_localVideoWnd.GetSafeHwnd();
		//setup local video in the engine to canvas.
		m_rtcEngine->setupLocalVideo(canvas);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setupLocalVideo"));
	}
}

//resume status.
void CAgoraAudioVolumeDlg::ResumeStatus()
{
	if (m_audioIndiaction)
	{
		delete []m_audioIndiaction->speakers;
		delete m_audioIndiaction;
		m_audioIndiaction = nullptr;
	}
	if (m_activeSpeackerUid) {
		delete m_activeSpeackerUid;
		m_activeSpeackerUid = nullptr;
	}
	InitCtrlText();
	m_staSpeaker_Info.SetWindowText(_T(""));
	m_edtChannel.SetWindowText(_T(""));
	m_lstInfo.ResetContent();
	m_joinChannel = false;
	m_initialize = false;
}

BEGIN_MESSAGE_MAP(CAgoraAudioVolumeDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraAudioVolumeDlg::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraAudioVolumeDlg::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraAudioVolumeDlg::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraAudioVolumeDlg::OnEIDUserOffline)
	ON_MESSAGE(WM_MSGID(EID_AUDIO_VOLUME_INDICATION), &CAgoraAudioVolumeDlg::OnEIDAudioVolumeIndication)
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraAudioVolumeDlg::OnBnClickedButtonJoinchannel)
	ON_LBN_SELCHANGE(IDC_LIST_INFO_BROADCASTING, &CAgoraAudioVolumeDlg::OnSelchangeListInfoBroadcasting)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_CAP_VOLUME, &CAgoraAudioVolumeDlg::OnReleasedcaptureSliderCapVolume)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_SIGNAL_VOLUME2, &CAgoraAudioVolumeDlg::OnReleasedcaptureSliderSignalVolume2)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_PLAYBACK_VOLUME, &CAgoraAudioVolumeDlg::OnReleasedcaptureSliderPlaybackVolume)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_PLAYBACK_SIGNAL_VOLUME, &CAgoraAudioVolumeDlg::OnReleasedcaptureSliderPlaybackSignalVolume)
	ON_WM_TIMER()
END_MESSAGE_MAP()




void CAgoraAudioVolumeDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (bShow)//bShwo is true ,show window 
	{
		InitCtrlText();
		RenderLocalVideo();
	}
	else {
		ResumeStatus();
	}

}


BOOL CAgoraAudioVolumeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);
	RECT rcArea;
	m_staVideoArea.GetClientRect(&rcArea);
	m_localVideoWnd.MoveWindow(&rcArea);
	m_localVideoWnd.ShowWindow(SW_SHOW);

	m_sldCapVol.SetRange(0, 255);
	m_sldCapSigVol.SetRange(0, 400);
	m_sldPlaybackVol.SetRange(0, 255);
	m_sldPlaybackSigVol.SetRange(0, 400);

	ResumeStatus();
	return TRUE;
}


BOOL CAgoraAudioVolumeDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CAgoraAudioVolumeDlg::OnBnClickedButtonJoinchannel()
{
	if (!m_rtcEngine || !m_initialize)
		return;
	CString strInfo;
	if (!m_joinChannel) {
		CString strChannelName;
		m_edtChannel.GetWindowText(strChannelName);
		if (strChannelName.IsEmpty()) {
			AfxMessageBox(_T("Fill channel name first"));
			return;
		}
		std::string szChannelId = cs2utf8(strChannelName);
		ChannelMediaOptions options;
		options.channelProfile = CHANNEL_PROFILE_LIVE_BROADCASTING;
		options.clientRoleType = CLIENT_ROLE_BROADCASTER;
		//join channel in the engine.
		if (0 == m_rtcEngine->joinChannel(APP_TOKEN, szChannelId.c_str(), 0, options)) {
			strInfo.Format(_T("join channel %s, use ChannelMediaOptions"), getCurrentTime());
			m_btnJoinChannel.EnableWindow(FALSE);
			m_staSpeaker_Info.SetWindowText(_T(""));
			SetTimer(1001, 1000, NULL);
		}
	}
	else {
		//leave channel in the engine.
		if (0 == m_rtcEngine->leaveChannel()) {
			strInfo.Format(_T("leave channel %s"), getCurrentTime());
			KillTimer(1001);
			m_staSpeaker_Info.SetWindowText(_T(""));
		}
	}
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}


void CAgoraAudioVolumeDlg::OnSelchangeListInfoBroadcasting()
{
	int sel = m_lstInfo.GetCurSel();
	if (sel < 0)return;
	CString strDetail;
	m_lstInfo.GetText(sel, strDetail);
	m_details.SetWindowText(strDetail);
}


void CAgoraAudioVolumeDlg::OnReleasedcaptureSliderCapVolume(NMHDR *pNMHDR, LRESULT *pResult)
{

	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	int vol = m_sldCapVol.GetPos();
	(*m_audioDeviceManager)->setRecordingDeviceVolume(vol);
	*pResult = 0;
}


void CAgoraAudioVolumeDlg::OnReleasedcaptureSliderSignalVolume2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	int vol = m_sldCapSigVol.GetPos();
	m_rtcEngine->adjustRecordingSignalVolume(vol);
	*pResult = 0;
}


void CAgoraAudioVolumeDlg::OnReleasedcaptureSliderPlaybackVolume(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	int vol = m_sldPlaybackVol.GetPos();
	(*m_audioDeviceManager)->setPlaybackDeviceVolume(vol);
	*pResult = 0;
}


void CAgoraAudioVolumeDlg::OnReleasedcaptureSliderPlaybackSignalVolume(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	int vol = m_sldPlaybackSigVol.GetPos();
	m_rtcEngine->adjustPlaybackSignalVolume(vol);
	*pResult = 0;
}


LRESULT CAgoraAudioVolumeDlg::OnEIDAudioVolumeIndication(WPARAM wparam, LPARAM lparam)
{
	if (m_audioIndiaction) {
		delete m_audioIndiaction;
		m_audioIndiaction = nullptr;
	}
	m_audioIndiaction = reinterpret_cast<AudioIndication*>(wparam);
	return TRUE;
}

LRESULT CAgoraAudioVolumeDlg::OnEIDActiveSpeaker(WPARAM wparam, LPARAM lparam)
{
	if (m_activeSpeackerUid)
	{
		delete m_activeSpeackerUid;
		m_activeSpeackerUid = new uid_t(wparam);
	}
	return TRUE;
}


//audio volume indication
void CAudioVolumeEventHandler::onAudioVolumeIndication(const AudioVolumeInfo * speakers, unsigned int speakerNumber, int totalVolume)
{
	auto p = new AudioIndication;
	p->speakerNumber = speakerNumber;
	p->speakers = new AudioVolumeInfo[speakerNumber];
	for (unsigned int i = 0; i < speakerNumber; i++)
		p->speakers[i] = speakers[i];
	if (m_hMsgHanlder)
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_AUDIO_VOLUME_INDICATION), (WPARAM)p, 0);
}

//active speaker
void CAudioVolumeEventHandler::onActiveSpeaker(uid_t uid)
{
	if (m_hMsgHanlder)
	{
		::PostMessage(m_hMsgHanlder,WM_MSGID(EID_AUDIO_ACTIVE_SPEAKER), uid,0);
	}

}

//EID_JOINCHANNEL_SUCCESS message window handler
LRESULT CAgoraAudioVolumeDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	m_joinChannel = true;
	m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);
	m_btnJoinChannel.EnableWindow(TRUE);
	CString strInfo;
	strInfo.Format(_T("%s:join success, uid=%u"), getCurrentTime(), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_localVideoWnd.SetUID(wParam);
	//notify parent window
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
	return 0;
}

//EID_LEAVEHANNEL_SUCCESS message window handler
LRESULT CAgoraAudioVolumeDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
	m_joinChannel = false;
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	CString strInfo;
	strInfo.Format(_T("leave channel success %s"), getCurrentTime());
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), FALSE, 0);
	return 0;
}

//EID_USER_JOINED message window handler
LRESULT CAgoraAudioVolumeDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	CString strInfo;
	strInfo.Format(_T("%u joined"), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	return 0;
}

//EID_USER_OFFLINE message handler.
LRESULT CAgoraAudioVolumeDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
	uid_t remoteUid = (uid_t)wParam;
	CString strInfo;
	strInfo.Format(_T("%u offline, reason:%d"), remoteUid, lParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}

/*
note:
	Join the channel callback.This callback method indicates that the client
	successfully joined the specified channel.Channel ids are assigned based
	on the channel name specified in the joinChannel. If IRtcEngine::joinChannel
	is called without a user ID specified. The server will automatically assign one
parameters:
	channel:channel name.
	uid: user ID.If the UID is specified in the joinChannel, that ID is returned here;
	Otherwise, use the ID automatically assigned by the Agora server.
	elapsed: The Time from the joinChannel until this event occurred (ms).
*/
void CAudioVolumeEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)uid, (LPARAM)elapsed);
	}
}
/*
note:
	In the live broadcast scene, each anchor can receive the callback
	of the new anchor joining the channel, and can obtain the uID of the anchor.
	Viewers also receive a callback when a new anchor joins the channel and
	get the anchor's UID.When the Web side joins the live channel, the SDK will
	default to the Web side as long as there is a push stream on the
	Web side and trigger the callback.
parameters:
	uid: remote user/anchor ID for newly added channel.
	elapsed: The joinChannel is called from the local user to the delay triggered
	by the callback(ms).
*/
void CAudioVolumeEventHandler::onUserJoined(uid_t uid, int elapsed)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_JOINED), (WPARAM)uid, (LPARAM)elapsed);
	}
}

/*
note:
	Remote user (communication scenario)/anchor (live scenario) is called back from
	the current channel.A remote user/anchor has left the channel (or dropped the line).
	There are two reasons for users to leave the channel, namely normal departure and
	time-out:When leaving normally, the remote user/anchor will send a message like
	"goodbye". After receiving this message, determine if the user left the channel.
	The basis of timeout dropout is that within a certain period of time
	(live broadcast scene has a slight delay), if the user does not receive any
	packet from the other side, it will be judged as the other side dropout.
	False positives are possible when the network is poor. We recommend using the
	Agora Real-time messaging SDK for reliable drop detection.
parameters:
	uid: The user ID of an offline user or anchor.
	reason:Offline reason: USER_OFFLINE_REASON_TYPE.
*/
void CAudioVolumeEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_OFFLINE), (WPARAM)uid, (LPARAM)reason);
	}
}
/*
note:
	When the App calls the leaveChannel method, the SDK indicates that the App
	has successfully left the channel. In this callback method, the App can get
	the total call time, the data traffic sent and received by THE SDK and other
	information. The App obtains the call duration and data statistics received
	or sent by the SDK through this callback.
parameters:
	stats: Call statistics.
*/

void CAudioVolumeEventHandler::onLeaveChannel(const RtcStats& stats)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
	}
}



// show speakers
void CAgoraAudioVolumeDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1001)
	{
		CString strInfo;
		if (m_audioIndiaction)
		{
			strInfo = _T("speaks[");
			for (unsigned i = 0; i < m_audioIndiaction->speakerNumber; i++)
			{
				CString tmp;
				tmp.Format(_T("%d,"), m_audioIndiaction->speakers[i].uid);
				if (i == m_audioIndiaction->speakerNumber - 1)
				{
					tmp.Format(_T("%d"), m_audioIndiaction->speakers[i].uid);
				}
				strInfo += tmp;
			}
			strInfo += _T("]");
		}
		if (m_activeSpeackerUid)
		{
			CString tmp;
			tmp.Format(_T("active speacker uid:%d"), *m_activeSpeackerUid);
			strInfo += tmp;
		}
		m_staSpeaker_Info.SetWindowText(strInfo);
		return;
	}
	CDialogEx::OnTimer(nIDEvent);
}
