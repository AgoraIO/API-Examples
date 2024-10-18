#include "stdafx.h"
#include "APIExample.h"
#include "CDlgBeautyEx.h"
#include "CDlgBeauty.h"

IMPLEMENT_DYNAMIC(CDlgBeautyEx, CDialogEx)

CDlgBeautyEx::CDlgBeautyEx(CWnd* pParent /*=nullptr*/) : CDialogEx(IDD_DIALOG_BEAUTY_EX, pParent)
{
}

CDlgBeautyEx::~CDlgBeautyEx()
{
}

void CDlgBeautyEx::DoDataExchange(CDataExchange* pDX)
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

BEGIN_MESSAGE_MAP(CDlgBeautyEx, CDialogEx)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CHECK_MAKE_UP, &CDlgBeautyEx::OnBnClickedCheckMakeUp)
	ON_CBN_SELCHANGE(IDC_COMBO_BROW_COLOR, &CDlgBeautyEx::OnCbnSelchangeComboBrowColor)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER__BROW_STRENGTH, &CDlgBeautyEx::OnNMCustomdrawSliderBrowStrength)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER__LASH_STRENGTH, &CDlgBeautyEx::OnNMCustomdrawSliderLashStrength)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER__SHADOW_STRENGTH, &CDlgBeautyEx::OnNMCustomdrawSliderShadowStrength)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER__Pupil_STRENGTH, &CDlgBeautyEx::OnNMCustomdrawSliderPupilStrength)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER__BLUSH_STRENGTH, &CDlgBeautyEx::OnNMCustomdrawSliderBlushStrength)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER__LIP_STRENGTH, &CDlgBeautyEx::OnNMCustomdrawSliderLipStrength)
	ON_CBN_SELCHANGE(IDC_COMBO_LASH_STYLE, &CDlgBeautyEx::OnCbnSelchangeComboLashStyle)
	ON_CBN_SELCHANGE(IDC_COMBO_LASH_COLOR, &CDlgBeautyEx::OnCbnSelchangeComboLashColor)
	ON_CBN_SELCHANGE(IDC_COMBO_SHADOW_STYLE, &CDlgBeautyEx::OnCbnSelchangeComboShadowStyle)
	ON_CBN_SELCHANGE(IDC_COMBO_PUPIL_STYLE, &CDlgBeautyEx::OnCbnSelchangeComboPupilStyle)
	ON_CBN_SELCHANGE(IDC_COMBO_BLUSH_STYLE, &CDlgBeautyEx::OnCbnSelchangeComboBlushStyle)
	ON_CBN_SELCHANGE(IDC_COMBO_BLUSH_COLOR, &CDlgBeautyEx::OnCbnSelchangeComboBlushColor)
	ON_CBN_SELCHANGE(IDC_COMBO_LIP_STYLE, &CDlgBeautyEx::OnCbnSelchangeComboLipStyle)
	ON_CBN_SELCHANGE(IDC_COMBO_LIP_COLOR, &CDlgBeautyEx::OnCbnSelchangeComboLipColor)
	ON_BN_CLICKED(IDC_CHECK_BEAUTY_SHAPE, &CDlgBeautyEx::OnBnClickedCheckBeautyShape)
	ON_CBN_SELCHANGE(IDC_COMBO_FACE_SHAPE_AREA, &CDlgBeautyEx::OnCbnSelchangeComboFaceShapeArea)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER__SHAPE_AREA_INTENSITY, &CDlgBeautyEx::OnNMCustomdrawSliderShpaeAreaIntensity)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER__SHAPE_STYLE_INTENSITY, &CDlgBeautyEx::OnNMCustomdrawSliderShpaeStyleIntensity)
	ON_CBN_SELCHANGE(IDC_COMBO_FACE_SHAPE_STYLE, &CDlgBeautyEx::OnCbnSelchangeComboFaceShapeStyle)
	ON_CBN_SELCHANGE(IDC_COMBO_BROW_STYLE, &CDlgBeautyEx::OnCbnSelchangeComboBrowStyle)
