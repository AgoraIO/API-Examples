// CDlgBeauty.cpp: 
//

#include "stdafx.h"
#include "APIExample.h"
#include "CDlgBeauty.h"
#include "afxdialogex.h"

// CDlgBeauty 

IMPLEMENT_DYNAMIC(CDlgBeauty, CDialogEx)

CDlgBeauty::CDlgBeauty(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_BEAUTY, pParent)
{

}

CDlgBeauty::~CDlgBeauty()
{
}

void CDlgBeauty::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_CHANNELNAME, m_staChannel);
	DDX_Control(pDX, IDC_EDIT_CHANNELNAME, m_edtChannel);
	DDX_Control(pDX, IDC_BUTTON_JOINCHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_CHECK_BEAUTY_ENABLE, m_chkBeauty);
	DDX_Control(pDX, IDC_STATIC_BEAUTY_LIGHTENING_CONTRAST_LEVEL, m_staContrast);
	DDX_Control(pDX, IDC_STATIC_BEAUTY_REDNESS, m_staRedness);
	DDX_Control(pDX, IDC_STATIC_BEAUTY_SMOOTHNESS, m_staSmoothness);
	DDX_Control(pDX, IDC_STATIC_BEAUTY_LIGHTENING, m_staLightening);
	DDX_Control(pDX, IDC_CHECK_ENHANCE, m_chkEnhance);
	DDX_Control(pDX, IDC_SLIDER_STRENGTH, m_sldLength);
	DDX_Control(pDX, IDC_STATIC_SKIN_PROTECT, m_staSkin);
	DDX_Control(pDX, IDC_SLIDER_SKIN_PROTECT, m_sldSkin);
	DDX_Control(pDX, IDC_CHECK_VIDEO_DENOISE, m_chkVideoDenoise);
	DDX_Control(pDX, IDC_CHECK_VIDEO_DENOISE2, m_chkVirtual);
	DDX_Control(pDX, IDC_SLIDER_LIGHTENING, m_sdlLightening);
	DDX_Control(pDX, IDC_SLIDER_REDNESS, m_sldRedness);
	DDX_Control(pDX, IDC_SLIDER_SMOOTHNESS, m_sldSmoothness);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_COMBO_BEAUTE_LIGHTENING_CONTRAST_LEVEL, m_cmbContrast);
}


BEGIN_MESSAGE_MAP(CDlgBeauty, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CDlgBeauty::OnBnClickedButtonJoinchannel)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CDlgBeauty::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CDlgBeauty::OnEIDJoinChannelSuccess)
	ON_BN_CLICKED(IDC_CHECK_BEAUTY_ENABLE, &CDlgBeauty::OnBnClickedCheckBeautyEnable)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_REDNESS, &CDlgBeauty::OnNMCustomdrawSliderRedness)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER_LIGHTENING, &CDlgBeauty::OnThumbposchangingSliderLightening)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_SMOOTHNESS, &CDlgBeauty::OnNMCustomdrawSliderSmoothness)
	ON_CBN_SELCHANGE(IDC_COMBO_BEAUTE_LIGHTENING_CONTRAST_LEVEL, &CDlgBeauty::OnSelchangeComboBeauteLighteningContrastLevel)
	ON_BN_CLICKED(IDC_CHECK_ENHANCE, &CDlgBeauty::OnBnClickedCheckEnhance)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_STRENGTH, &CDlgBeauty::OnCustomdrawSliderStrength)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_SKIN_PROTECT, &CDlgBeauty::OnCustomdrawSliderSkinProtect)
	ON_BN_CLICKED(IDC_CHECK_VIDEO_DENOISE, &CDlgBeauty::OnBnClickedCheckVideoDenoise)
	ON_BN_CLICKED(IDC_CHECK_VIDEO_DENOISE2, &CDlgBeauty::OnBnClickedCheckVideoDenoise2)
END_MESSAGE_MAP()


// CDlgBeauty message hand;er


