#pragma once
#include "AGVideoWnd.h"

class CMultiChannelEventHandler : public IRtcEngineEventHandler
{
public:
	//set the message notify window handler
	void SetMsgReceiver(HWND hWnd) { m_hMsgHanlder = hWnd; }

	/*
	note:
		Join the channel callback.This callback method indicates that the client
		successfully joined the specified channel.Channel ids are assigned based
		on the channel name specified in the joinChannel. If IRtcEngine::joinChannel
		is called without a user ID specified. The server will automatically assign one
	parameters:
		channel:channel name.
		uid: user ID.If the UID is specified in the joinChannel, that ID is returned here;
		Otherwise, use the ID automatically assigned by the Agora server.
		elapsed: The Time from the joinChannel until this event occurred (ms).
	*/
	virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) override;
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
	virtual void onUserJoined(uid_t uid, int elapsed) override;
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
	virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) override;
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
	virtual void onLeaveChannel(const RtcStats& stats) override;
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
	virtual void onRemoteVideoStateChanged(uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed) override;
private:
	HWND m_hMsgHanlder;
};



class ChannelEventHandler :public agora::rtc::IChannelEventHandler
{
private:
	HWND m_hMsgHanlder;

public:

	void setMsgHandler(HWND msgHandler)
	{
		this->m_hMsgHanlder = msgHandler;

	}

