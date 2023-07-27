#pragma once

#include "AGVideoWnd.h"
#include <map>
#include <string>
#include <IAgoraRtcEngineEx.h>
#include "stdafx.h"
#include "YUVReader.h"
#include "VideoExtractor.h"
#include <algorithm>

#define VIDEO_TRACK_SIZE 4
#define VIDEO_WINDOWS_SIZE 4

class MultiVideoSourceTracksEventHandler : public agora::rtc::IRtcEngineEventHandler
{
public:
	//set the message notify window handler
	void SetMsgReceiver(HWND hWnd) { m_hMsgHanlder = hWnd; }

	int GetId() { return m_Id; };
	void SetId(int id) { m_Id = id; };

	std::string GetChannelName() { return m_strChannel; }
	/*
	note:
		Join the channel callback.This callback method indicates that the client
		successfully joined the specified channel.Channel ids are assigned based
		on the channel name specified in the joinChannel. If IRtcEngine::joinChannel
		is called without a user ID specified. The server will automatically assign one
	parameters:
		channel:channel name.
		uid: user ID��If the UID is specified in the joinChannel, that ID is returned here;
		Otherwise, use the ID automatically assigned by the Agora server.
		elapsed: The Time from the joinChannel until this event occurred (ms).
	*/
	virtual void onJoinChannelSuccess(const char* channel, agora::rtc::uid_t uid, int elapsed) override;
	/*
	note:
		In the live broadcast scene, each anchor can receive the callback
		of the new anchor joining the channel, and can obtain the uID of the anchor.
		Viewers also receive a callback when a new anchor joins the channel and
		get the anchor's UID.When the Web side joins the live channel, the SDK will
		default to the Web side as long as there is a push stream on the
		Web side and trigger the callback.
	parameters:
		uid: remote user/anchor ID for newly added channel.
		elapsed: The joinChannel is called from the local user to the delay triggered
		by the callback(ms).
	*/
	virtual void onUserJoined(agora::rtc::uid_t uid, int elapsed) override;
	/*
	note:
		Remote user (communication scenario)/anchor (live scenario) is called back from
		the current channel.A remote user/anchor has left the channel (or dropped the line).
		There are two reasons for users to leave the channel, namely normal departure and
		time-out:When leaving normally, the remote user/anchor will send a message like
		"goodbye". After receiving this message, determine if the user left the channel.
		The basis of timeout dropout is that within a certain period of time
		(live broadcast scene has a slight delay), if the user does not receive any
		packet from the other side, it will be judged as the other side dropout.
		False positives are possible when the network is poor. We recommend using the
		Agora Real-time messaging SDK for reliable drop detection.
	parameters:
		uid: The user ID of an offline user or anchor.
		reason:Offline reason: USER_OFFLINE_REASON_TYPE.
	*/
	virtual void onUserOffline(agora::rtc::uid_t uid, agora::rtc::USER_OFFLINE_REASON_TYPE reason) override;
	/*
	note:
		When the App calls the leaveChannel method, the SDK indicates that the App
		has successfully left the channel. In this callback method, the App can get
		the total call time, the data traffic sent and received by THE SDK and other
		information. The App obtains the call duration and data statistics received
		or sent by the SDK through this callback.
	parameters:
		stats: Call statistics.
	*/
	virtual void onLeaveChannel(const agora::rtc::RtcStats& stats) override;

	/**
		Occurs when the remote video state changes.
		@note This callback does not work properly when the number of users (in the Communication profile) or broadcasters (in the Live-broadcast profile) in the channel exceeds 17.

		@param uid ID of the remote user whose video state changes.
		@param state State of the remote video. See #REMOTE_VIDEO_STATE.
		@param reason The reason of the remote video state change. See
		#REMOTE_VIDEO_STATE_REASON.
		@param elapsed Time elapsed (ms) from the local user calling the
		\ref agora::rtc::IRtcEngine::joinChannel "joinChannel" method until the
		SDK triggers this callback.
	 */
	virtual void onRemoteVideoStateChanged(agora::rtc::uid_t uid, agora::rtc::REMOTE_VIDEO_STATE state, agora::rtc::REMOTE_VIDEO_STATE_REASON reason, int elapsed) override;


