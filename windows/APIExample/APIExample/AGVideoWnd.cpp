// AGVideoWnd.cpp : implement file
//

#include "stdafx.h"
#include "APIExample.h"
#include "AGVideoWnd.h"

IMPLEMENT_DYNAMIC(CAGInfoWnd, CWnd)

CAGInfoWnd::CAGInfoWnd()
: m_bShowTip(TRUE)
, m_nWidth(0)
, m_nHeight(0)
, m_nFps(0)
, m_nBitrate(0)
{
	m_brBack.CreateSolidBrush(RGB(0x00, 0xA0, 0xE9));
}

CAGInfoWnd::~CAGInfoWnd()
{
	m_brBack.DeleteObject();
}


BEGIN_MESSAGE_MAP(CAGInfoWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


void CAGInfoWnd::ShowTips(CString str, BOOL bShow)
{
	m_bShowTip = bShow;

	if (bShow)
		ShowWindow(SW_SHOW);
	else 
		ShowWindow(SW_HIDE);
	strText = str;
	Invalidate(TRUE);
}

void CAGInfoWnd::SetVideoResolution(int nWidth, int nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;

	if (m_bShowTip) {
		Invalidate(TRUE);
		UpdateWindow();
	}
}

void CAGInfoWnd::SetFrameRateInfo(int nFPS)
{
	m_nFps = nFPS;

	if (m_bShowTip) {
		Invalidate(TRUE);
		UpdateWindow();
	}
}

void CAGInfoWnd::SetBitrateInfo(int nBitrate)
{
	m_nBitrate = nBitrate;

	if (m_bShowTip) {
		Invalidate(TRUE);
		UpdateWindow();
	}
}

void CAGInfoWnd::OnPaint()
{
	CPaintDC dc(this);
	CRect	rcClient;
	CString strTip;

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(0xFF, 0xFF, 0xFF));
	
	if (m_bShowTip) {
		// 640x480,15fps,400k
		GetClientRect(&rcClient);
		rcClient.top += 4;
		//strTip.Format(_T("%dx%d, %dfps, %dK \n %u"), m_nWidth, m_nHeight, m_nFps, m_nBitrate, m_nUID);
		dc.DrawText(strText, &rcClient, DT_VCENTER | DT_CENTER);
	}
}

BOOL CAGInfoWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO:   add message handle code and /or call defalut values here
	CRect rcClient;

	GetClientRect(&rcClient);
	pDC->FillRect(&rcClient, &m_brBack);
	
	return TRUE;
}

// CAGVideoWnd

IMPLEMENT_DYNAMIC(CAGVideoWnd, CWnd)


CAGVideoWnd::CAGVideoWnd()
: m_nUID(0)
, m_crBackColor(RGB(0x58, 0x58, 0x58))
, m_bShowVideoInfo(FALSE)
, m_bBigShow(FALSE)
{

}

CAGVideoWnd::~CAGVideoWnd()
{
	m_imgBackGround.DeleteImageList();
}


BEGIN_MESSAGE_MAP(CAGVideoWnd, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



// CAGVideoWnd message handle
BOOL CAGVideoWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO:   add message handle code and /or call default values here
	CRect		rcClient;
	CPoint		ptDraw;
	//IMAGEINFO	imgInfo;

	GetClientRect(&rcClient);

	pDC->FillSolidRect(&rcClient, m_crBackColor);
	return TRUE;
}

void CAGVideoWnd::SetUID(UINT nUID)
{
	m_nUID = nUID;

    m_wndInfo.SetUID(nUID);
	//m_wndInfo.ShowWindow(SW_SHOW);
}

UINT CAGVideoWnd::GetUID()
{
	return m_nUID;
}

BOOL CAGVideoWnd::IsWndFree()
{
	return m_nUID == 0 ? TRUE : FALSE;
}

