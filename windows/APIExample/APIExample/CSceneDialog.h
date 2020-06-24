#pragma once
#include <afxdialogex.h>
class CSceneDialog :
    public CDialogEx
{
    DECLARE_DYNAMIC(CSceneDialog)
public:
    
    CSceneDialog(UINT nIDTemplate, CWnd *pParent = NULL);
    ~CSceneDialog();

    virtual bool InitAgora() = 0;
    virtual void UnInitAgora() = 0;
protected:
    DECLARE_MESSAGE_MAP()
};

