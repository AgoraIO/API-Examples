#pragma once

#include "connection_wrapper.h"
#include "AGVideoWnd.h"
#include <map>
#include <string>
// CMultiCameraDlg 
typedef struct _CameraInfos {
    std::string deviceId;
    std::string deviceName;
    std::vector<agora::rtc::VideoFormat> videoFormats;
}CAMERAINFO, *PCAMERAINFO;
#define MAX_VIDEO_COUNT  16
#define IDC_MULTICALERAM_VIDEO      10000
class CMultiCameraDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMultiCameraDlg)

public:
	CMultiCameraDlg(CWnd* pParent = nullptr);   
	virtual ~CMultiCameraDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_MULTICAMERA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Support
	// Create connection, set userRole, connect. Broadcaster has more fucntion:
	// start camera,  add local renders, publish video tracks, create audio track, publish audio track.
	// Broadcaster just publish video and audio, audience just subscribe.
    void JoinChannel();
	// Disconnect, reset connections. Broad caster has more function:
	// stop camera, remove local renders, unpublish video tracks, unpublish audio track,disbale audio track.
    void LeaveChannel();
	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();

private:
    void ShowVideoWnds();
public:
    afx_msg void OnSelchangeComboRole();
    afx_msg void OnBnClickedButtonJoinchannel();
    afx_msg LRESULT OnEIDSubscribeRemoteVideo(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDConnected(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDDisConnected(WPARAM wParam, LPARAM lParam);
    afx_msg void OnSelchangeComboCameras();
    afx_msg void OnSelchangeComboResolution();
private:
    std::unique_ptr<LowlevelSdk> sdk_ptr;
	//Cameras Information
    std::vector<CAMERAINFO> m_vecCameraInfos;
	//Connections, one connection for one camera track
    std::vector<std::shared_ptr<ConnectionWrapper>> m_vecConnections;
	//Only one audio track
    agora::agora_refptr<agora::rtc::ILocalAudioTrack> m_localAudioTrack;
	//Local video tracks for multiple cameras 
    std::vector<agora::agora_refptr<agora::rtc::ILocalVideoTrack>> m_vecCameraVideoTracks;
	//Local video renders for multiple cameras 
    std::vector<agora::agora_refptr<agora::rtc::IVideoRenderer>> m_vecLocalRenders;
	// remote video tracks
	std::vector<agora::agora_refptr<agora::rtc::IRemoteVideoTrack>> m_vecRemoteVideoTracks;
	// remote video renders
	std::vector<agora::agora_refptr<agora::rtc::IVideoRenderer>> m_vecRemoteRenders;
    std::map<std::string, HWND> m_mapUserVideoTrack;
    int m_maxVideoCount = 0;
    CAGVideoWnd m_videoWnds[MAX_VIDEO_COUNT];
    bool m_bConnected = false;
public:
    //Initialize the Agora SDK
    bool InitAgora();
    //UnInitialize the Agora SDK
    void UnInitAgora();
    CComboBox m_cmbResolution;
    CEdit m_edtChannelName;
    CStatic m_videoArea;
    CComboBox m_cmbCameras;
    CComboBox m_cmbRole;
    CButton m_btnJoinChannel;
	CListBox m_lstInfo;
};
