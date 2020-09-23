#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraBeautyAudio.h"



IMPLEMENT_DYNAMIC(CAgoraBeautyAudio, CDialogEx)

CAgoraBeautyAudio::CAgoraBeautyAudio(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_BEAUTY_AUDIO, pParent)
{

}

CAgoraBeautyAudio::~CAgoraBeautyAudio()
{
}


//Initialize the Ctrl Text.
void CAgoraBeautyAudio::InitCtrlText()
{
	m_staChannel.SetWindowText(commonCtrlChannel);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	m_staAudioChange.SetWindowText(beautyAudioCtrlChange);
	m_staAudioType.SetWindowText(beautyAudioCtrlPreSet);
	m_btnSetBeautyAudio.SetWindowText(beautyAudioCtrlSetAudioChange);
}



//Initialize the Agora SDK
bool CAgoraBeautyAudio::InitAgora()
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
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("initialize success"));
	//enable video in the engine.
	m_rtcEngine->enableVideo();
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("enable video"));
	//set channel profile in the engine to the CHANNEL_PROFILE_LIVE_BROADCASTING.
	m_rtcEngine->setChannelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("live broadcasting"));
	//set client role in the engine to the CLIENT_ROLE_BROADCASTER.
	m_rtcEngine->setClientRole(CLIENT_ROLE_BROADCASTER);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setClientRole broadcaster"));
	return true;
}


//UnInitialize the Agora SDK
void CAgoraBeautyAudio::UnInitAgora()
{
	if (m_rtcEngine) {
		if (m_joinChannel)
			//leave channel
			m_joinChannel = !m_rtcEngine->leaveChannel();
		//stop preview in the engine.
		m_rtcEngine->stopPreview();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stopPreview"));
		//disable video in the engine.
		m_rtcEngine->disableVideo();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("disableVideo"));
		//release engine.
		m_rtcEngine->release(true);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release rtc engine"));
		m_rtcEngine = NULL;
	}
}

//render local video from SDK local capture.
void CAgoraBeautyAudio::RenderLocalVideo()
{
	if (m_rtcEngine) {
		//start preview in the engine.
		m_rtcEngine->startPreview();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("startPreview"));
		VideoCanvas canvas;
		canvas.renderMode = RENDER_MODE_FIT;
		canvas.uid = 0;
		canvas.view = m_localVideoWnd.GetSafeHwnd();
		//setup local video in the engine to canvas.
		m_rtcEngine->setupLocalVideo(canvas);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setupLocalVideo"));
	}
}


//resume window status
void CAgoraBeautyAudio::ResumeStatus()
{
	InitCtrlText();
	m_staDetail.SetWindowText(_T(""));
	m_edtChannel.SetWindowText(_T(""));
	m_cmbAudioChange.SetCurSel(0);
	m_btnSetBeautyAudio.SetWindowText(beautyAudioCtrlSetAudioChange);
	OnSelchangeComboAudioChanger();
	m_lstInfo.ResetContent();
	m_joinChannel = false;
	m_initialize = false;
	m_beautyAudio = false;
}

void CAgoraBeautyAudio::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannel);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannel);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_STATIC_AUDIO_CHANGER, m_staAudioChange);
	DDX_Control(pDX, IDC_COMBO_AUDIO_CHANGER, m_cmbAudioChange);
	DDX_Control(pDX, IDC_STATIC_DETAIL, m_staDetail);
	DDX_Control(pDX, IDC_BUTTON_SET_BEAUTY_AUDIO, m_btnSetBeautyAudio);
	DDX_Control(pDX, IDC_STATIC_BEAUTY_AUDIO_TYPE, m_staAudioType);
	DDX_Control(pDX, IDC_COMBO_AUDIO_PERVERB_PRESET, m_cmbPerverbPreset);
}


