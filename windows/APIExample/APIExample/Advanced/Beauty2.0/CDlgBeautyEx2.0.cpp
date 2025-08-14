#include "stdafx.h"
#include "APIExample.h"
#include "CDlgBeautyEx2.0.h"
#include "CDlgBeauty2.0.h"

IMPLEMENT_DYNAMIC(CDlgBeautyEx2, CDialogEx)

CDlgBeautyEx2::CDlgBeautyEx2(CWnd* pParent /*=nullptr*/) : CDialogEx(IDD_DIALOG_BEAUTY_EX, pParent)
{
}

CDlgBeautyEx2::~CDlgBeautyEx2()
{
}

void CDlgBeautyEx2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_MAKE_UP, mCbMakeup);
	DDX_Control(pDX, IDC_COMBO_BROW_STYLE, mDdBrowStyle);
	DDX_Control(pDX, IDC_COMBO_BROW_COLOR, mDdBrowColor);
	DDX_Control(pDX, IDC_SLIDER__BROW_STRENGTH, mSdBrowStrength);
	DDX_Control(pDX, IDC_COMBO_LASH_STYLE, mDdLashStyle);
	DDX_Control(pDX, IDC_COMBO_LASH_COLOR, mDdLashColor);
	DDX_Control(pDX, IDC_SLIDER__LASH_STRENGTH, mSdLashStrength);
	DDX_Control(pDX, IDC_COMBO_SHADOW_STYLE, mDdShadowStyle);
	DDX_Control(pDX, IDC_SLIDER__SHADOW_STRENGTH, mSdShadowStrenght);
	DDX_Control(pDX, IDC_COMBO_PUPIL_STYLE, mDdPupilStyle);
	DDX_Control(pDX, IDC_SLIDER__Pupil_STRENGTH, mSdPupilStrength);
	DDX_Control(pDX, IDC_COMBO_BLUSH_STYLE, mDdBlushStyle);
	DDX_Control(pDX, IDC_COMBO_BLUSH_COLOR, mDdBlushColor);
	DDX_Control(pDX, IDC_SLIDER__BLUSH_STRENGTH, mSdBlushStrength);
	DDX_Control(pDX, IDC_COMBO_LIP_STYLE, mDdLipStyle);
	DDX_Control(pDX, IDC_COMBO_LIP_COLOR, mDdLipColor);
	DDX_Control(pDX, IDC_SLIDER__LIP_STRENGTH, mSdLipStrength);
	DDX_Control(pDX, IDC_CHECK_BEAUTY_SHAPE, mCbBeautyShape);
	DDX_Control(pDX, IDC_COMBO_FACE_SHAPE_AREA, mDdShapeArea);
	DDX_Control(pDX, IDC_SLIDER__SHAPE_AREA_INTENSITY, mSdShapeAreaIntensity);
	DDX_Control(pDX, IDC_COMBO_FACE_SHAPE_STYLE, mDdShapeStyle);
	DDX_Control(pDX, IDC_SLIDER__SHAPE_STYLE_INTENSITY, mSdShapeStyleIntensity);
}

