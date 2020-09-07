// CMultiCamera.cpp: 实现文件
//

#include "pch.h"
#include "APIExample.h"
#include "CMultiCamera.h"
#include "afxdialogex.h"


// CMultiCamera 对话框

IMPLEMENT_DYNAMIC(CMultiCamera, CDialogEx)

CMultiCamera::CMultiCamera(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MULTICAMERA, pParent)
{

}

CMultiCamera::~CMultiCamera()
{
}

void CMultiCamera::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMultiCamera, CDialogEx)
END_MESSAGE_MAP()


// CMultiCamera 消息处理程序