	/** Reports the warning code of `IChannel`.
	 @param rtcChannel IChannel
	 @param warn The warning code: #WARN_CODE_TYPE
	 @param msg The warning message.

	 */
	virtual void onChannelWarning(IChannel *rtcChannel, int warn, const char* msg) {
	}
	/** Reports the error code of `IChannel`.

	 @param rtcChannel IChannel
	 @param err The error code: #ERROR_CODE_TYPE
	 @param msg The error message.
	 */
	virtual void onChannelError(IChannel *rtcChannel, int err, const char* msg) {
	}
	/** Occurs when a user joins a channel.

	 This callback notifies the application that a user joins a specified channel.

	 @param rtcChannel IChannel
	 @param uid The user ID. If the `uid` is not specified in the \ref IChannel::joinChannel "joinChannel" method, the server automatically assigns a `uid`.

	 @param elapsed Time elapsed (ms) from the local user calling \ref IChannel::joinChannel "joinChannel" until this callback is triggered.
	 */
	virtual void onJoinChannelSuccess(IChannel *rtcChannel, uid_t uid, int elapsed) {
		if (m_hMsgHanlder)
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)rtcChannel, uid);
	}
	/** Occurs when a user rejoins the channel after being disconnected due to network problems.

	 @param rtcChannel IChannel
	 @param uid The user ID.
	 @param elapsed Time elapsed (ms) from the local user starting to reconnect until this callback is triggered.

	 */
	virtual void onRejoinChannelSuccess(IChannel *rtcChannel, uid_t uid, int elapsed) {
	}
	/** Occurs when a user leaves the channel.

	 This callback notifies the application that a user leaves the channel when the application calls the \ref agora::rtc::IChannel::leaveChannel "leaveChannel" method.

	 The application retrieves information, such as the call duration and statistics.

	 @param rtcChannel IChannel
	 @param stats The call statistics: RtcStats.
	 */
	virtual void onLeaveChannel(IChannel *rtcChannel, const RtcStats& stats) {
		if (m_hMsgHanlder)
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), (WPARAM)rtcChannel, 0);
	}
	/** Occurs when the user role switches in the live interactive streaming. For example, from a host to an audience or vice versa.

	 This callback notifies the application of a user role switch when the application calls the \ref IChannel::setClientRole "setClientRole" method.

	 The SDK triggers this callback when the local user switches the user role by calling the \ref IChannel::setClientRole "setClientRole" method after joining the channel.

	 @param rtcChannel IChannel
	 @param oldRole Role that the user switches from: #CLIENT_ROLE_TYPE.
	 @param newRole Role that the user switches to: #CLIENT_ROLE_TYPE.
	 */
	virtual void onClientRoleChanged(IChannel *rtcChannel, CLIENT_ROLE_TYPE oldRole, CLIENT_ROLE_TYPE newRole) {
	}
	/** Occurs when a remote user (`COMMUNICATION`)/ host (`LIVE_BROADCASTING`) joins the channel.

	 - `COMMUNICATION` profile: This callback notifies the application that another user joins the channel. If other users are already in the channel, the SDK also reports to the application on the existing users.
	 - `LIVE_BROADCASTING` profile: This callback notifies the application that the host joins the channel. If other hosts are already in the channel, the SDK also reports to the application on the existing hosts. We recommend limiting the number of hosts to 17.

	 The SDK triggers this callback under one of the following circumstances:
	 - A remote user/host joins the channel by calling the \ref agora::rtc::IChannel::joinChannel "joinChannel" method.
	 - A remote user switches the user role to the host by calling the \ref agora::rtc::IChannel::setClientRole "setClientRole" method after joining the channel.
	 - A remote user/host rejoins the channel after a network interruption.
	 - The host injects an online media stream into the channel by calling the \ref agora::rtc::IChannel::addInjectStreamUrl "addInjectStreamUrl" method.

	 @note In the `LIVE_BROADCASTING` profile:
	 - The host receives this callback when another host joins the channel.
	 - The audience in the channel receives this callback when a new host joins the channel.
	 - When a web application joins the channel, the SDK triggers this callback as long as the web application publishes streams.

	 @param rtcChannel IChannel
	 @param uid User ID of the user or host joining the channel.
	 @param elapsed Time delay (ms) from the local user calling the \ref IChannel::joinChannel "joinChannel" method until the SDK triggers this callback.
	 */
	virtual void onUserJoined(IChannel *rtcChannel, uid_t uid, int elapsed) {
		if (m_hMsgHanlder)
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_JOINED), (WPARAM)rtcChannel, (LPARAM)uid);
	}
	/** Occurs when a remote user ( `COMMUNICATION`)/host (`LIVE_BROADCASTING`) leaves the channel.

	 Reasons why the user is offline:

	 - Leave the channel: When the user/host leaves the channel, the user/host sends a goodbye message. When the message is received, the SDK assumes that the user/host leaves the channel.
	 - Drop offline: When no data packet of the user or host is received for a certain period of time, the SDK assumes that the user/host drops offline. Unreliable network connections may lead to false detections, so we recommend using the Agora RTM SDK for more reliable offline detection.

	 @param rtcChannel IChannel
	 @param uid User ID of the user leaving the channel or going offline.
	 @param reason Reason why the user is offline: #USER_OFFLINE_REASON_TYPE.
	 */
	virtual void onUserOffline(IChannel *rtcChannel, uid_t uid, USER_OFFLINE_REASON_TYPE reason) {
		if (m_hMsgHanlder)
			::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_OFFLINE), (WPARAM)rtcChannel, (LPARAM)uid);
	}
	/** Occurs when the SDK cannot reconnect to Agora's edge server 10 seconds after its connection to the server is interrupted.

	 The SDK triggers this callback when it cannot connect to the server 10 seconds after calling the \ref IChannel::joinChannel "joinChannel" method, whether or not it is in the channel.

	 This callback is different from \ref agora::rtc::IRtcEngineEventHandler::onConnectionInterrupted "onConnectionInterrupted":

	 - The SDK triggers the `onConnectionInterrupted` callback when it loses connection with the server for more than four seconds after it successfully joins the channel.
	 - The SDK triggers the `onConnectionLost` callback when it loses connection with the server for more than 10 seconds, whether or not it joins the channel.

	 If the SDK fails to rejoin the channel 20 minutes after being disconnected from Agora's edge server, the SDK stops rejoining the channel.

	 @param rtcChannel IChannel
	 */
	virtual void onConnectionLost(IChannel *rtcChannel) {
	}
	/** Occurs when the token expires.

	 After a token is specified by calling the \ref IChannel::joinChannel "joinChannel" method, if the SDK losses connection with the Agora server due to network issues, the token may expire after a certain period of time and a new token may be required to reconnect to the server.

	 This callback notifies the app to generate a new token and call `joinChannel` to rejoin the channel with the new token.

	 @param rtcChannel IChannel
	 */
	virtual void onRequestToken(IChannel *rtcChannel) {
	}
	/** Occurs when the token expires in 30 seconds.

	 The user becomes offline if the token used in the \ref IChannel::joinChannel "joinChannel" method expires. The SDK triggers this callback 30 seconds before the token expires to remind the application to get a new token. Upon receiving this callback, generate a new token on the server and call the \ref IChannel::renewToken "renewToken" method to pass the new token to the SDK.

	 @param rtcChannel IChannel
	 @param token Token that expires in 30 seconds.
	 */
	virtual void onTokenPrivilegeWillExpire(IChannel *rtcChannel, const char* token) {
		
	}
	/** Reports the statistics of the current call.

	 The SDK triggers this callback once every two seconds after the user joins the channel.

	 @param rtcChannel IChannel
	 @param stats Statistics of the RtcEngine: RtcStats.
	 */
	virtual void onRtcStats(IChannel *rtcChannel, const RtcStats& stats) {
		
	}
	/** Reports the last mile network quality of each user in the channel once every two seconds.

	 Last mile refers to the connection between the local device and Agora's edge server. This callback reports once every two seconds the last mile network conditions of each user in the channel. If a channel includes multiple users, the SDK triggers this callback as many times.

	 @param rtcChannel IChannel
	 @param uid User ID. The network quality of the user with this @p uid is reported. If @p uid is 0, the local network quality is reported.
	 @param txQuality Uplink transmission quality rating of the user in terms of the transmission bitrate, packet loss rate, average RTT (Round-Trip Time), and jitter of the uplink network. @p txQuality is a quality rating helping you understand how well the current uplink network conditions can support the selected VideoEncoderConfiguration. For example, a 1000 Kbps uplink network may be adequate for video frames with a resolution of 640 * 480 and a frame rate of 15 fps in the `LIVE_BROADCASTING` profile, but may be inadequate for resolutions higher than 1280 * 720. See #QUALITY_TYPE.
	 @param rxQuality Downlink network quality rating of the user in terms of the packet loss rate, average RTT, and jitter of the downlink network. See #QUALITY_TYPE.
	 */
	virtual void onNetworkQuality(IChannel *rtcChannel, uid_t uid, int txQuality, int rxQuality) {
		
	}
	/** Reports the statistics of the video stream from each remote user/host.
	 *
	 * The SDK triggers this callback once every two seconds for each remote
	 * user/host. If a channel includes multiple remote users, the SDK
	 * triggers this callback as many times.
	 *
	 * @param rtcChannel IChannel
	 * @param stats Statistics of the remote video stream. See
	 * RemoteVideoStats.
	 */
	virtual void onRemoteVideoStats(IChannel *rtcChannel, const RemoteVideoStats& stats) {
	}
	/** Reports the statistics of the audio stream from each remote user/host.

	 This callback replaces the \ref agora::rtc::IRtcEngineEventHandler::onAudioQuality "onAudioQuality" callback.

	 The SDK triggers this callback once every two seconds for each remote user/host. If a channel includes multiple remote users, the SDK triggers this callback as many times.

	 @param rtcChannel IChannel
	 @param stats The statistics of the received remote audio streams. See RemoteAudioStats.
	 */
	virtual void onRemoteAudioStats(IChannel *rtcChannel, const RemoteAudioStats& stats) {
		
	}
	/** Occurs when the remote audio state changes.

	  This callback indicates the state change of the remote audio stream.
	  @note This callback does not work properly when the number of users (in the `COMMUNICATION` profile) or hosts (in the `LIVE_BROADCASTING` profile) in the channel exceeds 17.

	  @param rtcChannel IChannel
	  @param uid ID of the remote user whose audio state changes.
	  @param state State of the remote audio. See #REMOTE_AUDIO_STATE.
	  @param reason The reason of the remote audio state change.
	  See #REMOTE_AUDIO_STATE_REASON.
	  @param elapsed Time elapsed (ms) from the local user calling the
	  \ref IChannel::joinChannel "joinChannel" method until the SDK
	  triggers this callback.
	 */
	virtual void onRemoteAudioStateChanged(IChannel *rtcChannel, uid_t uid, REMOTE_AUDIO_STATE state, REMOTE_AUDIO_STATE_REASON reason, int elapsed) {

	}

	/** Occurs when the audio publishing state changes.
	 *
	 * @since v3.1.0
	 *
	 * This callback indicates the publishing state change of the local audio stream.
	 *
	 * @param rtcChannel IChannel
	 * @param oldState The previous publishing state. For details, see #STREAM_PUBLISH_STATE.
	 * @param newState The current publishing state. For details, see #STREAM_PUBLISH_STATE.
	 * @param elapseSinceLastState The time elapsed (ms) from the previous state to the current state.
	 */
	virtual void onAudioPublishStateChanged(IChannel *rtcChannel, STREAM_PUBLISH_STATE oldState, STREAM_PUBLISH_STATE newState, int elapseSinceLastState) {
		
	}

	/** Occurs when the video publishing state changes.
	 *
	 * @since v3.1.0
	 *
	 * This callback indicates the publishing state change of the local video stream.
	 *
	 * @param rtcChannel IChannel
	 * @param oldState The previous publishing state. For details, see #STREAM_PUBLISH_STATE.
	 * @param newState The current publishing state. For details, see #STREAM_PUBLISH_STATE.
	 * @param elapseSinceLastState The time elapsed (ms) from the previous state to the current state.
	 */
	virtual void onVideoPublishStateChanged(IChannel *rtcChannel, STREAM_PUBLISH_STATE oldState, STREAM_PUBLISH_STATE newState, int elapseSinceLastState) {
	
	}

	/** Occurs when the audio subscribing state changes.
	 *
	 * @since v3.1.0
	 *
	 * This callback indicates the subscribing state change of a remote audio stream.
	 *
	 * @param rtcChannel IChannel
	 * @param uid The ID of the remote user.
	 * @param oldState The previous subscribing state. For details, see #STREAM_SUBSCRIBE_STATE.
	 * @param newState The current subscribing state. For details, see #STREAM_SUBSCRIBE_STATE.
	 * @param elapseSinceLastState The time elapsed (ms) from the previous state to the current state.
	 */
	virtual void onAudioSubscribeStateChanged(IChannel *rtcChannel, uid_t uid, STREAM_SUBSCRIBE_STATE oldState, STREAM_SUBSCRIBE_STATE newState, int elapseSinceLastState) {
		
	}

	/** Occurs when the audio subscribing state changes.
	 *
	 * @since v3.1.0
	 *
	 * This callback indicates the subscribing state change of a remote video stream.
	 *
	 * @param rtcChannel IChannel=
	 * @param uid The ID of the remote user.
	 * @param oldState The previous subscribing state. For details, see #STREAM_SUBSCRIBE_STATE.
	 * @param newState The current subscribing state. For details, see #STREAM_SUBSCRIBE_STATE.
	 * @param elapseSinceLastState The time elapsed (ms) from the previous state to the current state.
	 */
	virtual void onVideoSubscribeStateChanged(IChannel *rtcChannel, uid_t uid, STREAM_SUBSCRIBE_STATE oldState, STREAM_SUBSCRIBE_STATE newState, int elapseSinceLastState) {
		
	}

	/** Reports which user is the loudest speaker.

	If the user enables the audio volume indication by calling the \ref IRtcEngine::enableAudioVolumeIndication(int, int, bool) "enableAudioVolumeIndication" method, this callback returns the @p uid of the active speaker detected by the audio volume detection module of the SDK.

	@note
	- To receive this callback, you need to call the \ref IRtcEngine::enableAudioVolumeIndication(int, int, bool) "enableAudioVolumeIndication" method.
	- This callback returns the user ID of the user with the highest voice volume during a period of time, instead of at the moment.

	@param rtcChannel IChannel
	@param uid User ID of the active speaker. A `uid` of 0 represents the local user.
	*/
	virtual void onActiveSpeaker(IChannel *rtcChannel, uid_t uid) {
		
	}
	/** Occurs when the video size or rotation of a specified user changes.

	@param rtcChannel IChannel
	@param uid User ID of the remote user or local user (0) whose video size or rotation changes.
	@param width New width (pixels) of the video.
	@param height New height (pixels) of the video.
	@param rotation New rotation of the video [0 to 360).
	*/
	virtual void onVideoSizeChanged(IChannel *rtcChannel, uid_t uid, int width, int height, int rotation) {
		
	}
	/** Occurs when the remote video state changes.

	 @note This callback does not work properly when the number of users (in the `COMMUNICATION` profile) or hosts (in the `LIVE_BROADCASTING` profile) in the channel exceeds 17.

	 @param rtcChannel IChannel
	 @param uid ID of the remote user whose video state changes.
	 @param state State of the remote video. See #REMOTE_VIDEO_STATE.
	 @param reason The reason of the remote video state change. See
	 #REMOTE_VIDEO_STATE_REASON.
	 @param elapsed Time elapsed (ms) from the local user calling the
	 \ref agora::rtc::IChannel::joinChannel "joinChannel" method until the
	 SDK triggers this callback.
	 */
	virtual void onRemoteVideoStateChanged(IChannel *rtcChannel, uid_t uid, REMOTE_VIDEO_STATE state, REMOTE_VIDEO_STATE_REASON reason, int elapsed) {
		
	}
	/** Occurs when the local user receives the data stream from the remote user within five seconds.

	 The SDK triggers this callback when the local user receives the stream message that the remote user sends by calling the \ref agora::rtc::IChannel::sendStreamMessage "sendStreamMessage" method.

	 @param rtcChannel IChannel
	 @param uid User ID of the remote user sending the message.
	 @param streamId Stream ID.
	 @param data The data received by the local user.
	 @param length Length of the data in bytes.
	*/
	virtual void onStreamMessage(IChannel *rtcChannel, uid_t uid, int streamId, const char* data, size_t length) {
		
	}
	/** Occurs when the local user does not receive the data stream from the remote user within five seconds.

	 The SDK triggers this callback when the local user fails to receive the stream message that the remote user sends by calling the \ref agora::rtc::IChannel::sendStreamMessage "sendStreamMessage" method.

	 @param rtcChannel IChannel
	 @param uid User ID of the remote user sending the message.
	 @param streamId Stream ID.
	 @param code Error code: #ERROR_CODE_TYPE.
	 @param missed Number of lost messages.
	 @param cached Number of incoming cached messages when the data stream is interrupted.
	 */
	virtual void onStreamMessageError(IChannel *rtcChannel, uid_t uid, int streamId, int code, int missed, int cached) {
		
	}
	/** Occurs when the state of the media stream relay changes.
	 *
	 * The SDK returns the state of the current media relay with any error
	 * message.
	 * @param rtcChannel IChannel
	 * @param state The state code in #CHANNEL_MEDIA_RELAY_STATE.
	 * @param code The error code in #CHANNEL_MEDIA_RELAY_ERROR.
	 */
	virtual void onChannelMediaRelayStateChanged(IChannel *rtcChannel, CHANNEL_MEDIA_RELAY_STATE state, CHANNEL_MEDIA_RELAY_ERROR code) {
		
	}
	/** Reports events during the media stream relay.
	 * @param rtcChannel IChannel
	 * @param code The event code in #CHANNEL_MEDIA_RELAY_EVENT.
	 */
	virtual void onChannelMediaRelayEvent(IChannel *rtcChannel, CHANNEL_MEDIA_RELAY_EVENT code) {
		
	}
	/**
	 Occurs when the state of the RTMP streaming changes.

	 The SDK triggers this callback to report the result of the local user calling the \ref agora::rtc::IChannel::addPublishStreamUrl "addPublishStreamUrl" or \ref agora::rtc::IChannel::removePublishStreamUrl "removePublishStreamUrl" method.

	 This callback indicates the state of the RTMP streaming. When exceptions occur, you can troubleshoot issues by referring to the detailed error descriptions in the *errCode* parameter.

	 @param rtcChannel IChannel
	 @param url The RTMP URL address.
	 @param state The RTMP streaming state. See: #RTMP_STREAM_PUBLISH_STATE.
	 @param errCode The detailed error information for streaming. See: #RTMP_STREAM_PUBLISH_ERROR.
	 */
	virtual void onRtmpStreamingStateChanged(IChannel *rtcChannel, const char *url, RTMP_STREAM_PUBLISH_STATE state, RTMP_STREAM_PUBLISH_ERROR_TYPE errCode) {
		
	}

	/** Reports events during the RTMP streaming.
	 *
	 * @since v3.1.0
	 *
	 * @param rtcChannel IChannel
	 * @param url The RTMP streaming URL.
	 * @param eventCode The event code. See #RTMP_STREAMING_EVENT
	 */
	virtual void onRtmpStreamingEvent(IChannel *rtcChannel, const char* url, RTMP_STREAMING_EVENT eventCode) {
		
	}

	/** Occurs when the publisher's transcoding is updated.

	When the `LiveTranscoding` class in the \ref agora::rtc::IChannel::setLiveTranscoding "setLiveTranscoding" method updates, the SDK triggers the `onTranscodingUpdated` callback to report the update information to the local host.

	@note If you call the `setLiveTranscoding` method to set the LiveTranscoding class for the first time, the SDK does not trigger the `onTranscodingUpdated` callback.

	@param rtcChannel IChannel
	*/
	virtual void onTranscodingUpdated(IChannel *rtcChannel) {
		
	}
	/** Occurs when a voice or video stream URL address is added to the live interactive streaming.

	 @param rtcChannel IChannel
	 @param url The URL address of the externally injected stream.
	 @param uid User ID.
	 @param status State of the externally injected stream: #INJECT_STREAM_STATUS.
	 */
	virtual void onStreamInjectedStatus(IChannel *rtcChannel, const char* url, uid_t uid, int status) {
		
	}
	/** Occurs when the published media stream falls back to an audio-only stream due to poor network conditions or switches back to the video after the network conditions improve.

	If you call \ref IRtcEngine::setLocalPublishFallbackOption "setLocalPublishFallbackOption" and set *option* as #STREAM_FALLBACK_OPTION_AUDIO_ONLY, the SDK triggers this callback when the published stream falls back to audio-only mode due to poor uplink conditions, or when the audio stream switches back to the video after the uplink network condition improves.

	@param rtcChannel IChannel
	@param isFallbackOrRecover Whether the published stream falls back to audio-only or switches back to the video:
	- true: The published stream falls back to audio-only due to poor network conditions.
	- false: The published stream switches back to the video after the network conditions improve.
	*/
	virtual void onLocalPublishFallbackToAudioOnly(IChannel *rtcChannel, bool isFallbackOrRecover) {
		
	}
	/** Occurs when the remote media stream falls back to audio-only stream
	 * due to poor network conditions or switches back to the video stream
	 * after the network conditions improve.
	 *
	 * If you call
	 * \ref IRtcEngine::setRemoteSubscribeFallbackOption
	 * "setRemoteSubscribeFallbackOption" and set
	 * @p option as #STREAM_FALLBACK_OPTION_AUDIO_ONLY, the SDK triggers this
	 * callback when the remote media stream falls back to audio-only mode due
	 * to poor uplink conditions, or when the remote media stream switches
	 * back to the video after the uplink network condition improves.
	 *
	 * @note Once the remote media stream switches to the low stream due to
	 * poor network conditions, you can monitor the stream switch between a
	 * high and low stream in the RemoteVideoStats callback.
	 * @param rtcChannel IChannel
	 * @param uid ID of the remote user sending the stream.
	 * @param isFallbackOrRecover Whether the remotely subscribed media stream
	 * falls back to audio-only or switches back to the video:
	 * - true: The remotely subscribed media stream falls back to audio-only
	 * due to poor network conditions.
	 * - false: The remotely subscribed media stream switches back to the
	 * video stream after the network conditions improved.
	 */
	virtual void onRemoteSubscribeFallbackToAudioOnly(IChannel *rtcChannel, uid_t uid, bool isFallbackOrRecover) {
		
	}
	/** Occurs when the connection state between the SDK and the server changes.

	 @param rtcChannel IChannel
	 @param state See #CONNECTION_STATE_TYPE.
	 @param reason See #CONNECTION_CHANGED_REASON_TYPE.
	 */
	virtual void onConnectionStateChanged(IChannel *rtcChannel,
		CONNECTION_STATE_TYPE state,
		CONNECTION_CHANGED_REASON_TYPE reason) {
		
	}
};