BEGIN_MESSAGE_MAP(CDlgBeautyEx2, CDialogEx)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CHECK_MAKE_UP, &CDlgBeautyEx2::OnBnClickedCheckMakeUp)
	ON_CBN_SELCHANGE(IDC_COMBO_BROW_COLOR, &CDlgBeautyEx2::OnCbnSelchangeComboBrowColor)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER__BROW_STRENGTH, &CDlgBeautyEx2::OnNMCustomdrawSliderBrowStrength)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER__LASH_STRENGTH, &CDlgBeautyEx2::OnNMCustomdrawSliderLashStrength)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER__SHADOW_STRENGTH, &CDlgBeautyEx2::OnNMCustomdrawSliderShadowStrength)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER__Pupil_STRENGTH, &CDlgBeautyEx2::OnNMCustomdrawSliderPupilStrength)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER__BLUSH_STRENGTH, &CDlgBeautyEx2::OnNMCustomdrawSliderBlushStrength)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER__LIP_STRENGTH, &CDlgBeautyEx2::OnNMCustomdrawSliderLipStrength)
	ON_CBN_SELCHANGE(IDC_COMBO_LASH_STYLE, &CDlgBeautyEx2::OnCbnSelchangeComboLashStyle)
	ON_CBN_SELCHANGE(IDC_COMBO_LASH_COLOR, &CDlgBeautyEx2::OnCbnSelchangeComboLashColor)
	ON_CBN_SELCHANGE(IDC_COMBO_SHADOW_STYLE, &CDlgBeautyEx2::OnCbnSelchangeComboShadowStyle)
	ON_CBN_SELCHANGE(IDC_COMBO_PUPIL_STYLE, &CDlgBeautyEx2::OnCbnSelchangeComboPupilStyle)
	ON_CBN_SELCHANGE(IDC_COMBO_BLUSH_STYLE, &CDlgBeautyEx2::OnCbnSelchangeComboBlushStyle)
	ON_CBN_SELCHANGE(IDC_COMBO_BLUSH_COLOR, &CDlgBeautyEx2::OnCbnSelchangeComboBlushColor)
	ON_CBN_SELCHANGE(IDC_COMBO_LIP_STYLE, &CDlgBeautyEx2::OnCbnSelchangeComboLipStyle)
	ON_CBN_SELCHANGE(IDC_COMBO_LIP_COLOR, &CDlgBeautyEx2::OnCbnSelchangeComboLipColor)
	ON_BN_CLICKED(IDC_CHECK_BEAUTY_SHAPE, &CDlgBeautyEx2::OnBnClickedCheckBeautyShape)
	ON_CBN_SELCHANGE(IDC_COMBO_FACE_SHAPE_AREA, &CDlgBeautyEx2::OnCbnSelchangeComboFaceShapeArea)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER__SHAPE_AREA_INTENSITY, &CDlgBeautyEx2::OnNMCustomdrawSliderShpaeAreaIntensity)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER__SHAPE_STYLE_INTENSITY, &CDlgBeautyEx2::OnNMCustomdrawSliderShpaeStyleIntensity)
	ON_CBN_SELCHANGE(IDC_COMBO_FACE_SHAPE_STYLE, &CDlgBeautyEx2::OnCbnSelchangeComboFaceShapeStyle)
	ON_CBN_SELCHANGE(IDC_COMBO_BROW_STYLE, &CDlgBeautyEx2::OnCbnSelchangeComboBrowStyle)
END_MESSAGE_MAP()

BOOL CDlgBeautyEx2::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitCtrlText();
	
	HideFaceShapeControls();
	
	if (m_initialize && *m_initialize && m_rtcEngine && *m_rtcEngine) {
		InitializeBeautyResources();
	}
	
	return TRUE;
}

void CDlgBeautyEx2::initData(bool* initialize, agora::rtc::IRtcEngine** engine, CDlgBeauty2* beautyDlg)
{
	m_initialize = initialize;
	m_rtcEngine = engine;
	m_beautyDlg = beautyDlg;
}

void CDlgBeautyEx2::InitializeBeautyResources()
{
	if (!m_initialize || !*m_initialize || !m_rtcEngine || !*m_rtcEngine) {
		return;
	}
	
	if (m_videoEffectObjectRef) {
		return;
	}
	
	int ret = (*m_rtcEngine)->enableExtension("agora_video_filters_clear_vision", "clear_vision", true);
	if (ret != 0) {
		CString strMsg;
		strMsg.Format(_T("Enable beauty extension failed: %d"), ret);
		AfxMessageBox(strMsg);
		return;
	}
	
	(*m_rtcEngine)->setParameters("{\"rtc.video.yuvconverter_enable_hardware_buffer\":true}");
	
	CString strExePath = GetExePath();
	CString strModelPath = strExePath + _T("\\beauty_agora\\beauty_material.bundle\\beauty_material_v2.0.0");
	
	if (!PathFileExists(strModelPath)) {
		CString strMsg;
		strMsg.Format(_T("Beauty resource path not exist: %s"), strModelPath);
		AfxMessageBox(strMsg);
		return;
	}
	
	std::string modelPath = cs2utf8(strModelPath);
	m_videoEffectObjectRef = (*m_rtcEngine)->createVideoEffectObject(
		modelPath.c_str(),
		agora::media::PRIMARY_CAMERA_SOURCE
	);
	
	if (m_videoEffectObjectRef) {
		CString strInfo;
		strInfo.Format(_T("Beauty resource loaded successfully: %s"), strModelPath);
		AfxMessageBox(strInfo);
	} else {
		AfxMessageBox(_T("Create VideoEffectObject failed"));
	}
}

