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
	m_staParam1.SetWindowText(beautyAudioCtrlParam1);
	m_staParam2.SetWindowText(beautyAudioCtrlParam2);
	m_staAINSMode.SetWindowText(beautyAudioCtrlAINSMode);
	m_staAITuner.SetWindowText(beautyAudioCtrlAITuner);
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

	m_rtcEngine->setAudioProfile(AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO, AUDIO_SCENARIO_GAME_STREAMING);
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
		if (m_initialize) {
			m_rtcEngine->release(true);
			m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("release rtc engine"));
		}
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
		canvas.renderMode = media::base::RENDER_MODE_FIT;
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
	m_edtParam1.SetWindowText(_T(""));
	m_edtParam2.SetWindowText(_T(""));
	m_cmbAudioChange.SetCurSel(0);
	m_btnSetBeautyAudio.SetWindowText(beautyAudioCtrlSetAudioChange);
	OnSelchangeComboAudioChanger();
	m_lstInfo.ResetContent();
	m_joinChannel = false;
	m_initialize = false;
	m_beautyAudio = false;

	m_slcVoiceFormant.SetRange(0, 100);
	m_slcVoiceFormant.SetPos(50);
	m_staVoiceFormantVlaue.SetWindowText(_T("0"));

	m_cmbAINSMode.ResetContent();
	int nIndex = 0;
	for (auto& str : m_setAINSMode)
	{
		m_cmbAINSMode.InsertString(nIndex++, str);
	}
	m_cmbAINSMode.SetCurSel(0);

	m_cmbAITuner.ResetContent();
	nIndex = 0;
	for (auto& str : m_setAITuner)
	{
		m_cmbAITuner.InsertString(nIndex++, str.first);
	}
	m_cmbAITuner.SetCurSel(0);

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
	DDX_Control(pDX, IDC_COMBO_AUDIO_AINS_MODE, m_cmbAINSMode);
	DDX_Control(pDX, IDC_COMBO_AUDIO_AI_TUNER, m_cmbAITuner);
	DDX_Control(pDX, IDC_STATIC_BEAUTY_AUDIO_AINS_MODE, m_staAINSMode);
	DDX_Control(pDX, IDC_STATIC_BEAUTY_AUDIO_AI_TUNER, m_staAITuner);
	DDX_Control(pDX, IDC_STATIC_PARAM1, m_staParam1);
	DDX_Control(pDX, IDC_STATIC_PARAM2, m_staParam2);
	DDX_Control(pDX, IDC_EDIT_PARAM1, m_edtParam1);
	DDX_Control(pDX, IDC_EDIT_PARAM2, m_edtParam2);
	DDX_Control(pDX, IDC_SLIDER_VOICE_FORMANT, m_slcVoiceFormant);
	DDX_Control(pDX, IDC_STATIC_VOICE_FORMAT_VALUE, m_staVoiceFormantVlaue);
}


