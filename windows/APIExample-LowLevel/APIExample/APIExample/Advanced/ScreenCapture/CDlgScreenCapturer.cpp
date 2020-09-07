// CDlgScreenCapturer.cpp: 
//

#include "pch.h"
#include "APIExample.h"
#include "CDlgScreenCapturer.h"
#include "afxdialogex.h"


// CDlgScreenCapturer

CMonitors::CMonitors()
{

}
CMonitors::~CMonitors()
{

}


BOOL CMonitors::MonitorFunc(HMONITOR hMonitor, HDC hDc, LPRECT lpRect, LPARAM lParam)
{
	CMonitors* pThis = (CMonitors*)lParam;

	MONITORINFOEX info;
	info.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(hMonitor, &info);

	DEVMODE devMode;
	devMode.dmSize = sizeof(DEVMODE);
	EnumDisplaySettings(info.szDevice, ENUM_CURRENT_SETTINGS, &devMode);

	DEVICE_SCALE_FACTOR scale;
	HRESULT hr = GetScaleFactorForMonitor(hMonitor, &scale);
	MonitorInformation monitorInfo;
	monitorInfo.monitorInfo = info;
	monitorInfo.hMonitor = hMonitor;
	/*if (info.rcMonitor.left < 0
		|| info.rcMonitor.top < 0) {
		monitorInfo.canShare = false;
	}*/
	// UINT dpiX = 0;
	// UINT dpiY = 0;
	// GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI ,&dpiX, &dpiY);
	pThis->m_vecMonitorInfos.push_back(monitorInfo);
	if (monitorInfo.canShare)
		pThis->m_vecEffectiveMonitorInfos.push_back(monitorInfo);
	return TRUE;
}

void CMonitors::Clear()
{
	m_vecMonitorInfos.clear();
	m_screenRegion = { 0,0,0,0 };
}

void CMonitors::EnumMonitor()
{
	Clear();
	EnumDisplayMonitors(NULL, NULL, (MONITORENUMPROC)MonitorFunc, (LPARAM)this);

	for (size_t i = 0; i < m_vecMonitorInfos.size(); i++) {
		MonitorInformation& monitorInfo = m_vecMonitorInfos[i];
		char szName[MAX_PATH] = { 0 };
		sprintf_s(szName, MAX_PATH, "Screen%d", i + 1);
		monitorInfo.monitorName = szName;
	}

	if (m_vecMonitorInfos.size() == 1) {
		m_screenRegion = m_vecMonitorInfos[0].monitorInfo.rcMonitor;
	}
	else {
		m_screenRegion.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
		m_screenRegion.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
		m_screenRegion.right = m_screenRegion.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
		m_screenRegion.bottom = m_screenRegion.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);
	}
}

agora::rtc::Rectangle CMonitors::GetScreenRect()
{
	return RectToRectangle(m_screenRegion);
}

agora::rtc::Rectangle CMonitors::RectToRectangle(RECT rc)
{
	agora::rtc::Rectangle agoraRect = { rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top };

	return agoraRect;
}

bool CMonitors::IsValid()
{
	int left = GetSystemMetrics(SM_XVIRTUALSCREEN);
	int top = GetSystemMetrics(SM_YVIRTUALSCREEN);

	if (left < 0 || top < 0) {
		return false;
	}

	return true;
}

CMonitors::MonitorInformation CMonitors::GetMonitorInformation(int index)
{
	return m_vecEffectiveMonitorInfos[index];
}

agora::rtc::Rectangle CMonitors::GetMonitorRectangle(int index)
{
	MonitorInformation& info = m_vecEffectiveMonitorInfos[index];
	RECT rc = info.monitorInfo.rcMonitor;

	agora::rtc::Rectangle rcAgora = { 0, 0, (rc.right - rc.left)*info.scale_num / info.scale_den, (rc.bottom - rc.top)*info.scale_num / info.scale_den };
	rcAgora.x = rc.left * info.scale_num / info.scale_den;
	rcAgora.y = rc.top * info.scale_num / info.scale_den;
	return rcAgora;
}

bool CMonitors::CheckMonitorValid(HMONITOR hMonitor)
{
	for (size_t i = 0; i < m_vecEffectiveMonitorInfos.size(); i++) {
		MonitorInformation info = m_vecEffectiveMonitorInfos[i];
		if (info.hMonitor == hMonitor) {
			return info.canShare;
		}
	}

	return false;
}