CString CDlgBeautyEx2::GetExePath()
{
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH);
	CString strExePath = szPath;
	int nPos = strExePath.ReverseFind('\\');
	return strExePath.Left(nPos);
}

void CDlgBeautyEx2::CleanupBeautyResources()
{
	if (m_videoEffectObjectRef && m_rtcEngine && *m_rtcEngine) {
		(*m_rtcEngine)->enableExtension(
			"agora_video_filters_clear_vision", 
			"clear_vision", 
			false,
			agora::media::PRIMARY_CAMERA_SOURCE
		);
		
		m_videoEffectObjectRef = nullptr;
	}
}

void CDlgBeautyEx2::OnClose()
{
	CleanupBeautyResources();
	
	if (m_beautyDlg) {
		m_beautyDlg->UpdateExtentCbState(false);
	}
	CDialogEx::OnClose();
}

void CDlgBeautyEx2::InitCtrlData()
{
	m_makeupOptions.reset();
	//make up
	mCbMakeup.SetCheck(m_makeupOptions.enable_mu);

	mDdBrowStyle.SetCurSel(m_makeupOptions.browStyle);
	mDdBrowColor.SetCurSel(m_makeupOptions.browColor);
	mSdBrowStrength.SetRange(0, 100);
	mSdBrowStrength.SetPos(m_makeupOptions.browStrength);

	mDdLashStyle.SetCurSel(m_makeupOptions.lashStyle);
	mDdLashColor.SetCurSel(m_makeupOptions.lashColor);
	mSdLashStrength.SetRange(0, 100);
	mSdLashStrength.SetPos(m_makeupOptions.lashStrength);

	mDdShadowStyle.SetCurSel(m_makeupOptions.shadowStyle);
	mSdShadowStrenght.SetRange(0, 100);
	mSdShadowStrenght.SetPos(m_makeupOptions.shadowStrength);

	mDdPupilStyle.SetCurSel(m_makeupOptions.pupilStyle);
	mSdPupilStrength.SetRange(0, 100);
	mSdPupilStrength.SetPos(m_makeupOptions.pupilStrength);

	mDdBlushStyle.SetCurSel(m_makeupOptions.blushStyle);
	mDdBlushColor.SetCurSel(m_makeupOptions.blushColor);
	mSdBlushStrength.SetRange(0, 100);
	mSdBlushStrength.SetPos(m_makeupOptions.blushStrength);

	mDdLipStyle.SetCurSel(m_makeupOptions.lipStyle);
	mDdLipColor.SetCurSel(m_makeupOptions.lipColor);
	mSdLipStrength.SetRange(0, 100);
	mSdLipStrength.SetPos(m_makeupOptions.lipStrength);

	//beauty shape
	mCbBeautyShape.SetCheck(0);
	mDdShapeArea.SetCurSel(0);
	mSdShapeAreaIntensity.SetRange(0, 100);
	mSdShapeAreaIntensity.SetPos(0);
	mDdShapeStyle.SetCurSel(0);
	mSdShapeStyleIntensity.SetRange(0, 100);
	mSdShapeStyleIntensity.SetPos(0);
}

