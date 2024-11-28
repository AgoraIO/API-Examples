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
	mBeautyDlgEx = new CDlgBeautyEx();
}

CDlgBeauty::~CDlgBeauty()
{
	if (mBeautyDlgEx)
	{
		delete mBeautyDlgEx;
		mBeautyDlgEx = nullptr;
	}
		
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
	DDX_Control(pDX, IDC_CHECK_LOWLIGHT, m_chkLowlight);
	DDX_Control(pDX, IDC_SLIDER_LIGHTENING, m_sdlLightening);
	DDX_Control(pDX, IDC_SLIDER_REDNESS, m_sldRedness);
	DDX_Control(pDX, IDC_SLIDER_SMOOTHNESS, m_sldSmoothness);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_COMBO_BEAUTE_LIGHTENING_CONTRAST_LEVEL, m_cmbContrast);
	DDX_Control(pDX, IDC_RADIO_VIRTUAL_BG_BLUR, m_radioVirtualBgBlur);
	DDX_Control(pDX, IDC_RADIO_VIRTUAL_BG_COLOR, m_radioVirtualBgColor);
	DDX_Control(pDX, IDC_RADIO_VIRTUAL_BG_IMAGE, m_radioVirtualBgImage);
	DDX_Control(pDX, IDC_RADIO_VIRTUAL_BG_VIDEO, m_radioVirtualBgVideo);
	DDX_Control(pDX, IDC_SLIDER_BRIHTNESS, mSliderBright);
	DDX_Control(pDX, IDC_CHECK_EXTENTION, mCkExtention);
}


BEGIN_MESSAGE_MAP(CDlgBeauty, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CDlgBeauty::OnBnClickedButtonJoinchannel)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CDlgBeauty::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CDlgBeauty::OnEIDJoinChannelSuccess)
	ON_BN_CLICKED(IDC_CHECK_BEAUTY_ENABLE, &CDlgBeauty::OnBnClickedCheckBeautyEnable)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_REDNESS, &CDlgBeauty::OnNMCustomdrawSliderRedness)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_LIGHTENING, &CDlgBeauty::OnThumbposchangingSliderLightening)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_SMOOTHNESS, &CDlgBeauty::OnNMCustomdrawSliderSmoothness)
	ON_CBN_SELCHANGE(IDC_COMBO_BEAUTE_LIGHTENING_CONTRAST_LEVEL, &CDlgBeauty::OnSelchangeComboBeauteLighteningContrastLevel)
	ON_BN_CLICKED(IDC_CHECK_ENHANCE, &CDlgBeauty::OnBnClickedCheckEnhance)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_STRENGTH, &CDlgBeauty::OnCustomdrawSliderStrength)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_SKIN_PROTECT, &CDlgBeauty::OnCustomdrawSliderSkinProtect)
	ON_BN_CLICKED(IDC_CHECK_VIDEO_DENOISE, &CDlgBeauty::OnBnClickedCheckVideoDenoise)
	ON_BN_CLICKED(IDC_CHECK_VIDEO_DENOISE2, &CDlgBeauty::OnBnClickedCheckVideoDenoise2)
	ON_BN_CLICKED(IDC_CHECK_LOWLIGHT, &CDlgBeauty::OnBnClickedCheckLowlight)
	ON_WM_SHOWWINDOW()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_VIRTUAL_BG_BLUR, IDC_RADIO_VIRTUAL_BG_IMAGE, &CDlgBeauty::OnBnClickedStaticVirtualBgChoose)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_BRIHTNESS, &CDlgBeauty::OnNMCustomdrawSliderBrihtness)
	ON_BN_CLICKED(IDC_CHECK_EXTENTION, &CDlgBeauty::OnBnClickedCheckExtention)
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
		op.publishCameraTrack = true;
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