bool CMonitors::GetMonitorRectangle(HMONITOR hMonitor, agora::rtc::Rectangle& screenRegion)
{
	for (size_t i = 0; i < m_vecEffectiveMonitorInfos.size(); i++) {
		MonitorInformation info = m_vecEffectiveMonitorInfos[i];
		if (info.hMonitor == hMonitor) {
			screenRegion = RectToRectangle(info.monitorInfo.rcMonitor);
			return true;
		}
	}
	return false;
}

bool CMonitors::GetWindowRect(HWND hWnd, agora::rtc::Rectangle& regionRect)
{
	if (!hWnd)
		return false;

	HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	RECT rcWnd = { 0 };
	::GetWindowRect(hWnd, &rcWnd);
	int scale_den = 1;
	int scale_num = 1;
	for (size_t i = 0; i < m_vecEffectiveMonitorInfos.size(); i++) {
		if (hMonitor == m_vecEffectiveMonitorInfos[i].hMonitor) {
			scale_num = m_vecEffectiveMonitorInfos[i].scale_num;
			scale_den = m_vecEffectiveMonitorInfos[i].scale_den;
			break;
		}
	}
	agora::rtc::Rectangle rcScreen;
	/*GetMonitorRectangle(hMonitor, rcScreen);

	regionRect = RectToRectangle(rcWnd);
	regionRect.x = rcWnd.left - rcScreen.x;
	regionRect.y = rcWnd.top - rcScreen.y;*/
	return true;
}


IMPLEMENT_DYNAMIC(CDlgScreenCapturer, CDialogEx)

CDlgScreenCapturer::CDlgScreenCapturer(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SCREENCAPTURER, pParent)
{

}

CDlgScreenCapturer::~CDlgScreenCapturer()
{
}

void CDlgScreenCapturer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_videoArea);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannelName);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_COMBO_ROLE, m_cmbRole);
	DDX_Control(pDX, IDC_COMBO_SCREEN, m_cmbScreenCap);

}


BEGIN_MESSAGE_MAP(CDlgScreenCapturer, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CDlgScreenCapturer::OnBnClickedButtonJoinchannel)
	ON_MESSAGE(MULTI_CAMERA_CONNECTED, &CDlgScreenCapturer::OnEIDConnected)
	ON_MESSAGE(MULTI_CAMERA_DISCONNECTED, &CDlgScreenCapturer::OnEIDDisConnected)
	ON_MESSAGE(MULTI_CAMERA_SUBSCRIBE_VIDEO_TRACKS, &CDlgScreenCapturer::OnEIDSubscribeRemoteVideo)
	ON_WM_SHOWWINDOW()

END_MESSAGE_MAP()


// CDlgScreenCapturer


void CDlgScreenCapturer::OnBnClickedButtonJoinchannel()
{
	if (!m_bConnected) {
		JoinChannel();
	}
	else {
		LeaveChannel();
	}
}

BOOL CDlgScreenCapturer::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_cmbRole.InsertString(0, _T("主播"));
	m_cmbRole.InsertString(1, _T("观众"));
	m_cmbRole.SetCurSel(0);
	for (int i = 0; i < SCREEN_VIDEO_COUNT; ++i) {
		m_videoWnd[i].Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), &m_videoArea, IDC_SCREENCAPTURER_VIDEO);

	}
	
	RECT rcArea;
	m_videoArea.GetClientRect(&rcArea);
	m_videoWnd[0].MoveWindow(&rcArea);
	m_videoWnd[0].ShowWindow(SW_HIDE);
	m_videoWnd[0].ShowWindow(SW_SHOW);
	return TRUE;  // return TRUE unless you set the focus to a control
}

//Initialize the Agora SDK
bool CDlgScreenCapturer::InitAgora()
{
	// set service config
	LowlevelSdk::Config config;
	config.enable_audio_device = config.enable_audio_processor = true;
	config.has_video = true;
	//initialize low level api and agora service 
	sdk_ptr = std::make_unique<LowlevelSdk>(config);

	// initialize agora service
	if (!sdk_ptr->AgoraService()) {
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("Init Agora Service failed."));
		return FALSE;
	}
	m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("Init Agora Service ."));
	return true;
}
//UnInitialize the Agora SDK
void CDlgScreenCapturer::UnInitAgora()
{
	if (m_bConnected) {
		LeaveChannel();
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("Leave channel if still connected"));
	}

	// disable audio track
	if (m_localAudioTrack) {
		m_localAudioTrack->setEnabled(false);
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("disable audio"));
	}
	// reset connections
	if (m_connection)
		m_connection.reset();
	
	// camra tracks, video tracks, audio track will be reseted
	sdk_ptr.reset();
	m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("reset sdk_ptr"));
}


