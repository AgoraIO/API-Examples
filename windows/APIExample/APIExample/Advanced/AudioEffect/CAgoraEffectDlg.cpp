#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraEffectDlg.h"


IMPLEMENT_DYNAMIC(CAgoraEffectDlg, CDialogEx)

CAgoraEffectDlg::CAgoraEffectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_AUDIO_EFFECT, pParent)
{

}

CAgoraEffectDlg::~CAgoraEffectDlg()
{
}

void CAgoraEffectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannel);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannel);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_STATIC_AUDIO_EFFECT_PATH, m_staEffectPath);
	DDX_Control(pDX, IDC_EDIT_AUDIO_EFFECT_PATH, m_edtEffectPath);
	DDX_Control(pDX, IDC_BUTTON_ADD_EFFECT, m_btnAddEffect);
	DDX_Control(pDX, IDC_BUTTON_PRELOAD, m_btnPreLoad);
	DDX_Control(pDX, IDC_BUTTON_UNLOAD_EFFECT, m_btnUnload);
	DDX_Control(pDX, IDC_BUTTON_REMOVE, m_btnRemove);
	DDX_Control(pDX, IDC_BUTTON_PAUSE_EFFECT, m_btnPause);
	DDX_Control(pDX, IDC_BUTTON_RESUME_EFFECT, m_btnResume);
	DDX_Control(pDX, IDC_STATIC_DETAIL, m_staDetails);
	DDX_Control(pDX, IDC_STATIC_AUDIO_REPEAT, m_staLoops);
	DDX_Control(pDX, IDC_EDIT_AUDIO_REPEAT_TIMES, m_edtLoops);
	DDX_Control(pDX, IDC_STATIC_AUDIO_AGIN, m_staGain);
	DDX_Control(pDX, IDC_EDIT_AUDIO_AGIN, m_edtGain);
	DDX_Control(pDX, IDC_SPIN_AGIN, m_spinGain);
	DDX_Control(pDX, IDC_STATIC_AUDIO_PITCH, m_staPitch);
	DDX_Control(pDX, IDC_EDIT_AUDIO_PITCH, m_edtPitch);
	DDX_Control(pDX, IDC_SPIN_PITCH, m_spinPitch);
	DDX_Control(pDX, IDC_STATIC_AUDIO_PAN, m_staPan);
	DDX_Control(pDX, IDC_COMBO_PAN, m_cmbPan);
	DDX_Control(pDX, IDC_CHK_PUBLISH, m_chkPublish);
	DDX_Control(pDX, IDC_BUTTON_PLAY_EFFECT, m_btnPlay);
	DDX_Control(pDX, IDC_BUTTON_PAUSE_ALL_EFFECT, m_btnPauseAll);
	DDX_Control(pDX, IDC_BUTTON_STOP_ALL_EFFECT2, m_btnStopAll);
	DDX_Control(pDX, IDC_BUTTON_STOP_EFFECT, m_btnStopEffect);
	DDX_Control(pDX, IDC_STATIC_AUDIO_EFFECT, m_staEffect);
	DDX_Control(pDX, IDC_COMBO2, m_cmbEffect);
	DDX_Control(pDX, IDC_STATIC_AUDIO_VLOUME, m_staVolume);
	DDX_Control(pDX, IDC_SLIDER_VLOUME, m_sldVolume);
}


