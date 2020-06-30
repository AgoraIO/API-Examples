#pragma once
#include "AGVideoWnd.h"
#include <set>
class CAgoraRtmpStreamingDlgRtcEngineEventHandler
    : public IRtcEngineEventHandler
{
public:
    CAgoraRtmpStreamingDlgRtcEngineEventHandler() {}
    ~CAgoraRtmpStreamingDlgRtcEngineEventHandler() {}
    void SetMsgReceiver(HWND hWnd) { m_hMsgHanlder = hWnd; }
    virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) override;
    virtual void onLeaveChannel(const RtcStats& stats) override;
    virtual void onUserJoined(uid_t uid, int elapsed) override;
    virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) override;
    virtual void onRtmpStreamingStateChanged(const char *url, RTMP_STREAM_PUBLISH_STATE state, RTMP_STREAM_PUBLISH_ERROR errCode)override;

private:
    HWND m_hMsgHanlder;
};

// CAgoraRtmpStreamingDlg dialog

class CAgoraRtmpStreamingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAgoraRtmpStreamingDlg)

public:
	CAgoraRtmpStreamingDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CAgoraRtmpStreamingDlg();
    bool InitAgora();
    void UnInitAgora();
// Dialog Data
    enum { IDD = IDD_DIALOG_RTMP_STREAMING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
    void InitCtrlText();
    void RenderLocalVideo();
    void RemoveAllRtmpUrls();

    IRtcEngine* m_rtcEngine = nullptr;
    CAgoraRtmpStreamingDlgRtcEngineEventHandler m_eventHandler;
    bool joinChannel = false;
    bool m_initialize = false;
    CAGVideoWnd m_localVideoWnd;
    std::set<CString> m_urlSet;
    int  m_removeUrlCount = 0;
    bool m_bRemoveAll = false;
   
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnBnClickedButtonJoinchannel();
    afx_msg void OnBnClickedButtonAddstream();
    afx_msg void OnBnClickedButtonRemoveStream();
    afx_msg void OnBnClickedButtonRemoveAllstream();

    afx_msg LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDRtmpStateChanged(WPARAM wParam, LPARAM lParam);
    afx_msg void OnSelchangeListInfoBroadcasting();
    CEdit m_edtChannelName;
    CEdit m_edtRtmpUrl;
    CButton m_btnAddStream;
    CButton m_btnRemoveStream;
    CComboBox m_cmbRtmpUrl;
    CStatic m_staChannelName;
    CStatic m_staPublishUrl;
    CButton m_btnRemoveAll;
    CStatic m_staRemoveUrl;
    // m_staVideoArea
    CButton m_btnJoinChannel;
    CListBox m_lstInfo;
    CStatic m_staVideoArea;
    CStatic m_staDetail;
};
