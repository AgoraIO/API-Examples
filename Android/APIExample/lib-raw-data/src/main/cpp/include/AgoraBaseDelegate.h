//
//  AgoraRtcEngineKit.h
//  AgoraRtcEngineKit
//
//  Created by Sting Feng on 2015-8-11.
//  Copyright (c) 2015 Agora. All rights reserved.
//

@class AgoraRtcEngineKit;
#import "AgoraObjects.h"

@protocol AgoraBaseDelegate <NSObject>
@optional
#pragma mark SDK common delegates

#pragma mark String UID

/** Occurs when the local user successfully registers a user account by calling the
 * This callback reports the user ID and user account of the local user.
 * @param uid The ID of the local user.
 * @param userAccount The user account of the local user.
 */
- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine didLocalUserRegisteredWithUserId:(NSUInteger)uid userAccount:(NSString * _Nonnull)userAccount NS_SWIFT_NAME(rtcEngine(_:didLocalUserRegisteredWithUserId:userAccount:));

/** Occurs when the SDK gets the user ID and user account of the remote user.
 
 * After a remote user joins the channel, the SDK gets the UID and user account of the remote user,
 * caches them in a mapping table object (`userInfo`), and triggers this callback on the local client.
 
 * @param uid The ID of the remote user.
 * @param userInfo The `AgoraUserInfo` object that contains the user ID and user account of the remote user.
 */
- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine didUserInfoUpdatedWithUserId:(NSUInteger)uid userInfo:(AgoraUserInfo* _Nonnull)userInfo NS_SWIFT_NAME(rtcEngine(_:didUserInfoUpdatedWithUserId:userInfo:));

/**
 *  The warning occurred in SDK. The APP could igonre the warning, and the SDK could try to resume automically.
 *
 *  @param engine      The engine kit
 *  @param warningCode The warning code
 */
- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine didOccurWarning:(AgoraWarningCode)warningCode NS_SWIFT_NAME(rtcEngine(_:didOccurWarning:));

/**
 *  The error occurred in SDK. The SDK couldn't resume to normal state, and the app need to handle it.
 *
 *  @param engine    The engine kit
 *  @param errorCode The error code
 */
- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine didOccurError:(AgoraErrorCode)errorCode NS_SWIFT_NAME(rtcEngine(_:didOccurError:));

/**
 *  Event of load media engine success
 *
 *  @param engine The engine kit
 */
- (void)rtcEngineMediaEngineDidLoaded:(AgoraRtcEngineKit * _Nonnull)engine NS_SWIFT_NAME(rtcEngineMediaEngineDidLoaded(_:));

/**
 *  Event of media engine start call success
 *
 *  @param engine The engine kit
 */
- (void)rtcEngineMediaEngineDidStartCall:(AgoraRtcEngineKit * _Nonnull)engine NS_SWIFT_NAME(rtcEngineMediaEngineDidStartCall(_:));

/**
 * when token is enabled, and specified token is invalid or expired, this function will be called.
 * APP should generate a new token and call renewToken() to refresh the token.
 * NOTE: to be compatible with previous version, ERR_TOKEN_EXPIRED and ERR_INVALID_TOKEN are also reported via onError() callback.
 * You should move renew of token logic into this callback.
 *  @param engine The engine kit
 */
- (void)rtcEngineRequestToken:(AgoraRtcEngineKit * _Nonnull)engine NS_SWIFT_NAME(rtcEngineRequestToken(_:));

/**
 *  Event of disconnected with server. This event is reported at the moment SDK loses connection with server.
 *  In the mean time SDK automatically tries to reconnect with the server until APP calls leaveChannel.
 *
 *  @param engine The engine kit
 */
- (void)rtcEngineConnectionDidInterrupted:(AgoraRtcEngineKit * _Nonnull)engine NS_SWIFT_NAME(rtcEngineConnectionDidInterrupted(_:));

/**
 *  Event of loss connection with server. This event is reported after the connection is interrupted and exceed the retry period (10 seconds by default).
 *  In the mean time SDK automatically tries to reconnect with the server until APP calls leaveChannel.
 *
 *  @param engine The engine kit
 */
- (void)rtcEngineConnectionDidLost:(AgoraRtcEngineKit * _Nonnull)engine NS_SWIFT_NAME(rtcEngineConnectionDidLost(_:));