BEGIN_MESSAGE_MAP(CAgoraBeautyAudio, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraBeautyAudio::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraBeautyAudio::OnEIDLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraBeautyAudio::OnEIDUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraBeautyAudio::OnEIDUserOffline)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANGED), &CAgoraBeautyAudio::OnEIDRemoteVideoStateChanged)
	ON_BN_CLICKED(IDC_BUTTON_JOINCHANNEL, &CAgoraBeautyAudio::OnBnClickedButtonJoinchannel)
	ON_BN_CLICKED(IDC_BUTTON_SET_BEAUTY_AUDIO, &CAgoraBeautyAudio::OnBnClickedButtonSetAudioChange)
	ON_LBN_SELCHANGE(IDC_LIST_INFO_BROADCASTING, &CAgoraBeautyAudio::OnSelchangeListInfoBroadcasting)
	ON_CBN_SELCHANGE(IDC_COMBO_AUDIO_CHANGER, &CAgoraBeautyAudio::OnSelchangeComboAudioChanger)
	ON_CBN_SELCHANGE(IDC_COMBO_AUDIO_PERVERB_PRESET, &CAgoraBeautyAudio::OnSelchangeComboAudioPerverbPreset)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_VOICE_FORMANT, &CAgoraBeautyAudio::OnNMCustomdrawSliderVoiceFormant)
	ON_CBN_SELCHANGE(IDC_COMBO_AUDIO_AINS_MODE, &CAgoraBeautyAudio::OnCbnSelchangeComboAudioAinsMode)
	ON_CBN_SELCHANGE(IDC_COMBO_AUDIO_AI_TUNER, &CAgoraBeautyAudio::OnCbnSelchangeComboAudioAITuner)
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
		std::make_pair(CString(_T("AudioEffect")), 
			std::vector<CString>({
				_T("AUDIO_EFFECT_OFF"),
				_T("ROOM_ACOUSTICS_KTV")
				,_T("ROOM_ACOUSTICS_VOCAL_CONCERT") ,
				_T("ROOM_ACOUSTICS_STUDIO") ,
				_T("ROOM_ACOUSTICS_PHONOGRAPH") ,
				_T("ROOM_ACOUSTICS_VIRTUAL_STEREO") ,
				_T("ROOM_ACOUSTICS_SPACIAL"),
				_T("ROOM_ACOUSTICS_ETHEREAL"), 
				_T("ROOM_ACOUSTICS_3D_VOICE"), 
				_T("VOICE_CHANGER_EFFECT_UNCLE"), 
				_T("VOICE_CHANGER_EFFECT_OLDMAN"), 
				_T("VOICE_CHANGER_EFFECT_BOY"), 
				_T("VOICE_CHANGER_EFFECT_SISTER"), 
				_T("VOICE_CHANGER_EFFECT_GIRL"),
				_T("VOICE_CHANGER_EFFECT_PIGKING"), 
				_T("VOICE_CHANGER_EFFECT_HULK"), 
				_T("STYLE_TRANSFORMATION_RNB"), 
				_T("STYLE_TRANSFORMATION_POPULAR"),
				_T("PITCH_CORRECTION"), })));

	m_mapBeauty.insert(
		std::make_pair(CString(_T("VoiceConversion")),
			std::vector<CString>({
				_T("VOICE_CONVERSION_OFF"),
				_T("VOICE_CHANGER_NEUTRAL"),
				_T("VOICE_CHANGER_SWEET"),
				_T("VOICE_CHANGER_SOLID"),
				_T("VOICE_CHANGER_BASS"),
				_T("VOICE_CHANGER_CARTOON"),
				_T("VOICE_CHANGER_CHILDLIKE"),
				_T("VOICE_CHANGER_PHONE_OPERATOR"),
				_T("VOICE_CHANGER_MONSTER"),
				_T("VOICE_CHANGER_TRANSFORMERS"),
				_T("VOICE_CHANGER_GROOT"),
				_T("VOICE_CHANGER_DARTH_VADER"),
				_T("VOICE_CHANGER_IRON_LADY"),
				_T("VOICE_CHANGER_SHIN_CHAN"),
				_T("VOICE_CHANGER_GIRLISH_MAN"),
				_T("VOICE_CHANGER_CHIPMUNK"),
				})));
	
	m_mapBeauty.insert(
		std::make_pair(CString(_T("VoiceBeautifier")),
			std::vector<CString>({ 
				_T("VOICE_BEAUTIFIER_OFF"),
				_T("CHAT_BEAUTIFIER_MAGNETIC"),
				_T("CHAT_BEAUTIFIER_FRESH"),
				_T("CHAT_BEAUTIFIER_VITALITY"), 
				_T("TIMBRE_TRANSFORMATION_DEEP"), 
				_T("TIMBRE_TRANSFORMATION_MELLOW"), 
				_T("TIMBRE_TRANSFORMATION_FALSETTO"), 
				_T("TIMBRE_TRANSFORMATION_FULL"), 
				_T("TIMBRE_TRANSFORMATION_CLEAR"), 
				_T("TIMBRE_TRANSFORMATION_RESOUNDING"), 
				_T("TIMBRE_TRANSFORMATION_RINGING"), 
				})));

	m_cmbAudioChange.InsertString(nIndex++, _T("AudioEffect"));
	m_cmbAudioChange.InsertString(nIndex++, _T("VoiceConversion"));
	m_cmbAudioChange.InsertString(nIndex++, _T("VoiceBeautifier"));

	m_setAINSMode.push_back(_T("AINS_MODE_OFF"));
	m_setAINSMode.push_back(_T("AINS_MODE_BALANCED"));
	m_setAINSMode.push_back(_T("AINS_MODE_AGGRESSIVE"));
	m_setAINSMode.push_back(_T("AINS_MODE_ULTRALOWLATENCY"));

	m_setAITuner.insert(std::make_pair(_T("AI_TUNER_OFF"), VOICE_AI_TUNER_TYPE::VOICE_AI_TUNER_MATURE_MALE));
	m_setAITuner.insert(std::make_pair(_T("AI_TUNER_MATURE_MALE"), VOICE_AI_TUNER_TYPE::VOICE_AI_TUNER_MATURE_MALE));
	m_setAITuner.insert(std::make_pair(_T("AI_TUNER_FRESH_MALE"), VOICE_AI_TUNER_TYPE::VOICE_AI_TUNER_FRESH_MALE));
	m_setAITuner.insert(std::make_pair(_T("AI_TUNER_ELEGANT_FEMALE"), VOICE_AI_TUNER_TYPE::VOICE_AI_TUNER_ELEGANT_FEMALE));
	m_setAITuner.insert(std::make_pair(_T("AI_TUNER_SWEET_FEMALE"), VOICE_AI_TUNER_TYPE::VOICE_AI_TUNER_SWEET_FEMALE));
	m_setAITuner.insert(std::make_pair(_T("AI_TUNER_WARM_MALE_SINGING"), VOICE_AI_TUNER_TYPE::VOICE_AI_TUNER_WARM_MALE_SINGING));
	m_setAITuner.insert(std::make_pair(_T("AI_TUNER_GENTLE_FEMALE_SINGING"), VOICE_AI_TUNER_TYPE::VOICE_AI_TUNER_GENTLE_FEMALE_SINGING));
	m_setAITuner.insert(std::make_pair(_T("AI_TUNER_HUSKY_MALE_SINGING"), VOICE_AI_TUNER_TYPE::VOICE_AI_TUNER_HUSKY_MALE_SINGING));
	m_setAITuner.insert(std::make_pair(_T("AI_TUNER_WARM_ELEGANT_FEMALE_SINGING"), VOICE_AI_TUNER_TYPE::VOICE_AI_TUNER_WARM_ELEGANT_FEMALE_SINGING));
	m_setAITuner.insert(std::make_pair(_T("AI_TUNER_POWERFUL_MALE_SINGING"), VOICE_AI_TUNER_TYPE::VOICE_AI_TUNER_POWERFUL_MALE_SINGING));
	m_setAITuner.insert(std::make_pair(_T("AI_TUNER_DREAMY_FEMALE_SINGING"), VOICE_AI_TUNER_TYPE::VOICE_AI_TUNER_DREAMY_FEMALE_SINGING));
	

	m_setChanger.insert(std::make_pair(_T("AUDIO_EFFECT_OFF"), AUDIO_EFFECT_OFF));
	m_setChanger.insert(std::make_pair(_T("ROOM_ACOUSTICS_KTV"), ROOM_ACOUSTICS_KTV));
	m_setChanger.insert(std::make_pair(_T("ROOM_ACOUSTICS_VOCAL_CONCERT"), ROOM_ACOUSTICS_VOCAL_CONCERT));
	m_setChanger.insert(std::make_pair(_T("ROOM_ACOUSTICS_STUDIO"), ROOM_ACOUSTICS_STUDIO));
	m_setChanger.insert(std::make_pair(_T("ROOM_ACOUSTICS_PHONOGRAPH"), ROOM_ACOUSTICS_PHONOGRAPH));
	m_setChanger.insert(std::make_pair(_T("ROOM_ACOUSTICS_VIRTUAL_STEREO"), ROOM_ACOUSTICS_VIRTUAL_STEREO));
	m_setChanger.insert(std::make_pair(_T("ROOM_ACOUSTICS_SPACIAL"), ROOM_ACOUSTICS_SPACIAL));
	m_setChanger.insert(std::make_pair(_T("ROOM_ACOUSTICS_ETHEREAL"), ROOM_ACOUSTICS_ETHEREAL));
	m_setChanger.insert(std::make_pair(_T("ROOM_ACOUSTICS_3D_VOICE"), ROOM_ACOUSTICS_3D_VOICE));
	m_setChanger.insert(std::make_pair(_T("VOICE_CHANGER_EFFECT_UNCLE"), VOICE_CHANGER_EFFECT_UNCLE));
	m_setChanger.insert(std::make_pair(_T("VOICE_CHANGER_EFFECT_OLDMAN"), VOICE_CHANGER_EFFECT_OLDMAN));
	m_setChanger.insert(std::make_pair(_T("VOICE_CHANGER_EFFECT_BOY"), VOICE_CHANGER_EFFECT_BOY));
	m_setChanger.insert(std::make_pair(_T("VOICE_CHANGER_EFFECT_SISTER"), VOICE_CHANGER_EFFECT_SISTER));
	m_setChanger.insert(std::make_pair(_T("VOICE_CHANGER_EFFECT_GIRL"), VOICE_CHANGER_EFFECT_GIRL));
	m_setChanger.insert(std::make_pair(_T("VOICE_CHANGER_EFFECT_PIGKING"), VOICE_CHANGER_EFFECT_PIGKING));
	m_setChanger.insert(std::make_pair(_T("VOICE_CHANGER_EFFECT_HULK"), VOICE_CHANGER_EFFECT_HULK));
	m_setChanger.insert(std::make_pair(_T("STYLE_TRANSFORMATION_RNB"), STYLE_TRANSFORMATION_RNB));
	m_setChanger.insert(std::make_pair(_T("STYLE_TRANSFORMATION_POPULAR"), STYLE_TRANSFORMATION_POPULAR));
	m_setChanger.insert(std::make_pair(_T("PITCH_CORRECTION"), PITCH_CORRECTION));


	m_setReverbPreSet.insert(std::make_pair(_T("VOICE_BEAUTIFIER_OFF"), VOICE_BEAUTIFIER_OFF));
	m_setReverbPreSet.insert(std::make_pair(_T("CHAT_BEAUTIFIER_MAGNETIC"), CHAT_BEAUTIFIER_MAGNETIC));
	m_setReverbPreSet.insert(std::make_pair(_T("CHAT_BEAUTIFIER_FRESH"), CHAT_BEAUTIFIER_FRESH));
	m_setReverbPreSet.insert(std::make_pair(_T("CHAT_BEAUTIFIER_VITALITY"), CHAT_BEAUTIFIER_VITALITY));
	m_setReverbPreSet.insert(std::make_pair(_T("TIMBRE_TRANSFORMATION_DEEP"), TIMBRE_TRANSFORMATION_DEEP));
	m_setReverbPreSet.insert(std::make_pair(_T("TIMBRE_TRANSFORMATION_MELLOW"), TIMBRE_TRANSFORMATION_MELLOW));
	m_setReverbPreSet.insert(std::make_pair(_T("TIMBRE_TRANSFORMATION_FALSETTO"), TIMBRE_TRANSFORMATION_FALSETTO));
	m_setReverbPreSet.insert(std::make_pair(_T("TIMBRE_TRANSFORMATION_FULL"), TIMBRE_TRANSFORMATION_FULL));
	m_setReverbPreSet.insert(std::make_pair(_T("TIMBRE_TRANSFORMATION_CLEAR"), TIMBRE_TRANSFORMATION_CLEAR));
	m_setReverbPreSet.insert(std::make_pair(_T("TIMBRE_TRANSFORMATION_RESOUNDING"), TIMBRE_TRANSFORMATION_RESOUNDING));
	m_setReverbPreSet.insert(std::make_pair(_T("TIMBRE_TRANSFORMATION_RINGING"), TIMBRE_TRANSFORMATION_RINGING));

	m_setVoiceConversion.insert(std::make_pair(_T("VOICE_CONVERSION_OFF"), VOICE_CONVERSION_OFF));
	m_setVoiceConversion.insert(std::make_pair(_T("VOICE_CHANGER_NEUTRAL"), VOICE_CHANGER_NEUTRAL));
	m_setVoiceConversion.insert(std::make_pair(_T("VOICE_CHANGER_SWEET"), VOICE_CHANGER_SWEET));
	m_setVoiceConversion.insert(std::make_pair(_T("VOICE_CHANGER_SOLID"), VOICE_CHANGER_SOLID));
	m_setVoiceConversion.insert(std::make_pair(_T("VOICE_CHANGER_BASS"), VOICE_CHANGER_BASS));
	m_setVoiceConversion.insert(std::make_pair(_T("VOICE_CHANGER_CARTOON"), VOICE_CHANGER_CARTOON));
	m_setVoiceConversion.insert(std::make_pair(_T("VOICE_CHANGER_CHILDLIKE"), VOICE_CHANGER_CHILDLIKE));
	m_setVoiceConversion.insert(std::make_pair(_T("VOICE_CHANGER_PHONE_OPERATOR"), VOICE_CHANGER_PHONE_OPERATOR));
	m_setVoiceConversion.insert(std::make_pair(_T("VOICE_CHANGER_MONSTER"), VOICE_CHANGER_MONSTER));
	m_setVoiceConversion.insert(std::make_pair(_T("VOICE_CHANGER_TRANSFORMERS"), VOICE_CHANGER_TRANSFORMERS));
	m_setVoiceConversion.insert(std::make_pair(_T("VOICE_CHANGER_GROOT"), VOICE_CHANGER_GROOT));
	m_setVoiceConversion.insert(std::make_pair(_T("VOICE_CHANGER_DARTH_VADER"), VOICE_CHANGER_DARTH_VADER));
	m_setVoiceConversion.insert(std::make_pair(_T("VOICE_CHANGER_IRON_LADY"), VOICE_CHANGER_IRON_LADY));
	m_setVoiceConversion.insert(std::make_pair(_T("VOICE_CHANGER_SHIN_CHAN"), VOICE_CHANGER_SHIN_CHAN));
	m_setVoiceConversion.insert(std::make_pair(_T("VOICE_CHANGER_GIRLISH_MAN"), VOICE_CHANGER_GIRLISH_MAN));
	m_setVoiceConversion.insert(std::make_pair(_T("VOICE_CHANGER_CHIPMUNK"), VOICE_CHANGER_CHIPMUNK));

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
		m_rtcEngine->setAudioProfile(AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO, AUDIO_SCENARIO_GAME_STREAMING);
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