// set control text from config.
void CDlgBeautyEx2::InitCtrlText()
{
	mDdBrowStyle.ResetContent();
	mDdBrowStyle.InsertString(0, TEXT("CLOSE"));
	mDdBrowStyle.InsertString(1, TEXT("eyebrow001"));
	mDdBrowStyle.InsertString(2, TEXT("eyebrow002"));
	mDdBrowStyle.InsertString(3, TEXT("eyebrow003"));
	
	mDdBrowColor.ResetContent();
	mDdBrowColor.InsertString(0, TEXT("Brown"));
	mDdBrowColor.InsertString(1, TEXT("Gray Brown"));
	mDdBrowColor.InsertString(2, TEXT("Dark Brown"));

	mDdLashStyle.ResetContent();
	mDdLashStyle.InsertString(0, TEXT("CLOSE"));
	mDdLashStyle.InsertString(1, TEXT("eyelash003"));
	mDdLashStyle.InsertString(2, TEXT("eyelash005"));
	
	mDdLashColor.ResetContent();
	mDdLashColor.InsertString(0, TEXT("Black"));
	mDdLashColor.InsertString(1, TEXT("Brown"));
	mDdLashColor.InsertString(2, TEXT("Blue"));

	mDdShadowStyle.ResetContent();
	mDdShadowStyle.InsertString(0, TEXT("CLOSE"));
	mDdShadowStyle.InsertString(1, TEXT("eyeshadow001"));
	mDdShadowStyle.InsertString(2, TEXT("eyeshadow006"));

	mDdPupilStyle.ResetContent();
	mDdPupilStyle.InsertString(0, TEXT("CLOSE"));
	mDdPupilStyle.InsertString(1, TEXT("facial002"));

	mDdBlushStyle.ResetContent();
	mDdBlushStyle.InsertString(0, TEXT("CLOSE"));
	mDdBlushStyle.InsertString(1, TEXT("blush001"));
	mDdBlushStyle.InsertString(2, TEXT("blush002"));
	mDdBlushStyle.InsertString(3, TEXT("blush004"));
	mDdBlushStyle.InsertString(4, TEXT("blush009"));
	
	mDdBlushColor.ResetContent();
	mDdBlushColor.InsertString(0, TEXT("Pink"));
	mDdBlushColor.InsertString(1, TEXT("Orange"));
	mDdBlushColor.InsertString(2, TEXT("Red"));
	mDdBlushColor.InsertString(3, TEXT("Coral"));
	mDdBlushColor.InsertString(4, TEXT("Purple"));
	mDdBlushColor.InsertString(5, TEXT("Brown"));

	mDdLipStyle.ResetContent();
	mDdLipStyle.InsertString(0, TEXT("CLOSE"));
	mDdLipStyle.InsertString(1, TEXT("Moisturizing"));
	mDdLipStyle.InsertString(2, TEXT("Matte"));
	
	mDdLipColor.ResetContent();
	mDdLipColor.InsertString(0, TEXT("Natural"));
	mDdLipColor.InsertString(1, TEXT("Pink"));
	mDdLipColor.InsertString(2, TEXT("Orange"));
	mDdLipColor.InsertString(3, TEXT("Red"));
	mDdLipColor.InsertString(4, TEXT("Coral"));
	mDdLipColor.InsertString(5, TEXT("Purple"));

	//beauty shape
	mDdShapeArea.ResetContent();
	mDdShapeArea.InsertString(0, TEXT("FACE_SHAPE_AREA_NONE"));
	mDdShapeArea.InsertString(1, TEXT("FACE_SHAPE_AREA_HEADSCALE"));
	mDdShapeArea.InsertString(2, TEXT("FACE_SHAPE_AREA_FOREHEAD"));
	mDdShapeArea.InsertString(3, TEXT("FACE_SHAPE_AREA_FACECONTOUR"));
	mDdShapeArea.InsertString(4, TEXT("FACE_SHAPE_AREA_FACELENGTH"));
	mDdShapeArea.InsertString(5, TEXT("FACE_SHAPE_AREA_FACEWIDTH"));
	mDdShapeArea.InsertString(6, TEXT("FACE_SHAPE_AREA_CHEEKBONE"));
	mDdShapeArea.InsertString(7, TEXT("FACE_SHAPE_AREA_CHEEK"));
	mDdShapeArea.InsertString(8, TEXT("FACE_SHAPE_AREA_CHIN"));
	mDdShapeArea.InsertString(9, TEXT("FACE_SHAPE_AREA_EYESCALE"));
	mDdShapeArea.InsertString(10, TEXT("FACE_SHAPE_AREA_NOSELENGTH"));
	mDdShapeArea.InsertString(11, TEXT("FACE_SHAPE_AREA_NOSEWIDTH"));
	mDdShapeArea.InsertString(12, TEXT("FACE_SHAPE_AREA_MOUTHSCALE"));

	mDdShapeStyle.ResetContent();
	mDdShapeStyle.InsertString(0, TEXT("FACE_SHAPE_STYLE_FEMALE"));
	mDdShapeStyle.InsertString(1, TEXT("FACE_SHAPE_STYLE_MALE"));

	InitCtrlData();
}