/**
 *  Event of connection banned by server. This event is reported after the connection is banned by server.
 *  In the mean time SDK will not try to reconnect the server.
 */
- (void)rtcEngineConnectionDidBanned:(AgoraRtcEngineKit * _Nonnull)engine NS_SWIFT_NAME(rtcEngineConnectionDidBanned(_:));

/**
 *  Statistics of rtc engine status. Updated every two seconds.
 *
 *  @param engine The engine kit
 *  @param stats  The statistics of rtc status, including duration, sent bytes and received bytes
 */
- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine reportRtcStats:(AgoraChannelStats * _Nonnull)stats NS_SWIFT_NAME(rtcEngine(_:reportRtcStats:));

/**
 *  The network quality of lastmile test.
 *
 *  @param engine  The engine kit
 *  @param quality The network quality
 */
- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine lastmileQuality:(AgoraNetworkQuality)quality NS_SWIFT_NAME(rtcEngine(_:lastmileQuality:));

/**
 * Reports the last-mile network probe result.
 *
 * The SDK triggers this callback within 30 seconds after the app calls the [startLastmileProbeTest]([AgoraRtcEngineKit startLastmileProbeTest:]) method.
 *
 * @param engine AgoraRtcEngineKit object.
 * @param result The uplink and downlink last-mile network probe test result, see [AgoraLastmileProbeResult](AgoraLastmileProbeResult).
 */
- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine lastmileProbeTestResult:(AgoraLastmileProbeResult * _Nonnull)result NS_SWIFT_NAME(rtcEngine(_:lastmileProbeTest:));

/**
 *  Event of API call executed
 *  @deprecated This callback is deprecated. Use other specific event callbacks instead.
 *
 *  @param engine The engine kit
 *  @param api    The API description
 *  @param error  The error code
 */
- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine didApiCallExecute:(NSInteger)error api:(NSString * _Nonnull)api result:(NSString * _Nonnull)result NS_SWIFT_NAME(rtcEngine(_:didApiCallExecute:api:result:)) __deprecated;


#if (!(TARGET_OS_IPHONE) && (TARGET_OS_MAC))
/**
 *  the notificaitoin of device added removed
 *
 *  @param engine The engine kit
 *  @param deviceId   the identification of device
 *  @param deviceType type of device: -1: audio unknown; 0: audio recording ; 1: audio playout ; 2: render; 4: capture
 *  @param state      state of device: 0: added; 1: removed
 */
- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine device:(NSString * _Nonnull)deviceId type:(AgoraMediaDeviceType)deviceType stateChanged:(NSInteger)state NS_SWIFT_NAME(rtcEngine(_:device:type:stateChanged:));

#endif

/**
 *  An error of encryption occurred during SDK runtime.
 *
 *  @param errorType AgoraEncryptionErrorType
 */
- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine didOccurEncryptionError:(AgoraEncryptionErrorType)errorType NS_SWIFT_NAME(rtcEngine(_:didOccur:));
/**
   * Reports the user log upload result
   * @param requestId RequestId of the upload
   * @param success Is upload success
   * @param reason Reason of the upload, 0: OK, 1 Network Error, 2 Server Error.
   */
- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine uploadLogResultRequestId:(NSString * _Nonnull)requestId success:(BOOL)success reason:(AgoraUploadErrorReason)reason NS_SWIFT_NAME(rtcEngine(_:uploadLogResultRequestId:success:reason:));

#pragma mark Local user common delegates

/**
 *  Event of cient role change. only take effect under broadcasting mode
 *
 *  @param engine The engine kit
 *  @param oldRole The previous role of the local user
 *  @param newRole The new role of the local user
 *  @param newRoleOptions The client role options of the current role of the local user
 */
- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine didClientRoleChanged:(AgoraClientRole)oldRole newRole:(AgoraClientRole)newRole newRoleOptions:(AgoraClientRoleOptions * _Nullable)newRoleOptions NS_SWIFT_NAME(rtcEngine(_:didClientRoleChanged:newRole:newRoleOptions:));

/**
 *  Event of cient role change. only take effect under broadcasting mode
 *
 *  @param engine The engine kit
 *  @param reason The reason of the failure of the local user role switches
 *  @param currentRole The current role of the user
 */
- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine didClientRoleChangeFailed:(AgoraClientRoleChangeFailedReason)reason currentRole:(AgoraClientRole)currentRole NS_SWIFT_NAME(rtcEngine(_:didClientRoleChangeFailed:currentRole:));

