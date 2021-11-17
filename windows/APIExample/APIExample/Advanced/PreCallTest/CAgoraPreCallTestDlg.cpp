#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraPreCallTestDlg.h"



IMPLEMENT_DYNAMIC(CAgoraPreCallTestDlg, CDialogEx)

CAgoraPreCallTestDlg::CAgoraPreCallTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_PERCALL_TEST, pParent)
{

}

CAgoraPreCallTestDlg::~CAgoraPreCallTestDlg()
{
}

void CAgoraPreCallTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ADUIO_INPUT, m_staAudioInput);
	DDX_Control(pDX, IDC_STATIC_ADUIO_INPUT_VOL, m_staAudioInputVol);
	DDX_Control(pDX, IDC_STATIC_ADUIO_SCENARIO, m_staAudioOutput);
	DDX_Control(pDX, IDC_STATIC_ADUIO_OUTPUT_VOL, m_staAudioOutputVol);
	DDX_Control(pDX, IDC_STATIC_CAMERA, m_staVideo);
	DDX_Control(pDX, IDC_COMBO_VIDEO, m_cmbVideo);
	DDX_Control(pDX, IDC_COMBO_AUDIO_INPUT, m_cmbAudioInput);
	DDX_Control(pDX, IDC_COMBO_AUDIO_OUTPUT, m_cmbAudioOutput);
	DDX_Control(pDX, IDC_SLIDER_INPUT_VOL, m_sldAudioInputVol);
	DDX_Control(pDX, IDC_SLIDER_OUTPUT_VOL, m_sldAudioOutputVol);
	DDX_Control(pDX, IDC_BUTTON_AUDIO_INPUT_TEST, m_btnAudioInputTest);
	DDX_Control(pDX, IDC_BUTTON_AUDIO_OUTPUT_TEST, m_btnAudioOutputTest);
	DDX_Control(pDX, IDC_BUTTON_CAMERA, m_btnVideoTest);
	DDX_Control(pDX, IDC_BUTTON_ECHO_TEST1, m_btnEchoTest1);
	DDX_Control(pDX, IDC_BUTTON_ECHO_TEST2, m_btnEchoTest2);
	DDX_Control(pDX, IDC_STATIC_VIDEO, m_staVideoArea);
	DDX_Control(pDX, IDC_LIST_INFO_BROADCASTING, m_lstInfo);
	DDX_Control(pDX, IDC_STATIC_DETAIL, m_staDetails);
}


BEGIN_MESSAGE_MAP(CAgoraPreCallTestDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_AUDIO_INPUT_TEST, &CAgoraPreCallTestDlg::OnBnClickedButtonAudioInputTest)
	ON_BN_CLICKED(IDC_BUTTON_AUDIO_OUTPUT_TEST, &CAgoraPreCallTestDlg::OnBnClickedButtonAudioOutputTest)
	ON_BN_CLICKED(IDC_BUTTON_CAMERA, &CAgoraPreCallTestDlg::OnBnClickedButtonCamera)
	ON_LBN_SELCHANGE(IDC_LIST_INFO_BROADCASTING, &CAgoraPreCallTestDlg::OnSelchangeListInfoBroadcasting)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_INPUT_VOL, &CAgoraPreCallTestDlg::OnReleasedcaptureSliderInputVol)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_OUTPUT_VOL, &CAgoraPreCallTestDlg::OnReleasedcaptureSliderOutputVol)
	ON_MESSAGE(WM_MSGID(EID_LASTMILE_PROBE_RESULT), &CAgoraPreCallTestDlg::OnEIDLastmileProbeResult)
	ON_MESSAGE(WM_MSGID(EID_LASTMILE_QUAILTY), &CAgoraPreCallTestDlg::OnEIDLastmileQuality)
	ON_MESSAGE(WM_MSGID(EID_AUDIO_VOLUME_INDICATION), &CAgoraPreCallTestDlg::OnEIDAudioVolumeIndication)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_ECHO_TEST1, &CAgoraPreCallTestDlg::OnEchoTest1)
	ON_BN_CLICKED(IDC_BUTTON_ECHO_TEST2, &CAgoraPreCallTestDlg::OnEchoTest2)
END_MESSAGE_MAP()

