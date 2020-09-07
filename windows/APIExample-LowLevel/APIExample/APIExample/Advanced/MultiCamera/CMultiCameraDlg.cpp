// CMultiCameraDlg.cpp: 
//

#include "pch.h"
#include "APIExample.h"
#include "CMultiCameraDlg.h"
#include "afxdialogex.h"


// CMultiCameraDlg 对话框

IMPLEMENT_DYNAMIC(CMultiCameraDlg, CDialogEx)

CMultiCameraDlg::CMultiCameraDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MULTICAMERA, pParent)
{

}

CMultiCameraDlg::~CMultiCameraDlg()
{
}

void CMultiCameraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CAMERAS, m_cmbCameras);
	DDX_Control(pDX, IDC_COMBO_ROLE, m_cmbRole);
	DDX_Control(pDX, IDC_COMBO_Resolution, m_cmbResolution);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannelName);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_videoArea);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
}


BEGIN_MESSAGE_MAP(CMultiCameraDlg, CDialogEx)
    ON_CBN_SELCHANGE(IDC_COMBO_ROLE, &CMultiCameraDlg::OnSelchangeComboRole)
    ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CMultiCameraDlg::OnBnClickedButtonJoinchannel)
    ON_CBN_SELCHANGE(IDC_COMBO_CAMERAS, &CMultiCameraDlg::OnSelchangeComboCameras)
    ON_CBN_SELCHANGE(IDC_COMBO_Resolution, &CMultiCameraDlg::OnSelchangeComboResolution)
    ON_MESSAGE(MULTI_CAMERA_SUBSCRIBE_VIDEO_TRACKS, &CMultiCameraDlg::OnEIDSubscribeRemoteVideo)
    ON_MESSAGE(MULTI_CAMERA_CONNECTED, &CMultiCameraDlg::OnEIDConnected)
    ON_MESSAGE(MULTI_CAMERA_DISCONNECTED, &CMultiCameraDlg::OnEIDDisConnected)
    
END_MESSAGE_MAP()


// CMultiCameraDlg


BOOL CMultiCameraDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    // initialize render wnd
    for (int i = 0; i < MAX_VIDEO_COUNT; ++i) {
        m_videoWnds[i].Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, IDC_MULTICALERAM_VIDEO + i);
        //set window background color.
        m_videoWnds[i].SetFaceColor(RGB(0x58, 0x58, 0x58));
        m_videoWnds[i].ShowWindow(SW_HIDE);
    }
    m_cmbRole.InsertString(0, _T("主播"));
    m_cmbRole.InsertString(1, _T("观众"));
    m_cmbRole.SetCurSel(0);
   
    return TRUE;  // return TRUE unless you set the focus to a control
}

void CMultiCameraDlg::OnSelchangeComboRole()
{
  
}

