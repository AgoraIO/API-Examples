#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraCrossChannelDlg.h"
#include <algorithm>

IMPLEMENT_DYNAMIC(CAgoraCrossChannelDlg, CDialogEx)

CAgoraCrossChannelDlg::CAgoraCrossChannelDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CROSS_CHANNEL, pParent)
{

}

CAgoraCrossChannelDlg::~CAgoraCrossChannelDlg()
{
}

void CAgoraCrossChannelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannel);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannel);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_STATIC_CROSS_CHANNEL, m_staCrossChannel);
	DDX_Control(pDX, IDC_EDIT_CROSS_CHANNEL, m_edtCrossChannel);
	DDX_Control(pDX, IDC_STATIC_TOKEN, m_staToken);
	DDX_Control(pDX, IDC_EDIT_TOKEN, m_edtToken);
	DDX_Control(pDX, IDC_USER_ID, m_staUserID);
	DDX_Control(pDX, IDC_EDIT_USER_ID, m_edtUserID);
	DDX_Control(pDX, IDC_CROSS_CHANNEL_LIST, m_staCrossChannel);
	DDX_Control(pDX, IDC_COMBO_CROSS_CAHNNEL_LIST, m_cmbCrossChannelList);
	DDX_Control(pDX, IDC_STATIC_CROSS_CHANNEL, m_staCrossChannel);
	DDX_Control(pDX, IDC_CROSS_CHANNEL_LIST, m_staCrossChannelList);
	DDX_Control(pDX, IDC_BUTTON_ADD_CROSS_CHANNEL, m_btnAddChannel);
	DDX_Control(pDX, IDC_BUTTON_REMOVE_CROSS_CHANNEL2, m_btnRemove);
	DDX_Control(pDX, IDC_BUTTON_START_MEDIA_RELAY, m_btnStartMediaRelay);
	DDX_Control(pDX, IDC_STATIC_DETAIL, m_staDetails);
	DDX_Control(pDX, IDC_BUTTON_UPDATE, m_btnUpdate);
}


BEGIN_MESSAGE_MAP(CAgoraCrossChannelDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraCrossChannelDlg::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraCrossChannelDlg::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraCrossChannelDlg::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraCrossChannelDlg::OnEIDUserOffline)
	ON_MESSAGE(WM_MSGID(EID_CHANNEL_MEDIA_RELAY_EVENT), &CAgoraCrossChannelDlg::OnEIDChannelMediaRelayEvent)
	ON_MESSAGE(WM_MSGID(EID_CHANNEL_MEDIA_RELAY_STATE_CHNAGENED), &CAgoraCrossChannelDlg::OnEIDChannelMediaRelayStateChanged)


	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraCrossChannelDlg::OnBnClickedButtonJoinchannel)
	ON_BN_CLICKED(IDC_BUTTON_ADD_CROSS_CHANNEL, &CAgoraCrossChannelDlg::OnBnClickedButtonAddCrossChannel)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_CROSS_CHANNEL2, &CAgoraCrossChannelDlg::OnBnClickedButtonRemoveCrossChannel2)
	ON_BN_CLICKED(IDC_BUTTON_START_MEDIA_RELAY, &CAgoraCrossChannelDlg::OnBnClickedButtonStartMediaRelay)
	ON_LBN_SELCHANGE(IDC_LIST_INFO_BROADCASTING, &CAgoraCrossChannelDlg::OnSelchangeListInfoBroadcasting)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, &CAgoraCrossChannelDlg::OnBnClickedButtonUpdate)
END_MESSAGE_MAP()


//Initialize the Ctrl Text.
void CAgoraCrossChannelDlg::InitCtrlText()
{
	m_staChannel.SetWindowText(commonCtrlChannel);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	m_btnUpdate.SetWindowText(CrossChannelUpdateMediaRelay);
	m_btnAddChannel.SetWindowText(CrossChannelAddChannel);
	m_btnRemove.SetWindowText(CrossChannelRemoveChannel);
	m_btnStartMediaRelay.SetWindowText(CrossChannelStartMediaRelay);
	m_staCrossChannel.SetWindowText(CrossChannelCtrlCrossChannel);
	m_staCrossChannelList.SetWindowText(CrossChannelCrossChannelList);
	m_staToken.SetWindowText(CrossChannelCtrlToken);
	m_staUserID.SetWindowText(CrossChannelCtrlUid);
}



//Initialize the Agora SDK
bool CAgoraCrossChannelDlg::InitAgora()
{
	//create Agora RTC engine
	m_rtcEngine = createAgoraRtcEngine();
	if (!m_rtcEngine) {
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("createAgoraRtcEngine failed"));
		return false;
	}
	//set message notify receiver window
	m_eventHandler.SetMsgReceiver(m_hWnd);
	m_srcInfo = new ChannelMediaInfo;
	m_srcInfo->channelName = nullptr;
	m_srcInfo->token = nullptr;
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
void CAgoraCrossChannelDlg::UnInitAgora()
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
		if(m_srcInfo->channelName)
			delete m_srcInfo->channelName;
		delete m_srcInfo;
	}
}

