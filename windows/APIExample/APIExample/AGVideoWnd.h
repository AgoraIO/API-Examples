#pragma once

#define WM_SHOWMODECHANGED	WM_USER+300
#define WM_LBUTTON_DOWN_WND			WM_USER+301
#define WM_LBUTTON_UP_WND			WM_USER+302
#define WND_VIDEO_WIDTH     192
#define WND_VIDEO_HEIGHT    144

#define WND_INFO_WIDTH      130
#define WND_INFO_HEIGHT     130

class CAGInfoWnd : public CWnd
{
	DECLARE_DYNAMIC(CAGInfoWnd)

public:
	CAGInfoWnd();
	virtual ~CAGInfoWnd();

	void ShowTips(BOOL bShow = TRUE, BOOL bIsRemote = TRUE);
	void SetVideoResolution(UINT nWidth, UINT nHeight);
	void SetFrameRateInfo(UINT nFPS);
	void SetVideoBitrate(UINT nBitrate);
	void SetAudioBitrate(UINT bitrate);
	void SetVideoLossRate(UINT lossRate);
	void SetAudioLossRate(UINT lossRate);
	void SetAudioDelay(UINT delay);
	void SetVideoDelay(UINT delay);

	void Reset();

protected:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	DECLARE_MESSAGE_MAP()

private:
	BOOL		m_bShowTip;

	COLORREF	m_crBackColor;

	UINT		m_nWidth = 0;
	UINT		m_nHeight = 0;
	UINT		m_nFps = 0;
	UINT		m_nVideoBitrate = 0;
	UINT		m_nAudioBitrate = 0;
	UINT		m_nVideoLossRate = 0;
	UINT		m_nAudioLossRate = 0;
	UINT		m_nAudioDelay = 0;
	UINT		m_nVideoDelay = 0;
	BOOL		m_isRemote;
	CBrush		m_brBack;
};


class CAGVideoWnd : public CWnd
{
	DECLARE_DYNAMIC(CAGVideoWnd)

public:
	CAGVideoWnd();
	virtual ~CAGVideoWnd();

	void SetUID(UINT dwUID);

	UINT GetUID();
	BOOL IsWndFree();

	void SetFaceColor(COLORREF crBackColor);
	BOOL SetBackImage(UINT nID, UINT nWidth, UINT nHeight, COLORREF crMask = RGB(0xFF, 0xff, 0xFF));


	void SetVideoStatsInfo(UINT nWidth, UINT nHeight, UINT nFps, UINT nBitrate, UINT nLossRate, UINT delay = 0);
	void SetAudioStatsInfo(UINT nBitrate, UINT lossRate, UINT delay = 0);
	void ShowStatsInfo(BOOL bShow, BOOL bIsRemote = TRUE);

	void Reset();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

private:
	CImageList		m_imgBackGround;
	COLORREF		m_crBackColor;

	CAGInfoWnd		m_wndInfo;
	UINT			m_nUID;
};