void CDlgBeauty::OnBnClickedButtonJoinchannel()
{
	if (!m_joinChannel) {
		CString strChannelName;
		m_edtChannel.GetWindowText(strChannelName);
		std::string szChannelId = cs2utf8(strChannelName);
		if (strChannelName.IsEmpty()) {
			AfxMessageBox(_T("Fill channel name first"));
			return;
		}
		CString strInfo;
		agora::rtc::Rectangle rect;
		auto params = agora::rtc::ScreenCaptureParameters(640, 360, 15, 800);

		RECT rc;
		::GetWindowRect(GetDesktopWindow()->GetSafeHwnd(), &rc);
		rect = { rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top };

		//setup local video in the engine to canvas.
		agora::rtc::VideoCanvas canvas;
		canvas.renderMode = media::base::RENDER_MODE_FIT;
		canvas.uid = 0;
		canvas.view = m_videoWnds[0].GetSafeHwnd();
		canvas.sourceType = VIDEO_SOURCE_TRANSCODED;
		canvas.mirrorMode = VIDEO_MIRROR_MODE_DISABLED;
		m_rtcEngine->setupLocalVideo(canvas);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setupLocalVideo"));
	
		//join channel
		agora::rtc::ChannelMediaOptions op;
		op.publishTrancodedVideoTrack = true;
		op.channelProfile = CHANNEL_PROFILE_LIVE_BROADCASTING;
		op.clientRoleType = agora::rtc::CLIENT_ROLE_BROADCASTER;
		if (0 == m_rtcEngine->joinChannel(APP_TOKEN, szChannelId.data(), 0, op)) {
			strInfo.Format(_T("join channel %s, use ChannelMediaOptions"), strChannelName);
			m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
		}

		m_btnJoinChannel.SetWindowText(commonCtrlLeaveChannel);
	}
	else {
		//leave channel
		m_rtcEngine->leaveChannel();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("leave channel"));
		//stop preview
		//m_rtcEngine->stopPreview();
		//m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stop preview"));
		//m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	}
	m_joinChannel = !m_joinChannel;
}

bool CDlgBeauty::InitAgora()
{
	//create Agora RTC engine
	m_rtcEngine = createAgoraRtcEngine();
	if (!m_rtcEngine) {
		m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("createAgoraRtcEngine failed"));
		return false;
	}
	m_eventHandler.SetMsgReceiver(m_hWnd);
	agora::rtc::RtcEngineContext context;
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

	ret = m_rtcEngine->loadExtensionProvider("libagora_segmentation_extension.dll");
	CString strInfo;
	strInfo.Format(_T("loadExtensionProvider: %d"), ret);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("libagora_segmentation_extension.dll"));
	ret = m_rtcEngine->enableExtension("agora_segmentation", "PortraitSegmentation", true);
	strInfo.Format(_T("enableExtension: %d"), ret);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("agora_segmentation"));
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("PortraitSegmentation"));
	
	ret = m_rtcEngine->enableExtension("agora", "beauty", true);
	strInfo.Format(_T("enableExtension: %d"), ret);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("agora"));
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("beauty"));
	//enable video in the engine.
	m_rtcEngine->enableVideo();
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("enable video"));

	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("live broadcasting"));
	//set client role in the engine to the CLIENT_ROLE_BROADCASTER.
	m_rtcEngine->setClientRole(agora::rtc::CLIENT_ROLE_BROADCASTER);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setClientRole broadcaster"));

	//start preview
	m_rtcEngine->startPreview();
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("start preview"));

	VideoCanvas canvas;
	canvas.renderMode = media::base::RENDER_MODE_FIT;
	canvas.uid = 0;
	canvas.view = m_videoWnds[0].GetSafeHwnd();
	//setup local video in the engine to the canvas. 
	m_rtcEngine->setupLocalVideo(canvas);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("render local video"));

	return true;
}
//UnInitialize the Agora SDK
void CDlgBeauty::UnInitAgora()
{
	if (m_rtcEngine) {
		if (m_joinChannel) {
			//leave channel primary camera
			m_joinChannel = !m_rtcEngine->leaveChannel();
			m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("leaveChannel"));
			//stop local video transcoder
			m_rtcEngine->stopLocalVideoTranscoder();
			m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("stop local video transcoder"));

			//stop screen capture
			m_rtcEngine->stopScreenCapture();
			m_lstInfo.InsertString(m_lstInfo.GetCount() - 1, _T("stop screen capture"));
			
			m_joinChannel = false;
			m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
		}

		//stop preview in the engine.
		m_rtcEngine->stopPreview();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stopPreview"));

		//release engine.
		m_rtcEngine->release(true);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release rtc engine"));
		m_rtcEngine = NULL;
	}
}


//Initialize the Ctrl Text.
void CDlgBeauty::InitCtrlText()
{
	m_cmbContrast.InsertString(0, L"CONTRAST_LOW");
	m_cmbContrast.InsertString(1, L"CONTRAST_NORMAL");
	m_cmbContrast.InsertString(2, L"CONTRAST_HIGH");
	m_cmbContrast.SetCurSel(0);
	m_staChannel.SetWindowText(commonCtrlChannel);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
}

// resume window status.
void CDlgBeauty::ResumeStatus()
{
	m_staVideoArea.ShowWindow(SW_SHOW);
	RECT rcArea;
	m_staVideoArea.GetClientRect(&rcArea);

	m_videoWnds[0].MoveWindow(&rcArea);

	m_videoWnds[0].ShowWindow(SW_SHOW);
	m_videoWnds[1].ShowWindow(SW_HIDE);

	m_lstInfo.ResetContent();
	
	m_btnJoinChannel.EnableWindow(TRUE);
	m_edtChannel.SetWindowText(_T(""));
	m_joinChannel = false;
	m_initialize = false;

	m_sldLength.SetRange(0, 100);
	m_sldLength.SetPos(50);
	m_sldSkin.SetRange(0, 100);
	m_sldSkin.SetPos(100);

	m_sdlLightening.SetRange(0, 100);
	m_sdlLightening.SetPos(0);
	m_sldRedness.SetRange(0, 100);
	m_sldRedness.SetPos(0);
	m_sldSmoothness.SetRange(0, 100);
	m_sldSmoothness.SetPos(0);
	
}