void CMultiCameraDlg::JoinChannel()
{
    ConnectionConfig config;
    config.channelProfile = agora::CHANNEL_PROFILE_LIVE_BROADCASTING;
    CString channelName;
    m_edtChannelName.GetWindowText(channelName);
    m_btnJoinChannel.EnableWindow(FALSE);
    m_cmbRole.EnableWindow(FALSE);
    m_btnJoinChannel.SetWindowText(_T("LeaveChannel"));
    if (m_cmbRole.GetCurSel() == 0) {//broadcaster
		// create multi camera tracks
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("create camera video tracks"));
		sdk_ptr->CreateCameraTracks();
		// set user role broadcaster
        config.clientRoleType = agora::rtc::CLIENT_ROLE_BROADCASTER;
		// no subscribe audio
        config.subscribeAllAudio = false;
		// no subscribe video
        config.subscribeAllVideo = false;
		// Get camera tracks
        m_vecCameraVideoTracks = sdk_ptr->GetCameraTracks();

        for (int i = 0; i < m_vecCameraVideoTracks.size(); ++i) {
			// start camera
			CString strCamera;
			strCamera.Format(_T("start camera %d"), i);
            m_vecCameraVideoTracks[i]->setEnabled(true);
			m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, strCamera);
			// create connection
            auto connection = ConnectionWrapper::CreateConnection(sdk_ptr->AgoraService(), config);

			if (!connection) {
				m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("Failed to creating Agora connection!"));
				return;
			}

			strCamera.Format(_T("start connection %d"), i);
			m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, strCamera);
			//set msg receiver for IRtcConnectionObserver and ILocalUserObserver
            connection->SetMsgReceiver(m_hWnd);
            
			// set user role
            connection->GetLocalUser()->GetLocalUser()->setUserRole(config.clientRoleType);
			m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("setUserRole Broadcaster"));
			// connect( join channel)
            connection->Connect(appid.c_str(), cs2utf8(channelName).c_str(), nullptr);
			m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("Connect"));
			// create local renders
            auto renderer = sdk_ptr->CreateWindowedRender(m_videoWnds[i]);
			m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("Create local render"));
			// add local renders
			m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("Add local render"));
            m_vecCameraVideoTracks[i]->addRenderer(renderer);
            m_vecLocalRenders.push_back(renderer);
			// publish video tracks
			m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("publish video tracks"));
            connection->GetLocalUser()->PublishVideoTrack(m_vecCameraVideoTracks[i]);
            m_vecConnections.push_back(connection);
        }
		// create audio track
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("create audio tracks"));
        m_localAudioTrack = sdk_ptr->CreateAudioTrack();
        if (!m_localAudioTrack)
            return;
		// publish audio track
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("publish audio tracks"));
        m_vecConnections[0]->GetLocalUser()->PublishAudioTrack(m_localAudioTrack);
        m_maxVideoCount = m_vecCameraVideoTracks.size();
    }
    else {//Audience
        config.clientRoleType = agora::rtc::CLIENT_ROLE_AUDIENCE;
		// audience subscribe audio
        config.subscribeAllAudio = true;
		// audience subscribe video
        config.subscribeAllVideo = true;
        m_maxVideoCount = 6;
		// create connection
        auto connection = ConnectionWrapper::CreateConnection(sdk_ptr->AgoraService(), config);
        if (!connection) {
			m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("Failed to creating Agora connection!"));
            return;
        }
		//set user role
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("setUserRole audience"));
        connection->GetLocalUser()->GetLocalUser()->setUserRole(config.clientRoleType);
        connection->SetMsgReceiver(m_hWnd);
       //
        bool connected =
            connection->Connect(appid.c_str(), cs2utf8(channelName).c_str(), "123");
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("connect"));
        if (!connected) {// false just mean calling connect failed.
			m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("Failed to connect to Agora channel!"));
            return;
        }
        m_vecConnections.push_back(connection);
    }
    ShowVideoWnds();
}

void CMultiCameraDlg::LeaveChannel()
{
    m_btnJoinChannel.SetWindowText(_T("JoinChannel"));
    if (m_cmbRole.GetCurSel() == 0) {//主播
        m_vecCameraVideoTracks = sdk_ptr->GetCameraTracks();

        for (int i = 0; i < m_vecCameraVideoTracks.size(); ++i) {
			// un oublish video track
            m_vecConnections[i]->GetLocalUser()->UnpublishVideoTrack(m_vecCameraVideoTracks[i]);
            if (i == 0) {
				// disabkle audio track
				m_localAudioTrack->setEnabled(false);
				// unpublish audio track
                m_vecConnections[i]->GetLocalUser()->UnpublishAudioTrack(m_localAudioTrack);
            }
			CString strInfo;
			strInfo.Format(_T("remove local render%d"), i);
			// remove local renders
            m_vecCameraVideoTracks[i]->removeRenderer(m_vecLocalRenders[i]);
			// stop camera
			m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, strInfo);

            m_vecCameraVideoTracks[i]->setEnabled(false);
			strInfo.Format(_T("stop camera %d"), i);
			m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, strInfo);

            m_videoWnds[i].Invalidate();
			// disconnect
			m_vecConnections[i]->Disconnect();

			strInfo.Format(_T("disconnect connection %d"), i);
			m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, strInfo);
			//reset connection
			m_vecConnections[i].reset();
        }
		sdk_ptr->DestroyCameraTracks();
        m_vecCameraVideoTracks.clear();
        m_vecLocalRenders.clear();
        m_vecConnections.clear();
    }
    else {//audience
		//disconnect
        m_vecConnections[0]->Disconnect();
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("disconnect"));
		//reset connection
		m_vecConnections[0].reset();
    }
}