void CDlgBeautyEx2::HideFaceShapeControls()
{	
	CWnd* pShapeCheckbox = GetDlgItem(IDC_CHECK_BEAUTY_SHAPE);
	if (pShapeCheckbox) {
		pShapeCheckbox->ShowWindow(SW_HIDE);
	}
	
	CWnd* pShapeAreaCombo = GetDlgItem(IDC_COMBO_FACE_SHAPE_AREA);
	if (pShapeAreaCombo) {
		pShapeAreaCombo->ShowWindow(SW_HIDE);
	}
	
	CWnd* pShapeStyleCombo = GetDlgItem(IDC_COMBO_FACE_SHAPE_STYLE);
	if (pShapeStyleCombo) {
		pShapeStyleCombo->ShowWindow(SW_HIDE);
	}
	
	CWnd* pShapeAreaIntensitySlider = GetDlgItem(IDC_SLIDER__SHAPE_AREA_INTENSITY);
	if (pShapeAreaIntensitySlider) {
		pShapeAreaIntensitySlider->ShowWindow(SW_HIDE);
	}
	
	CWnd* pShapeStyleIntensitySlider = GetDlgItem(IDC_SLIDER__SHAPE_STYLE_INTENSITY);
	if (pShapeStyleIntensitySlider) {
		pShapeStyleIntensitySlider->ShowWindow(SW_HIDE);
	}
}

void CDlgBeautyEx2::SetBeauty()
{
	if (!m_videoEffectObjectRef) {
		InitializeBeautyResources();
	}
	
	if (!m_videoEffectObjectRef) {
		AfxMessageBox(_T("VideoEffectObject not created successfully"));
		return;
	}
	
	ApplyMakeupEffect();
}