void CDlgBeauty::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow) {
		//init control text.
		InitCtrlText();
		mBeautyDlgEx->InitCtrlText();
	}
	else {
		//resume window status.
		ResumeStatus();
		if (mBeautyDlgEx)
		{
			mCkExtention.SetCheck(false);
			mBeautyDlgEx->ShowWindow(SW_HIDE);
		}
	}
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


	CString strInfo;
	ret = m_rtcEngine->enableExtension("agora_video_filters_clear_vision", "clear_vision", true);
	strInfo.Format(_T("enableExtension: %d"), ret);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("agora_video_filters_clear_vision"));
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("clear_vision"));
	//enable video in the engine.
	m_rtcEngine->enableVideo();
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("enable video"));

	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("live broadcasting"));
	//set client role in the engine to the CLIENT_ROLE_BROADCASTER.
	m_rtcEngine->setClientRole(agora::rtc::CLIENT_ROLE_BROADCASTER);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setClientRole broadcaster"));

	//start preview
	CameraCapturerConfiguration config;
	
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
		if (m_initialize) {
			m_rtcEngine->release(true);
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release rtc engine"));
		}
		
		m_rtcEngine = NULL;
	}
}


//Initialize the Ctrl Text.
void CDlgBeauty::InitCtrlText()
{
	m_cmbContrast.ResetContent();
	m_cmbContrast.InsertString(0, L"CONTRAST_LOW");
	m_cmbContrast.InsertString(1, L"CONTRAST_NORMAL");
	m_cmbContrast.InsertString(2, L"CONTRAST_HIGH");
	m_cmbContrast.SetCurSel(0);
	m_staChannel.SetWindowText(commonCtrlChannel);
	m_btnJoinChannel.SetWindowText(commonCtrlJoinChannel);
}

void CDlgBeauty::UpdateExtentCbState(bool isCheck)
{
	mCkExtention.SetCheck(isCheck);
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

	mSliderBright.SetRange(0, 100);
	
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
	mBeautyDlgEx->Create(CDlgBeautyEx::IDD);
	mBeautyDlgEx->initData(&m_initialize, &m_rtcEngine,this);
	mBeautyDlgEx->InitCtrlText();
	InitCtrlText();
	for (int i = 0; i < 2; ++i) {
		m_videoWnds[i].Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), &m_staVideoArea, i);
		//set window background color.
		m_videoWnds[i].SetFaceColor(RGB(0x58, 0x58, 0x58));
	}
	CString strPath = GetExePath() + _T("\\agora.png");
	m_imgPath = cs2utf8(strPath);
	ResumeStatus();

	return TRUE;  // return TRUE unless you set the focus to a control
				 
}

void CDlgBeauty::SetBeauty()
{
	if (!m_rtcEngine || !m_initialize)
		return;
	BeautyOptions option;
	option.rednessLevel = m_sldRedness.GetPos() / 100.0f;
	option.lighteningLevel = m_sdlLightening.GetPos() / 100.0f;
	option.smoothnessLevel = m_sldSmoothness.GetPos() / 100.0f;
	option.lighteningContrastLevel = (agora::rtc::BeautyOptions::LIGHTENING_CONTRAST_LEVEL)m_cmbContrast.GetCurSel();
	int ret = m_rtcEngine->setBeautyEffectOptions(m_chkBeauty.GetCheck() != 0, option);
	if (ret < 0) {
		CString strInfo;
		strInfo.Format(_T("setBeautyEffectOptions: %d"), ret);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	}

	FilterEffectOptions optionFilter;
	optionFilter.path = "built_in_whiten_filter";
	optionFilter.strength = mSliderBright.GetPos() / 100.0f;
	ret = m_rtcEngine->setFilterEffectOptions(m_chkBeauty.GetCheck() != 0, optionFilter);
	if (ret < 0) {
		CString strInfo;
		strInfo.Format(_T("setFilterEffectOptions: %d"), ret);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	}
}

void CDlgBeauty::SetColorful()
{
	if (!m_rtcEngine || !m_initialize)
		return;
	char szJson[512] = { 0 };
	agora::rtc::ColorEnhanceOptions options;
	options.skinProtectLevel = m_sldSkin.GetPos() / 100.f;
	options.strengthLevel = m_sldLength.GetPos() / 100.f;
	sprintf_s(szJson, 512, "{\"enable\":%d, \"strength\":%f, \"skinProtect\": %f}",
		m_chkEnhance.GetCheck()!=0, m_sldLength.GetPos()/100.f, m_sldSkin.GetPos()/100.f );
	m_rtcEngine->setColorEnhanceOptions(m_chkEnhance.GetCheck() != 0, options);
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
	VideoDenoiserOptions options;
	options.level = VideoDenoiserOptions::VIDEO_DENOISER_LEVEL_FAST;
	options.mode = VideoDenoiserOptions::VIDEO_DENOISER_MANUAL;
	m_rtcEngine->setVideoDenoiserOptions(m_chkVideoDenoise.GetCheck() != 0, options);
}