void CAgoraBeautyAudio::SetVoiceChange()
{
	if (!m_rtcEngine)
		return;
	CString strInfo;

	CString str;
	m_cmbPerverbPreset.GetWindowText(str);
	//enable audio beauty.
	if (m_setChanger.find(str) != m_setChanger.end())
	{
		int param1;
		int param2;
		
		CString strParam;
		m_edtParam1.GetWindowText(strParam);
		param1 = _ttol(strParam);
		m_edtParam2.GetWindowText(strParam);
		param2 = _ttol(strParam);
		AUDIO_EFFECT_PRESET preset = m_setChanger[str];
		m_rtcEngine->setAudioEffectParameters(preset, param1, param2);
	}
	
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("setVoiceBeautifierPreset"));

	strInfo.Format(str);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
}

//set audio changer or unset audio changer.
void CAgoraBeautyAudio::OnBnClickedButtonSetAudioChange()
{
	CString str;
	m_cmbPerverbPreset.GetWindowText(str);
	if (!m_beautyAudio) {
		CString infoStr;
		if (m_cmbAudioChange.GetCurSel() == 0) {
			m_rtcEngine->setAudioEffectPreset(m_setChanger[str]);
			infoStr.Format(_T("setAudioEffectPreset %d"), m_setChanger[str]);
			m_lstInfo.InsertString(m_lstInfo.GetCount(), infoStr);
		}
		else if (m_cmbAudioChange.GetCurSel() == 1) {
			m_rtcEngine->setVoiceConversionPreset(m_setVoiceConversion[str]);
			infoStr.Format(_T("setVoiceConversionPreset %d"), m_setVoiceConversion[str]);
			m_lstInfo.InsertString(m_lstInfo.GetCount(), infoStr);
		}
		else {
			m_rtcEngine->setVoiceBeautifierPreset(m_setReverbPreSet[str]);
			infoStr.Format(_T("setVoiceBeautifierPreset %d"), m_setReverbPreSet[str]);
			m_lstInfo.InsertString(m_lstInfo.GetCount(), infoStr);
		}
		m_btnSetBeautyAudio.SetWindowText(beautyAudioCtrlUnSetAudioChange);
	}
	else {
		//set audio beauty to VOICE_CHANGER_OFF.
		
		if (m_cmbAudioChange.GetCurSel() == 0) {
			m_rtcEngine->setAudioEffectPreset(AUDIO_EFFECT_OFF);
		}
		else if (m_cmbAudioChange.GetCurSel() == 1) {
			m_rtcEngine->setVoiceConversionPreset(VOICE_CONVERSION_OFF);
		}
		else {
			m_rtcEngine->setVoiceBeautifierPreset(VOICE_BEAUTIFIER_OFF);
		}
		m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("unset beauty voice"));
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
	m_cmbAudioChange.EnableWindow(FALSE);
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
	m_cmbAudioChange.EnableWindow(TRUE);
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

//EID_REMOTE_VIDEO_STATE_CHANGED message window handler.
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
		::PostMessage(m_hMsgHanlder, WM_MSGID(EID_REMOTE_VIDEO_STATE_CHANGED), (WPARAM)stateChanged, 0);
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

	m_edtParam1.EnableWindow(m_cmbAudioChange.GetCurSel() == 0);
	m_edtParam2.EnableWindow(m_cmbAudioChange.GetCurSel() == 0);

}


