#pragma once
#include "AGVideoWnd.h"

class CDlgBeauty2;

struct MakeupOptions2
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

	MakeupOptions2()
	{
		reset();
	}

};

class CDlgBeautyEx2 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgBeautyEx2)

public:
	CDlgBeautyEx2(CWnd* pParent = nullptr);
	virtual ~CDlgBeautyEx2();
	enum
	{
		IDD = IDD_DIALOG_BEAUTY_EX
	};
private:
	bool* m_initialize = nullptr;
	agora::rtc::IRtcEngine** m_rtcEngine = nullptr;
	CDlgBeauty2* m_beautyDlg = nullptr;
	MakeupOptions2 m_makeupOptions;
	FaceShapeAreaOptions m_faceShapeAreaOptions;
	FaceShapeBeautyOptions m_faceShapeBeautyOptions;
	
	// 使用智能指针管理VideoEffectObject
	agora_refptr<agora::rtc::IVideoEffectObject> m_videoEffectObjectRef;

	// 辅助方法声明
	std::string GetFaceShapeAreaParamName(FaceShapeAreaOptions::FACE_SHAPE_AREA area);
	int MapUIToResourceId(const CString& resourceType, int uiIndex);
	void InitializeBeautyResources();
	void CleanupBeautyResources();
	void HideFaceShapeControls();
	CString GetExePath();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()

public:
	void initData(bool* initialize, agora::rtc::IRtcEngine** engine, CDlgBeauty2* beautyDlg);
	void InitCtrlText();
	void InitCtrlData();
	void SetBeauty();
	
	// 移除SetVideoEffectObject方法，改为内部创建
	void ApplyMakeupEffect();
	void ApplyFaceShapeEffect();

	// UI控件成员变量
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
	
	// 消息处理函数
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