void CDlgBeauty::OnBnClickedCheckVideoDenoise2()
{
	agora::rtc::SegmentationProperty property;
	agora::rtc::VirtualBackgroundSource background;
	background.color = 0xFFFFFF;
	background.blur_degree = VirtualBackgroundSource::BLUR_DEGREE_HIGH;
	background.background_source_type = VirtualBackgroundSource::BACKGROUND_BLUR;
	int ret = m_rtcEngine->enableVirtualBackground(m_chkVirtual.GetCheck() != 0, background, property, agora::media::PRIMARY_CAMERA_SOURCE);
	CString strInfo;
	strInfo.Format(_T("enableVirtualBackground: %d"), ret);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_radioVirtualBgBlur.SetCheck(TRUE);
	m_radioVirtualBgColor.SetCheck(FALSE);
	m_radioVirtualBgImage.SetCheck(FALSE);
	m_radioVirtualBgVideo.SetCheck(FALSE);
	//m_rtcEngine->setExtensionProperty("video_segmentation_provider", "PortraitSegmentation", "configs", "{\"enable\":true,\"seg_params\":{\"matting_mode\":1},\"back_replace_params\":{\"type\":1,\"color\":16723281,\"source\":\"/sdcard/canoe_water_nature.jpg\", \"blur_degree\":2}}");
	//
}

void CDlgBeauty::OnBnClickedCheckLowlight()
{
	char szJson[512] = { 0 };
	sprintf_s(szJson, 512, "{\"enable\":%d, \"mode\":%d, \"level\": %d}",
		m_chkLowlight.GetCheck() != 0, 1, 1);
	LowlightEnhanceOptions options(LowlightEnhanceOptions::LOW_LIGHT_ENHANCE_MANUAL, LowlightEnhanceOptions::LOW_LIGHT_ENHANCE_LEVEL_FAST);
	m_rtcEngine->setLowlightEnhanceOptions(m_chkLowlight.GetCheck() != 0, options);
}




void CDlgBeauty::OnBnClickedStaticVirtualBgChoose(UINT idCtl)
{
	int isChecked = m_chkVirtual.GetCheck();
	if (isChecked && m_rtcEngine) {
		VirtualBackgroundSource source;

		if (idCtl == IDC_RADIO_VIRTUAL_BG_BLUR) {
			source.background_source_type = VirtualBackgroundSource::BACKGROUND_BLUR;
		}
		else if (idCtl == IDC_RADIO_VIRTUAL_BG_COLOR) {
			source.background_source_type = VirtualBackgroundSource::BACKGROUND_COLOR;
			source.color = 0x0000ff;
		}
		else if (idCtl == IDC_RADIO_VIRTUAL_BG_IMAGE) {
			source.background_source_type = VirtualBackgroundSource::BACKGROUND_IMG;
			source.source = m_imgPath.c_str();
		}
		else if (idCtl == IDC_RADIO_VIRTUAL_BG_VIDEO) {
			source.background_source_type = VirtualBackgroundSource::BACKGROUND_VIDEO;
			source.source = "https://agora-adc-artifacts.s3.cn-north-1.amazonaws.com.cn/resources/sample.mp4";
		}
		m_rtcEngine->enableVirtualBackground(true, source, SegmentationProperty(), PRIMARY_CAMERA_SOURCE);
	}
}


void CDlgBeauty::OnNMCustomdrawSliderBrihtness(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	SetBeauty();
	*pResult = 0;
}


void CDlgBeauty::OnBnClickedCheckExtention()
{
	int checked = mCkExtention.GetCheck();
	if (checked)
	{
		mBeautyDlgEx->ShowWindow(SW_SHOW);
	}
	else {
		mBeautyDlgEx->ShowWindow(SW_HIDE);
	}
}
