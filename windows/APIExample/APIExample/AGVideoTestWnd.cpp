#include "stdafx.h"
#include "AGVideoTestWnd.h"


// CAGVideoTestWnd

IMPLEMENT_DYNAMIC(CAGVideoTestWnd, CWnd)

CAGVideoTestWnd::CAGVideoTestWnd()
: m_nVolRange(255)
, m_nCurVol(0)
, m_crVolbarFreeColor(RGB(32, 32, 32))
, m_crVolbarBusyColor(RGB(208, 208, 208))
, m_crVolbarBackColor(RGB(0x26, 0x26, 0x26))
, m_crBackColor(RGB(0x70, 0x70, 0x70))
, m_nVolbarWidth(15)
{

}

CAGVideoTestWnd::~CAGVideoTestWnd()
{
}


BEGIN_MESSAGE_MAP(CAGVideoTestWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CAGVideoTestWnd Message handle

int CAGVideoTestWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  add you own creation code here
	CRect		rcChildRect;

	DWORD dwWndStyle = WS_VISIBLE | WS_CHILD;
	rcChildRect.SetRect(15, 0, lpCreateStruct->cx-30, lpCreateStruct->cy);
	m_wndVideoWnd.Create(NULL, _T("AgoraVideoWnd"), dwWndStyle, rcChildRect, this, IDC_STATIC);

	return 0;
}

void CAGVideoTestWnd::OnPaint()
{
	// TODO: add message handle code here
	CPaintDC dc(this);

	CRect rcClient;

	GetClientRect(&rcClient);
	dc.FillSolidRect(0, 0, rcClient.Width(), rcClient.Height(), m_crBackColor);

	dc.FillSolidRect(0, 0, m_nVolbarWidth, rcClient.Height(), m_crVolbarBackColor);
	dc.FillSolidRect(rcClient.Width() - m_nVolbarWidth, 0, m_nVolbarWidth, rcClient.Height(), m_crVolbarBackColor);

	int nMarkCount = rcClient.Height() / 5;
	int nTopPoint = m_nCurVol*nMarkCount / m_nVolRange;

	for (int nIndex = 0; nIndex < nMarkCount; nIndex++) {
		if (nIndex <= nTopPoint) {
			dc.FillSolidRect(0, rcClient.bottom - 5 * nIndex - 3, m_nVolbarWidth, 3, m_crVolbarBusyColor);
			dc.FillSolidRect(rcClient.Width() - m_nVolbarWidth, rcClient.bottom - 5 * nIndex - 3, m_nVolbarWidth, 3, m_crVolbarBusyColor);
		}
		else {
			dc.FillSolidRect(0, rcClient.bottom - 5 * nIndex - 3, m_nVolbarWidth, 3, m_crVolbarFreeColor);
			dc.FillSolidRect(rcClient.Width() - m_nVolbarWidth, rcClient.bottom - 5 * nIndex - 3, m_nVolbarWidth, 3, m_crVolbarFreeColor);
		}
	}
}


void CAGVideoTestWnd::SetVolRange(int nRange)
{
	if (nRange > 100 || nRange < 0)
		nRange = 100;

	m_nVolRange = nRange;

	Invalidate(FALSE);
}

void CAGVideoTestWnd::SetCurVol(int nCurVol)
{
	if (nCurVol < 0 || nCurVol > m_nVolRange)
		nCurVol = 0;
	CRect rcClient;
	GetClientRect(&rcClient);
	m_nCurVol = nCurVol;
	RECT lrc;
	lrc.left = 0;
	lrc.right = m_nVolbarWidth;
	lrc.top = 0;
	lrc.bottom = rcClient.Height();
	InvalidateRect(&lrc);
	RECT rrc;
	rrc.left = rcClient.Width() - m_nVolbarWidth;
	rrc.right = rcClient.Width();
	rrc.top = 0;
	rrc.bottom = rcClient.Height();
	InvalidateRect(&rrc);
}


void CAGVideoTestWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (m_wndVideoWnd.GetSafeHwnd() != NULL)
		m_wndVideoWnd.MoveWindow(15, 0, cx - 30, cy);

	// TODO:   add message handle code here
}
