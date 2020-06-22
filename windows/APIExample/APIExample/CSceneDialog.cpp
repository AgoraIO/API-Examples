#include "stdafx.h"
#include "CSceneDialog.h"

IMPLEMENT_DYNAMIC(CSceneDialog, CDialogEx)

CSceneDialog::CSceneDialog(UINT nIDTemplate, CWnd *pParent)
    :CDialogEx(nIDTemplate, pParent)
{
}


CSceneDialog::~CSceneDialog()
{
}

BEGIN_MESSAGE_MAP(CSceneDialog, CDialogEx)
END_MESSAGE_MAP()