BEGIN_MESSAGE_MAP(CAgoraEffectDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraEffectDlg::OnBnClickedButtonJoinchannel)
	ON_BN_CLICKED(IDC_BUTTON_ADD_EFFECT, &CAgoraEffectDlg::OnBnClickedButtonAddEffect)
	ON_BN_CLICKED(IDC_BUTTON_PRELOAD, &CAgoraEffectDlg::OnBnClickedButtonPreload)
	ON_BN_CLICKED(IDC_BUTTON_UNLOAD_EFFECT, &CAgoraEffectDlg::OnBnClickedButtonUnloadEffect)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, &CAgoraEffectDlg::OnBnClickedButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE_EFFECT, &CAgoraEffectDlg::OnBnClickedButtonPauseEffect)
	ON_BN_CLICKED(IDC_BUTTON_RESUME_EFFECT, &CAgoraEffectDlg::OnBnClickedButtonResumeEffect)
	ON_BN_CLICKED(IDC_BUTTON_PLAY_EFFECT, &CAgoraEffectDlg::OnBnClickedButtonPlayEffect)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE_ALL_EFFECT, &CAgoraEffectDlg::OnBnClickedButtonPauseAllEffect)
	ON_BN_CLICKED(IDC_BUTTON_STOP_ALL_EFFECT2, &CAgoraEffectDlg::OnBnClickedButtonStopAllEffect2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_AGIN, &CAgoraEffectDlg::OnDeltaposSpinGain)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PITCH, &CAgoraEffectDlg::OnDeltaposSpinPitch)
	ON_LBN_SELCHANGE(IDC_LIST_INFO_BROADCASTING, &CAgoraEffectDlg::OnSelchangeListInfoBroadcasting)
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraEffectDlg::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraEffectDlg::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_AUDIO_EFFECT_FINISHED), &CAgoraEffectDlg::OnEIDAudioEffectFinished)

	ON_BN_CLICKED(IDC_BUTTON_STOP_EFFECT, &CAgoraEffectDlg::OnBnClickedButtonStopEffect)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_VLOUME, &CAgoraEffectDlg::OnReleasedcaptureSliderVolume)
	ON_NOTIFY(NM_THEMECHANGED, IDC_SPIN_AGIN, &CAgoraEffectDlg::OnNMThemeChangedSpinAgin)
END_MESSAGE_MAP()


//Initialize the Ctrl Text.
void CAgoraEffectDlg::InitCtrlText()
{
	m_staChannel.SetWindowText(commonCtrlChannel);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	m_staEffectPath.SetWindowText(AudioEffectCtrlEffectPath);
	m_staEffect.SetWindowText(AudioEffectCtrlEffect);
	m_staGain.SetWindowText(AudioEffectCtrlGain);
	m_staPan.SetWindowText(AudioEffectCtrlPan);
	m_staPitch.SetWindowText(AudioEffectCtrlPitch);
	m_staLoops.SetWindowText(AudioEffectCtrlLoops);
	m_chkPublish.SetWindowText(AudioEffectCtrlPublish);
	m_btnAddEffect.SetWindowText(AudioEffectCtrlAddEffect);
	m_btnPause.SetWindowText(AudioEffectCtrlPauseEffect);
	m_btnRemove.SetWindowText(AudioEffectCtrlRemoveEffect);
	m_btnPlay.SetWindowText(AudioEffectCtrlPlayEffect);
	m_btnPauseAll.SetWindowText(AudioEffectCtrlPauseAllEffect);
	m_btnPreLoad.SetWindowText(AudioEffectCtrlPreLoad);
	m_btnUnload.SetWindowText(AudioEffectCtrlUnPreload);
	m_btnResume.SetWindowText(AudioEffectCtrlResumeEffect);
	m_btnStopAll.SetWindowText(AudioEffectCtrlStopAllEffect);
	m_btnStopEffect.SetWindowText(AudioEffectCtrlStopEffect);
	m_staVolume.SetWindowText(AudioEffectCtrlVolume);
}



//Initialize the Agora SDK
bool CAgoraEffectDlg::InitAgora()
{
	//create Agora RTC engine
	m_rtcEngine = createAgoraRtcEngine();
	if (!m_rtcEngine) {
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("createAgoraRtcEngine failed"));
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


//UnInitialize the Agora SDK
void CAgoraEffectDlg::UnInitAgora()
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
void CAgoraEffectDlg::RenderLocalVideo()
{
	if (m_rtcEngine) {
		//start preview in the engine.
		m_rtcEngine->startPreview();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("startPreview"));
		VideoCanvas canvas;
		canvas.renderMode = media::base::RENDER_MODE_FIT;
		canvas.uid = 0;
		canvas.view = m_localVideoWnd.GetSafeHwnd();
		//setup local video in the engine to canvas.
		m_rtcEngine->setupLocalVideo(canvas);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setupLocalVideo"));
	}
}