	void onLocalVideoStats(agora::rtc::VIDEO_SOURCE_TYPE source, const LocalVideoStats& stats) override;


	void onLocalAudioStats(const LocalAudioStats& stats) override;


	void onRemoteAudioStats(const RemoteAudioStats& stats) override;


	void onRemoteVideoStats(const RemoteVideoStats& stats) override;

private:
	HWND m_hMsgHanlder = NULL;
	std::string m_strChannel = "";
	int m_Id = 0;
};


class MultiVideoSourceTracksVideoFramePusher {

public:

	void BeginPushVideoFrame(agora::rtc::IRtcEngineEx* rtcEngine, const char* channelId, uid_t uid, bool isEncoded);

	void EndPushVideoFrame();

	bool IsPusing();

private:
	agora::media::base::ExternalVideoFrame m_videoFrame;
	agora::util::AutoPtr<agora::media::IMediaEngine> m_mediaEngine;
	agora::rtc::RtcConnection m_rtcConnection;
	MultiVideoSourceTracksEventHandler m_eventHandler;

	agora::rtc::IRtcEngineEx* m_rtcEngine = nullptr;
	YUVReader *m_yuvReader = nullptr;
	VideoExtractor* m_videoExtractor = nullptr;

	int m_videoTrackId = 0;
	bool m_isEncoded = false;

	void BeginPushRawVideoFrame(const char* channelId, uid_t uid);

	void OnPushRawVideoFrame(int width, int height, unsigned char* buffer, int size);

	void EndPushRawVideoFrame();

	void BeginPushEncodedVideoFrame(const char* channelId, uid_t uid);

	void OnPushEncodedVideoFrame(unsigned char* buffer, int size, bool isKeyFrame, int fps);

	void EndPushEncodedVideoFrame();

};


class MultiVideoSourceTracks : public CDialogEx
{
	DECLARE_DYNAMIC(MultiVideoSourceTracks)

public:
	MultiVideoSourceTracks(CWnd* pParent = nullptr);
	virtual ~MultiVideoSourceTracks();

	enum { IDD = IDD_DIALOG_MULTI_VIDEO_SOURCE_TRACKS };

	//Initialize the Agora SDK
	bool InitAgora();
	//UnInitialize the Agora SDK
	void UnInitAgora();
	// resume window status.
	void ResumeStatus();

	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonJoinchannel();
	afx_msg void OnBnClickedButtonCreateTrack();
	afx_msg void OnBnClickedButtonCreateEncodeTrack();
	afx_msg void OnBnClickedButtonDestroyTrack();

protected:
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Support

	virtual BOOL OnInitDialog();

private:

	CEdit m_edtChannelName;
	CStatic m_videoArea;
	CButton m_btnJoinChannel;
	CButton m_btnCreateTrack;
	CButton m_btnDestroyTrack;
	CListBox m_lstInfo;
	CStatic m_staChannel;

	std::string m_strChannel;
	bool m_joinChannel;
	bool m_initialize;

	agora::rtc::IRtcEngineEx* m_rtcEngine = nullptr;
	
	MultiVideoSourceTracksEventHandler m_eventHandler;
	CAGVideoWnd m_videoWnds[VIDEO_WINDOWS_SIZE];

	MultiVideoSourceTracksVideoFramePusher m_videoFramePushers[VIDEO_TRACK_SIZE];
	
	
	LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDLocalVideoStats(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDLocalAudioStats(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDRemoteVideoStats(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDRemoteAudioStats(WPARAM wParam, LPARAM lParam);

	void ShowVideoWnds();

	void InitCtrlText();

};