END_MESSAGE_MAP()

void CDlgBeautyEx::OnClose()
{
	if (m_beautyDlg) {
		m_beautyDlg->UpdateExtentCbState(false);
	}
	CDialogEx::OnClose();
}

BOOL CDlgBeautyEx::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitCtrlText();
	return TRUE;
}


void CDlgBeautyEx::initData(bool* initialize, agora::rtc::IRtcEngine** engine, CDlgBeauty* beautyDlg)
{
	m_initialize = initialize;
	m_rtcEngine = engine;
	m_beautyDlg = beautyDlg;
}

void CDlgBeautyEx::InitCtrlData()
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
void CDlgBeautyEx::InitCtrlText()
{
	//make up
	mDdBrowStyle.ResetContent();
	mDdBrowStyle.InsertString(0,TEXT("CLOSE"));
	mDdBrowStyle.InsertString(1,TEXT("TYPE1"));
	mDdBrowStyle.InsertString(2,TEXT("TYPE2"));
	mDdBrowColor.ResetContent();
	mDdBrowColor.InsertString(0,TEXT("NONE"));
	mDdBrowColor.InsertString(1,TEXT("BLACK"));
	mDdBrowColor.InsertString(2,TEXT("BROWN"));

	mDdLashStyle.ResetContent();
	mDdLashStyle.InsertString(0,TEXT("CLOSE"));
	mDdLashStyle.InsertString(1,TEXT("TYPE1"));
	mDdLashStyle.InsertString(2,TEXT("TYPE2"));
	mDdLashColor.ResetContent();
	mDdLashColor.InsertString(0,TEXT("NONE"));
	mDdLashColor.InsertString(1,TEXT("BLACK"));
	mDdLashColor.InsertString(2,TEXT("BROWN"));

	mDdShadowStyle.ResetContent();
	mDdShadowStyle.InsertString(0,TEXT("CLOSE"));
	mDdShadowStyle.InsertString(1,TEXT("TYPE1"));
	mDdShadowStyle.InsertString(2,TEXT("TYPE2"));

	mDdPupilStyle.ResetContent();
	mDdPupilStyle.InsertString(0,TEXT("CLOSE"));
	mDdPupilStyle.InsertString(1,TEXT("TYPE1"));
	mDdPupilStyle.InsertString(2,TEXT("TYPE2"));

	mDdBlushStyle.ResetContent();
	mDdBlushStyle.InsertString(0,TEXT("CLOSE"));
	mDdBlushStyle.InsertString(1,TEXT("TYPE1"));
	mDdBlushStyle.InsertString(2,TEXT("TYPE2"));
	mDdBlushColor.ResetContent();
	mDdBlushColor.InsertString(0,TEXT("NONE"));
	mDdBlushColor.InsertString(1,TEXT("COLOR1"));
	mDdBlushColor.InsertString(2,TEXT("COLOR2"));
	mDdBlushColor.InsertString(3,TEXT("COLOR3"));
	mDdBlushColor.InsertString(4,TEXT("COLOR4"));
	mDdBlushColor.InsertString(5,TEXT("COLOR5"));

	mDdLipStyle.ResetContent();
	mDdLipStyle.InsertString(0,TEXT("CLOSE"));
	mDdLipStyle.InsertString(1,TEXT("TYPE1"));
	mDdLipStyle.InsertString(2,TEXT("TYPE2"));
	mDdLipColor.ResetContent();
	mDdLipColor.InsertString(0,TEXT("NONE"));
	mDdLipColor.InsertString(1,TEXT("COLOR1"));
	mDdLipColor.InsertString(2,TEXT("COLOR2"));
	mDdLipColor.InsertString(3,TEXT("COLOR3"));
	mDdLipColor.InsertString(4,TEXT("COLOR4"));
	mDdLipColor.InsertString(5,TEXT("COLOR5"));

	//beauty shape
	mDdShapeArea.ResetContent();
	mDdShapeArea.InsertString(0,TEXT("FACE_SHAPE_AREA_NONE"));
	mDdShapeArea.InsertString(1,TEXT("FACE_SHAPE_AREA_HEADSCALE"));
	mDdShapeArea.InsertString(2,TEXT("FACE_SHAPE_AREA_FOREHEAD"));
	mDdShapeArea.InsertString(3,TEXT("FACE_SHAPE_AREA_FACECONTOUR"));
	mDdShapeArea.InsertString(4,TEXT("FACE_SHAPE_AREA_FACELENGTH"));
	mDdShapeArea.InsertString(5,TEXT("FACE_SHAPE_AREA_FACEWIDTH"));
	mDdShapeArea.InsertString(6,TEXT("FACE_SHAPE_AREA_CHEEKBONE"));
	mDdShapeArea.InsertString(7,TEXT("FACE_SHAPE_AREA_CHEEK"));
	mDdShapeArea.InsertString(8,TEXT("FACE_SHAPE_AREA_CHIN"));
	mDdShapeArea.InsertString(9,TEXT("FACE_SHAPE_AREA_EYESCALE"));
	mDdShapeArea.InsertString(10,TEXT("FACE_SHAPE_AREA_NOSELENGTH"));
	mDdShapeArea.InsertString(11,TEXT("FACE_SHAPE_AREA_NOSEWIDTH"));
	mDdShapeArea.InsertString(12,TEXT("FACE_SHAPE_AREA_MOUTHSCALE"));

	mDdShapeStyle.ResetContent();
	mDdShapeStyle.InsertString(0,TEXT("FACE_SHAPE_STYLE_FEMALE"));
	mDdShapeStyle.InsertString(1,TEXT("FACE_SHAPE_STYLE_MALE"));

	InitCtrlData();

}

