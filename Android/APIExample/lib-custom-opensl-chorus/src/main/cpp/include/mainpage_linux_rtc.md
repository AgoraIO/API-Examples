# Agora C++ API Reference for All Platforms

Agora provides ensured quality of experience (QoE) for worldwide Internet-based voice and video communications through a virtual global network optimized for real-time web and mobile-to-mobile applications.

This page provides reference for Agora low-level APIs:

* [Agora service](#agoraservice)
* [RTC connection](#rtcconnection)
* [Local user](#localuser)
* [Local audio track](#localaudiotrack)
* [Remote audio track](#remoteaudiotrack)
* [Local video track](#localvideotrack)
* [Remote video track](#remotevideotrack)
* [Media node factory](#medianodefactory)
* [Audio data sender](#audiodatasender)
* [Video data sender](#videodatasender)

<a name="agoraservice"></a>

### Agora service

IAgoraService is the entry point of Agora Low Level APIs. Use this interface to create access points to Agora Modules, including RTC Connection, Media Tracks, etc.

| Method                                                       | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| \ref createAgoraService "createAgoraService"                 | Creates an IAgoraService object and returns the pointer.     |
| \ref agora::base::IAgoraService::initialize "initialize"     | Initializes the IAgoraService object.                        |
| \ref agora::base::IAgoraService::release "release"           | Releases the IAgoraService object.                           |
| \ref agora::base::IAgoraService::setLogFile "setLogFile"     | Sets the log file path and size.                             |
| \ref agora::base::IAgoraService::setLogFilter "setLogFilter" | Sets the log output level.                                   |
| \ref agora::base::IAgoraService::createRtcConnection "createRtcConnection" | Creates an RTC connection object and returns the pointer.    |
| \ref agora::base::IAgoraService::createCustomAudioTrack(agora_refptr<rtc::IAudioPcmDataSender> audioSource) "createCustomAudioTrack"1 | Creates a local audio track object with the PCM data receiver and returns the pointer. |
| \ref agora::base::IAgoraService::createCustomAudioTrack(agora_refptr<rtc::IAudioEncodedFrameSender> audioSource, TMixMode mixMode) "createCustomAudioTrack"2 | Creates a local audio track object with the receiver for encoded audio data and returns the pointer. |
| \ref agora::base::IAgoraService::createMediaNodeFactory "createMediaNodeFactory" | Creates a media node factory and returns the pointer.        |
| \ref agora::base::IAgoraService::createCustomVideoTrack(agora_refptr<rtc::IVideoFrameSender> videoSource) "createCustomVideoTrack"1 | Creates a local video track object with the custom video source ans returns the pointer. |
| \ref agora::base::IAgoraService::createCustomVideoTrack(agora_refptr<rtc::IVideoEncodedImageSender> videoSource, SenderOptions& options) "createCustomVideoTrack"2 | Creates a local video track object with the receiver for encoded video data and returns the pointer. |

<a name="rtcconnection"></a>

### RTC connection

IRtcConnection allows an app to establish a connection to an Agora channel. In order to establish a connection, the app must provide an app ID, a token, and the ID of the channel that you want to join.

| Method                                                       | Description                                              |
| ------------------------------------------------------------ | -------------------------------------------------------- |
| \ref agora::rtc::IRtcConnection::connect "connect"           | Connects to an Agora channel.                            |
| \ref agora::rtc::IRtcConnection::disconnect "disconnect"     | Disconnects from the channel.                            |
| \ref agora::rtc::IRtcConnection::startLastmileProbeTest "startLastmileProbeTest" | Starts the last-mile network probe test.                 |
| \ref agora::rtc::IRtcConnection::stopLastmileProbeTest "stopLastmileProbeTest" | Stops the last-mile network probe test.                  |
| \ref agora::rtc::IRtcConnection::renewToken "renewToken"     | Renews the token.                                        |
| \ref agora::rtc::IRtcConnection::getConnectionInfo "getConnectionInfo" | Gets the current connection information.                 |
| \ref agora::rtc::IRtcConnection::getLocalUser "getLocalUser" | Gets the ILocalUser object.                              |
| \ref agora::rtc::IRtcConnection::getRemoteUsers "getRemoteUsers" | Gets the information of remote users.                    |
| \ref agora::rtc::IRtcConnection::getUserInfo "getUserInfo"   | Gets the information of a specified user in the channel. |
| \ref agora::rtc::IRtcConnection::registerObserver "registerObserver" | Registers an IRtcConnectionObserver object.              |
| \ref agora::rtc::IRtcConnection::unregisterObserver "unregisterObserver" | Releases the registered IRtcConnectionObserver object.   |
| \ref agora::rtc::IRtcConnection::getConnId "getConnId"       | Gets the ID of the current connection.                   |
| \ref agora::rtc::IRtcConnection::getTransportStats "getTransportStats" | Gets the transportation statistics of this connection.   |
| \ref agora::rtc::IRtcConnection::getAgoraParameter "getAgoraParameter" | Gets an IAgoraParameter object.                          |

Connecting to an Agora channel is done asynchronously, and apps can listen for the connection states of both the local and remote user using an RTC connection observer.

| Event                                                        | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| \ref agora::rtc::IRtcConnectionObserver::onConnected "onConnected" | Occurs when the connection state changes to STATE_CONNECTED. |
| \ref agora::rtc::IRtcConnectionObserver::onDisconnected "onDisconnected" | Occurs when the connection state changes to STATE_DISCONNECTED. |
| \ref agora::rtc::IRtcConnectionObserver::onReconnecting "onReconnecting" | Occurs when the connection state changes to STATE_RECONNECTING. |
| \ref agora::rtc::IRtcConnectionObserver::onConnectionLost "onConnectionLost" | Occurs when the SDK loses connection with the Agora channel. |
| \ref agora::rtc::IRtcConnectionObserver::onLastmileQuality "onLastmileQuality" | Reports the quality of the last mile network.                |
| \ref agora::rtc::IRtcConnectionObserver::onLastmileProbeResult "onLastmileProbeResult" | Reports the last-mile network probe result.                  |
| \ref agora::rtc::IRtcConnectionObserver::onTokenPrivilegeWillExpire "onTokenPrivilegeWillExpire" | Occurs when the token privilege expires in 30 seconds.       |
| \ref agora::rtc::IRtcConnectionObserver::onTokenPrivilegeDidExpire "onTokenPrivilegeDidExpire" | Occurs when the token privilege has expired.                 |
| \ref agora::rtc::IRtcConnectionObserver::onConnectionFailure "onConnectionFailure" | Occurs when the connection state changes to STATE_FAILED.    |
| \ref agora::rtc::IRtcConnectionObserver::onUserJoined "onUserJoined" | Occurs when a remote user has successfully joined the channel. |
| \ref agora::rtc::IRtcConnectionObserver::onUserLeft "onUserLeft" | Occurs when a remote user has left the channel.              |
| \ref agora::rtc::IRtcConnectionObserver::onTransportStats "onTransportStats" | Reports the transport statistics of the current connection,  |
| \ref agora::rtc::IRtcConnectionObserver::onChangeRoleSuccess "onChangeRoleSuccess" | Occurs when the role of the local user changes.              |
| \ref agora::rtc::IRtcConnectionObserver::onChangeRoleFailure "onChangeRoleFailure" | Occurs when the role of the local user changes.              |
| \ref agora::rtc::IRtcConnectionObserver::onUserNetworkQuality "onUserNetworkQuality" | Reports the network quality of each user.                    |

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
| \ref agora::rtc::ILocalUserObserver::onLocalAudioTrackStatistics "onLocalAudioTrackStatistics" | Reports the statistics of the local audio track.             |
| \ref agora::rtc::ILocalUserObserver::onRemoteAudioTrackStatistics "onRemoteAudioTrackStatistics" | Reports the statistics of the remote audio track.            |
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

<a name="localaudiotrack"></a>

### Local audio track

A local audio track can be created by \ref agora::base::IAgoraService::createCustomAudioTrack "createCustomAudioTrack".

After the local audio track is created, you can publish one or multiple local audio tracks using \ref agora::rtc::ILocalUser::publishAudio "ILocalUser::publishAudio".

| Method                                                       | Description                                    |
| ------------------------------------------------------------ | ---------------------------------------------- |
| \ref agora::rtc::ILocalAudioTrack::setEnabled "setEnabled"   | Enables or disables the local audio track.     |
| \ref agora::rtc::ILocalAudioTrack::isEnabled "isEnabled"     | Gets whether the local audio track is enabled. |
| \ref agora::rtc::ILocalAudioTrack::getState "getState"       | Gets the state of the local audio.             |
| \ref agora::rtc::ILocalAudioTrack::GetStats "GetStats"       | Gets the statistics of the local audio track.  |
| \ref agora::rtc::ILocalAudioTrack::adjustPublishVolume "adjustPublishVolume" | Adjusts the audio volume for publishing.       |
| \ref agora::rtc::ILocalAudioTrack::getPublishVolume "getPublishVolume" | Gets the current volume for publishing.        |

<a name="remoteaudiotrack"></a>

### Remote audio track

| Method                                                       | Description                                    |
| ------------------------------------------------------------ | ---------------------------------------------- |
| \ref agora::rtc::IRemoteAudioTrack::getStatistics "getStatistics" | Gets the statistics of the remote audio track. |
| \ref agora::rtc::IRemoteAudioTrack::getState "getState"      | Gets the state of the remote audio.            |

<a name="localvideotrack"></a>

### Local video track

A local video track can be created by \ref agora::base::IAgoraService::createCustomVideoTrack "createCustomVideoTrack".

It originates from one video source, for example, the camera capturer, the screen capturer, or a custom video source.

To customize the video track, choose the video node that you want to use, for example,  the video source, the video filter, or video renderer in IVideoNodeFactory.

Apps can publish one or multiple local video tracks through \ref agora::rtc::ILocalUser::publishVideo "ILocalUser::publishVideo". Each video track has its own video stream. In other words, you can send the camera capturing stream and screen sharing stream simultaneously.

| Method                                                       | Description                                |
| ------------------------------------------------------------ | ------------------------------------------ |
| \ref agora::rtc::ILocalVideoTrack::setEnabled "setEnabled"   | Enables or disables the local video track. |
| \ref agora::rtc::ILocalVideoTrack::setVideoEncoderConfiguration "setVideoEncoderConfiguration" | Sets the video encoder configuration.      |
| \ref agora::rtc::ILocalVideoTrack::getState "getState"       | Gets the state of the local video.         |

<a name="remotevideotrack"></a>

### Remote video track

| Method                                                       | Description                                     |
| ------------------------------------------------------------ | ----------------------------------------------- |
| \ref agora::rtc::IRemoteVideoTrack::getState "getState"      | Gets the state of the remote video.             |
| \ref agora::rtc::IRemoteVideoTrack::getTrackInfo "getTrackInfo" | Gets the information of the remote video track. |
| \ref agora::rtc::IRemoteVideoTrack::registerVideoEncodedImageReceiver "registerVideoEncodedImageReceiver" | Registers an IVideoEncodedImageReceiver object. |
| \ref agora::rtc::IRemoteVideoTrack::unregisterVideoEncodedImageReceiver "unregisterVideoEncodedImageReceiver" | Releases the IVideoEncodedImageReceiver object. |
| \ref agora::rtc::IRemoteVideoTrack::addRenderer "addRenderer" | Adds a video renderer to the video track.       |

<a name="medianodefactory"></a>

### Media node factory

Use this factory to create audio and video nodes for the audio and video tracks.

| Method                                                       | Description                            |
| ------------------------------------------------------------ | -------------------------------------- |
| \ref agora::rtc::IMediaNodeFactory::createAudioPcmDataSender "createAudioPcmDataSender" | Creates a PCM audio data sender.       |
| \ref agora::rtc::IMediaNodeFactory::createAudioEncodedFrameSender "createAudioEncodedFrameSender" | Creates an encoded audio data sender.  |
| \ref agora::rtc::IMediaNodeFactory::createVideoFrameSender "createVideoFrameSender" | Creates a video frame sender.          |
| \ref agora::rtc::IMediaNodeFactory::createVideoEncodedImageSender "createVideoEncodedImageSender" | Creates an encoded video image sender. |

<a name="audiodatasender"></a>

### Audio data sender

| Method                                                       | Description                                            |
| ------------------------------------------------------------ | ------------------------------------------------------ |
| \ref agora::rtc::IAudioPcmDataSender::sendAudioPcmData "sendAudioPcmData" | Sends the PCM audio data to the local audio track.     |
| \ref agora::rtc::IAudioEncodedFrameSender::sendEncodedAudioFrame "sendEncodedAudioFrame" | Sends the encoded audio data to the local audio track. |

<a name="videodatasender"></a>

### Video data sender

| Method                                                       | Description                                       |
| ------------------------------------------------------------ | ------------------------------------------------- |
| \ref agora::rtc::IVideoFrameSender::sendVideoFrame "sendVideoFrame" | Sends the video frame to the video track.         |
| \ref agora::rtc::IVideoEncodedImageSender::sendEncodedVideoImage "sendEncodedVideoImage" | Sends the encoded video image to the video track. |