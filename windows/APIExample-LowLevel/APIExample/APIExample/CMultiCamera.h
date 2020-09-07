#pragma once


// CMultiCamera 对话框

class CMultiCamera : public CDialogEx
{
	DECLARE_DYNAMIC(CMultiCamera)

public:
	CMultiCamera(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CMultiCamera();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MULTICAMERA };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