void CMultiCameraDlg::OnBnClickedButtonJoinchannel()
{
    if (!m_bConnected) {
        JoinChannel();
    }
    else {
        LeaveChannel();
    }
}


void CMultiCameraDlg::OnSelchangeComboCameras()
{
    int sel = m_cmbCameras.GetCurSel();
    m_cmbResolution.ResetContent();
    
    std::vector<agora::rtc::VideoFormat> vecFormats = m_vecCameraInfos[sel].videoFormats;
    agora::rtc::VideoFormat curCameraVideoFormat = sdk_ptr->GetCameraVideoFormat(sel);
    int format_sel = 0;
	// Select resolution fps for cameras to show
    for (int i = 0; i < vecFormats.size(); ++i) {
        agora::rtc::VideoFormat format = vecFormats[i];
        CString strVideoFormat;
        strVideoFormat.Format(_T("%dx%d %dfps"), format.width, format.height, format.fps);
        m_cmbResolution.InsertString(i, strVideoFormat);
        if (curCameraVideoFormat.width == format.width
            && curCameraVideoFormat.height == format.height
            && curCameraVideoFormat.fps == format.fps) {
            format_sel = i;
        }
    }
	
    m_cmbResolution.SetCurSel(format_sel);
}


void CMultiCameraDlg::OnSelchangeComboResolution()
{
    int camera_sel = m_cmbCameras.GetCurSel();
    int format_sel = m_cmbResolution.GetCurSel();
    // set capture format and set video encoder configuration 
    sdk_ptr->setCameraConfiguration(camera_sel, format_sel);
}


void CMultiCameraDlg::ShowVideoWnds()
{
    m_videoArea.ShowWindow(SW_HIDE);
    int row = 2;
    int col = 2;

    if (m_maxVideoCount == 1) {
        row = 1;
        col = 1;
    }else  if (m_maxVideoCount == 2) {
        row = 1;
        col = 2;
    }
    else  if (m_maxVideoCount > 2 && m_maxVideoCount<=4) {
        row = 2;
        col = 2;
    }
    else  if (m_maxVideoCount > 4 && m_maxVideoCount <= 6) {
        row = 2;
        col = 3;
    }
    else  if (m_maxVideoCount > 6 && m_maxVideoCount <= 9) {
        row = 3;
        col = 3;
    }
    else  if (m_maxVideoCount > 9 && m_maxVideoCount <= 12) {
        row = 3;
        col = 4;
    }
    else  if (m_maxVideoCount > 13 && m_maxVideoCount <= 16) {
        row = 4;
        col = 4;
    }
  
    RECT rcArea;
    m_videoArea.GetClientRect(&rcArea);
    int space = 1;

    int w = (rcArea.right - rcArea.left - space * (col - 1)) / col;
    int h = (rcArea.bottom - rcArea.top - space * (row - 1)) / row;

    for (int r = 0; r < row; r++) {
        for (int c = 0; c < col; c++) {
            int x = rcArea.left + (w + space) * c;
            int y = rcArea.top + (h + space) * r;
            int nIndex = r * col + c;
            m_videoWnds[nIndex].MoveWindow(x, y, w, h, TRUE);
            m_videoWnds[nIndex].ShowWindow(SW_SHOW);
            m_videoWnds[nIndex].SetParent(this);

            if (!m_videoWnds[nIndex].IsWindowVisible()) {
                m_videoWnds[nIndex].ShowWindow(SW_SHOW);
            }
        }
    }

    for (int i = row * col; i < MAX_VIDEO_COUNT; ++i) {
        m_videoWnds[i].ShowWindow(SW_HIDE);
    }
}

LRESULT CMultiCameraDlg::OnEIDConnected(WPARAM wParam, LPARAM lParam)
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

LRESULT CMultiCameraDlg::OnEIDDisConnected(WPARAM wParam, LPARAM lParam)
{
	m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("disconnected"));
    m_bConnected = false;
    m_btnJoinChannel.EnableWindow(TRUE);
    return 0;
}