BEGIN_MESSAGE_MAP(CAgoraBeautyAudio, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraBeautyAudio::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraBeautyAudio::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraBeautyAudio::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraBeautyAudio::OnEIDUserOffline)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANED), &CAgoraBeautyAudio::OnEIDRemoteVideoStateChanged)
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraBeautyAudio::OnBnClickedButtonJoinchannel)
	ON_BN_CLICKED(IDC_BUTTON_SET_BEAUTY_AUDIO, &CAgoraBeautyAudio::OnBnClickedButtonSetAudioChange)
	ON_LBN_SELCHANGE(IDC_LIST_INFO_BROADCASTING, &CAgoraBeautyAudio::OnSelchangeListInfoBroadcasting)
	ON_CBN_SELCHANGE(IDC_COMBO_AUDIO_CHANGER, &CAgoraBeautyAudio::OnSelchangeComboAudioChanger)
END_MESSAGE_MAP()




BOOL CAgoraBeautyAudio::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);
	RECT rcArea;
	m_staVideoArea.GetClientRect(&rcArea);
	m_localVideoWnd.MoveWindow(&rcArea);
	m_localVideoWnd.ShowWindow(SW_SHOW);

	int nIndex = 0;
	m_mapBeauty.insert(
		std::make_pair(CString(_T("VOICE_CHANGER")), 
			std::vector<CString>({_T("VOICE_CHANGER_OLDMAN"),_T("VOICE_CHANGER_BABYBOY")
				,_T("VOICE_CHANGER_BABYGIRL") ,_T("VOICE_CHANGER_ZHUBAJIE") ,
				_T("VOICE_CHANGER_HULK") ,_T("AUDIO_REVERB_FX_UNCLE") 
				,_T("AUDIO_REVERB_FX_SISTER") })));  
	m_mapBeauty.insert(
		std::make_pair(CString(_T("SPACE_CREATING")),
			std::vector<CString>({ _T("VOICE_CHANGER_ETHEREAL"),
				_T("VOICE_BEAUTY_SPACIAL"),_T("AUDIO_REVERB_FX_KTV") 
				,_T("AUDIO_REVERB_FX_VOCAL_CONCERT") ,_T("AUDIO_REVERB_FX_STUDIO") 
				,_T("AUDIO_REVERB_FX_PHONOGRAPH") ,_T("AUDIO_REVERB_VOCAL_CONCERT") 
				,_T("AUDIO_REVERB_KTV"),_T("AUDIO_REVERB_STUDIO")
				,_T("AUDIO_VIRTUAL_STEREO") })));         
	m_mapBeauty.insert(
		std::make_pair(CString(_T("TONE_CHANGER")),
			std::vector<CString>({ _T("VOICE_BEAUTY_VIGOROUS")
				,_T("VOICE_BEAUTY_DEEP")
				,_T("VOICE_BEAUTY_MELLOW")
				,_T("VOICE_BEAUTY_FALSETTO")
				,_T("VOICE_BEAUTY_FULL")
				,_T("VOICE_BEAUTY_CLEAR")
				,_T("VOICE_BEAUTY_RESOUNDING")
				,_T("VOICE_BEAUTY_RINGING") })));
	m_mapBeauty.insert(
		std::make_pair(CString(_T("CHAT_BEAUTY")),
			std::vector<CString>({ _T("GENERAL_BEAUTY_VOICE_MALE_MAGNETIC"),
				_T("GENERAL_BEAUTY_VOICE_FEMALE_FRESH"),
				_T("GENERAL_BEAUTY_VOICE_FEMALE_VITALITY") })));
	m_mapBeauty.insert(
		std::make_pair(CString(_T("STYLE_SOUND")),
			std::vector<CString>({ _T("AUDIO_REVERB_FX_POPULAR")
				,_T("AUDIO_REVERB_FX_RNB")
				,_T("AUDIO_REVERB_POPULAR")
				,_T("AUDIO_REVERB_RNB")
				,_T("AUDIO_REVERB_ROCK")
				,_T("AUDIO_REVERB_HIPHOP")})));  
	m_cmbAudioChange.InsertString(nIndex++, _T("VOICE_CHANGER"));
	m_cmbAudioChange.InsertString(nIndex++, _T("SPACE_CREATING"));
	m_cmbAudioChange.InsertString(nIndex++, _T("TONE_CHANGER"));
	m_cmbAudioChange.InsertString(nIndex++, _T("CHAT_BEAUTY"));
	m_cmbAudioChange.InsertString(nIndex++, _T("STYLE_SOUND"));


	m_setChanger.insert(std::make_pair(_T("VOICE_CHANGER_OFF"), VOICE_CHANGER_OFF));
	m_setChanger.insert(std::make_pair(_T("VOICE_CHANGER_OLDMAN"), VOICE_CHANGER_OLDMAN));
	m_setChanger.insert(std::make_pair(_T("VOICE_BEAUTY_VIGOROUS"), VOICE_BEAUTY_VIGOROUS));
	m_setChanger.insert(std::make_pair(_T("VOICE_CHANGER_BABYBOY"), VOICE_CHANGER_BABYBOY));
	m_setChanger.insert(std::make_pair(_T("VOICE_CHANGER_BABYGIRL"), VOICE_CHANGER_BABYGIRL));
	m_setChanger.insert(std::make_pair(_T("VOICE_CHANGER_ZHUBAJIE"), VOICE_CHANGER_ZHUBAJIE));
	m_setChanger.insert(std::make_pair(_T("VOICE_CHANGER_ETHEREAL"), VOICE_CHANGER_ETHEREAL));
	m_setChanger.insert(std::make_pair(_T("VOICE_CHANGER_HULK"), VOICE_CHANGER_HULK));
	m_setChanger.insert(std::make_pair(_T("VOICE_BEAUTY_VIGOROUS"), VOICE_BEAUTY_VIGOROUS));
	m_setChanger.insert(std::make_pair(_T("VOICE_BEAUTY_DEEP"), VOICE_BEAUTY_DEEP));
	m_setChanger.insert(std::make_pair(_T("VOICE_BEAUTY_MELLOW"), VOICE_BEAUTY_MELLOW));
	m_setChanger.insert(std::make_pair(_T("VOICE_BEAUTY_FALSETTO"), VOICE_BEAUTY_FALSETTO));
	m_setChanger.insert(std::make_pair(_T("VOICE_BEAUTY_FULL"), VOICE_BEAUTY_FULL));
	m_setChanger.insert(std::make_pair(_T("VOICE_BEAUTY_CLEAR"), VOICE_BEAUTY_CLEAR));
	m_setChanger.insert(std::make_pair(_T("VOICE_BEAUTY_RESOUNDING"), VOICE_BEAUTY_RESOUNDING));
	m_setChanger.insert(std::make_pair(_T("VOICE_BEAUTY_RINGING"), VOICE_BEAUTY_RINGING));
	m_setChanger.insert(std::make_pair(_T("VOICE_BEAUTY_SPACIAL"), VOICE_BEAUTY_SPACIAL));
	m_setChanger.insert(std::make_pair(_T("GENERAL_BEAUTY_VOICE_MALE_MAGNETIC"), GENERAL_BEAUTY_VOICE_MALE_MAGNETIC));
	m_setChanger.insert(std::make_pair(_T("GENERAL_BEAUTY_VOICE_FEMALE_FRESH"), GENERAL_BEAUTY_VOICE_FEMALE_FRESH));
	m_setChanger.insert(std::make_pair(_T("GENERAL_BEAUTY_VOICE_FEMALE_VITALITY"), GENERAL_BEAUTY_VOICE_FEMALE_VITALITY));


		
	m_setReverbPreSet.insert(std::make_pair(_T("AUDIO_REVERB_OFF"), AUDIO_REVERB_OFF));
	m_setReverbPreSet.insert(std::make_pair(_T("AUDIO_REVERB_FX_KTV"),AUDIO_REVERB_FX_KTV));
	m_setReverbPreSet.insert(std::make_pair(_T("AUDIO_REVERB_FX_VOCAL_CONCERT"), AUDIO_REVERB_FX_VOCAL_CONCERT));
	m_setReverbPreSet.insert(std::make_pair(_T("AUDIO_REVERB_FX_UNCLE"), AUDIO_REVERB_FX_UNCLE));
	m_setReverbPreSet.insert(std::make_pair(_T("AUDIO_REVERB_FX_SISTER"), AUDIO_REVERB_FX_SISTER));
	m_setReverbPreSet.insert(std::make_pair(_T("AUDIO_REVERB_FX_STUDIO"), AUDIO_REVERB_FX_STUDIO));
	m_setReverbPreSet.insert(std::make_pair(_T("AUDIO_REVERB_FX_POPULAR"), AUDIO_REVERB_FX_POPULAR));
	m_setReverbPreSet.insert(std::make_pair(_T("AUDIO_REVERB_FX_RNB"), AUDIO_REVERB_FX_RNB));
	m_setReverbPreSet.insert(std::make_pair(_T("AUDIO_REVERB_FX_PHONOGRAPH"), AUDIO_REVERB_FX_PHONOGRAPH));
	m_setReverbPreSet.insert(std::make_pair(_T("AUDIO_REVERB_POPULAR"), AUDIO_REVERB_POPULAR));
	m_setReverbPreSet.insert(std::make_pair(_T("AUDIO_REVERB_RNB"), AUDIO_REVERB_RNB));
	m_setReverbPreSet.insert(std::make_pair(_T("AUDIO_REVERB_ROCK"),AUDIO_REVERB_ROCK));
	m_setReverbPreSet.insert(std::make_pair(_T("AUDIO_REVERB_HIPHOP"), AUDIO_REVERB_HIPHOP));
	m_setReverbPreSet.insert(std::make_pair(_T("AUDIO_REVERB_VOCAL_CONCERT"), AUDIO_REVERB_VOCAL_CONCERT));
	m_setReverbPreSet.insert(std::make_pair(_T("AUDIO_REVERB_KTV"), AUDIO_REVERB_KTV));
	m_setReverbPreSet.insert(std::make_pair(_T("AUDIO_REVERB_STUDIO"), AUDIO_REVERB_STUDIO));
	m_setReverbPreSet.insert(std::make_pair(_T("AUDIO_VIRTUAL_STEREO"), AUDIO_VIRTUAL_STEREO));
	ResumeStatus();
	return TRUE;  
}