/**
 *  The statistics of the call when leave channel
 *
 *  @param engine The engine kit
 *  @param stats  The statistics of the call, including duration, sent bytes and received bytes
 */
- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine didLeaveChannelWithStats:(AgoraChannelStats * _Nonnull)stats NS_SWIFT_NAME(rtcEngine(_:didLeaveChannelWith:));

#pragma mark Local user audio delegates
/**
 *  Event of the first audio frame is sent.
 *
 *  @param engine  The engine kit
 *  @param elapsed The elapsed time(ms) from the beginning of the session.
 */
- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine firstLocalAudioFramePublished:(NSInteger)elapsed NS_SWIFT_NAME(rtcEngine(_:firstLocalAudioFramePublished:));

/** The statistics of the local audio stream.

 Same as [localAudioStatBlock]([AgoraRtcEngineKit localAudioStatBlock:]). The SDK updates the application on the statistics of the local audio stream once every two seconds.

 * @param engine The AgoraRtcEngineKit object.
 * @param stats  AgoraRtcLocalAudioStats
 */
- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine localAudioStats:(AgoraRtcLocalAudioStats * _Nonnull)stats NS_SWIFT_NAME(rtcEngine(_:localAudioStats:));

/**
 *  Event of local audio route changed
 *
 *  @param engine The engine kit
 *  @param routing the current audio output routing
 */
- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine didAudioRouteChanged:(AgoraAudioOutputRouting)routing NS_SWIFT_NAME(rtcEngine(_:didAudioRouteChanged:));

/**
 *  Event of finish audio mixing.
 *
 *  @param engine The engine kit
 */
- (void)rtcEngineLocalAudioMixingDidFinish:(AgoraRtcEngineKit * _Nonnull)engine NS_SWIFT_NAME(rtcEngineLocalAudioMixingDidFinish(_:));

/** Audio mixing state changed.

 * @param engine  The AgoraRtcEngineKit object.
 * @param state  AgoraAudioMixingStateType
 * @param reasonCode  AgoraAudioMixingReasonType
 */
- (void)rtcEngine:(AgoraRtcEngineKit *_Nonnull)engine audioMixingStateChanged:(AgoraAudioMixingStateType)state
                                                                    reasonCode:(AgoraAudioMixingReasonCode)reasonCode NS_SWIFT_NAME(rtcEngine(_:audioMixingStateChanged:reasonCode:));

/**
 * @brief Reports current AudioMixing progress.
 *
 * The callback occurs once every one second during the playback and reports the current playback progress.
 * @param position Current AudioMixing progress (millisecond).
 */
- (void)rtcEngine:(AgoraRtcEngineKit *_Nonnull)engine audioMixingPositionChanged:(NSInteger)position NS_SWIFT_NAME(rtcEngine(_:audioMixingPositionChanged:));

/**
 *  Event of finish audio effect.
 *
 *  @param engine The engine kit
 */
- (void)rtcEngineDidAudioEffectFinish:(AgoraRtcEngineKit * _Nonnull)engine soundId:(int)soundId NS_SWIFT_NAME(rtcEngineDidAudioEffectFinish(_:soundId:));


#pragma mark Local user video delegates
/**
 *  Event of camera opened
 *
 *  @param engine The engine kit
 */
- (void)rtcEngineCameraDidReady:(AgoraRtcEngineKit * _Nonnull)engine NS_SWIFT_NAME(rtcEngineCameraDidReady(_:));

#if TARGET_OS_IPHONE
/**
 *  Event of camera focus position changed
 *
 *  @param engine The engine kit
 *  @param rect   The focus rect in local preview
 */
- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine cameraFocusDidChangedToRect:(CGRect)rect NS_SWIFT_NAME(rtcEngine(_:cameraFocusDidChangedTo:));

/** A camera exposure position changed.

 @param engine AgoraRtcEngineKit object
 @param rect   Exposure rectangle in the local preview
 */
- (void)rtcEngine:(AgoraRtcEngineKit* _Nonnull)engine cameraExposureDidChangedToRect:(CGRect)rect NS_SWIFT_NAME(rtcEngine(_:cameraExposureDidChangedTo:));

#endif

/**
 *  Event of camera stopped
 *
 *  @param engine The engine kit
 */