//init ctrl text.
void CAgoraPreCallTestDlg::InitCtrlText()
{
	m_staVideo.SetWindowText(PerCallTestCtrlCamera);
	m_staAudioInput.SetWindowText(PerCallTestCtrlAudioInput);
	m_staAudioOutput.SetWindowText(PerCallTestCtrlAudioOutput);
	m_staAudioInputVol.SetWindowText(PerCallTestCtrlAudioVol);
	m_staAudioOutputVol.SetWindowText(PerCallTestCtrlAudioVol);
	m_btnAudioInputTest.SetWindowText(PerCallTestCtrlStartTest);
	m_btnAudioOutputTest.SetWindowText(PerCallTestCtrlStartTest);
	m_btnVideoTest.SetWindowText(PerCallTestCtrlStartTest);
	m_btnEchoTest2.SetWindowText(PerCallTestCtrlStartEchoTest);
	m_btnEchoTest1.SetWindowText(PerCallTestCtrlStartEchoTest1);
}

//Initialize the Agora SDK
bool CAgoraPreCallTestDlg::InitAgora()
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
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("initialize rtc engine"));
	LastmileProbeConfig config;
	config.probeUplink = true;
	config.probeDownlink = true;
	config.expectedUplinkBitrate = 100000;
	config.expectedDownlinkBitrate = 100000;
	//start last mile probe test.
	m_rtcEngine->startLastmileProbeTest(config);
	m_rtcEngine->enableAudio();
	m_rtcEngine->enableVideo();
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("startLastmileProbeTest"));
	//create audio and video device manager.
	m_audioDeviceManager = new AAudioDeviceManager(m_rtcEngine);
	m_videoDeviceManager = new AVideoDeviceManager(m_rtcEngine);
	return true;
}

void CAgoraPreCallTestDlg::UnInitAgora()
{
	if (m_rtcEngine) {
		//release device manager.
		m_audioDeviceManager->release();
		m_videoDeviceManager->release();
		m_rtcEngine->stopLastmileProbeTest();
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stopLastmileProbeTest"));
		//release engine.
		m_rtcEngine->release(true);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release rtc engine"));
		m_rtcEngine = NULL;
	}
}



//resume status.
void CAgoraPreCallTestDlg::ResumeStatus()
{
	InitCtrlText();
	m_netQuality = 0;
	m_lstInfo.ResetContent();
	m_cmbAudioOutput.ResetContent();
	m_cmbAudioInput.ResetContent();
	m_cmbVideo.ResetContent();
	m_mapAudioInput.clear();
	m_mapAudioOutput.clear();
	m_mapCamera.clear();
	m_cameraTest = false;
	m_audioInputTest = false;
	m_audioOutputTest = false;
	m_echoTest = false;
}


void CAgoraPreCallTestDlg::UpdateViews()
{
	char szDeviceName[1024];
	char szDeviceId[1024];

	m_cmbAudioInput.ResetContent();
	m_cmbAudioOutput.ResetContent();
	m_cmbVideo.ResetContent();
	int nVol;
	(*m_audioDeviceManager)->getPlaybackDeviceVolume(&nVol);
	m_sldAudioOutputVol.SetPos(nVol);
	(*m_audioDeviceManager)->getRecordingDeviceVolume(&nVol);
	m_sldAudioInputVol.SetPos(nVol);
	//get audio record devices and add to combobox and insert map.
	IAudioDeviceCollection *audioRecordDevices = (*m_audioDeviceManager)->enumerateRecordingDevices();
	for (int i = 0; i < audioRecordDevices->getCount(); i++)
	{
		int nRet = audioRecordDevices->getDevice(i, szDeviceName, szDeviceId);
		m_cmbAudioInput.AddString(utf82cs(szDeviceName));
		m_mapAudioInput.insert(std::make_pair(utf82cs(szDeviceName), szDeviceId));
	}
	audioRecordDevices->release();
	m_cmbAudioInput.SetCurSel(0);
	//get audio playback devices and add to combobox and insert map.
	IAudioDeviceCollection *audioPlaybackDevices = (*m_audioDeviceManager)->enumeratePlaybackDevices();
	for (int i = 0; i < audioPlaybackDevices->getCount(); i++)
	{
		int nRet = audioPlaybackDevices->getDevice(i, szDeviceName, szDeviceId);
		m_cmbAudioOutput.AddString(utf82cs(szDeviceName));
		m_mapAudioOutput.insert(std::make_pair(utf82cs(szDeviceName), szDeviceId));
	}
	audioPlaybackDevices->release();
	m_cmbAudioOutput.SetCurSel(0);

	//get camera devices and add to combobox and insert map.
	auto cameraDevices = (*m_videoDeviceManager)->enumerateVideoDevices();
	for (int i = 0; i < cameraDevices->getCount(); i++)
	{
		int nRet = cameraDevices->getDevice(i, szDeviceName, szDeviceId);
		m_cmbVideo.AddString(utf82cs(szDeviceName));
		m_mapCamera.insert(std::make_pair(utf82cs(szDeviceName), szDeviceId));
	}
	m_cmbVideo.SetCurSel(0);
	cameraDevices->release();
}



void CAgoraPreCallTestDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (bShow)
	{
		InitCtrlText();
		UpdateViews();
	}
	else {
		ResumeStatus();
	}
}


BOOL CAgoraPreCallTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	RECT rcArea;
	m_staVideoArea.GetWindowRect(&rcArea);
	CBitmap bmpNetQuality;
	bmpNetQuality.LoadBitmap(IDB_BITMAP_NETWORK_STATE);
	m_imgNetQuality.Create(32, 32, ILC_COLOR24 | ILC_MASK, 6, 1);
	m_imgNetQuality.Add(&bmpNetQuality, RGB(0xFF, 0, 0xFF));
	m_sldAudioInputVol.SetRange(0, 255);
	m_sldAudioOutputVol.SetRange(0, 255);
	m_VideoTest.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 1, 1), this, NULL);
	m_VideoTest.MoveWindow(&rcArea);
	m_VideoTest.SetVolRange(100);
	ResumeStatus();
	return TRUE; 
}

//last mile quality notify  
LRESULT CAgoraPreCallTestDlg::OnEIDLastmileQuality(WPARAM wparam, LPARAM lparam)
{
	int quality = wparam;
	m_netQuality = quality;
	CString strInfo;
	strInfo.Format(_T("current network quality:%d"), quality);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	RECT rc = { 16,40,100,100 };
	this->InvalidateRect(&rc);
	return TRUE;
}

LRESULT CAgoraPreCallTestDlg::OnEIDLastmileProbeResult(WPARAM wparam, LPARAM lparam)
{
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("OnLastmileProbeResult"));
	return TRUE;
}

//audio volume indication message handler
LRESULT CAgoraPreCallTestDlg::OnEIDAudioVolumeIndication(WPARAM wparam, LPARAM lparam)
{
	//set audio volume to show test window.
	m_VideoTest.SetCurVol(wparam);
	return TRUE;
}



BOOL CAgoraPreCallTestDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CAgoraPreCallTestDlg::OnBnClickedButtonAudioInputTest()
{
	int nSel = m_cmbAudioInput.GetCurSel();
	if (nSel < 0)return;
	CString strAudioInputName;
	m_cmbAudioInput.GetWindowText(strAudioInputName);
	if (!m_audioInputTest)
	{
		//set audio recording device with device id.
		(*m_audioDeviceManager)->setRecordingDevice(m_mapAudioInput[strAudioInputName].c_str());
		//enable audio volume indication
		m_rtcEngine->enableAudioVolumeIndication(1000, 10, true);
		//start audio recording device test
		(*m_audioDeviceManager)->startRecordingDeviceTest(1000);
		m_btnAudioInputTest.SetWindowText(PerCallTestCtrlStopTest);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("start audio recording device test."));
	}
	else {
		//stop audio recording device test.
		(*m_audioDeviceManager)->stopRecordingDeviceTest();
		//disable audio volume indication.
		m_rtcEngine->enableAudioVolumeIndication(1000, 10, false);
		m_btnAudioInputTest.SetWindowText(PerCallTestCtrlStartTest);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stop audio recording device test."));
	}
	m_audioInputTest = !m_audioInputTest;
}