void CDlgBeautyEx2::ApplyMakeupEffect()
{
	if (!m_videoEffectObjectRef) {
		return;
	}
	
	try {
		int ret = m_videoEffectObjectRef->addOrUpdateVideoEffect(
			static_cast<uint32_t>(agora::rtc::IVideoEffectObject::VIDEO_EFFECT_NODE_ID::STYLE_MAKEUP),
			""
		);
		
		if (ret != 0) {
			CString strMsg;
			strMsg.Format(_T("addOrUpdateVideoEffect failed: %d"), ret);
			AfxMessageBox(strMsg);
			return;
		}
		
		m_videoEffectObjectRef->setVideoEffectBoolParam("makeup_options", "enable_mu", m_makeupOptions.enable_mu);
		
		if (m_makeupOptions.enable_mu) {
			CString strParams;
			strParams.Format(_T("browStyle: %d, browColor: %d, browStrength: %f, lashStyle: %d, lashColor: %d, lashStrength: %f, shadowStyle: %d, shadowStrength: %f, pupilStyle: %d, pupilStrength: %f, blushStyle: %d, blushColor: %d, blushStrength: %f, lipStyle: %d, lipColor: %d, lipStrength: %f\n"),
				m_makeupOptions.browStyle, m_makeupOptions.browColor, m_makeupOptions.browStrength,
				m_makeupOptions.lashStyle, m_makeupOptions.lashColor, m_makeupOptions.lashStrength,
				m_makeupOptions.shadowStyle, m_makeupOptions.shadowStrength,
				m_makeupOptions.pupilStyle, m_makeupOptions.pupilStrength, 
				m_makeupOptions.blushStyle, m_makeupOptions.blushColor, m_makeupOptions.blushStrength,
				m_makeupOptions.lipStyle, m_makeupOptions.lipColor, m_makeupOptions.lipStrength
			);
			OutputDebugString(strParams);
			
			int browStyleId = MapUIToResourceId(_T("eyebrow"), m_makeupOptions.browStyle);
			m_videoEffectObjectRef->setVideoEffectIntParam("makeup_options", "browStyle", browStyleId);
			m_videoEffectObjectRef->setVideoEffectIntParam("makeup_options", "browColor", m_makeupOptions.browColor);
			m_videoEffectObjectRef->setVideoEffectFloatParam("makeup_options", "browStrength", m_makeupOptions.browStrength);
			
			int lashStyleId = MapUIToResourceId(_T("eyelash"), m_makeupOptions.lashStyle);
			m_videoEffectObjectRef->setVideoEffectIntParam("makeup_options", "lashStyle", lashStyleId);
			m_videoEffectObjectRef->setVideoEffectIntParam("makeup_options", "lashColor", m_makeupOptions.lashColor);
			m_videoEffectObjectRef->setVideoEffectFloatParam("makeup_options", "lashStrength", m_makeupOptions.lashStrength);
			
			int shadowStyleId = MapUIToResourceId(_T("eyeshadow"), m_makeupOptions.shadowStyle);
			m_videoEffectObjectRef->setVideoEffectIntParam("makeup_options", "shadowStyle", shadowStyleId);
			m_videoEffectObjectRef->setVideoEffectFloatParam("makeup_options", "shadowStrength", m_makeupOptions.shadowStrength);
			
			int pupilStyleId = MapUIToResourceId(_T("pupil"), m_makeupOptions.pupilStyle);
			m_videoEffectObjectRef->setVideoEffectIntParam("makeup_options", "pupilStyle", pupilStyleId);
			m_videoEffectObjectRef->setVideoEffectFloatParam("makeup_options", "pupilStrength", m_makeupOptions.pupilStrength);
			
			int blushStyleId = MapUIToResourceId(_T("blush"), m_makeupOptions.blushStyle);
			m_videoEffectObjectRef->setVideoEffectIntParam("makeup_options", "blushStyle", blushStyleId);
			m_videoEffectObjectRef->setVideoEffectIntParam("makeup_options", "blushColor", m_makeupOptions.blushColor);
			m_videoEffectObjectRef->setVideoEffectFloatParam("makeup_options", "blushStrength", m_makeupOptions.blushStrength);
			
			m_videoEffectObjectRef->setVideoEffectIntParam("makeup_options", "lipStyle", m_makeupOptions.lipStyle);
			m_videoEffectObjectRef->setVideoEffectIntParam("makeup_options", "lipColor", m_makeupOptions.lipColor);
			m_videoEffectObjectRef->setVideoEffectFloatParam("makeup_options", "lipStrength", m_makeupOptions.lipStrength);
		}
	} catch (...) {
		AfxMessageBox(_T("Exception occurred when calling VideoEffectObject methods"));
	}
}

void CDlgBeautyEx2::ApplyFaceShapeEffect()
{
	if (!m_videoEffectObjectRef) {
		return;
	}
	
	try {
		int ret = m_videoEffectObjectRef->addOrUpdateVideoEffect(
			static_cast<uint32_t>(agora::rtc::IVideoEffectObject::VIDEO_EFFECT_NODE_ID::BEAUTY),
			""
		);
		
		if (ret != 0) {
			return;
		}
		
		if (m_faceShapeAreaOptions.shapeArea != FaceShapeAreaOptions::FACE_SHAPE_AREA_NONE) {
			std::string paramName = GetFaceShapeAreaParamName(m_faceShapeAreaOptions.shapeArea);
			m_videoEffectObjectRef->setVideoEffectFloatParam("face_buffing_option", paramName.c_str(), m_faceShapeAreaOptions.shapeIntensity / 100.0f);
		}
		
		if (m_faceShapeBeautyOptions.shapeStyle != FaceShapeBeautyOptions::FACE_SHAPE_BEAUTY_STYLE_FEMALE) {
			m_videoEffectObjectRef->setVideoEffectBoolParam("face_shape_beauty_option", "enable", true);
			m_videoEffectObjectRef->setVideoEffectIntParam("face_shape_beauty_option", "intensity", static_cast<int>(m_faceShapeBeautyOptions.styleIntensity));
		}
	} catch (...) {
		AfxMessageBox(_T("Exception occurred when applying face shape effects"));
	}
}