void CDlgScreenCapturer::JoinChannel()
{
	ConnectionConfig config;
	config.channelProfile = agora::CHANNEL_PROFILE_LIVE_BROADCASTING;
	CString channelName;
	m_edtChannelName.GetWindowText(channelName);
	m_btnJoinChannel.EnableWindow(FALSE);
	m_cmbRole.EnableWindow(FALSE);
	m_cmbScreenCap.EnableWindow(FALSE);
	m_btnJoinChannel.SetWindowText(_T("LeaveChannel"));
	if (m_cmbRole.GetCurSel() == 0) {//broadcaster
		// set user role broadcaster
		config.clientRoleType = agora::rtc::CLIENT_ROLE_BROADCASTER;
		// no subscribe audio
		config.subscribeAllAudio = false;
		// no subscribe video
		config.subscribeAllVideo = false;
		
	   // create connection
		m_connection = ConnectionWrapper::CreateConnection(sdk_ptr->AgoraService(), config);
		if (!m_connection) {
			m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("Failed to creating Agora connection!"));
			return;
		}
		
		//set msg receiver for IRtcConnectionObserver and ILocalUserObserver
		m_connection->SetMsgReceiver(m_hWnd);
		// set user role
		m_connection->GetLocalUser()->GetLocalUser()->setUserRole(config.clientRoleType);
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("setUserRole Broadcaster"));
		// connect( join channel)
		m_connection->Connect(appid.c_str(), cs2utf8(channelName).c_str(), nullptr);
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("Connect"));
			
		// create audio track
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("create audio tracks"));
		m_localAudioTrack = sdk_ptr->CreateAudioTrack();
		if (!m_localAudioTrack)
			return;
		// publish audio track
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("publish audio tracks"));
		m_connection->GetLocalUser()->PublishAudioTrack(m_localAudioTrack);

		//create screen capture
		m_localScreenCapture = sdk_ptr->CreateScreenCapture();
		if (m_cmbScreenCap.GetCurSel() < m_monitors.GetMonitorCount()) {
			agora::rtc::Rectangle rect = m_monitors.GetMonitorRectangle(m_cmbScreenCap.GetCurSel());
			m_localScreenCapture->initWithScreenRect(rect, rect);
		}
		else {
			agora::rtc::Rectangle rect;
			int wndIdnex = m_cmbScreenCap.GetCurSel() - m_monitors.GetMonitorCount();
			m_localScreenCapture->initWithWindowId(m_vecHwnds[wndIdnex], rect);
		}

		//create screen capture video track
		m_screenCaptureVideoTrack = sdk_ptr->CreateScreenTrack();
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("create screen track"));
		// enable video track
		m_screenCaptureVideoTrack->setEnabled(true);
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("enable screen track"));
		// create local renders
		m_localScreenCaptureRender = sdk_ptr->CreateWindowedRender(m_videoWnd[0]);
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("Create local render"));
		// add local renders
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("Add local render"));
		m_screenCaptureVideoTrack->addRenderer(m_localScreenCaptureRender);
		// publish video tracks
		m_connection->GetLocalUser()->PublishVideoTrack(m_screenCaptureVideoTrack);
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("publish video tracks"));
		
	}
	else {//Audience
		config.clientRoleType = agora::rtc::CLIENT_ROLE_AUDIENCE;
		// audience subscribe audio
		config.subscribeAllAudio = true;
		// audience subscribe video
		config.subscribeAllVideo = true;
		m_maxVideoCount = 6;
		// create connection
		m_connection = ConnectionWrapper::CreateConnection(sdk_ptr->AgoraService(), config);
		if (!m_connection) {
			m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("Failed to creating Agora connection!"));
			return;
		}
		//set user role
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("setUserRole audience"));
		m_connection->GetLocalUser()->GetLocalUser()->setUserRole(config.clientRoleType);
		m_connection->SetMsgReceiver(m_hWnd);
		//
		bool connected =
			m_connection->Connect(appid.c_str(), cs2utf8(channelName).c_str(), "123");
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("connect"));
		if (!connected) {// false just mean calling connect failed.
			m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("Failed to connect to Agora channel!"));
			return;
		}
		
	}
}