BOOL CAGVideoWnd::SetBackImage(UINT nID, UINT nWidth, UINT nHeight, COLORREF crMask)
{
	CBitmap bmBackImage;

	if (!bmBackImage.LoadBitmap(nID))
		return FALSE;

	m_imgBackGround.DeleteImageList();

	m_imgBackGround.Create(nWidth, nHeight, ILC_COLOR24 | ILC_MASK, 1, 1);
	m_imgBackGround.Add(&bmBackImage, crMask);
	bmBackImage.DeleteObject();

	Invalidate(TRUE);

	return TRUE;
}

void CAGVideoWnd::SetFaceColor(COLORREF crBackColor)
{
	m_crBackColor = crBackColor;

	Invalidate(TRUE);
}

void CAGVideoWnd::SetVideoResolution(UINT nWidth, UINT nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;

	m_wndInfo.SetVideoResolution(nWidth, nHeight);
}

void CAGVideoWnd::GetVideoResolution(UINT *nWidth, UINT *nHeight)
{
	*nWidth = m_nWidth;
	*nHeight = m_nHeight;
}

void CAGVideoWnd::SetBitrateInfo(int nReceivedBitrate)
{
	m_nBitRate = nReceivedBitrate;
	m_wndInfo.SetBitrateInfo(nReceivedBitrate);
}

void CAGVideoWnd::SetFrameRateInfo(int nReceiveFrameRate)
{
	m_nFrameRate = nReceiveFrameRate;
	m_wndInfo.SetFrameRateInfo(nReceiveFrameRate);
}

void CAGVideoWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  add message handle code and /or call default values here

	::SendMessage(GetParent()->GetSafeHwnd(), WM_LBUTTON_DOWN_WND, (WPARAM)point.x, (LPARAM)point.y);

	CWnd::OnLButtonDown(nFlags, point);
}

void CAGVideoWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  add message handle code and /or call default values here

	::SendMessage(GetParent()->GetSafeHwnd(), WM_LBUTTON_UP_WND, (WPARAM)point.x, (LPARAM)point.y);

	CWnd::OnLButtonDown(nFlags, point);
}

void CAGVideoWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  add message handle code and /or call default values here
	::SendMessage(GetParent()->GetSafeHwnd(), WM_SHOWMODECHANGED, (WPARAM)this, (LPARAM)m_nUID);

	CWnd::OnRButtonDown(nFlags, point);
}


int CAGVideoWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  add you own creation code here
	m_wndInfo.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, WND_INFO_WIDTH, WND_INFO_HEIGHT), this, IDC_STATIC);
    m_wndInfo.ShowWindow(SW_HIDE);
	return 0;
}


void CAGVideoWnd::ShowVideoInfo(CString str, BOOL bShow)
{
	m_bShowVideoInfo = bShow;

	m_wndInfo.ShowTips(str, bShow);
	Invalidate(TRUE);
}

void CAGVideoWnd::SetBigShowFlag(BOOL bBigShow)
{
	CRect	rcClient;

	m_bBigShow = bBigShow;
	GetClientRect(&rcClient);

	int x = (rcClient.Width()- WND_INFO_WIDTH) / 2;
	int y = rcClient.Height() - WND_INFO_HEIGHT;
	
	if (m_wndInfo.GetSafeHwnd() != NULL) {
		if (m_bBigShow)
			y -= 4;

		m_wndInfo.MoveWindow(x, y, WND_INFO_WIDTH, WND_INFO_HEIGHT);
	}
};


void CAGVideoWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	int x = (cx - WND_INFO_WIDTH) / 2;
	int y = cy - WND_INFO_HEIGHT;
	// TODO:  add message handle code here
	if (m_wndInfo.GetSafeHwnd() != NULL) {
		if (m_bBigShow)
			y -= 4;

		m_wndInfo.MoveWindow(x, y, WND_INFO_WIDTH, WND_INFO_HEIGHT);
	}
}

void CAGVideoWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO:  add message handle code and /or call default values here
	::SendMessage(GetParent()->GetSafeHwnd(), WM_SHOWMODECHANGED, (WPARAM)this, (LPARAM)m_nUID);

	CWnd::OnLButtonDblClk(nFlags, point);
}
