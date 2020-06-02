#pragma once
#include <string>
#include <set>
class CAgoraRtmpStreamingRtcEngineEventHandler
    : public IRtcEngineEventHandler
{
public:
    CAgoraRtmpStreamingRtcEngineEventHandler() {}
    ~CAgoraRtmpStreamingRtcEngineEventHandler() {}
    void SetMsgReceiver(HWND hWnd) { m_hMsgHanlder = hWnd; }
    virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) override;
    virtual void onStreamInjectedStatus(const char* url, uid_t uid, int status) override;
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

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_RTMP_PUBLISH };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
    afx_msg LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);

  //  afx_msg LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
  //  afx_msg LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDRtmpStateChanged(WPARAM wParam, LPARAM lParam);

private:
    IRtcEngine* m_rtcEngine;
    CAgoraRtmpStreamingRtcEngineEventHandler m_eventHandlerInjection;
    bool joinChannel = false;
    bool m_initialize = false; 
    CEdit m_edtRtmpUrl;
    CEdit m_edtChannelName;
    CButton m_btnJoinChannel;
    CButton m_btnAddStream;
    CStatic m_staInfo;
    CStatic m_staLocalVideo;
    std::set<CString> m_urlSet;
    int removeUrlCount = 0;
    bool bRemoveAll = false;
public:
    afx_msg void OnBnClickedJoinchannel();
    afx_msg void OnBnClickedButtonAddstream();
    afx_msg void OnBnClickedButtonRemoveallstream();
    afx_msg void OnDestroy();
    afx_msg void OnBnClickedButtonRemovestream();
    virtual BOOL OnInitDialog();
    CComboBox m_cmbRtmpUrl;
    CButton m_btnRemoveStream;

    void RemoveAllRtmpUrls();
};