void CDlgScreenCapturer::LeaveChannel()
{
	m_btnJoinChannel.SetWindowText(_T("JoinChannel"));
	if (m_cmbRole.GetCurSel() == 0) {//主播
		
		m_connection->GetLocalUser()->UnpublishVideoTrack(m_screenCaptureVideoTrack);
		
		// disabkle audio track
		m_localAudioTrack->setEnabled(false);
	    // unpublish audio track
	    m_connection->GetLocalUser()->UnpublishAudioTrack(m_localAudioTrack);
		
		// remove local renders
		m_screenCaptureVideoTrack->removeRenderer(m_localScreenCaptureRender);
		// stop camera
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("remove local render"));
		m_screenCaptureVideoTrack->setEnabled(false);

		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("stop screenCapture"));

		m_videoWnd[0].Invalidate();
		// disconnect
		m_connection->Disconnect();
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("disconnect connection"));
		//reset connection
		m_connection.reset();
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("reset connection"));
	}
	else {//audience
		//disconnect
		m_connection->Disconnect();
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("disconnect"));
		//reset connection
		m_connection.reset();
	}
}

LRESULT CDlgScreenCapturer::OnEIDConnected(WPARAM wParam, LPARAM lParam)
{
	m_bConnected = true;
	m_btnJoinChannel.EnableWindow(TRUE);

	m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("connected"));
	if (m_cmbRole.GetCurSel() == 0) {//主播
		
	}
	else {//观众

	}
	return 0;
}

LRESULT CDlgScreenCapturer::OnEIDDisConnected(WPARAM wParam, LPARAM lParam)
{
	m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("disconnected"));
	m_bConnected = false;
	m_cmbScreenCap.EnableWindow(TRUE);
	m_btnJoinChannel.EnableWindow(TRUE);
	m_cmbRole.EnableWindow(TRUE);
	return 0;
}

LRESULT CDlgScreenCapturer::OnEIDSubscribeRemoteVideo(WPARAM wParam, LPARAM lParam)
{
	if (m_vecRemoteVideoTracks.size() > SCREEN_VIDEO_COUNT)
		return 0;

	agora::rtc::IRemoteVideoTrack* videoTrack = (agora::rtc::IRemoteVideoTrack*)lParam;
	agora::rtc::VideoTrackInfo trackInfo;
	videoTrack->getTrackInfo(trackInfo);
	char szVideoTrack[MAX_PATH] = { 0 };
	sprintf_s(szVideoTrack, MAX_PATH, "%u%u", trackInfo.ownerUid, trackInfo.trackId);

	if (m_mapUserVideoTrack.find(szVideoTrack) == m_mapUserVideoTrack.end()) {
		int index = m_mapUserVideoTrack.size();
		auto renderer = sdk_ptr->CreateWindowedRender(m_videoWnd[index].GetSafeHwnd());
		videoTrack->addRenderer(renderer);
		m_vecRemoteVideoTracks.push_back(videoTrack);
		m_vecRemoteRenders.push_back(renderer);
		m_mapUserVideoTrack.insert(std::make_pair(szVideoTrack, m_videoWnd[index].GetSafeHwnd()));

	}
	return 0;
}

void CDlgScreenCapturer::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (bShow) {
		InitMonitorInfos();
		ReFreshWnd();
	}
	else {
		ResumeStatus();
	}
}

void CDlgScreenCapturer::ResumeStatus()
{
	m_lstInfo.ResetContent();
	m_edtChannelName.SetWindowText(_T(""));
	m_cmbScreenCap.ResetContent();
}