LRESULT CDlgBeauty::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	int cId = (int)wParam;

	CString strInfo;
	strInfo.Format(_T("join channel success, uid=%u, cId=%d"), wParam, cId);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}

//EID_LEAVE_CHANNEL message window handler.
LRESULT CDlgBeauty::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
	int cId = (int)wParam;

	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
	CString strInfo;
	strInfo.Format(_T("leave channel success"));
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}

BOOL CDlgBeauty::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	InitCtrlText();
	for (int i = 0; i < 2; ++i) {
		m_videoWnds[i].Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), &m_staVideoArea, i);
		//set window background color.
		m_videoWnds[i].SetFaceColor(RGB(0x58, 0x58, 0x58));
	}
	ResumeStatus();

	return TRUE;  // return TRUE unless you set the focus to a control
				 
}

void CDlgBeauty::SetBeauty()
{
	BeautyOptions option;
	option.rednessLevel = m_sldRedness.GetPos() / 100.0f;
	option.lighteningLevel = m_sdlLightening.GetPos() / 100.0f;
	option.smoothnessLevel = m_sldSmoothness.GetPos() / 100.0f;
	option.lighteningContrastLevel = (agora::rtc::BeautyOptions::LIGHTENING_CONTRAST_LEVEL)m_cmbContrast.GetCurSel();
	int ret = m_rtcEngine->setBeautyEffectOptions(m_chkBeauty.GetCheck() != 0, option);
	CString strInfo;
	strInfo.Format(_T("setBeautyEffectOptions: %d"), ret);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}

void CDlgBeauty::SetColorful()
{
	char szJson[512] = { 0 };
	sprintf_s(szJson, 512, "{\"enable\":%d, \"strength\":%f, \"skinProtect\": %f}",
		m_chkEnhance.GetCheck()!=0, m_sldLength.GetPos()/100.f, m_sldSkin.GetPos()/100.f );
	m_rtcEngine->setExtensionProperty("agora", "beauty", "color_enhance_option", szJson);
}

void CDlgBeauty::OnBnClickedCheckBeautyEnable()
{
	SetBeauty();
}


void CDlgBeauty::OnNMCustomdrawSliderRedness(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	
	*pResult = 0;
	SetBeauty();
}


void CDlgBeauty::OnThumbposchangingSliderLightening(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTRBTHUMBPOSCHANGING *pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);
	
	*pResult = 0;
	SetBeauty();
}


void CDlgBeauty::OnNMCustomdrawSliderSmoothness(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	SetBeauty();
	*pResult = 0;
}


void CDlgBeauty::OnSelchangeComboBeauteLighteningContrastLevel()
{
	SetBeauty();
}


void CDlgBeauty::OnBnClickedCheckEnhance()
{
	SetColorful();
}


void CDlgBeauty::OnCustomdrawSliderStrength(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	SetColorful();
	*pResult = 0;
}


void CDlgBeauty::OnCustomdrawSliderSkinProtect(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	SetColorful();
	*pResult = 0;
}


void CDlgBeauty::OnBnClickedCheckVideoDenoise()
{
	char szJson[512] = { 0 };
	sprintf_s(szJson, 512, "{\"enable\":%d, \"mode\":%d, \"level\": %d}",
		m_chkVideoDenoise.GetCheck() != 0, 1, 1);
	m_rtcEngine->setExtensionProperty("agora", "beauty", "video_denoiser_option", szJson);
}


void CDlgBeauty::OnBnClickedCheckVideoDenoise2()
{
	agora::rtc::VirtualBackgroundSource background;
	background.color = 0xFFFFFF;
	background.blur_degree = VirtualBackgroundSource::BLUR_DEGREE_HIGH;
	background.background_source_type = VirtualBackgroundSource::BACKGROUND_BLUR;
	int ret = m_rtcEngine->enableVirtualBackground(m_chkVirtual.GetCheck() != 0, background);
	CString strInfo;
	strInfo.Format(_T("enableVirtualBackground: %d"), ret);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	//m_rtcEngine->setExtensionProperty("video_segmentation_provider", "PortraitSegmentation", "configs", "{\"enable\":true,\"seg_params\":{\"matting_mode\":1},\"back_replace_params\":{\"type\":1,\"color\":16723281,\"source\":\"/sdcard/canoe_water_nature.jpg\", \"blur_degree\":2}}");
	//
}