void CAgoraPreCallTestDlg::OnBnClickedButtonAudioOutputTest()
{
	TCHAR	szWavPath[MAX_PATH];
	int nSel = m_cmbAudioInput.GetCurSel();
	if (nSel < 0)return;
	CString strAudioInputName;
	m_cmbAudioInput.GetWindowText(strAudioInputName);
	if (!m_audioOutputTest)
	{
		::GetModuleFileName(NULL, szWavPath, MAX_PATH);
		LPTSTR lpLastSlash = (LPTSTR)_tcsrchr(szWavPath, _T('\\')) + 1;
		_tcscpy_s(lpLastSlash, 16, _T("test.wav"));
		SaveResourceToFile(_T("WAVE"), IDR_TEST_WAVE, szWavPath);
		//set audio playback device with device id.
		(*m_audioDeviceManager)->setPlaybackDevice(m_mapAudioInput[strAudioInputName].c_str());
		//start audio playback device test with wav file path.
#ifdef UNICODE
		CHAR szWavPathA[MAX_PATH];
		::WideCharToMultiByte(CP_ACP, 0, szWavPath, -1, szWavPathA, MAX_PATH, NULL, NULL);
		(*m_audioDeviceManager)->startPlaybackDeviceTest(szWavPathA);
#else
		(*m_audioDeviceManager)->startPlaybackDeviceTest(szWavPathA);
#endif
		m_btnAudioOutputTest.SetWindowText(PerCallTestCtrlStopTest);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("start audio playback device test."));
	}
	else {
		//stop audio playback device test. 
		(*m_audioDeviceManager)->stopPlaybackDeviceTest();
		m_btnAudioOutputTest.SetWindowText(PerCallTestCtrlStartTest);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stop audio playback device test. "));
	}
	m_audioOutputTest = !m_audioOutputTest;
}


void CAgoraPreCallTestDlg::OnBnClickedButtonCamera()
{
	int nSel = m_cmbAudioInput.GetCurSel();
	if (nSel < 0)return;
	CString strCamereaDeivce;
	m_cmbVideo.GetWindowText(strCamereaDeivce);
	if (!m_cameraTest)
	{
		//set camera device with device id.
		(*m_videoDeviceManager)->setDevice(m_mapCamera[strCamereaDeivce].c_str());
		//start camera device test.
		(*m_videoDeviceManager)->startDeviceTest(m_VideoTest.GetVideoSafeHwnd());
		m_btnVideoTest.SetWindowText(PerCallTestCtrlStopTest);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("start camera device test. "));

	}
	else {
		//stop camera device test.
		(*m_videoDeviceManager)->stopDeviceTest();
		m_btnVideoTest.SetWindowText(PerCallTestCtrlStartTest);
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stop camera device test. "));
	}
	m_cameraTest = !m_cameraTest;
}


void CAgoraPreCallTestDlg::OnSelchangeListInfoBroadcasting()
{
	int sel = m_lstInfo.GetCurSel();
	if (sel < 0)return;
	CString strDetail;
	m_lstInfo.GetText(sel, strDetail);
	m_staDetails.SetWindowText(strDetail);
}


void CAgoraPreCallTestDlg::OnReleasedcaptureSliderInputVol(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	int vol = m_sldAudioInputVol.GetPos();
	//set audio record device volume 
	(*m_audioDeviceManager)->setRecordingDeviceVolume(vol);
	*pResult = 0;
}


void CAgoraPreCallTestDlg::OnReleasedcaptureSliderOutputVol(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	int vol = m_sldAudioOutputVol.GetPos();
	//set audio playback device volume
	(*m_audioDeviceManager)->setPlaybackDeviceVolume(vol);
	*pResult = 0;
}


void CAgoraPreCallTestDlg::OnPaint()
{
	CPaintDC dc(this); 
	//draw quality bitmap
	m_imgNetQuality.Draw(&dc, m_netQuality, CPoint(16, 40), ILD_NORMAL);
}


void CAgoraPreCallTestDlg::OnEchoTest1()
{
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("Start Audio Call Loop Test."));
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("You will hear your voice after 10 secs"));
	m_rtcEngine->startEchoTest(10);
}


void CAgoraPreCallTestDlg::OnEchoTest2()
{
	if (!m_echoTest) 
	{
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("Start Audio and Video Call Loop Test."));
		EchoTestConfiguration config;
		config.channelId = "randomChannel";
		config.enableAudio = true;
		config.enableVideo = true;
		config.view = m_VideoTest.GetVideoSafeHwnd();
		m_rtcEngine->startEchoTest(config);
		m_echoTest = true;
		m_btnEchoTest2.SetWindowText(PerCallTestCtrlStopEchoTest);
	}
	else
	{
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("Stop Audio and Video Call Loop Test."));
		m_rtcEngine->stopEchoTest();
		m_echoTest = false;
		m_btnEchoTest2.SetWindowText(PerCallTestCtrlStartEchoTest);
	}
}
