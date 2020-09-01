
#include "stdafx.h"
#include "APIExample.h"
#include "CAgoraMediaEncryptDlg.h"



IMPLEMENT_DYNAMIC(CAgoraMediaEncryptDlg, CDialogEx)

CAgoraMediaEncryptDlg::CAgoraMediaEncryptDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MEDIA_ENCRYPT, pParent)
{

}

CAgoraMediaEncryptDlg::~CAgoraMediaEncryptDlg()
{
}

void CAgoraMediaEncryptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAgoraMediaEncryptDlg, CDialogEx)
END_MESSAGE_MAP()