//render local video from SDK local capture.
void CAgoraCrossChannelDlg::RenderLocalVideo()
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
void CAgoraCrossChannelDlg::ResumeStatus()
{
	InitCtrlText();
	m_lstInfo.ResetContent();
	m_cmbCrossChannelList.ResetContent();
	m_staDetails.SetWindowText(_T(""));
	m_edtChannel.SetWindowText(_T(""));
	m_edtCrossChannel.SetWindowText(_T(""));
	m_edtToken.SetWindowText(_T(""));
	m_edtUserID.SetWindowText(_T(""));
	int offset = 0;
	for (auto & mediaInfo : m_vecChannelMedias)
	{
		delete mediaInfo.channelName;
		delete mediaInfo.token;
	}
	m_vecChannelMedias.clear();
	m_joinChannel = false;
	m_initialize = false;
	m_startMediaRelay = false;
}


void CAgoraCrossChannelDlg::OnShowWindow(BOOL bShow, UINT nStatus)
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


BOOL CAgoraCrossChannelDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);
	RECT rcArea;
	m_staVideoArea.GetClientRect(&rcArea);
	m_localVideoWnd.MoveWindow(&rcArea);
	m_localVideoWnd.ShowWindow(SW_SHOW);
	ResumeStatus();
	return TRUE;
}


BOOL CAgoraCrossChannelDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CAgoraCrossChannelDlg::OnBnClickedButtonJoinchannel()
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
			//save channel name and token;
			m_srcInfo->channelName = new char[szChannelId.size() + 1];
			strcpy_s(const_cast<char*>(m_srcInfo->channelName), szChannelId.size() + 1, szChannelId.data());
			m_srcInfo->token = APP_TOKEN;
		}
	}
	else {
		//leave channel in the engine.
		if (0 == m_rtcEngine->leaveChannel()) {
			strInfo.Format(_T("leave channel %s"), getCurrentTime());
			delete m_srcInfo->channelName;
			m_srcInfo->channelName = nullptr;
		}
	}
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}

//add item into combobox
void CAgoraCrossChannelDlg::OnBnClickedButtonAddCrossChannel()
{
	CString strChannel;
	CString strUID;
	CString strToken;
	m_edtCrossChannel.GetWindowText(strChannel);
	m_edtToken.GetWindowText(strToken);
	m_edtUserID.GetWindowText(strUID);

	if (strChannel.IsEmpty() || strUID.IsEmpty())
	{
		AfxMessageBox(_T("The channel and user ID cannot be empty"));
		return;
	}
	
	std::string szChannel = cs2utf8(strChannel);
	std::string szToken = cs2utf8(strToken);
	auto it = std::find_if(m_vecChannelMedias.begin(), m_vecChannelMedias.end(),
		[&](const ChannelMediaInfo& info) {
			return info.channelName == szChannel && info.token == szToken;
		});

	if (it == m_vecChannelMedias.end()) {
		ChannelMediaInfo mediaInfo;
		mediaInfo.channelName = new char[strChannel.GetLength() + 1];
		mediaInfo.token = new char[strToken.GetLength() + 1];
		mediaInfo.uid = _ttol(strUID);
		strcpy_s(const_cast<char*>(mediaInfo.channelName), strChannel.GetLength() + 1, szChannel.data());
		strcpy_s(const_cast<char*>(mediaInfo.token), strToken.GetLength() + 1, szToken.data());
		//add mediaInfo to vector.
		m_vecChannelMedias.push_back(mediaInfo);
		m_cmbCrossChannelList.AddString(strChannel);
		m_cmbCrossChannelList.SetCurSel(m_cmbCrossChannelList.GetCount() - 1);
	}
	else
	{
		AfxMessageBox(_T("already added for same channel and token"));
	}
	
}

//remove combobox item
void CAgoraCrossChannelDlg::OnBnClickedButtonRemoveCrossChannel2()
{
	int nSel = m_cmbCrossChannelList.GetCurSel();
	if (nSel < 0)return;
	CString strChannelName;
	m_cmbCrossChannelList.GetWindowText(strChannelName);
	std::string szChannelName = cs2utf8(strChannelName);

	auto it = std::find_if(m_vecChannelMedias.begin(), m_vecChannelMedias.end(),
		[&szChannelName](const ChannelMediaInfo& info) {
			return info.channelName == szChannelName;
		});

	if (it != m_vecChannelMedias.end()) {
		delete it->channelName;
		delete it->token;
		m_vecChannelMedias.erase(it);
	}
	m_cmbCrossChannelList.DeleteString(nSel);
	m_cmbCrossChannelList.SetCurSel(m_cmbCrossChannelList.GetCount() - 1);
	
}