//resume window status
void CAgoraEffectDlg::ResumeStatus()
{
	InitCtrlText();
	m_lstInfo.ResetContent();
	m_edtChannel.SetWindowText(_T(""));
	m_edtEffectPath.SetWindowText(_T(""));
	m_edtGain.SetWindowText(_T("100.0"));
	m_edtLoops.SetWindowText(_T("0"));
	m_edtPitch.SetWindowText(_T("1.0"));
	m_cmbPan.SetCurSel(0);
	m_cmbEffect.ResetContent();
	m_chkPublish.SetCheck(TRUE);
	m_btnPauseAll.SetWindowText(AudioEffectCtrlPauseAllEffect);
	m_pauseAll = false;
	m_joinChannel = false;
	m_initialize = false;
	m_audioMixing = false;
}

void CAgoraEffectDlg::OnBnClickedButtonJoinchannel()
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

//add effect button click handler
void CAgoraEffectDlg::OnBnClickedButtonAddEffect()
{
	CString strPath;
	m_edtEffectPath.GetWindowText(strPath);
	//judge file is exists.
	if (PathFileExists(strPath))
	{
		m_cmbEffect.InsertString(m_cmbEffect.GetCount(), strPath);
		m_mapEffect.insert(std::make_pair(strPath, m_soundId++));
	}
	else {
		MessageBox(_T("file is not exists."));
	}
	m_cmbEffect.SetCurSel(0);
}


//pre load button click handler
void CAgoraEffectDlg::OnBnClickedButtonPreload()
{
	if (m_cmbEffect.GetCurSel() < 0)
	{
		return;
	}
	CString strEffect;
	m_cmbEffect.GetWindowText(strEffect);
	std::string strPath = cs2utf8(strEffect);
	//pre load effect
	m_rtcEngine->preloadEffect(m_mapEffect[strEffect], strPath.c_str());
	CString strInfo;
	strInfo.Format(_T("preload effect :path:%s"), strEffect);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}

//un load button click handler
void CAgoraEffectDlg::OnBnClickedButtonUnloadEffect()
{
	if (m_cmbEffect.GetCurSel() < 0)
	{
		return;
	}
	CString strEffect;
	m_cmbEffect.GetWindowText(strEffect);
	// un load effect 
	m_rtcEngine->unloadEffect(m_mapEffect[strEffect]);
	CString strInfo;
	strInfo.Format(_T("unload effect :path:%s"), strEffect);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}


//remove effect button click handler.
void CAgoraEffectDlg::OnBnClickedButtonRemove()
{
	if (m_cmbEffect.GetCurSel() < 0)
	{
		return;
	}
	CString strEffect;
	m_cmbEffect.GetWindowText(strEffect);
	m_cmbEffect.DeleteString(m_cmbEffect.GetCurSel());
	CString strInfo;
	strInfo.Format(_T("remove effect :path:%s"), strEffect);
	m_rtcEngine->stopEffect(m_mapEffect[strEffect]);
	m_mapEffect.erase(m_mapEffect.find(strEffect));
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_cmbEffect.SetCurSel(0);
	
}

//pause effect button click handler.
void CAgoraEffectDlg::OnBnClickedButtonPauseEffect()
{
	if (m_cmbEffect.GetCurSel() < 0)
	{
		return;
	}
	CString strEffect;
	m_cmbEffect.GetWindowText(strEffect);
	//pause effect by sound id
	m_rtcEngine->pauseEffect(m_mapEffect[strEffect]);

	CString strInfo;
	strInfo.Format(_T("pause effect :path:%s"), strEffect);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}