void CAgoraBeautyAudio::OnShowWindow(BOOL bShow, UINT nStatus)
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


void CAgoraBeautyAudio::OnBnClickedButtonJoinchannel()
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
		//join channel in the engine.
		if (0 == m_rtcEngine->joinChannel(APP_TOKEN, szChannelId.c_str(), "", 0)) {
			strInfo.Format(_T("join channel %s"), getCurrentTime());
			m_btnJoinChannel.EnableWindow(FALSE);
		}
	}
	else {
		//leave channel in the engine.
		if (0 == m_rtcEngine->leaveChannel()) {
			strInfo.Format(_T("leave channel %s"), getCurrentTime());
		}
	}
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}

//set audio changer or unset audio changer.
void CAgoraBeautyAudio::OnBnClickedButtonSetAudioChange()
{
	CString strInfo;
	if (!m_beautyAudio)
	{
		CString str;
		m_cmbPerverbPreset.GetWindowText(str);
		//enable audio beauty.
		if (m_setChanger.find(str) != m_setChanger.end())
		{
			m_rtcEngine->setLocalVoiceChanger(m_setChanger[str]);
		}
		if (m_setReverbPreSet.find(str) != m_setReverbPreSet.end())
		{
			m_rtcEngine->setLocalVoiceReverbPreset(m_setReverbPreSet[str]);
		}
		strInfo.Format(_T("set :%s"), str);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
		m_btnSetBeautyAudio.SetWindowText(beautyAudioCtrlUnSetAudioChange);
	}
	else {
		//set audio beauty to VOICE_CHANGER_OFF.
		m_rtcEngine->setLocalVoiceChanger(VOICE_CHANGER_OFF);
		m_rtcEngine->setLocalVoiceReverbPreset(AUDIO_REVERB_OFF);
		m_lstInfo.InsertString(m_lstInfo.GetCount(),_T("unset beauty voice"));
		m_btnSetBeautyAudio.SetWindowText(beautyAudioCtrlSetAudioChange);
	}
	m_beautyAudio = !m_beautyAudio;

}


