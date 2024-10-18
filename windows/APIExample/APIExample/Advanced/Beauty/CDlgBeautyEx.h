#pragma once
#include "AGVideoWnd.h"

class CDlgBeauty;

struct MakeupOptions
{
	bool enable_mu;

	int browStyle;
	int browColor;
	float browStrength;

	int lashStyle;
	int lashColor;
	float lashStrength;

	int shadowStyle;
	float shadowStrength;

	int pupilStyle;
	float pupilStrength;

	int blushStyle;
	int blushColor;
	float blushStrength;

	int lipStyle;
	int lipColor;
	float lipStrength;

	void reset()
	{
		enable_mu = false;

		browStyle = 0;
		browColor = 0;
		browStrength = 0.0f;

		lashStyle = 0;
		lashColor = 0;
		lashStrength = 0.0f;

		shadowStyle = 0;
		shadowStrength = 0.0f;

		pupilStyle = 0;
		pupilStrength = 0.0f;

		blushStyle = 0;
		blushColor = 0;
		blushStrength = 0.0f;

		lipStyle = 0;
		lipColor = 0;
		lipStrength = 0.0f;
	}

	MakeupOptions()
	{
		reset();
	}
	//to json string
	std::string toJsonString()
	{
		std::string jsonStr = "{\"enable_mu\":";
		jsonStr += enable_mu ? "true" : "false";
		jsonStr += ",\"browStyle\":" + std::to_string(browStyle);
		jsonStr += ",\"browColor\":" + std::to_string(browColor);
		jsonStr += ",\"browStrength\":" + std::to_string(browStrength);
		jsonStr += ",\"lashStyle\":" + std::to_string(lashStyle);
		jsonStr += ",\"lashColor\":" + std::to_string(lashColor);
		jsonStr += ",\"lashStrength\":" + std::to_string(lashStrength);
		jsonStr += ",\"shadowStyle\":" + std::to_string(shadowStyle);
		jsonStr += ",\"shadowStrength\":" + std::to_string(shadowStrength);
		jsonStr += ",\"pupilStyle\":" + std::to_string(pupilStyle);
		jsonStr += ",\"pupilStrength\":" + std::to_string(pupilStrength);
		jsonStr += ",\"blushStyle\":" + std::to_string(blushStyle);
		jsonStr += ",\"blushColor\":" + std::to_string(blushColor);
		jsonStr += ",\"blushStrength\":" + std::to_string(blushStrength);
		jsonStr += ",\"lipStyle\":" + std::to_string(lipStyle);
		jsonStr += ",\"lipColor\":" + std::to_string(lipColor);
		jsonStr += ",\"lipStrength\":" + std::to_string(lipStrength);
		jsonStr += "}";
		return jsonStr;
	}

};

class CDlgBeautyEx : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgBeautyEx)

public:
	CDlgBeautyEx(CWnd* pParent = nullptr); // standard constructor
	virtual ~CDlgBeautyEx();
	enum
	{
		IDD = IDD_DIALOG_BEAUTY_EX
	};
private:
	bool* m_initialize = nullptr;
	agora::rtc::IRtcEngine** m_rtcEngine = nullptr;
	CDlgBeauty* m_beautyDlg = nullptr;
	MakeupOptions m_makeupOptions;
	FaceShapeAreaOptions m_faceShapeAreaOptions;
	FaceShapeBeautyOptions m_faceShapeBeautyOptions;
protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()

public:
	void initData(bool* initialize, agora::rtc::IRtcEngine** engine, CDlgBeauty* beautyDlg);
	void InitCtrlText();
	void InitCtrlData();
	void SetBeauty();

	CButton mCbMakeup;
	CComboBox mDdBrowColor;
	CComboBox mDdBrowStyle;
	CSliderCtrl mSdBrowStrength;
	CComboBox mDdLashStyle;
	CComboBox mDdLashColor;
	CSliderCtrl mSdLashStrength;
	CComboBox mDdShadowStyle;
	CSliderCtrl mSdShadowStrenght;
	CComboBox mDdPupilStyle;
	CSliderCtrl mSdPupilStrength;
	CComboBox mDdBlushStyle;
	CComboBox mDdBlushColor;
	CSliderCtrl mSdBlushStrength;
	CComboBox mDdLipStyle;
	CComboBox mDdLipColor;
	CSliderCtrl mSdLipStrength;
	CButton mCbBeautyShape;
	CComboBox mDdShapeArea;
	CSliderCtrl mSdShapeAreaIntensity;
	CComboBox mDdShapeStyle;
	CSliderCtrl mSdShapeStyleIntensity;
	afx_msg void OnBnClickedCheckMakeUp();
	afx_msg void OnCbnSelchangeComboBrowColor();
	afx_msg void OnNMCustomdrawSliderBrowStrength(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomdrawSliderLashStrength(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomdrawSliderShadowStrength(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomdrawSliderPupilStrength(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomdrawSliderBlushStrength(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomdrawSliderLipStrength(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCbnSelchangeComboLashStyle();
	afx_msg void OnCbnSelchangeComboLashColor();
	afx_msg void OnCbnSelchangeComboShadowStyle();
	afx_msg void OnCbnSelchangeComboPupilStyle();
	afx_msg void OnCbnSelchangeComboBlushStyle();
	afx_msg void OnCbnSelchangeComboBlushColor();
	afx_msg void OnCbnSelchangeComboLipStyle();
	afx_msg void OnCbnSelchangeComboLipColor();
	afx_msg void OnBnClickedCheckBeautyShape();
	afx_msg void OnCbnSelchangeComboFaceShapeArea();
	afx_msg void OnNMCustomdrawSliderShpaeAreaIntensity(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomdrawSliderShpaeStyleIntensity(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCbnSelchangeComboFaceShapeStyle();
	afx_msg void OnCbnSelchangeComboBrowStyle();
};
