#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraRegionConnDlg.h"



IMPLEMENT_DYNAMIC(CAgoraRegionConnDlg, CDialogEx)

CAgoraRegionConnDlg::CAgoraRegionConnDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_REGIONAL_CONNECTION, pParent)
{

}

CAgoraRegionConnDlg::~CAgoraRegionConnDlg()
{
}

void CAgoraRegionConnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannel);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannel);
	DDX_Control(pDX, IDC_STATIC_AREA_CODE, m_staAreaCode);
	DDX_Control(pDX, IDC_COMBO_AREA_CODE, m_cmbAreaCode);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_STATIC_DETAIL, m_staDetails);
}

LRESULT CAgoraRegionConnDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	m_joinChannel = true;
	m_btnJoinChannel.EnableWindow(TRUE);
	m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);
	CString strInfo;
	strInfo.Format(_T("%s:join success, uid=%u"), getCurrentTime(), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_localVideoWnd.SetUID(wParam);
	//notify parent window
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), TRUE, 0);
	return TRUE;
}

LRESULT CAgoraRegionConnDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
	m_joinChannel = false;
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);

	CString strInfo;
	strInfo.Format(_T("leave channel success %s"), getCurrentTime());
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	::PostMessage(GetParent()->GetSafeHwnd(), WM_MSGID(EID_JOINCHANNEL_SUCCESS), FALSE, 0);
	return TRUE;
}

LRESULT CAgoraRegionConnDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	CString strInfo;
	strInfo.Format(_T("%u joined"), wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	VideoCanvas canvas;
	canvas.uid = wParam;
	canvas.view = m_remoteVideoWnd.GetSafeHwnd();
	canvas.renderMode = media::base::RENDER_MODE_FIT;
	//setup remote video in engine to the canvas.
	m_rtcEngine->setupRemoteVideo(canvas);
	m_remoteVideoWnd.SetUID(wParam);
	return TRUE;
}

LRESULT CAgoraRegionConnDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
	uid_t remoteUid = (uid_t)wParam;
	
	CString strInfo;
	strInfo.Format(_T("%u offline, reason:%d"), remoteUid, lParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	return TRUE;
}


BEGIN_MESSAGE_MAP(CAgoraRegionConnDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraRegionConnDlg::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraRegionConnDlg::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraRegionConnDlg::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraRegionConnDlg::OnEIDUserOffline)
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraRegionConnDlg::OnBnClickedButtonJoinchannel)
	ON_LBN_SELCHANGE(IDC_LIST_INFO_BROADCASTING, &CAgoraRegionConnDlg::OnSelchangeListInfoBroadcasting)
END_MESSAGE_MAP()


//Initialize the Ctrl Text.
void CAgoraRegionConnDlg::InitCtrlText()
{
	m_staChannel.SetWindowText(commonCtrlChannel);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	m_staAreaCode.SetWindowText(RegionConnCtrlAreaCode);
}


//Initialize the Agora SDK
bool CAgoraRegionConnDlg::InitAgora()
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
	CString area_code;
	m_cmbAreaCode.GetWindowText(area_code);

	//set area code 
	CString strInfo;
	context.areaCode = m_mapAreaCode[area_code];
	strInfo.Format(_T("areaCode: %u"), context.areaCode);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

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
void CAgoraRegionConnDlg::UnInitAgora()
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
		if (m_initialize) {
			m_rtcEngine->release(true);
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release rtc engine"));
		}
		m_rtcEngine = NULL;
	}
}

//render local video from SDK local capture.
void CAgoraRegionConnDlg::RenderLocalVideo()
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
void CAgoraRegionConnDlg::ResumeStatus()
{
	InitCtrlText();
	m_lstInfo.ResetContent();
	m_edtChannel.SetWindowText(_T(""));
	m_cmbAreaCode.SetCurSel(0);
	m_joinChannel = false;
	m_initialize = false;
}


void CAgoraRegionConnDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (bShow) {
		InitCtrlText();
		RenderLocalVideo();
	}
	else {
		ResumeStatus();
	}
}


BOOL CAgoraRegionConnDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CAgoraRegionConnDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_localVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);
	RECT rcArea;
	m_staVideoArea.GetClientRect(&rcArea);
	rcArea.right = rcArea.left + (rcArea.right - rcArea.left) / 2;
	m_localVideoWnd.MoveWindow(&rcArea);
	m_localVideoWnd.ShowWindow(SW_SHOW);

	m_remoteVideoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, ID_BASEWND_VIDEO + 100);
	RECT rcArea2;
	m_staVideoArea.GetClientRect(&rcArea2);
	rcArea2.left = rcArea2.left + (rcArea2.right - rcArea2.left) / 2;
	m_remoteVideoWnd.MoveWindow(&rcArea2);
	m_remoteVideoWnd.ShowWindow(SW_SHOW);

	int nIndex = 0;

	m_cmbAreaCode.InsertString(nIndex++, _T("AREA_CODE_GLOB"));
	m_cmbAreaCode.InsertString(nIndex++, _T("AREA_CODE_CN"));
	m_cmbAreaCode.InsertString(nIndex++, _T("AREA_CODE_NA"));
	m_cmbAreaCode.InsertString(nIndex++, _T("AREA_CODE_EUR"));
	m_cmbAreaCode.InsertString(nIndex++, _T("AREA_CODE_AS"));

	m_mapAreaCode.insert(std::make_pair(_T("AREA_CODE_CN"),AREA_CODE_CN));
	m_mapAreaCode.insert(std::make_pair(_T("AREA_CODE_NA"), AREA_CODE_NA));
	m_mapAreaCode.insert(std::make_pair(_T("AREA_CODE_EUR"), AREA_CODE_EU));
	m_mapAreaCode.insert(std::make_pair(_T("AREA_CODE_AS"), AREA_CODE_AS));
	m_mapAreaCode.insert(std::make_pair(_T("AREA_CODE_GLOB"), AREA_CODE_GLOB));

	m_cmbAreaCode.SetCurSel(0);
	ResumeStatus();
	return TRUE;
}


void CAgoraRegionConnDlg::OnBnClickedButtonJoinchannel()
{
	if (!m_initialize)
	{
		InitAgora();
		RenderLocalVideo();
	}
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


void CAgoraRegionConnDlg::OnSelchangeListInfoBroadcasting()
{
	int sel = m_lstInfo.GetCurSel();
	if (sel < 0)return;
	CString strDetail;
	m_lstInfo.GetText(sel, strDetail);
	m_staDetails.SetWindowText(strDetail);
}