struct ChannelInfo
{
	std::string channelName;
	IChannel* channel;
	IChannelEventHandler* evnetHandler;

	ChannelInfo(std::string channelName_,IChannel* channel_,IChannelEventHandler *eventHandler_):
		channelName(channelName_), channel(channel_), evnetHandler(eventHandler_){}
};

class CAgoraMultiChannelDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAgoraMultiChannelDlg)

public:
	CAgoraMultiChannelDlg(CWnd* pParent = nullptr);   
	virtual ~CAgoraMultiChannelDlg();

	enum { IDD = IDD_DIALOG_MULTI_CHANNEL };
public:
	//Initialize the Ctrl Text.
	void InitCtrlText();
	//Initialize the Agora SDK
	bool InitAgora();
	//UnInitialize the Agora SDK
	void UnInitAgora();
	//render local video from SDK local capture.
	void RenderLocalVideo();
	//resume window status
	void ResumeStatus();
private:
	bool m_joinChannel = false;
	bool m_initialize = false;
	bool m_audioMixing = false;
	IRtcEngine* m_rtcEngine = nullptr;
	CAGVideoWnd m_localVideoWnd;
	CMultiChannelEventHandler m_eventHandler;
	std::vector<ChannelInfo> m_channels;
	CString m_strMainChannel;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);  
	LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDRemoteVideoStateChanged(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_staVideoArea;
	CListBox m_lstInfo;
	CStatic m_staChannel;
	CEdit m_edtChannel;
	CButton m_btnJoinChannel;
	CStatic m_staChannelList;
	CComboBox m_cmbChannelList;
	CButton m_btnLeaveChannel;
	CStatic m_staDetail;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonJoinchannel();
	afx_msg void OnBnClickedButtonLeaveChannel();
	afx_msg void OnSelchangeListInfoBroadcasting();
	CButton m_chkPublishAudio;
	CButton m_chkPublishVideo;
};