//start media relay or stop media relay
void CAgoraCrossChannelDlg::OnBnClickedButtonStartMediaRelay()
{
	if (!m_startMediaRelay)
	{
		int nDestCount = m_vecChannelMedias.size();
		if (nDestCount<=0)
		{
			AfxMessageBox(_T("please config target channel config first"));
			return;
		}
		ChannelMediaInfo  *lpDestInfos = new ChannelMediaInfo[nDestCount];
		for (int nIndex = 0; nIndex < nDestCount; nIndex++) {
			lpDestInfos[nIndex].channelName = m_vecChannelMedias[nIndex].channelName;
			lpDestInfos[nIndex].token = m_vecChannelMedias[nIndex].token;
			lpDestInfos[nIndex].uid = m_vecChannelMedias[nIndex].uid;
		}
		ChannelMediaRelayConfiguration cmrc;
		cmrc.srcInfo = m_srcInfo;
		cmrc.destInfos = lpDestInfos;
		cmrc.destCount = nDestCount;
		int ret = 0;
		//start Channel Media Relay from cmrc.
		ret = m_rtcEngine->startOrUpdateChannelMediaRelay(cmrc);
		m_lstInfo.AddString(_T("startOrUpdateChannelMediaRelay"));
		delete lpDestInfos;
		m_btnStartMediaRelay.SetWindowText(CrossChannelStopMediaRelay);
	}
	else {
		//stop Channel Media Relay.
		m_rtcEngine->stopChannelMediaRelay();
		m_lstInfo.AddString(_T("stopChannelMediaRelay"));
		m_btnStartMediaRelay.SetWindowText(CrossChannelStartMediaRelay);
	}
	m_startMediaRelay = !m_startMediaRelay;

}

//update update Channel Media Relay.
void CAgoraCrossChannelDlg::OnBnClickedButtonUpdate()
{
	if (m_startMediaRelay)
	{
		int nDestCount = m_vecChannelMedias.size();
		ChannelMediaInfo  *lpDestInfos = new ChannelMediaInfo[nDestCount];
		for (int nIndex = 0; nIndex < nDestCount; nIndex++) {
			lpDestInfos[nIndex].channelName = m_vecChannelMedias[nIndex].channelName;
			lpDestInfos[nIndex].token = m_vecChannelMedias[nIndex].token;
			lpDestInfos[nIndex].uid = m_vecChannelMedias[nIndex].uid;
		}
		ChannelMediaRelayConfiguration cmrc;
		cmrc.srcInfo = m_srcInfo;
		cmrc.destInfos = lpDestInfos;
		cmrc.destCount = nDestCount;
		int ret = 0;
		//update Channel Media Relay.
		ret = m_rtcEngine->startOrUpdateChannelMediaRelay(cmrc);
		m_lstInfo.AddString(_T("startOrUpdateChannelMediaRelay"));
		delete lpDestInfos;
	}
}

void CAgoraCrossChannelDlg::OnSelchangeListInfoBroadcasting()
{
	int sel = m_lstInfo.GetCurSel();
	if (sel < 0)return;
	CString strDetail;
	m_lstInfo.GetText(sel, strDetail);
	m_staDetails.SetWindowText(strDetail);
}


LRESULT CAgoraCrossChannelDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	m_joinChannel = true;
	m_btnJoinChannel.EnableWindow(TRUE);
	m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);
	CString strInfo;
	strInfo.Format(_T("%s:join success, uid=%u"), getCurrentTime(), wParam);
	m_srcInfo->uid = wParam;
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_localVideoWnd.SetUID(wParam);
	//notify parent window
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
	return TRUE;
}

LRESULT CAgoraCrossChannelDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
	m_joinChannel = false;
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);

	CString strInfo;
	strInfo.Format(_T("leave channel success %s"), getCurrentTime());
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), FALSE, 0);
	return TRUE;
}

LRESULT CAgoraCrossChannelDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	CString strInfo;
	strInfo.Format(_T("%u joined"), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return TRUE;
}

LRESULT CAgoraCrossChannelDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
	uid_t remoteUid = (uid_t)wParam;
	VideoCanvas canvas;
	canvas.uid = remoteUid;
	canvas.view = NULL;
	m_rtcEngine->setupRemoteVideo(canvas);
	CString strInfo;
	strInfo.Format(_T("%u offline, reason:%d"), remoteUid, lParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	return TRUE;
}

//media relay state changed handler
LRESULT CAgoraCrossChannelDlg::OnEIDChannelMediaRelayStateChanged(WPARAM wParam, LPARAM lParam)
{
	CHANNEL_MEDIA_RELAY_STATE state = (CHANNEL_MEDIA_RELAY_STATE)wParam;
	CHANNEL_MEDIA_RELAY_ERROR code = (CHANNEL_MEDIA_RELAY_ERROR)lParam;
	CString strInfo;
	strInfo.Format(_T("channel state:%d, code:%d"), state, code);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return TRUE;
}

// media relay event handler.
LRESULT CAgoraCrossChannelDlg::OnEIDChannelMediaRelayEvent(WPARAM wParam, LPARAM lParam)
{
	int evt = wParam;
	CString strInfo;
	strInfo.Format(_T("channel media event:%d"), evt);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return TRUE;
}


