// AGVideoWnd.cpp : implement file
//

#include "stdafx.h"
#include "APIExample.h"
#include "AGVideoWnd.h"
#include <Gdiplus.h>

IMPLEMENT_DYNAMIC(CAGInfoWnd, CWnd)

CAGInfoWnd::CAGInfoWnd()
	: m_bShowTip(TRUE)
	, m_nWidth(0)
	, m_nHeight(0)
	, m_nFps(0)
	, m_nVideoBitrate(0)
{
	m_brBack.CreateSolidBrush(RGB(0x58, 0x58, 0x58));
}

CAGInfoWnd::~CAGInfoWnd()
{
	m_brBack.DeleteObject();
}


BEGIN_MESSAGE_MAP(CAGInfoWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


void CAGInfoWnd::ShowTips(BOOL bShow, BOOL bIsRemote)
{
	if (m_bShowTip != bShow) {
		if (bShow)
			ShowWindow(SW_SHOW);
		else
			ShowWindow(SW_HIDE);
	}
	
	m_bShowTip = bShow;
	m_isRemote = bIsRemote;
	Invalidate(TRUE);
}

void CAGInfoWnd::SetVideoResolution(UINT nWidth, UINT nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;

	if (m_bShowTip) {
		Invalidate(TRUE);
	}
}

void CAGInfoWnd::SetFrameRateInfo(UINT nFPS)
{
	m_nFps = nFPS;

	if (m_bShowTip) {
		Invalidate(TRUE);
	}
}

void CAGInfoWnd::SetVideoBitrate(UINT nBitrate)
{
	m_nVideoBitrate = nBitrate;

	if (m_bShowTip) {
		Invalidate(TRUE);
	}
}


void CAGInfoWnd::SetAudioBitrate(UINT bitrate)
{
	m_nAudioBitrate = bitrate;

	if (m_bShowTip) {
		Invalidate(TRUE);
	}
}


void CAGInfoWnd::SetVideoLossRate(UINT lossRate)
{
	m_nVideoLossRate = lossRate;

	if (m_bShowTip) {
		Invalidate(TRUE);
	}
}


void CAGInfoWnd::SetAudioLossRate(UINT lossRate)
{
	m_nAudioLossRate = lossRate;

	if (m_bShowTip) {
		Invalidate(TRUE);
	}
}


void CAGInfoWnd::SetAudioDelay(UINT delay)
{
	m_nAudioDelay = delay;
	if (m_bShowTip) {
		Invalidate(TRUE);
	}
}


void CAGInfoWnd::SetVideoDelay(UINT delay)
{
	m_nVideoDelay = delay;
	if (m_bShowTip) {
		Invalidate(TRUE);
	}
}


void CAGInfoWnd::Reset()
{
	m_nWidth = m_nHeight = m_nFps = m_nVideoBitrate
		= m_nVideoLossRate = m_nVideoDelay
		= m_nAudioBitrate = m_nAudioLossRate
		= m_nAudioDelay = 0;
	if (m_bShowTip) {
		Invalidate(TRUE);
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
		if (m_isRemote) {
			strTip.Format(_T("%dx%d, %dfps\nVRecv: %dkbps\nVLossRate: %d%%\nVDelay: %dms\nARecv: %dkbps\nALossRate: %d%%\nADelay: %dms"),
				m_nWidth, m_nHeight, m_nFps, m_nVideoBitrate, m_nVideoLossRate, m_nVideoDelay, m_nAudioBitrate, m_nAudioLossRate, m_nAudioDelay);
		}
		else {
			strTip.Format(_T("%dx%d, %dfps\nVSent: %dkbps\nVLossRate: %d%%\nASent: %dkbps\nALossRate: %d%%"),
				m_nWidth, m_nHeight, m_nFps, m_nVideoBitrate, m_nVideoLossRate, m_nAudioBitrate, m_nAudioLossRate);
		}
		
		dc.DrawText(strTip, &rcClient, DT_VCENTER | DT_CENTER);
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


void CAGVideoWnd::SetVideoStatsInfo(UINT nWidth, UINT nHeight, UINT nFps, UINT nBitrate, UINT lossRate, UINT delay)
{
	m_wndInfo.SetVideoResolution(nWidth, nHeight);
	m_wndInfo.SetFrameRateInfo(nFps);
	m_wndInfo.SetVideoBitrate(nBitrate);
	m_wndInfo.SetVideoLossRate(lossRate);
	m_wndInfo.SetVideoDelay(delay);
}


void CAGVideoWnd::SetAudioStatsInfo(UINT nBitrate, UINT lossRate, UINT delay)
{
	m_wndInfo.SetAudioBitrate(nBitrate);
	m_wndInfo.SetAudioLossRate(lossRate);
	m_wndInfo.SetAudioDelay(delay);
}

void CAGVideoWnd::SetFaceColor(COLORREF crBackColor)
{
	m_crBackColor = crBackColor;

	Invalidate(TRUE);
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
	m_wndInfo.ShowTips(FALSE);
	return 0;
}


void CAGVideoWnd::ShowStatsInfo(BOOL bShow, BOOL bIsRemote)
{
	m_wndInfo.ShowTips(bShow, bIsRemote);
	if (!bIsRemote) {
		m_wndInfo.MoveWindow(0, 0, WND_INFO_WIDTH, WND_INFO_HEIGHT - 25);
	}
	
}


void CAGVideoWnd::Reset()
{
	m_wndInfo.Reset();
	Invalidate(TRUE);
}

void CAGVideoWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
}

void CAGVideoWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO:  add message handle code and /or call default values here
	::SendMessage(GetParent()->GetSafeHwnd(), WM_SHOWMODECHANGED, (WPARAM)this, (LPARAM)m_nUID);

	CWnd::OnLButtonDblClk(nFlags, point);
}