void CDlgBeautyEx::SetBeauty()
{
	if (*m_initialize && *m_rtcEngine)
	{
		std::string str = m_makeupOptions.toJsonString();
		(*m_rtcEngine)->setExtensionProperty("agora_video_filters_clear_vision", "clear_vision", "makeup_options", str.c_str(), PRIMARY_CAMERA_SOURCE);
		bool flag = mCbBeautyShape.GetCheck() != 0;
		(*m_rtcEngine)->setFaceShapeBeautyOptions(flag, m_faceShapeBeautyOptions);
		(*m_rtcEngine)->setFaceShapeAreaOptions(m_faceShapeAreaOptions);
	}
}




void CDlgBeautyEx::OnBnClickedCheckMakeUp()
{
	m_makeupOptions.enable_mu = mCbMakeup.GetCheck();
	SetBeauty();
}

void CDlgBeautyEx::OnCbnSelchangeComboBrowStyle()
{
	m_makeupOptions.browStyle = mDdBrowStyle.GetCurSel();
	SetBeauty();
}

void CDlgBeautyEx::OnCbnSelchangeComboBrowColor()
{
	m_makeupOptions.browColor = mDdBrowColor.GetCurSel();
	SetBeauty();
}


void CDlgBeautyEx::OnNMCustomdrawSliderBrowStrength(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	m_makeupOptions.browStrength = mSdBrowStrength.GetPos() / 100.0f;
	SetBeauty();
	*pResult = 0;
}
void CDlgBeautyEx::OnNMCustomdrawSliderLashStrength(NMHDR* pNMHDR, LRESULT* pResult) {
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	m_makeupOptions.lashStrength = mSdLashStrength.GetPos() / 100.0f;
	SetBeauty();
	*pResult = 0;
}
void CDlgBeautyEx::OnNMCustomdrawSliderShadowStrength(NMHDR* pNMHDR, LRESULT* pResult) {
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	m_makeupOptions.shadowStrength = mSdShadowStrenght.GetPos() / 100.0f;
	SetBeauty();
	*pResult = 0;
}
void CDlgBeautyEx::OnNMCustomdrawSliderPupilStrength(NMHDR* pNMHDR, LRESULT* pResult) {
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	m_makeupOptions.pupilStrength = mSdPupilStrength.GetPos() / 100.0f;
	SetBeauty();
	*pResult = 0;
}
void CDlgBeautyEx::OnNMCustomdrawSliderBlushStrength(NMHDR* pNMHDR, LRESULT* pResult) {
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	m_makeupOptions.blushStrength = mSdBlushStrength.GetPos() / 100.0f;
	SetBeauty();
	*pResult = 0;
}
void CDlgBeautyEx::OnNMCustomdrawSliderLipStrength(NMHDR* pNMHDR, LRESULT* pResult) {
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	m_makeupOptions.lipStrength = mSdLipStrength.GetPos() / 100.0f;
	SetBeauty();
	*pResult = 0;
}