LRESULT CMultiCameraDlg::OnEIDSubscribeRemoteVideo(WPARAM wParam, LPARAM lParam)
{
	agora::rtc::IRemoteVideoTrack* videoTrack = (agora::rtc::IRemoteVideoTrack*)lParam;
	agora::rtc::VideoTrackInfo trackInfo;
	videoTrack->getTrackInfo(trackInfo);
	char szVideoTrack[MAX_PATH] = { 0 };
	sprintf_s(szVideoTrack, MAX_PATH, "%u%u", trackInfo.ownerUid, trackInfo.trackId);

	if (m_mapUserVideoTrack.find(szVideoTrack) == m_mapUserVideoTrack.end()) {
		int index = m_mapUserVideoTrack.size();
		auto renderer = sdk_ptr->CreateWindowedRender(m_videoWnds[index].GetSafeHwnd());
		videoTrack->addRenderer(renderer);
		m_vecRemoteVideoTracks.push_back(videoTrack);
		m_vecRemoteRenders.push_back(renderer);
		m_mapUserVideoTrack.insert(std::make_pair(szVideoTrack, m_videoWnds[index].GetSafeHwnd()));

	}
    return 0;
}
//Initialize the Agora SDK
bool CMultiCameraDlg::InitAgora()
{
    // set service config
    LowlevelSdk::Config config;
    config.enable_audio_device = config.enable_audio_processor = true;
    config.has_video = true;
    //initialize low level api and agora service 
    sdk_ptr = std::make_unique<LowlevelSdk>(config);

    if (!sdk_ptr->AgoraService()) {
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("Init Agora Service failed."));
        return FALSE;
    }
	m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("Init Agora Service ."));
    //initliaze all available cameras
    if (!sdk_ptr->InitCameras()) {
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("Init Cameras failed."));
        return FALSE;
    }
	m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("Init Cameras ."));
    // show cameras information
    auto cameraInfos = sdk_ptr->GetCameraInfos();
	CString strCameras;
	strCameras.Format(_T("Get cameras inforamation, count:%d"), cameraInfos.size());
	m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, strCameras);
    if (cameraInfos.size() > 0) {
        auto device_info = cameraInfos[0];
        for (int i = 0; i < device_info->NumberOfDevices(); ++i) {
            char deviceId[MAX_DEVICE_ID_LEN] = { 0 };
            char deviceName[MAX_DEVICE_ID_LEN] = { 0 };
			// Get camera information
            device_info->GetDeviceName(i, deviceName, MAX_DEVICE_ID_LEN, deviceId, MAX_DEVICE_ID_LEN);
            CAMERAINFO cameraInfo;
            cameraInfo.deviceId = deviceId;
            cameraInfo.deviceName = deviceName;
            int count = device_info->NumberOfCapabilities(deviceId);
			// Get video format for camers
            for (int j = 0; j < count; ++j) {
                agora::rtc::VideoFormat format;
                device_info->GetCapability(deviceId, j, format);
                cameraInfo.videoFormats.push_back(format);
            }
            m_vecCameraInfos.push_back(cameraInfo);
            m_cmbCameras.InsertString(i, utf82cs(deviceName));
        }

        std::vector<agora::rtc::VideoFormat> videoFormats = m_vecCameraInfos[0].videoFormats;
		//show resolution and fps
        for (int i = 0; i < videoFormats.size(); ++i) {
            agora::rtc::VideoFormat format = videoFormats[i];
            CString strVideoFormat;
            strVideoFormat.Format(_T("%dx%d %dfps"), format.width, format.height, format.fps);
            m_cmbResolution.InsertString(i, strVideoFormat);
        }
        m_cmbCameras.SetCurSel(0);
        m_cmbResolution.SetCurSel(0);
    }

	return true;
}
//UnInitialize the Agora SDK
void CMultiCameraDlg::UnInitAgora()
{
    if (m_bConnected)
        LeaveChannel();
	// disable audio track
	if (m_localAudioTrack) {
		m_localAudioTrack->setEnabled(false);
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("disanle local audio"));
	}
    // reset connections
	for (int i = 0; i < m_vecConnections.size(); ++i) {
		m_vecConnections[i].reset();
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("reset connection"));
	}
	// clear cameras information
    m_vecCameraInfos.clear();
	m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("clear cameras information"));
	// camra tracks, video tracks, audio track will be reseted
    sdk_ptr.reset();
	m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("reset  "));
    m_cmbCameras.ResetContent();
    m_cmbResolution.ResetContent();
}