std::string CDlgBeautyEx2::GetFaceShapeAreaParamName(FaceShapeAreaOptions::FACE_SHAPE_AREA area)
{
	switch (area) {
		case FaceShapeAreaOptions::FACE_SHAPE_AREA_HEADSCALE:
			return "headScale";
		case FaceShapeAreaOptions::FACE_SHAPE_AREA_FOREHEAD:
			return "forehead";
		case FaceShapeAreaOptions::FACE_SHAPE_AREA_FACECONTOUR:
			return "faceContour";
		case FaceShapeAreaOptions::FACE_SHAPE_AREA_FACELENGTH:
			return "faceLength";
		case FaceShapeAreaOptions::FACE_SHAPE_AREA_FACEWIDTH:
			return "faceWidth";
		case FaceShapeAreaOptions::FACE_SHAPE_AREA_CHEEKBONE:
			return "cheekBone";
		case FaceShapeAreaOptions::FACE_SHAPE_AREA_CHEEK:
			return "cheek";
		case FaceShapeAreaOptions::FACE_SHAPE_AREA_CHIN:
			return "chin";
		case FaceShapeAreaOptions::FACE_SHAPE_AREA_EYESCALE:
			return "eyeScale";
		case FaceShapeAreaOptions::FACE_SHAPE_AREA_NOSELENGTH:
			return "noseLength";
		case FaceShapeAreaOptions::FACE_SHAPE_AREA_NOSEWIDTH:
			return "noseWidth";
		case FaceShapeAreaOptions::FACE_SHAPE_AREA_MOUTHSCALE:
			return "mouthScale";
		default:
			return "";
	}
}

void CDlgBeautyEx2::OnBnClickedCheckMakeUp()
{
	m_makeupOptions.enable_mu = mCbMakeup.GetCheck();
	SetBeauty();
}

void CDlgBeautyEx2::OnCbnSelchangeComboBrowStyle()
{
	m_makeupOptions.browStyle = mDdBrowStyle.GetCurSel();
	SetBeauty();
}

void CDlgBeautyEx2::OnCbnSelchangeComboBrowColor()
{
	m_makeupOptions.browColor = mDdBrowColor.GetCurSel();
	SetBeauty();
}


void CDlgBeautyEx2::OnNMCustomdrawSliderBrowStrength(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	m_makeupOptions.browStrength = mSdBrowStrength.GetPos() / 100.0f;
	SetBeauty();
	*pResult = 0;
}
void CDlgBeautyEx2::OnNMCustomdrawSliderLashStrength(NMHDR* pNMHDR, LRESULT* pResult) {
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	m_makeupOptions.lashStrength = mSdLashStrength.GetPos() / 100.0f;
	SetBeauty();
	*pResult = 0;
}
void CDlgBeautyEx2::OnNMCustomdrawSliderShadowStrength(NMHDR* pNMHDR, LRESULT* pResult) {
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	m_makeupOptions.shadowStrength = mSdShadowStrenght.GetPos() / 100.0f;
	SetBeauty();
	*pResult = 0;
}
void CDlgBeautyEx2::OnNMCustomdrawSliderPupilStrength(NMHDR* pNMHDR, LRESULT* pResult) {
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	m_makeupOptions.pupilStrength = mSdPupilStrength.GetPos() / 100.0f;
	SetBeauty();
	*pResult = 0;
}
void CDlgBeautyEx2::OnNMCustomdrawSliderBlushStrength(NMHDR* pNMHDR, LRESULT* pResult) {
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	m_makeupOptions.blushStrength = mSdBlushStrength.GetPos() / 100.0f;
	SetBeauty();
	*pResult = 0;
}
void CDlgBeautyEx2::OnNMCustomdrawSliderLipStrength(NMHDR* pNMHDR, LRESULT* pResult) {
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	m_makeupOptions.lipStrength = mSdLipStrength.GetPos() / 100.0f;
	SetBeauty();
	*pResult = 0;
}