//resume effect button click handler.
void CAgoraEffectDlg::OnBnClickedButtonResumeEffect()
{
	if (m_cmbEffect.GetCurSel() < 0)
	{
		return;
	}
	CString strEffect;
	m_cmbEffect.GetWindowText(strEffect);
	// resume effect by sound id.
	int ret = m_rtcEngine->resumeEffect(m_mapEffect[strEffect]);
	
	CString strInfo;
	strInfo.Format(_T("resume effect ret:%d :path:%s"),ret, strEffect);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}

//play effect button click handler.
void CAgoraEffectDlg::OnBnClickedButtonPlayEffect()
{
	if (m_cmbEffect.GetCurSel() < 0)
	{
		return;
	}
	CString strEffect;
	m_cmbEffect.GetWindowText(strEffect);
	std::string strFile = cs2utf8(strEffect);
	CString strLoops;
	m_edtLoops.GetWindowText(strLoops);
	int loops = _ttol(strLoops);
	if (loops == 0) {
		m_edtLoops.SetWindowText(_T("1"));
		loops = 1;
	}
	CString strPitch;
	m_edtPitch.GetWindowText(strPitch);
	double pitch = _ttof(strPitch);

	CString strGain;
	m_edtGain.GetWindowText(strGain);
	int gain = _ttol(strGain);

	CString strPan;
	m_cmbPan.GetWindowText(strPan);
	double pan = _ttof(strPan);

	BOOL publish = m_chkPublish.GetCheck();
	//play effect by effect path.
	int ret = m_rtcEngine->playEffect(m_mapEffect[strEffect], strFile.c_str(), loops, pitch, pan, gain, publish);
	
	CString strInfo;
	strInfo.Format(_T("play effect :path:%s, ret:%d"), strEffect, ret);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	strInfo.Format(_T("loops:%d,pitch:%.1f,pan:%.0f,gain:%d,publish:%d"),
		loops, pitch, pan, gain, publish);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	
	
}

//stop effect button click handler.
void CAgoraEffectDlg::OnBnClickedButtonStopEffect()
{
	if (m_cmbEffect.GetCurSel() < 0)
	{
		return;
	}
	CString strEffect;
	m_cmbEffect.GetWindowText(strEffect);
	//stop effect by sound id.
	m_rtcEngine->stopEffect(m_mapEffect[strEffect]);

	CString strInfo;
	strInfo.Format(_T("stop effect :path:%s"), strEffect);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}

//pause all effect button click handler.
void CAgoraEffectDlg::OnBnClickedButtonPauseAllEffect()
{
	if (!m_pauseAll)
	{
		//pause all effect
		m_rtcEngine->pauseAllEffects();
		CString strInfo;
		strInfo.Format(_T("pause All Effects"));
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
		m_btnPauseAll.SetWindowText(AudioEffectCtrlResumeEffect);
	}
	else {
		//resume all effect
		m_rtcEngine->resumeAllEffects();
		CString strInfo;
		strInfo.Format(_T("resume All Effects"));
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
		m_btnPauseAll.SetWindowText(AudioEffectCtrlPauseAllEffect);
	}
	m_pauseAll = !m_pauseAll;
}

//stop all effect button click handler.
void CAgoraEffectDlg::OnBnClickedButtonStopAllEffect2()
{
	//stop all effect
	m_rtcEngine->stopAllEffects();
	CString strInfo;
	strInfo.Format(_T("stop All Effects"));
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}


void CAgoraEffectDlg::OnDeltaposSpinGain(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	CString strGain;
	m_edtGain.GetWindowText(strGain);
	double gain = _ttof(strGain);
	if ((pNMUpDown->iDelta < 0))
		gain = (gain + 10.0f <= 100 ? gain + 10.0 : gain);
	if ((pNMUpDown->iDelta > 0))
		gain = (gain - 10.0f >= 0.0 ? gain - 10.0 : gain);
	strGain.Format(_T("%.1f"), gain);
	m_edtGain.SetWindowText(strGain);
	*pResult = 0;
}


