# Agora C++ API Reference for All Platforms

Agora provides ensured quality of experience (QoE) for worldwide Internet-based voice and video communications through a virtual global network optimized for real-time web and mobile-to-mobile applications.

This page provides Reference for Agora low-level APIs:

* [Agora service](#agoraservice)
* [RTC connection](#rtcconnection)
* [Local user](#localuser)
* [Local audio track](#localaudiotrack)
* [Remote audio track](#remoteaudiotrack)
* [Local video track](#localvideotrack)
* [Remote video track](#remotevideotrack)
* [Media node factory](#medianodefactory)
* [Audio data sender](#audiodatasender)
* [Audio filter](#audiofilter)
* [Audio device manager](#audiodevicemanager)
* [Video data sender](#videodatasender)
* [Video filter](#videofilter)
* [Video renderer](#videorenderer)
* [Video camera capturer](#videocameracapturer)
* [Video screen capturer](#videoscreencapturer)
* [Media player](#mediaplayer)
* [RTMP streaming service](#rtmpstreamingservice)
* [RTM service](#rtmservice)
* [RTM channel](#rtmchannel)
* [RTM message](#rtmmessage)



<a name="agoraservice"></a>

### Agora service

IAgoraService is the entry point of Agora Low Level APIs. Use this interface to create access points to Agora Modules, including RTC Connection, Media Tracks, Audio Device Manager and etc. In addition, in order to customize different user scenarios, the interface allows application to configure service and media sessions on the global level, for example, AgoraServiceConfiguration or AudioSessionConfiguration.

| Method                                                       | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| \ref createAgoraService "createAgoraService"                 | Creates an Agora Service object and returns the pointer.     |
| \ref agora::base::IAgoraService::initialize "initialize"     | Initializes the Agora Service object.                        |
| \ref agora::base::IAgoraService::setAudioSessionPreset "setAudioSessionPreset" | Configures the preset audio session.                         |
| \ref agora::base::IAgoraService::setAudioSessionConfiguration "setAudioSessionConfiguration" | Sets the audio session configuration.                        |
| \ref agora::base::IAgoraService::getAudioSessionConfiguration "getAudioSessionConfiguration" | Gets the audio session configuration.                        |
| \ref agora::base::IAgoraService::setLogFile "setLogFile"     | Sets the log file path and size.                             |
| \ref agora::base::IAgoraService::setLogFilter "setLogFilter" | Sets the log output level.                                   |
| \ref agora::base::IAgoraService::createRtcConnection "createRtcConnection" | Creates an RTC connection object and returns the pointer.    |
| \ref agora::base::IAgoraService::createLocalAudioTrack "createLocalAudioTrack" | Creates a local audio track object and returns the pointer.  |
| \ref agora::base::IAgoraService::createCustomAudioTrack(agora_refptr<rtc::IAudioEncodedFrameSender> audioSource, TMixMode mixMode) "createCustomAudioTrack"1 | Creates a local audio track object with the receiver for encoded audio data and returns the pointer. |
| \ref agora::base::IAgoraService::createCustomAudioTrack(agora_refptr<rtc::IAudioPcmDataSender> audioSource) "createCustomAudioTrack"2 | Creates a local audio track object with the PCM data receiver and returns the pointer. |
| \ref agora::base::IAgoraService::createCustomAudioTrack(agora_refptr<rtc::IMediaPacketSender> audioSource) "createCustomAudioTrack"3 | Creates a local audio track object with the receiver for encoded audio data and returns the pointer. |
| \ref agora::base::IAgoraService::createAudioDeviceManager "createAudioDeviceManager" | Creates an audio device manager object and returns the pointer. |
| \ref agora::base::IAgoraService::createMediaNodeFactory "createMediaNodeFactory" | Creates a media node factory and returns the pointer.        |
| \ref agora::base::IAgoraService::createMediaPlayerVideoTrack "createMediaPlayerVideoTrack" | Creates a local video track object with an IMediaPlayerSource object and returns the pointer. |
| \ref agora::base::IAgoraService::createMediaPlayerAudioTrack "createMediaPlayerAudioTrack" | Creates a local video track object with an IMediaPlayerSource object and returns the pointer. |
| \ref agora::base::IAgoraService::createCameraVideoTrack "createCameraVideoTrack" | Creates a local video track object with the camera capturer and returns the pointer. |
| \ref agora::base::IAgoraService::createScreenVideoTrack "createScreenVideoTrack" | Creates a local video track object with the screen capturer and returns the pointer. |
| \ref agora::base::IAgoraService::createCustomVideoTrack(agora_refptr<rtc::IMediaPacketSender> videoSource) "createCustomVideoTrack"1 | Creates a local video track object with the custom video source and returns the pointer. |
| \ref agora::base::IAgoraService::createCustomVideoTrack(agora_refptr<rtc::IVideoEncodedImageSender> videoSource, SenderOptions& options) "createCustomVideoTrack"2 | Creates a local video track object with the receiver for encoded video data and returns the pointer. |
| \ref agora::base::IAgoraService::createCustomVideoTrack(agora_refptr<rtc::IVideoFrameSender> videoSource) "createCustomVideoTrack"3 | Creates a local video track object with the custom video source and returns the pointer. |
| \ref agora::base::IAgoraService::createRtmpStreamingService "createRtmpStreamingService" | Creates a live stream service object.                        |
| \ref agora::base::IAgoraService::createRtmService "createRtmService" | Creates an RTM service object.                               |

<a name="rtcconnection"></a>

### RTC connection

IRtcConnection allows an app to establish a connection to an Agora channel. In order to establish a connection, the app must provide an app ID, a token, and the ID of the channel that you want to join.

| Method                                                       | Description                                              |
| ------------------------------------------------------------ | -------------------------------------------------------- |
| \ref agora::rtc::IRtcConnection::connect "connect"          | Connects to an Agora channel.                            |
| \ref agora::rtc::IRtcConnection::disconnect "disconnect"    | Disconnects from the channel.                            |
| \ref agora::rtc::IRtcConnection::startLastmileProbeTest "startLastmileProbeTest" | Starts the last mile network probe test.                       |
| \ref agora::rtc::IRtcConnection::stopLastmileProbeTest "stopLastmileTest" | Stops the last mile network probe test.                        |
| \ref agora::rtc::IRtcConnection::renewToken "renewToken"    | Renews the token.                                        |
| \ref agora::rtc::IRtcConnection::getConnectionInfo "getConnectionInfo" | Gets the current connection information.                 |
| \ref agora::rtc::IRtcConnection::getLocalUser "getLocalUser" | Gets the ILocalUser object.                              |
| \ref agora::rtc::IRtcConnection::getRemoteUsers "getRemoteUsers" | Gets the information of remote users.                    |
| \ref agora::rtc::IRtcConnection::getUserInfo "getUserInfo"  | Gets the information of a specified user in the channel. |
| \ref agora::rtc::IRtcConnection::getUserInfoByUid "getUserInfoByUid"  | Gets the information of a specified user in the channel by uid, which is in integer format. |
| \ref agora::rtc::IRtcConnection::getUserInfoByUserAccount "getUserInfoByUserAccount"  | Gets the information of a specified user in the channel by user account, which is in string format. |
| \ref agora::rtc::IRtcConnection::registerObserver "registerObserver" | Registers an IRtcConnectionObserver object.              |
| \ref agora::rtc::IRtcConnection::unregisterObserver "unregisterObserver" | Releases the registered IRtcConnectionObserver object.   |
| \ref agora::rtc::IRtcConnection::getConnId "getConnId"      | Gets the ID of the current connection.                   |
| \ref agora::rtc::IRtcConnection::getTransportStats "getTransportStats" | Gets the transportation statistics of this connection.   |
| \ref agora::rtc::IRtcConnection::getAgoraParameter "getAgoraParameter" | Gets an IAgoraParameter object.                                      |
| \ref agora::rtc::IRtcConnection::createDataStream "createDataStream" | Creates a data stream.                          |
| \ref agora::rtc::IRtcConnection::sendStreamMessage "sendStreamMessage" | Sends data stream messages to all users in a channel.       |

Connecting to an Agora channel is done asynchronously, and apps can listen for the connection states of both the local and remote user using an RTC connection observer.

| Event                                                        | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| \ref agora::rtc::IRtcConnectionObserver::onConnected "onConnected" | Occurs when the connection state changes to STATE_CONNECTED. |
| \ref agora::rtc::IRtcConnectionObserver::onDisconnected "onDisconnected" | Occurs when the connection state changes to STATE_DISCONNECTED. |
| \ref agora::rtc::IRtcConnectionObserver::onReconnecting "onReconnecting" | Occurs when the connection state changes to STATE_RECONNECTING. |
| \ref agora::rtc::IRtcConnectionObserver::onConnectionLost "onConnectionLost" | Occurs when the SDK loses connection with the Agora channel. |
| \ref agora::rtc::IRtcConnectionObserver::onLastmileQuality "onLastmileQuality" | Reports the quality of the last mile network.                |
| \ref agora::rtc::IRtcConnectionObserver::onTokenPrivilegeWillExpire "onTokenPrivilegeWillExpire" | Occurs when the token privilege expires in 30 seconds.       |
| \ref agora::rtc::IRtcConnectionObserver::onTokenPrivilegeDidExpire "onTokenPrivilegeDidExpire" | Occurs when the token privilege has expired.                 |
| \ref agora::rtc::IRtcConnectionObserver::onConnectionFailure "onConnectionFailure" | Occurs when the connection state changes to STATE_FAILED.    |
| \ref agora::rtc::IRtcConnectionObserver::onUserJoined "onUserJoined" | Occurs when a remote user has successfully joined the channel. |
| \ref agora::rtc::IRtcConnectionObserver::onUserLeft "onUserLeft" | Occurs when a remote user has left the channel.              |
| \ref agora::rtc::IRtcConnectionObserver::onTransportStats "onTransportStats" | Reports the transport statistics of the current connection,  |
| \ref agora::rtc::IRtcConnectionObserver::onChangeRoleSuccess "onChangeRoleSuccess" | Occurs when the role of the local user changes.              |
| \ref agora::rtc::IRtcConnectionObserver::onChangeRoleFailure "onChangeRoleFailure" | Occurs when the role of the local user changes.              |
| \ref agora::rtc::IRtcConnectionObserver::onUserNetworkQuality "onUserNetworkQuality" | Reports the network quality of each user.                    |
| \ref agora::rtc::IRtcConnectionObserver::onApiCallExecuted "onApiCallExecuted" | Occurs when an API method is executed.                       |
| \ref agora::rtc::IRtcConnectionObserver::onError "onError"  | Reports the error code and error message.                    |
| \ref agora::rtc::IRtcConnectionObserver::onWarning "onWarning" | Reports the warning code and warning message.                |
| \ref agora::rtc::IRtcConnectionObserver::onChannelMediaRelayStateChanged "onChannelMediaRelayStateChanged" | Occurs when the state of the channel media relay changes.    |

<a name="localuser"></a>

### Local user

Once connected, each RTC connection has its own local user. Apps can get the local user
object using \ref agora::rtc::IRtcConnection::getLocalUser "IRtcConnection::getLocalUser".

Each local user has two user roles: broadcaster (publisher or subscriber) and audience
(subscriber only). The publisher publishes audio and video tracks, while the audience receives them.

| Method                                                       | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| \ref agora::rtc::ILocalUser::setUserRole "setUserRole"       | Sets the role of the user.                                   |
| \ref agora::rtc::ILocalUser::getUserRole "getUserRole"       | Gets the role of the user.                                   |
| \ref agora::rtc::ILocalUser::setAudioEncoderConfiguration "setAudioEncoderConfiguration" | Configures the audio encoder.                                |
| \ref agora::rtc::ILocalUser::getLocalAudioStatistics "getLocalAudioStatistics" | Gets the detailed statistics of the local audio.             |
| \ref agora::rtc::ILocalUser::publishAudio "publishAudio"     | Publishes a local audio track to the RTC connection.         |
| \ref agora::rtc::ILocalUser::unpublishAudio "unpublishAudio" | Stops publishing the local audio track to the RTC connection. |
| \ref agora::rtc::ILocalUser::publishVideo "publishVideo"     | Publishes a local video track to the RTC connection.         |
| \ref agora::rtc::ILocalUser::unpublishVideo "unpublishVideo" | Stops publishing the local video track to the RTC connection. |
| \ref agora::rtc::ILocalUser::subscribeAllAudio "subscribeAudio" | Subscribes to the audio of a specified remote user in the RTC connection. |
| \ref agora::rtc::ILocalUser::subscribeAllAudio "subscribeAllAudio" | Subscribes to the audio of all remote users in the RTC connection. |
| \ref agora::rtc::ILocalUser::unsubscribeAudio "unsubscribeAudio" | Stops subscribing to the audio of a specified remote user in the RTC connection. |
| \ref agora::rtc::ILocalUser::unsubscribeAllAudio "unsubscribeAllAudio" | Stops subscribing to the audio of all remote users in the RTC connection. |
| \ref agora::rtc::ILocalUser::pullMixedAudioPcmData "pullMixedAudioPcmData" | Pulls mixed PCM audio data from the RTC connection.          |
| \ref agora::rtc::ILocalUser::setPlaybackAudioFrameParameters "setPlaybackAudioFrameParameters" | Sets the audio frame parameters for the onPlaybackAudioFrame callback. |
| \ref agora::rtc::ILocalUser::setRecordingAudioFrameParameters "setRecordingAudioFrameParameters" | Sets the audio frame parameters for the onRecordAudioFrame callback. |
| \ref agora::rtc::ILocalUser::setMixedAudioFrameParameters "setMixedAudioFrameParameters" | Sets the audio frame parameters for the onMixedAudioFrame callback. |
| \ref agora::rtc::ILocalUser::setPlaybackAudioFrameBeforeMixingParameters "setPlaybackAudioFrameBeforeMixingParameters" | Sets the audio frame parameters for the onPlaybackAudioFrameBeforeMixing callback. |
| \ref agora::rtc::ILocalUser::registerAudioFrameObserver "registerAudioFrameObserver" | Registers an audio frame observer object.                    |
| \ref agora::rtc::ILocalUser::unregisterAudioFrameObserver "unregisterAudioFrameObserver" | Releases the audio frame observer object.                    |
| \ref agora::rtc::ILocalUser::subscribeVideo "subscribeVideo" | Subscribes to the video of a specified remote user in the RTC connection. |
| \ref agora::rtc::ILocalUser::subscribeAllVideo "subscribeAllVideo" | Subscribes to the video of all remote users in the RTC connection. |
| \ref agora::rtc::ILocalUser::unsubscribeVideo "unsubscribeVideo" | Stops subscribing to the video of a specified remote user in the RTC connection. |
| \ref agora::rtc::ILocalUser::unsubscribeAllVideo "unsubscribeAllVideo" | Stops subscribing to the video of all remote users in the RTC connection. |
| \ref agora::rtc::ILocalUser::setAudioVolumeIndicationParameters "setAudioVolumeIndicationParameters" | Sets the time interval and the volume smoothing factor of the onAudioVolumeIndication callback. |
| \ref agora::rtc::ILocalUser::registerLocalUserObserver "registerLocalUserObserver" | Registers a local user observer object.                      |
| \ref agora::rtc::ILocalUser::unregisterLocalUserObserver "unregisterLocalUserObserver" | Releases the local user observer object.                     |

| Event                                                        | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| \ref agora::rtc::ILocalUserObserver::onAudioTrackPublishSuccess "onAudioTrackPublishSuccess" | Occurs when the local audio track has published.             |
| \ref agora::rtc::ILocalUserObserver::onAudioTrackPublicationFailure "onAudioTrackPublicationFailure" | Occurs when the local audio track fails to be published.     |
| \ref agora::rtc::ILocalUserObserver::onLocalAudioTrackStateChanged "onLocalAudioTrackStateChanged" | Occurs when the state of the local audio track changes.      |
| \ref agora::rtc::ILocalUserObserver::onUserAudioTrackSubscribed "onUserAudioTrackSubscribed" | Occurs when the first remote audio frame is received.        |
| \ref agora::rtc::ILocalUserObserver::onUserAudioTrackStateChanged "onUserAudioTrackStateChanged" | Occurs when the state of a remote audio track changes.       |
| \ref agora::rtc::ILocalUserObserver::onVideoTrackPublishSuccess "onVideoTrackPublishSuccess" | Occurs when the local video track is published.              |
| \ref agora::rtc::ILocalUserObserver::onVideoTrackPublicationFailure "onVideoTrackPublicationFailure" | Occurs when the local video track fails to be published.     |
| \ref agora::rtc::ILocalUserObserver::onLocalVideoTrackStateChanged "onLocalVideoTrackStateChanged" | Occurs when the state of the local video track changes.      |
| \ref agora::rtc::ILocalUserObserver::onLocalVideoTrackStatistics "onLocalVideoTrackStatistics" | Reports the statistics of the local video track.             |
| \ref agora::rtc::ILocalUserObserver::onUserVideoTrackSubscribed "onUserVideoTrackSubscribed" | Occurs when the first remote video frame is received.        |
| \ref agora::rtc::ILocalUserObserver::onUserVideoTrackStateChanged "onUserVideoTrackStateChanged" | Occurs when the state of a remote video track changes.       |
| \ref agora::rtc::ILocalUserObserver::onRemoteVideoTrackStatistics "onRemoteVideoTrackStatistics" | Reports the statistics of the remote video track.            |
| \ref agora::rtc::ILocalUserObserver::onAudioVolumeIndication "onAudioVolumeIndication" | Reports which users are speaking, the speakers' volume, and whether the local user is speaking. |
| \ref agora::rtc::ILocalUserObserver::onUserInfoUpdated "onUserInfoUpdated" | Occurs when the user information is updated.                 |

<a name="localaudiotrack"></a>

### Local audio track

A local audio track can be created by \ref agora::base::IAgoraService::createLocalAudioTrack "IAgoraService::createLocalAudioTrack". It originates from the default audio recording device, that is, the built-in microphone. You can also use the APIs we provide in the \ref agora::rtc::INGAudioDeviceManager "IAudioDeviceManager" class if multiple recording devices are available in the system.

After the local audio track is created, you can publish one or multiple local audio tracks using \ref agora::rtc::ILocalUser::publishAudio "ILocalUser::publishAudio".

| Method                                                       | Description                                   |
| ------------------------------------------------------------ | --------------------------------------------- |
| \ref agora::rtc::ILocalAudioTrack::setEnabled "setEnabled"   | Enables or disables the local audio track.    |
| \ref agora::rtc::ILocalAudioTrack::getState "getState"       | Gets the state of the local audio.            |
| \ref agora::rtc::ILocalAudioTrack::GetStats "getStats"       | Gets the statistics of the local audio track. |
| \ref agora::rtc::ILocalAudioTrack::adjustPlayoutVolume "adjustPlayoutVolume" | Adjusts the audio volume for local playback.   |
| \ref agora::rtc::ILocalAudioTrack::adjustPublishVolume "adjustPublishVolume" | Adjusts the audio volume for publishing.      |
| \ref agora::rtc::ILocalAudioTrack::enableLocalPlayback "enableLocalPlayback" | Enables or disables local playback.           |
| \ref agora::rtc::ILocalAudioTrack::enableEarMonitor "enableEarMonitor" | Enables or disables ear monitor.              |
| \ref agora::rtc::IAudioTrack::addAudioFilter "addAudioFilter" | Adds an audio filter.                         |
| \ref agora::rtc::IAudioTrack::removeAudioFilter "removeAudioFilter" | Removes the audio filter.                     |

<a name="remoteaudiotrack"></a>

### Remote audio track

| Method                                                       | Description                              |
| ------------------------------------------------------------ | ---------------------------------------- |
| \ref agora::rtc::IRemoteAudioTrack::getStatistics "getStatistics" | Gets the statistics of the remote audio. |
| \ref agora::rtc::IRemoteAudioTrack::getState "getState"      | Gets the state of the remote audio.      |
| \ref agora::rtc::IAudioTrack::addAudioFilter "addAudioFilter" | Adds an audio filter.                    |
| \ref agora::rtc::IAudioTrack::removeAudioFilter "removeAudioFilter" | Removes the audio filter.                |

<a name="localvideotrack"></a>

### Local video track

A local video track can be created by either of the following methods:
- \ref agora::base::IAgoraService::createCameraVideoTrack "IAgoraService::createCameraVideoTrack"
- \ref agora::base::IAgoraService::createScreenVideoTrack "IAgoraService::createScreenVideoTrack"
- \ref agora::base::IAgoraService::createCustomVideoTrack(agora_refptr<rtc::IMediaPacketSender> source) "IAgoraService::createCustomVideoTrack"1
- \ref agora::base::IAgoraService::createCustomVideoTrack(agora_refptr<rtc::IVideoEncodedImageSender> videoSource, SenderOptions& options) "IAgoraService::createCustomVideoTrack"2
- \ref agora::base::IAgoraService::createCustomVideoTrack(agora_refptr<rtc::IVideoFrameSender> videoSource) "IAgoraService::createCustomVideoTrack"3

. It originates from one video source, for example, the camera capturer, the screen capturer, or a custom video source.

To customize the video track, choose the video node that you want to use, for example, the video source, the video filter, or video renderer in IVideoNodeFactory.

Apps can publish one or multiple local video tracks through \ref agora::rtc::ILocalUser::publishVideo "ILocalUser::publishVideo". Each video track has its own video stream. In other words, you can send the camera capturing stream and screen sharing stream simultaneously.

| Method                                                       | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| \ref agora::rtc::ILocalVideoTrack::setEnabled "setEnabled"    | Enables or disables the local video track.                   |
| \ref agora::rtc::ILocalVideoTrack::setVideoEncoderConfiguration "setVideoEncoderConfiguration" | Sets the video encoder configuration.                        |
| \ref agora::rtc::ILocalVideoTrack::enableSimulcastStream "enableSimulcastStream" | Enables or disables the simulcast stream mode.               |
| \ref aogra::rtc::ILocalVideoTrack::updateSimulcastStreamConfig  "updateSimulcastStreamConfig" | Update the simulcast stream config       | 
| \ref agora::rtc::ILocalVideoTrack::getState "getState"       | Gets the state of the local video.                           |
| \ref agora::rtc::ILocalVideoTrack::getStatistics "getStatistics" | Gets the statistics of the local video track.                |
| \ref agora::rtc::IVideoTrack::addVideoFilter "addVideoFilter" | Adds a video filter to the video track.                      |
| \ref agora::rtc::IVideoTrack::removeVideoFilter "removeVideoFilter" | Removes the video filter added by addVideoFilter.            |
| \ref agora::rtc::IVideoTrack::addRenderer "addRenderer"      | Adds a video renderer to the video track.                    |
| \ref agora::rtc::IVideoTrack::removeRenderer "removeRenderer" | Removes the video renderer added by addVideoRenderer from the video track. |

<a name="remotevideotrack"></a>

### Remote video track

| Method                                                       | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| \ref agora::rtc::IRemoteVideoTrack::getStatistics "getStatistics" | Gets the statistics of the remote video track.               |
| \ref agora::rtc::IRemoteVideoTrack::getState "getState"      | Gets the state of the remote video.                          |
| \ref agora::rtc::IRemoteVideoTrack::getTrackInfo "getTrackInfo" | Gets the information of the remote video track.              |
| \ref agora::rtc::IRemoteVideoTrack::registerVideoEncodedImageReceiver "registerVideoEncodedImageReceiver" | Registers an IVideoEncodedImageReceiver object.             |
| \ref agora::rtc::IRemoteVideoTrack::unregisterVideoEncodedImageReceiver "unregisterVideoEncodedImageReceiver" | Releases the IVideoEncodedImageReceiver object.              |
| \ref agora::rtc::IVideoTrack::addVideoFilter "addVideoFilter" | Adds a video filter to the video track.                      |
| \ref agora::rtc::IVideoTrack::removeVideoFilter "remoteVideoFilter" | Removes the video filter added by addVideoFilter.            |
| \ref agora::rtc::IVideoTrack::addRenderer "addRenderer"      | Adds a video renderer to the video track.                    |
| \ref agora::rtc::IVideoTrack::removeRenderer "removeRenderer" | Removes the video renderer added by addVideoRenderer from the video track. |

<a name="medianodefactory"></a>

### Media node factory

Use this factory to create audio and video nodes for the audio and video tracks.

| Method                                                       | Description                                                |
| ------------------------------------------------------------ | ---------------------------------------------------------- |
| \ref agora::rtc::IMediaNodeFactory::createAudioPcmDataSender "createAudioPcmDataSender" | Creates a PCM audio data sender.                           |
| \ref agora::rtc::IMediaNodeFactory::createAudioEncodedFrameSender "createAudioEncodedFrameSender" | Creates an encoded audio data sender.                      |
| \ref agora::rtc::IMediaNodeFactory::createAudioFilter "createAudioFilter" | Creates an audio filter.                                   |
| \ref agora::rtc::IMediaNodeFactory::createCameraCapturer "createCameraCapturer" | Creates a camera capturer.                                 |
| \ref agora::rtc::IMediaNodeFactory::createScreenCapturer "createScreenCapturer" | Creates a screen capturer.                                 |
| \ref agora::rtc::IMediaNodeFactory::createMediaPlayerSource "createMediaPlayerSource" | Creates a media player source object and returns the pointer.                                 |
| \ref agora::rtc::IMediaNodeFactory::createVideoFrameSender "createVideoFrameSender" | Creates a video frame sender.                              |
| \ref agora::rtc::IMediaNodeFactory::createVideoEncodedImageSender "createVideoEncodedImageSender" | Creates an encoded video image sender.                     |
| \ref agora::rtc::IMediaNodeFactory::createVideoRenderer "createVideoRenderer" | Creates a built-in video renderer.                         |

<a name="audiodatasender"></a>

### Audio data sender

| Method                                                       | Description                                            |
| ------------------------------------------------------------ | ------------------------------------------------------ |
| \ref agora::rtc::IAudioPcmDataSender::sendAudioPcmData "sendAudioPcmData" | Sends the PCM audio data to the local audio track.     |
| \ref agora::rtc::IAudioEncodedFrameSender::sendEncodedAudioFrame "sendEncodedAudioFrame" | Sends the encoded audio data to the local audio track. |

<a name="audiofilter"></a>

### Audio filter

| Method                                                       | Description                                 |
| ------------------------------------------------------------ | ------------------------------------------- |
| \ref agora::rtc::IAudioFilter::setEnabled "setEnabled"       | Enables or disables the audio filter.       |
| \ref agora::rtc::IAudioFilter::isEnabled "isEnabled"         | Checks whether the audio filter is enabled. |
| \ref agora::rtc::IAudioFilterBase::adaptAudioFrame "adaptAudioFrame" | Adapts the audio frame.                     |

<a name="audiodevicemanager"></a>

### Audio device manager

| Method                                                       | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| \ref agora::rtc::INGAudioDeviceManager::setMicrophoneVolume "setMicrophoneVolume" | Sets the volume of the microphone.                           |
| \ref agora::rtc::INGAudioDeviceManager::getMicrophoneVolume "getMicrophoneVolume" | Gets the volume of the microphone.                           |
| \ref agora::rtc::INGAudioDeviceManager::setSpeakerVolume "setSpeakerVolume" | Sets the volume of the speaker.                              |
| \ref agora::rtc::INGAudioDeviceManager::getSpeakerVolume "getSpeakerVolume" | Gets the volume of the speaker.                              |
| \ref agora::rtc::INGAudioDeviceManager::setMicrophoneMute "setMicrophoneMute" | Captures or stop capturing the local audio with the microphone. |
| \ref agora::rtc::INGAudioDeviceManager::getMicrophoneMute "getMicrophoneMute" | Gets the mute state of the microphone.                       |
| \ref agora::rtc::INGAudioDeviceManager::setSpeakerMute "setSpeakerMute" | Plays or stops playing the remote audio with the speaker.    |
| \ref agora::rtc::INGAudioDeviceManager::getSpeakerMute "getSpeakerMute" | Gets the mute state of the speaker.                          |
| \ref agora::rtc::INGAudioDeviceManager::setDefaultAudioRouting "setDefaultAudioRouting" | Sets the default audio routing. Applies to Android and iOS only. |
| \ref agora::rtc::INGAudioDeviceManager::changeAudioRouting "changeAudioRouting" | Changes the current audio routing. Applies to Android and iOS only. |
| \ref agora::rtc::INGAudioDeviceManager::getCurrentRouting "getCurrentRouting" | Gets the current audio routing. Applies to Android and iOS only. |
| \ref agora::rtc::INGAudioDeviceManager::getNumberOfPlayoutDevices "getNumberOfPlayoutDevices" | Gets the index numbers of all playout devices. Applies to Windows and macOS only. |
| \ref agora::rtc::INGAudioDeviceManager::getNumberOfRecordingDevices "getNumberOfRecordingDevices" | Gets the index numbers of all recording devices. Applies to Windows and macOS only. |
| \ref agora::rtc::INGAudioDeviceManager::getPlayoutDeviceInfo "getPlayoutDeviceInfo" | Gets the information of the current audio playout device. Applies to Windows and macOS only. |
| \ref agora::rtc::INGAudioDeviceManager::getRecordingDeviceInfo "getRecordingDeviceInfo" | Gets the information of the current recording device. Applies to Windows and macOS  only. |
| \ref agora::rtc::INGAudioDeviceManager::setPlayoutDevice "setPlayoutDevice" | Sets the audio playback device. Applies to Windows and macOS only. |
| \ref agora::rtc::INGAudioDeviceManager::setRecordingDevice "setRecordingDevice" | Sets the recording device. Applies to Windows and macOS only. |
| \ref agora::rtc::INGAudioDeviceManager::setApplicationVolume "setApplicationVolume" | Sets the volume of the app. Applies to Windows only.         |
| \ref agora::rtc::INGAudioDeviceManager::getApplicationVolume "getApplicationVolume" | Gets the volume of the app. Applies to Windows only.         |
| \ref agora::rtc::INGAudioDeviceManager::setApplicationMuteState "setApplicationMuteState" | Sets the mute state of the app.                              |
| \ref agora::rtc::INGAudioDeviceManager::getApplicationMuteState "getApplicationMuteState" | Gets the mute state of the app.                              |
| \ref agora::rtc::INGAudioDeviceManager::registerObserver "registerObserver" | Registers an IAudioDeviceManagerObserver object.             |
| \ref agora::rtc::INGAudioDeviceManager::unregisterObserver "unregisterObserver" | Releases the IAudioDeviceManagerObserver object.             |

| Event                                                        | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| \ref agora::rtc::IAudioDeviceManagerObserver::onDeviceStateChanged "onDeviceStateChanged" | Occurs when the device state changes.                        |
| \ref agora::rtc::IAudioDeviceManagerObserver::onRoutingChanged "onRoutingChanged" | Occurs when the audio route changes.                         |

<a name="videodatasender"></a>

### Video data sender

| Method                                                       | Description                                       |
| ------------------------------------------------------------ | ------------------------------------------------- |
| \ref agora::rtc::IVideoFrameSender::sendVideoFrame "sendVideoFrame" | Sends the video frame to the video track.         |
| \ref agora::rtc::IVideoEncodedImageSender::sendEncodedVideoImage "sendEncodedVideoImage" | Sends the encoded video image to the video track. |

<a name="videofilter"></a>

### Video filter

| Method                                                       | Description                                        |
| ------------------------------------------------------------ | -------------------------------------------------- |
| \ref agora::rtc::IVideoFilter::setEnabled "setEnabled"       | Enables or disables the video filter.              |
| \ref agora::rtc::IVideoFilter::isEnabled "isEnabled"          | Checks whether the video filter is enabled.        |
| \ref agora::rtc::IVideoFilter::setProperty "setProperty"     | Sets a private property in the IVideoFilter class. |
| \ref agora::rtc::IVideoFilter::getProperty "getProperty"     | Gets a private property in the IVideoFilter class. |
| \ref agora::rtc::IVideoBeautyFilter::setBeautyEffectOptions "setBeautyEffectOptions" | Sets the beauty effect options.                    |
| \ref agora::rtc::IVideoFilterBase::adaptVideoFrame "adaptVideoFrame" | Adapts the video frame.                            |

<a name="videorenderer"></a>

### Video renderer

| Method                                                       | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| \ref agora::rtc::IVideoRenderer::setRenderMode "setRenderMode" | Sets the render mode.                                        |
| \ref agora::rtc::IVideoRenderer::setMirror "setMirror"       | Sets whether to mirror the video.                            |
| \ref agora::rtc::IVideoRenderer::setView "setView"           | Sets the video display window.                               |
| \ref agora::rtc::IVideoRenderer::unsetView "unsetView"       | Stops rendering the video view on the window.                |
| \ref agora::rtc::IVideoSinkBase::onFrame "onFrame"           | Occurs when the IVideoSinkBase object receives the video frame. |

<a name="videocameracapturer"></a>

### Video camera capturer

| Method                                                       | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| \ref agora::rtc::ICameraCapturer::setCameraSource "setCameraSource" | Sets the camera source. Applies to Android and iOS only.     |
| \ref agora::rtc::ICameraCapturer::getCameraSource "getCameraSource" | Gets the camera source. Applies to Android and iOS only.     |
| \ref agora::rtc::ICameraCapturer::createDeviceInfo "createDeviceInfo" | Creates a DeviceInfo object. Applies to Windows, macOS, and Linux only. |
| \ref agora::rtc::ICameraCapturer::initWithDeviceId "initWithDeviceId" | Initialize the device with the device ID. Applies to Windows, macOS, and Linux only. |
| \ref agora::rtc::ICameraCapturer::initWithDeviceName "initWithDeviceName" | Initializes the device with the device name. Applies to Windows, macOS, and Linux only. |
| \ref agora::rtc::ICameraCapturer::setCaptureFormat "setCaptureFormat" | Sets the format of the video captured by the camera.         |
| \ref agora::rtc::ICameraCapturer::getCaptureFormat "getCaptureFormat" | Gets the format of the video captured by the camera.         |


<a name="videoscreencapturer"></a>

### Video screen capturer

| Method                                                       | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| \ref agora::rtc::IScreenCapturer::initWithDisplayId "initWithDisplayId" | Initializes the screen capturer by specifying a display ID. Applies to macOS only. |
| \ref agora::rtc::IScreenCapturer::initWithScreenRect "initWithScreenRect" | Initializes the screen capturer by specifying a screen Rect. Applies to Windows only. |
| \ref agora::rtc::IScreenCapturer::initWithWindowId "initWithWindowId" | Initializes the screen capturer by specifying a window ID. Applies to Windows and macOS only. |
| \ref agora::rtc::IScreenCapturer::setContentHint "setContentHint" | Sets the content hint for screen sharing. Applies to Windows and macOS only. |
| \ref agora::rtc::IScreenCapturer::updateScreenCaptureRegion "updateScreenCaptureRegion" | Updates the screen capture region. Applies to Windows and macOS only. |

<a name="mediaplayer"></a>

### Media player

| Method                                                       | Description                                           |
| ------------------------------------------------------------ | ----------------------------------------------------- |
| \ref agora::rtc::IMediaPlayerSource::open "open"                   | Opens a media file with a specified URL.              |
| \ref agora::rtc::IMediaPlayerSource::play "play"                   | Plays the media file.                                 |
| \ref agora::rtc::IMediaPlayerSource::stop "stop"                   | Stops playing the current media file.                 |
| \ref agora::rtc::IMediaPlayerSource::pause "pause"                 | Pauses playing the media file.                        |
| \ref agora::rtc::IMediaPlayerSource::getDuration "getDuration"     | Gets the duration of the media file.                  |
| \ref agora::rtc::IMediaPlayerSource::registerPlayerSourceObserver "registerPlayerSourceObserver" | Registers a media player source observer.                    |
| \ref agora::rtc::IMediaPlayerSource::unregisterPlayerSourceObserver "unregisterPlayerSourceObserver" | Releases the media player source observer.                   |


| Event                                                        | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| \ref agora::rtc::IMediaPlayerSourceObserver::onPlayerSourceStateChanged "onPlayerSourceStateChanged" | Occurs when the state of the media player has changed.       |
| \ref agora::rtc::IMediaPlayerSourceObserver::onPositionChanged "onPositionChanged" | Occurs when the playback position of the media player has changed. |

<a name="rtmpstreamingservice"></a>

### RTMP streaming service

| Method                                                       | Description                                            |
| ------------------------------------------------------------ | ------------------------------------------------------ |
| \ref agora::rtc::IRtmpStreamingService::addPublishStreamUrl "addPublishStreamUrl" | Publishes the local stream to the CDN.                 |
| \ref agora::rtc::IRtmpStreamingService::removePublishStreamUrl "removePublishStreamUrl" | Removes an RTMP stream from the CDN.                   |
| \ref agora::rtc::IRtmpStreamingService::setLiveTranscoding "setLiveTranscoding" | Sets the video layout and audio settings for CDN live. |
| \ref agora::rtc::IRtmpStreamingService::registerObserver "registerObserver" | Registers a RTMP streaming observer.                   |
| \ref agora::rtc::IRtmpStreamingService::unregisterObserver "unregisterObserver" | Releases the RTMP streaming observer.                  |

| Event                                                        | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| \ref agora::rtc::IRtmpStreamingObserver::onRtmpStreamingStateChanged "onRtmpStreamingStateChanged" | Occurs when the state of the RTMP streaming has changed.     |
| \ref agora::rtc::IRtmpStreamingObserver::onStreamPublished "onStreamPublished" | Reports the result of calling addPublishStreamUrl.           |
| \ref agora::rtc::IRtmpStreamingObserver::onStreamUnpublished "onStreamUnpublished" | Reports the result of calling removePublishStreamUrl.        |
| \ref agora::rtc::IRtmpStreamingObserver::onTranscodingUpdated "onTranscodingUpdated" | Occurs when the publisher's transcoding settings are updated. |

<a name="rtmservice"></a>

### RTM service

| Method                                                       | Description                                      |
| ------------------------------------------------------------ | ------------------------------------------------ |
| \ref agora::rtm::IRtmService::initialize "initialize"        | Initializes and gets an IRtmService instance.    |
| \ref agora::rtm::IRtmService::unregisterObserver "unregisterObserver" | Releases the IRtmServiceEventHandler object.      |
| \ref agora::rtm::IRtmService::release "release"              | Releases the IRtmService instance.               |
| \ref agora::rtm::IRtmService::login "login"                  | Logs in the RTM service.                         |
| \ref agora::rtm::IRtmService::logout "logout"                | Logs out of the RTM service.                     |
| \ref agora::rtm::IRtmService::sendMessageToPeer "sendMessageToPeer" | Sends a peer message to s specified remote user. |
| \ref agora::rtm::IRtmService::createChannel "createChannel"  | Creates an RTM channel.                          |

| Event                                                        | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| \ref agora::rtm::IRtmServiceEventHandler::onLoginSuccess "onLoginSuccess" | Occurs when the user successfully logs in the RTM service.   |
| \ref agora::rtm::IRtmServiceEventHandler::onLoginFailure "onLoginFailure" | Occurs when the user fails to log in the RTM service.        |
| \ref agora::rtm::IRtmServiceEventHandler::onLogout "onLogout" | Occurs when the user successfully logs out of the RTM service. |
| \ref agora::rtm::IRtmServiceEventHandler::onConnectionStateChanged "onConnectionStateChanged" | Occurs when the connection state of the local user has changed. |
| \ref agora::rtm::IRtmServiceEventHandler::onSendMessageState "onSendMessageState" | Reports the state of sending a message.                      |
| \ref agora::rtm::IRtmServiceEventHandler::onMessageReceivedFromPeer "onMessageReceivedFromPeer" | Occurs when the local user receives a message from a remote user. |

<a name="rtmchannel"></a>

### RTM channel

| Method                                                       | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| \ref agora::rtm::IChannel::setEventHandler "setEventHandler" | Sets an event handler for IChannel                           |
| \ref agora::rtm::IChannel::join "join"                       | Joins the current channel.                                   |
| \ref agora::rtm::IChannel::leave "leave"                     | Leaves the current channel.                                  |
| \ref agora::rtm::IChannel::sendMessage "sendMessage"         | Sends a channel message.                                     |
| \ref agora::rtm::IChannel::updateAttributes "updateAttributes" | Updates the channel attributes.                              |
| \ref agora::rtm::IChannel::deleteAttributes "deleteAttributes" | Removes the channel attributes.                             |
| \ref agora::rtm::IChannel::getId "getId"                     | Gets the current request ID.                                 |
| \ref agora::rtm::IChannel::release "release"                 | Releases the IChannel instance.                              |
| \ref agora::rtm::IChannelMember::getMemberId "getMemberId"   | Gets the ID of the channel member.                           |
| \ref agora::rtm::IChannelMember::getChannelId "getChannelId" | Gets the ID of the channel.                                  |
| \ref agora::rtm::IChannelMember::release "release"           | Release the IChannelMember class.                            |
| \ref agora::rtm::IChannelAttributes::createChannelAttributes "createChannelAttributes" | Creates an IChannelAttributes instance.                      |
| \ref agora::rtm::IChannelAttributes::addAttribute "addAttribute" | Adds an attribute to a specified channel.                   |
| \ref agora::rtm::IChannelAttributes::removeAttribute "removeAttribute" | Removes an attribute from the channel.                       |
| \ref agora::rtm::IChannelAttributes::getAttributesSize "getAttributesSize" | Gets the size of the attributes.                             |
| \ref agora::rtm::IChannelAttributes::getAttributes "getAttributes" | Gets the channel attributes.                                 |
| \ref agora::rtm::IChannelAttributes::getAttributeValue "getAttributeValue" | Gets the value of a channel attribute using the attribute key. |
| \ref agora::rtm::IChannelAttributes::release "release"       | Releases the IChannelAttributes instance.                    |

| Event                                                        | Description                                                |
| ------------------------------------------------------------ | ---------------------------------------------------------- |
| \ref agora::rtm::IChannelEventHandler::onJoinSuccess "onJoinSuccess" | Occurs when the local user successfully joins the channel. |
| \ref agora::rtm::IChannelEventHandler::onJoinFailure "onJoinFailure" | Occurs when the local user fails to join the channel.      |
| \ref agora::rtm::IChannelEventHandler::onLeave "onLeave"     | Occurs when the local user leaves the channel.             |
| \ref agora::rtm::IChannelEventHandler::onMessageReceived "onMessageReceived" | Occurs when the local user receives the channel message.   |
| \ref agora::rtm::IChannelEventHandler::onSendMessageState "onSendMessageState" | Reports the state od the message sent by the local user.   |
| \ref agora::rtm::IChannelEventHandler::onMemberJoined "onMemberJoined" | Occurs when another member joins the channel.              |
| \ref agora::rtm::IChannelEventHandler::onMemberLeft "onMemberLeft" | Occurs when the other member leaves the channel.           |
| \ref agora::rtm::IChannelEventHandler::onMembersGotten "onMembersGotten" | Reports all members in the channel                         |
| \ref agora::rtm::IChannelEventHandler::onAttributesUpdated "onAttributesUpdated" | Occurs when the channel attributes are updated.            |
| \ref agora::rtm::IChannelEventHandler::onUpdateAttributesResponse "onUpdateAttributesResponse" | Occurs when the local user calls updateAttributes.         |
| \ref agora::rtm::IChannelEventHandler::onAttributesDeleted "onAttributedDeleted" | Occurs when the channel attributed are deleted.            |
| \ref agora::rtm::IChannelEventHandler::onDeleteAttributesResponse "onDeleteAttributesResponse" | Occurs when the local user calls deleteAttributes.         |

<a name="rtmmessage"></a>

### RTM message

| Method                                                     | Description                                                  |
| ---------------------------------------------------------- | ------------------------------------------------------------ |
| \ref agora::rtm::IMessage::createMessage "createMessage"   | Creates an IMessage instance.                                |
| \ref agora::rtm::IMessage::getMessageId "getMessageId"     | Gets the ID of the message.                                  |
| \ref agora::rtm::IMessage::getMessageType "getMessageType" | Gets the message type.                                       |
| \ref agora::rtm::IMessage::setText "setText"               | Sets the content of the text message, or the text description of the raw message. |
| \ref agora::rtm::IMessage::getText "getText"               | Gets the content of the text message, or the text description of the raw message. |
| \ref agora::rtm::IMessage::release "release"               | Releases the IMessage instance.                              |