/*
	enum window callback function.
*/
BOOL CALLBACK CDlgScreenCapturer::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{

    // Skip windows that are invisible, minimized, have no title, or are owned,
	// unless they have the app window style set.
	HWND owner = ::GetWindow(hwnd, GW_OWNER);
	LONG exstyle = ::GetWindowLong(hwnd, GWL_EXSTYLE);
	if (::IsIconic(hwnd) || /*!::IsWindowVisible(hwnd) ||*/  
		(owner && !(exstyle & WS_EX_APPWINDOW))) {
		return TRUE;
	}

	// Skip the Program Manager window and the Start button.
	const size_t kClassLength = 256;
	char class_name[kClassLength];
	const int class_name_length = GetClassNameA(hwnd, class_name, kClassLength);

	//    RTC_DCHECK(class_name_length)
	//        << "Error retrieving the application's class name";
	// Skip Program Manager window and the Start button. This is the same logic
	// that's used in Win32WindowPicker in libjingle. Consider filtering other
	// windows as well (e.g. toolbars).
	if (strcmp(class_name, "Progman") == 0 || strcmp(class_name, "Button") == 0)
		return TRUE;

	// Windows 8 introduced a "Modern App" identified by their class name being
	// either ApplicationFrameWindow or windows.UI.Core.coreWindow. The
	// associated windows cannot be captured, so we skip them.
	// http://crbug.com/526883.

	ULONGLONG dwConditionMask = 0;
	VER_SET_CONDITION(dwConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
	VER_SET_CONDITION(dwConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
	OSVERSIONINFOEX osx;
	ZeroMemory(&osx, sizeof(OSVERSIONINFOEX));
	osx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osx.dwMajorVersion = 6;
	osx.dwMinorVersion = 2;
	int ret = VerifyVersionInfo(&osx, VER_MAJORVERSION | VER_MINORVERSION, dwConditionMask);
	//win8
	if (ret &&
		(strcmp(class_name, "ApplicationFrameWindow") == 0 ||
			strcmp(class_name, "Windows.UI.Core.CoreWindow") == 0)) {
		return TRUE;
	}
	std::unordered_set<HWND> m_setHwnds;
	std::vector<HWND> *vecHwnds = reinterpret_cast<std::vector<HWND> *>(lParam);
	LONG lStyle = ::GetWindowLong(hwnd, GWL_STYLE);
	if ((lStyle&WS_VISIBLE) != 0 && (lStyle&(WS_POPUP | WS_SYSMENU)) != 0
		&& m_setHwnds.find(hwnd) == m_setHwnds.end()) {
		TCHAR		strName[255];
		::GetWindowText(hwnd, strName, 255);
		char class_name[100] = { 0 };
		GetClassNameA(hwnd, class_name, 99);
		if (strcmp(class_name, "EdgeUiInputTopWndClass") == 0
			|| strcmp(class_name, "Shell_TrayWnd") == 0
			|| strcmp(class_name, "DummyDWMListenerWindow") == 0
			|| strcmp(class_name, "WorkerW") == 0
			|| strcmp(class_name, "PopupRbWebDialog") == 0
			|| strcmp(class_name, "TXGuiFoundation") == 0 //kuwo advertisement
			|| _tcsclen(strName) == 0)
		{
			return TRUE;
		}
		m_setHwnds.insert(hwnd);
		vecHwnds->push_back(hwnd);
	}

	return TRUE;
}

// call RefreashWndInfo to refresh window list and to m_cmbScreenCap.
void CDlgScreenCapturer::ReFreshWnd()
{
	//refresh window info.
	m_vecHwnds.clear();
	::EnumWindows(&CDlgScreenCapturer::EnumWindowsProc, (LPARAM)&m_vecHwnds);

	//enumerate hwnd to add m_cmbScreenCap.
	int i = m_cmbScreenCap.GetCount();

	for (auto iter = m_vecHwnds.begin(); iter != m_vecHwnds.end(); ++iter) {
		TCHAR		strName[255];
		::GetWindowText(*iter, strName, 255);
		HWND windowid = *iter;
		char class_name[100] = { 0 };
		GetClassNameA(*iter, class_name, 99);
		m_cmbScreenCap.InsertString(i++, strName);
	}

	m_cmbScreenCap.SetCurSel(0);
}

void CDlgScreenCapturer::InitMonitorInfos()
{
	m_monitors.EnumMonitor();

	std::vector<CMonitors::MonitorInformation>  infos = m_monitors.GetMonitors();
	for (size_t i = 0; i < infos.size(); i++) {
		CString strInfo;
		strInfo.Format(_T("Screen%d"), i + 1);
		m_cmbScreenCap.InsertString(i, strInfo);
	}
	m_cmbScreenCap.SetCurSel(0);
}
