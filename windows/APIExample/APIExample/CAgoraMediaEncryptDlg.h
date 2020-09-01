#pragma once

class CAgoraMediaEncryptDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAgoraMediaEncryptDlg)

public:
	CAgoraMediaEncryptDlg(CWnd* pParent = nullptr);  
	virtual ~CAgoraMediaEncryptDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MEDIA_ENCRYPT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);   

	DECLARE_MESSAGE_MAP()
};