//EID_JOINCHANNEL_SUCCESS message window handler
LRESULT CAgoraBeautyAudio::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
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
LRESULT CAgoraBeautyAudio::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
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
LRESULT CAgoraBeautyAudio::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	CString strInfo;
	strInfo.Format(_T("%u joined"), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}

//EID_USER_OFFLINE message handler.
LRESULT CAgoraBeautyAudio::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
	uid_t remoteUid = (uid_t)wParam;
	VideoCanvas canvas;
	canvas.uid = remoteUid;
	canvas.view = NULL;
	m_rtcEngine->setupRemoteVideo(canvas);
	CString strInfo;
	strInfo.Format(_T("%u offline, reason:%d"), remoteUid, lParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}

//EID_REMOTE_VIDEO_STATE_CHANED message window handler.
LRESULT CAgoraBeautyAudio::OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam)
{
	PVideoStateStateChanged stateChanged = (PVideoStateStateChanged)wParam;
	if (stateChanged) {
		//onRemoteVideoStateChanged
		CString strSateInfo;
		switch (stateChanged->state) {
		case REMOTE_VIDEO_STATE_STARTING:
			strSateInfo = _T("REMOTE_VIDEO_STATE_STARTING");
			break;
		case REMOTE_VIDEO_STATE_STOPPED:
			strSateInfo = _T("strSateInfo");
			break;
		case REMOTE_VIDEO_STATE_DECODING:
			strSateInfo = _T("REMOTE_VIDEO_STATE_DECODING");
			break;
		case REMOTE_VIDEO_STATE_FAILED:
			strSateInfo = _T("REMOTE_VIDEO_STATE_FAILED ");
			break;
		case REMOTE_VIDEO_STATE_FROZEN:
			strSateInfo = _T("REMOTE_VIDEO_STATE_FROZEN  ");
			break;
		}
		CString strInfo;
		strInfo.Format(_T("onRemoteVideoStateChanged: uid=%u, %s"), stateChanged->uid, strSateInfo);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	}
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
void CAudioChangeEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
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
void CAudioChangeEventHandler::onUserJoined(uid_t uid, int elapsed)
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
void CAudioChangeEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
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

void CAudioChangeEventHandler::onLeaveChannel(const RtcStats& stats)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
	}
}
/**
	Occurs when the remote video state changes.
	@note This callback does not work properly when the number of users (in the Communication profile) or broadcasters (in the Live-broadcast profile) in the channel exceeds 17.

	@param uid ID of the remote user whose video state changes.
	@param state State of the remote video. See #REMOTE_VIDEO_STATE.
	@param reason The reason of the remote video state change. See
	#REMOTE_VIDEO_STATE_REASON.
	@param elapsed Time elapsed (ms) from the local user calling the
	\ref agora::rtc::IRtcEngine::joinChannel "joinChannel" method until the
	SDK triggers this callback.
*/
void CAudioChangeEventHandler::onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed)
{
	if (m_hMsgHanlder) {
		PVideoStateStateChanged stateChanged = new VideoStateStateChanged;
		stateChanged->uid = uid;
		stateChanged->reason = reason;
		stateChanged->state = state;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANED), (WPARAM)stateChanged, 0);
	}
}


BOOL CAgoraBeautyAudio::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CAgoraBeautyAudio::OnSelchangeListInfoBroadcasting()
{
	int sel = m_lstInfo.GetCurSel();
	if (sel < 0)return;
	CString strDetail;
	m_lstInfo.GetText(sel, strDetail);
	m_staDetail.SetWindowText(strDetail);
}


void CAgoraBeautyAudio::OnSelchangeComboAudioChanger()
{
	m_cmbPerverbPreset.ResetContent();
	CString strType;
	m_cmbAudioChange.GetWindowText(strType);
	int nIndex = 0;
	for (auto & str : m_mapBeauty[strType])
	{
		m_cmbPerverbPreset.InsertString(nIndex++, str);
	}
	m_cmbPerverbPreset.SetCurSel(0);
}