void CDlgBeautyEx2::OnCbnSelchangeComboLashStyle()
{
	m_makeupOptions.lashStyle = mDdLashStyle.GetCurSel();
	SetBeauty();
}


void CDlgBeautyEx2::OnCbnSelchangeComboLashColor()
{
	m_makeupOptions.lashColor = mDdLashColor.GetCurSel();
	SetBeauty();
}


void CDlgBeautyEx2::OnCbnSelchangeComboShadowStyle()
{
	m_makeupOptions.shadowStyle = mDdShadowStyle.GetCurSel();
	SetBeauty();
}


void CDlgBeautyEx2::OnCbnSelchangeComboPupilStyle()
{
	m_makeupOptions.pupilStyle = mDdPupilStyle.GetCurSel();
	SetBeauty();
}


void CDlgBeautyEx2::OnCbnSelchangeComboBlushStyle()
{
	m_makeupOptions.blushStyle = mDdBlushStyle.GetCurSel();
	SetBeauty();
}


void CDlgBeautyEx2::OnCbnSelchangeComboBlushColor()
{
	m_makeupOptions.blushColor = mDdBlushColor.GetCurSel();
	SetBeauty();
}


void CDlgBeautyEx2::OnCbnSelchangeComboLipStyle()
{
	m_makeupOptions.lipStyle = mDdLipStyle.GetCurSel();
	SetBeauty();
}


void CDlgBeautyEx2::OnCbnSelchangeComboLipColor()
{
	m_makeupOptions.lipColor = mDdLipColor.GetCurSel();
	SetBeauty();
}


void CDlgBeautyEx2::OnBnClickedCheckBeautyShape() {
	SetBeauty();
}


void CDlgBeautyEx2::OnCbnSelchangeComboFaceShapeArea()
{
	m_faceShapeAreaOptions.shapeArea = (FaceShapeAreaOptions::FACE_SHAPE_AREA)(mDdShapeArea.GetCurSel() - 1);
	SetBeauty();
}


void CDlgBeautyEx2::OnNMCustomdrawSliderShpaeAreaIntensity(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	m_faceShapeAreaOptions.shapeIntensity = mSdShapeAreaIntensity.GetPos();
	SetBeauty();
	*pResult = 0;
}

void CDlgBeautyEx2::OnNMCustomdrawSliderShpaeStyleIntensity(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	m_faceShapeBeautyOptions.styleIntensity = mSdShapeStyleIntensity.GetPos();
	SetBeauty();
	*pResult = 0;
}


void CDlgBeautyEx2::OnCbnSelchangeComboFaceShapeStyle()
{
	m_faceShapeBeautyOptions.shapeStyle = (FaceShapeBeautyOptions::FACE_SHAPE_BEAUTY_STYLE)mDdShapeStyle.GetCurSel();
	SetBeauty();
}

int CDlgBeautyEx2::MapUIToResourceId(const CString& resourceType, int uiIndex)
{
	if (resourceType == _T("eyebrow")) {
		return uiIndex;
	}
	else if (resourceType == _T("eyelash")) {
		int lashMapping[] = {0, 3, 5};
		return (uiIndex < 3) ? lashMapping[uiIndex] : 0;
	}
	else if (resourceType == _T("eyeshadow")) {
		int shadowMapping[] = {0, 1, 6};
		return (uiIndex < 3) ? shadowMapping[uiIndex] : 0;
	}
	else if (resourceType == _T("blush")) {
		int blushMapping[] = {0, 1, 2, 4, 9};
		return (uiIndex < 5) ? blushMapping[uiIndex] : 0;
	}
	else if (resourceType == _T("pupil")) {
		int pupilMapping[] = {0, 2};
		return (uiIndex < 2) ? pupilMapping[uiIndex] : 0;
	}
	
	return uiIndex;
}