void CDlgBeautyEx::OnCbnSelchangeComboLashStyle()
{
	m_makeupOptions.lashStyle = mDdLashStyle.GetCurSel();
	SetBeauty();
}


void CDlgBeautyEx::OnCbnSelchangeComboLashColor()
{
	m_makeupOptions.lashColor = mDdLashColor.GetCurSel();
	SetBeauty();
}


void CDlgBeautyEx::OnCbnSelchangeComboShadowStyle()
{
	m_makeupOptions.shadowStyle = mDdShadowStyle.GetCurSel();
	SetBeauty();
}


void CDlgBeautyEx::OnCbnSelchangeComboPupilStyle()
{
	m_makeupOptions.pupilStyle = mDdPupilStyle.GetCurSel();
	SetBeauty();
}


void CDlgBeautyEx::OnCbnSelchangeComboBlushStyle()
{
	m_makeupOptions.blushStyle = mDdBlushStyle.GetCurSel();
	SetBeauty();
}


void CDlgBeautyEx::OnCbnSelchangeComboBlushColor()
{
	m_makeupOptions.blushColor = mDdBlushColor.GetCurSel();
	SetBeauty();
}


void CDlgBeautyEx::OnCbnSelchangeComboLipStyle()
{
	m_makeupOptions.lipStyle = mDdLipStyle.GetCurSel();
	SetBeauty();
}


void CDlgBeautyEx::OnCbnSelchangeComboLipColor()
{
	m_makeupOptions.lipColor = mDdLipColor.GetCurSel();
	SetBeauty();
}


void CDlgBeautyEx::OnBnClickedCheckBeautyShape() {
	SetBeauty();
}


void CDlgBeautyEx::OnCbnSelchangeComboFaceShapeArea()
{
	m_faceShapeAreaOptions.shapeArea = (FaceShapeAreaOptions::FACE_SHAPE_AREA)(mDdShapeArea.GetCurSel() - 1);
	SetBeauty();
}


void CDlgBeautyEx::OnNMCustomdrawSliderShpaeAreaIntensity(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	m_faceShapeAreaOptions.shapeIntensity = mSdShapeAreaIntensity.GetPos();
	SetBeauty();
	*pResult = 0;
}

void CDlgBeautyEx::OnNMCustomdrawSliderShpaeStyleIntensity(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	m_faceShapeBeautyOptions.styleIntensity = mSdShapeStyleIntensity.GetPos();
	SetBeauty();
	*pResult = 0;
}


void CDlgBeautyEx::OnCbnSelchangeComboFaceShapeStyle()
{
	m_faceShapeBeautyOptions.shapeStyle = (FaceShapeBeautyOptions::FACE_SHAPE_BEAUTY_STYLE)mDdShapeStyle.GetCurSel();
	SetBeauty();
}