- (void)rtcEngineVideoDidStop:(AgoraRtcEngineKit * _Nonnull)engine NS_SWIFT_NAME(rtcEngineVideoDidStop(_:));

/** Event of the first local video frame is published.
 *  @param engine  The engine kit
 *  @param elapsed The elapsed time(ms) from the beginning of the session.
 *
 */
- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine firstLocalVideoFramePublishedWithElapsed:(NSInteger)elapsed
       sourceType:(AgoraVideoSourceType)source NS_SWIFT_NAME(rtcEngine(_:firstLocalVideoFramePublishedWithElapsed:sourceType:));

/**
 *  Event of the first local frame starts rendering on the screen.
 *
 *  @param engine  The engine kit
 *  @param size    The size of local video stream
 *  @param elapsed The elapsed time(ms) from the beginning of the session.
 */
- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine firstLocalVideoFrameWithSize:(CGSize)size elapsed:(NSInteger)elapsed sourceType:(AgoraVideoSourceType)source NS_SWIFT_NAME(rtcEngine(_:firstLocalVideoFrameWith:elapsed:sourceType:));

/**
 *  The statistics of local video stream. Update every two seconds.
 *
 *  @param engine        The engine kit
 *  @param stats         The statistics of local video, including sent bitrate, sent framerate
 */
- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine localVideoStats:(AgoraRtcLocalVideoStats * _Nonnull)stats
       sourceType:(AgoraVideoSourceType)source NS_SWIFT_NAME(rtcEngine(_:localVideoStats:sourceType:));

#pragma mark Remote user audio delegates

/**
 *  The sdk reports the volume of a speaker. The interface is disable by default, and it could be enable by API "enableAudioVolumeIndication"
 *
 *  @param engine      The engine kit
 *  @param speakers    AgoraRtcAudioVolumeInfo array
 *  @param totalVolume The total volume of speakers
 */
- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine reportAudioVolumeIndicationOfSpeakers:(NSArray<AgoraRtcAudioVolumeInfo *> * _Nonnull)speakers totalVolume:(NSInteger)totalVolume NS_SWIFT_NAME(rtcEngine(_:reportAudioVolumeIndicationOfSpeakers:totalVolume:));

/**
 *  Event of remote start audio mixing.
 *
 *  @param engine The engine kit
 */
- (void)rtcEngineRemoteAudioMixingDidStart:(AgoraRtcEngineKit * _Nonnull)engine NS_SWIFT_NAME(rtcEngineRemoteAudioMixingDidStart(_:));

/**
 *  Event of remote finish audio mixing.
 *
 *  @param engine The engine kit
 */
- (void)rtcEngineRemoteAudioMixingDidFinish:(AgoraRtcEngineKit * _Nonnull)engine NS_SWIFT_NAME(rtcEngineRemoteAudioMixingDidFinish(_:));

/**
 *  The statistics of remote audio stream. Update every two seconds.
 *
 *  @param engine            The engine kit
 *  @param stats             The statistics of audio video, including user id, delay, resolution, received bitrate, received framerate, audio stream type
 */
- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine remoteAudioStats:(AgoraRtcRemoteAudioStats * _Nonnull)stats NS_SWIFT_NAME(rtcEngine(_:remoteAudioStats:));

#pragma mark Remote user video delegates

/**
 *  The statistics of remote video stream. Update every two seconds.
 *
 *  @param engine            The engine kit
 *  @param stats             The statistics of remote video, including user id, delay, resolution, received bitrate, received framerate, video stream type
 */
- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine remoteVideoStats:(AgoraRtcRemoteVideoStats * _Nonnull)stats NS_SWIFT_NAME(rtcEngine(_:remoteVideoStats:));

- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine rtmpStreamingChangedToState:(NSString * _Nonnull)url
                                                                    state:(AgoraRtmpStreamingState)state
                                                                  errCode:(AgoraRtmpStreamingErrorCode)errCode NS_SWIFT_NAME(rtcEngine(_:rtmpStreamingChangedToState:state:errCode:));

- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine rtmpStreamingEventWithUrl:(NSString* _Nonnull)url eventCode:(AgoraRtmpStreamingEvent)eventCode NS_SWIFT_NAME(rtcEngine(_:rtmpStreamingEventWithUrl:eventCode:));

- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine streamPublishedWithUrl:(NSString * _Nonnull)url errorCode:(AgoraErrorCode)errorCode NS_SWIFT_NAME(rtcEngine(_:streamPublishedWithUrl:errorCode:));

- (void)rtcEngine:(AgoraRtcEngineKit * _Nonnull)engine streamUnpublishedWithUrl:(NSString * _Nonnull)url NS_SWIFT_NAME(rtcEngine(_:streamUnpublishedWithUrl:));

- (void)rtcEngineTranscodingUpdated:(AgoraRtcEngineKit * _Nonnull)engine NS_SWIFT_NAME(rtcEngineTranscodingUpdated(_:));

#pragma mark Face Detection Delegate Methods
/**-----------------------------------------------------------------------------
 * @name Face Detection Delegate Methods
 * -----------------------------------------------------------------------------
 */

/** Reports the face detection result of the local user. (iOS only)

 **Since:** v3.0.1.

 Once you enable face detection by calling [enableFaceDetection]([AgoraRtcEngineKit enableFaceDetection:]), you can get the following information on the local user in real-time:

 - The width and height of the local video.
 - The position of the human face in the local video.
 - The distance between the human face and the device screen. This value is based on the fitting calculation of the local video size and the position of the human face.

 **Note**

 - If the SDK does not detect a face, it reduces the frequency of this callback to reduce power consumption on the local device.
 - The SDK stops triggering this callback when a human face is in close proximity to the screen.

 @param engine AgoraRtcEngineKit object.
 @param width The width (px) of the local video.
 @param height The height (px) of the local video.
 @param faces An AgoraFacePositionInfo array, which contains the information of the detected human face.

 The number of the AgoraFacePositionInfo array depends on the number of human faces detected. If the array length is 0, it means that no human face is detected.
 */
- (void)rtcEngine:(AgoraRtcEngineKit* _Nonnull)engine facePositionDidChangeWidth:(int)width previewHeight:(int)height faces:(NSArray<AgoraFacePositionInfo*>* _Nullable)faces  NS_SWIFT_NAME(rtcEngine(_:facePositionDidChangeWidth:previewHeight:faces:));

@end

/*

- (int)joinChannelByToken:(NSString * _Nullable)token
         channelId:(NSString * _Nonnull)channelId
        info:(NSString * _Nullable)info
     userId:(NSString * _Nullable)userId NS_SWIFT_NAME(joinChannel(byToken:channelId:info:userId:));
 
- (int)setRemoteRenderModeWithUid:(NSString * _Nonnull)userId
              mode:(AgoraVideoRenderMode)mode NS_SWIFT_NAME(setRemoteRenderModeWithUid(_:mode:));
 
- (int)switchView:(NSString * _Nonnull)userId1
       andAnother:(NSString * _Nonnull)userId2 NS_SWIFT_NAME(switchView(_:andAnother:)) __deprecated;

- (int)muteRemoteAudioStream:(NSString * _Nonnull)userId mute:(BOOL)mute NS_SWIFT_NAME(muteRemoteAudioStream(_:mute:));

- (int)setRemoteVideoStream:(NSString * _Nonnull)userId
                       type:(AgoraVideoStreamType)streamType NS_SWIFT_NAME(setRemoteVideoStream(_:type:));
- (int)muteRemoteVideoStream:(NSString * _Nonnull)userId
                        mute:(BOOL)mute NS_SWIFT_NAME(muteRemoteVideoStream(_:mute:));

- (void)setRemoteVideoRenderer:(id<AgoraVideoSinkProtocol> _Nullable)videoRenderer withUserId:(NSString * _Nonnull)userId NS_SWIFT_NAME(setRemoteVideoRenderer(_:withUserId:));

- (int)sendPublishingRequestToOwner:(NSString * _Nonnull)userId NS_SWIFT_NAME(sendPublishingRequestToOwner(_:));
- (int)answerPublishingRequestOfUid:(NSString * _Nonnull)userId accepted:(bool)accepted NS_SWIFT_NAME(answerPublishingRequestOfUid(_:accepted:));
- (int)sendUnpublishingRequestToUid:(NSString * _Nonnull)userId NS_SWIFT_NAME(sendUnpublishingRequestToUid(_:));

- (id<AgoraVideoSinkProtocol> _Nullable)remoteVideoRendererOfUserId:(NSString * _Nonnull)userId NS_SWIFT_NAME(remoteVideoRendererOfUserId(_:));
*/
