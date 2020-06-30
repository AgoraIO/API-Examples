#pragma once
#include "AGVideoWnd.h"

class CAgoraMetaDataObserver : public IMetadataObserver
{
public:
    void SetMsgReceiver(HWND hWnd) { m_hMsgHanlder = hWnd; }
    CAgoraMetaDataObserver(){}
    ~CAgoraMetaDataObserver() {  }
    virtual int getMaxMetadataSize()override;
    virtual bool onReadyToSendMetadata(Metadata &metadata)override;
    virtual void onMetadataReceived(const Metadata &metadata)override;
    void SetMaxMetadataSize(int maxSize);
    void SetSendSEI(std::string utf8Msg);
private:
    int m_maxSize = 1024;
    std::string m_sendSEI;
    HWND m_hMsgHanlder;
};


class CAgoraMetaDataEventHanlder : public IRtcEngineEventHandler
{
public:
    CAgoraMetaDataEventHanlder() {}
    ~CAgoraMetaDataEventHanlder() {}
    void SetMsgReceiver(HWND hWnd) { m_hMsgHanlder = hWnd; }
    virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) override;
    virtual void onUserJoined(uid_t uid, int elapsed) override;
    virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) override;
    virtual void onLeaveChannel(const RtcStats& stats) override;
    virtual void onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed) override;
private:
    HWND m_hMsgHanlder;
};

// CAgoraMetaDataDlg dialog

class CAgoraMetaDataDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAgoraMetaDataDlg)

public:
	CAgoraMetaDataDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CAgoraMetaDataDlg();
    bool InitAgora();
    void UnInitAgora();
    void RenderLocalVideo();
// Dialog Data

	enum { IDD = IDD_DIALOG_METADATA };

    //Agora Event handler
    afx_msg LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDMetadataReceived(WPARAM wParam, LPARAM lParam);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    CStatic m_staChannelName;
    CButton m_btnJoinChannel;
    CStatic m_staSendSEI;
    CEdit m_edtSendSEI;
    CEdit m_edtRecvSEI;
    CStatic m_staMetaData;
    CListBox m_lstInfo;
    afx_msg void OnBnClickedButtonJoinchannel();
    virtual BOOL OnInitDialog();

private:
    void InitCtrlText();
    IRtcEngine* m_rtcEngine = nullptr;
    CAgoraMetaDataEventHanlder m_eventHandler;
    bool joinChannel    = false;
    bool m_initialize   = false;
    bool m_remoteJoined = false;
    CAGVideoWnd m_localVideoWnd;
    CAGVideoWnd m_remoteVideoWnd;

    CAgoraMetaDataObserver m_metaDataObserver;
public:
    CStatic m_staVideoArea;
    CEdit m_edtChannelName;
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    CButton m_btnSendSEI;
    afx_msg void OnBnClickedButtonSend();
    afx_msg void OnBnClickedButtonClear();
    afx_msg void OnSelchangeListInfoMetadata();
};