void CAgoraBeautyAudio::OnSelchangeComboAudioPerverbPreset()
{ 
	CString str;
	m_btnSetBeautyAudio.GetWindowText(str);
	if (str.Compare(beautyAudioCtrlUnSetAudioChange) == 0) {
		SetVoiceChange();
	}
}


void CAgoraBeautyAudio::OnNMCustomdrawSliderVoiceFormant(NMHDR* pNMHDR, LRESULT* pResult)
{
	int pos = m_slcVoiceFormant.GetPos();
	double value = (pos - 50) * 1.0f / 50;
	CString strValue;
	strValue.Format(_T("%.2f"), value);
	m_staVoiceFormantVlaue.SetWindowText(strValue);
	if (m_rtcEngine) {
		m_rtcEngine->setLocalVoiceFormant(value);
	}
}


void CAgoraBeautyAudio::OnCbnSelchangeComboAudioAinsMode()
{
	if (!m_rtcEngine) {
		return;
	}
	int position = m_cmbAINSMode.GetCurSel();
	boolean enable = position > 0;
	AUDIO_AINS_MODE mode = AUDIO_AINS_MODE::AINS_MODE_BALANCED;
	if (position == 1) {
		mode = AUDIO_AINS_MODE::AINS_MODE_AGGRESSIVE;
	}else if (position == 2) {
		mode = AUDIO_AINS_MODE::AINS_MODE_ULTRALOWLATENCY;
	}

	CString str;
	m_cmbAINSMode.GetWindowText(str);

	int ret = m_rtcEngine->setAINSMode(enable, mode);

	str.Format(_T("setAINSMode enable=%d,ret=%d,type=%s"), enable, ret, str);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), str);
}


void CAgoraBeautyAudio::OnCbnSelchangeComboAudioAITuner()
{
	if (!m_rtcEngine) {
		return;
	}
	int position = m_cmbAITuner.GetCurSel();
	boolean enable = position > 0;

	CString str;
	m_cmbAITuner.GetWindowText(str);

	VOICE_AI_TUNER_TYPE type = m_setAITuner[str];
	
	int ret = m_rtcEngine->enableVoiceAITuner(enable, type);

	str.Format(_T("enableVoiceAITuner enable=%d,ret=%d,type=%s"), enable, ret, str);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), str);
}