void CAgoraEffectDlg::OnDeltaposSpinPitch(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	CString strPitch;
	m_edtPitch.GetWindowText(strPitch);
	double pitch = _ttof(strPitch);
	if ((pNMUpDown->iDelta < 0))
		pitch = (pitch + 0.1 <= 2 ? pitch + 0.1 : pitch);
	if ((pNMUpDown->iDelta > 0))
		pitch = (pitch - 0.1 >= 0.5 ? pitch - 0.1 : pitch);
	strPitch.Format(_T("%.1f"), pitch);
	m_edtPitch.SetWindowText(strPitch);
	*pResult = 0;
}


void CAgoraEffectDlg::OnSelchangeListInfoBroadcasting()
{
	int sel = m_lstInfo.GetCurSel();
	if (sel < 0)return;
	CString strDetail;
	m_lstInfo.GetText(sel, strDetail);
	m_staDetails.SetWindowText(strDetail);
}


void CAgoraEffectDlg::OnShowWindow(BOOL bShow, UINT nStatus)
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


BOOL CAgoraEffectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);
	RECT rcArea;
	m_staVideoArea.GetClientRect(&rcArea);
	m_localVideoWnd.MoveWindow(&rcArea);
	m_localVideoWnd.ShowWindow(SW_SHOW);
	int nIndex = 0;

	m_cmbPan.InsertString(nIndex++, _T("0"));
	m_cmbPan.InsertString(nIndex++, _T("-1"));
	m_cmbPan.InsertString(nIndex++, _T("1"));
	ResumeStatus();
	m_sldVolume.SetRange(0, 100);
	m_sldVolume.SetPos(100);
	m_edtGain.SetWindowText(L"100.0");
	return TRUE;
}


BOOL CAgoraEffectDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}





//EID_JOINCHANNEL_SUCCESS message window handler
LRESULT CAgoraEffectDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
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
LRESULT CAgoraEffectDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
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
LRESULT CAgoraEffectDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	CString strInfo;
	strInfo.Format(_T("%u joined"), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}

//EID_USER_OFFLINE message handler.
LRESULT CAgoraEffectDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
	uid_t remoteUid = (uid_t)wParam;
	CString strInfo;
	strInfo.Format(_T("%u offline, reason:%d"), remoteUid, lParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}

//EID_REMOTE_VIDEO_STATE_CHANGED message window handler.
LRESULT CAgoraEffectDlg::OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam)
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

LRESULT CAgoraEffectDlg::OnEIDAudioEffectFinished(WPARAM wParam, LPARAM lParam)
{
	int soundId = (int)wParam;
	for (auto iter : m_mapEffect) {
		if (soundId == iter.second) {
			m_lstInfo.InsertString(m_lstInfo.GetCount(), iter.first);
			m_lstInfo.InsertString(m_lstInfo.GetCount(), L"Play Audio Effect Finished");
		}
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
void CAudioEffectEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed)
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
void CAudioEffectEventHandler::onUserJoined(uid_t uid, int elapsed)
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
void CAudioEffectEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
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

void CAudioEffectEventHandler::onLeaveChannel(const RtcStats& stats)
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
void CAudioEffectEventHandler::onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed)
{
	if (m_hMsgHanlder) {
		PVideoStateStateChanged stateChanged = new VideoStateStateChanged;
		stateChanged->uid = uid;
		stateChanged->reason = reason;
		stateChanged->state = state;
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANGED), (WPARAM)stateChanged, 0);
	}
}

void CAudioEffectEventHandler::onAudioEffectFinished(int soundId)
{
	if (m_hMsgHanlder) {
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_AUDIO_EFFECT_FINISHED), (WPARAM)soundId, 0);
	}
}


void CAgoraEffectDlg::OnReleasedcaptureSliderVolume(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	int pos = m_sldVolume.GetPos();
	m_rtcEngine->setEffectsVolume(pos);
	//m_mediaPlayer->seek(pos);
	*pResult = 0;
}


void CAgoraEffectDlg::OnNMThemeChangedSpinAgin(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
}
