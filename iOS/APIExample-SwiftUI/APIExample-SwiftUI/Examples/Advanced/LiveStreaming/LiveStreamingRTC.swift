//
//  LiveStreamingRTC.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/9/25.
//

import Foundation
import Combine
import AgoraRtcKit

class LiveStreamingRTC: NSObject, ObservableObject {
    @Published var role: AgoraClientRole = .broadcaster
    @Published var showUltraLowEntry: Bool = false
    @Published var showLinkStreamEntry: Bool = false
    var agoraKit: AgoraRtcEngineKit!
    
    private var isJoined: Bool = false
    private var foregroundView: VideoUIView?
    private var backgroundView: VideoUIView?
    private var configs: [String: Any]!
    var isUltraLowLatencyOn: Bool = false
    var isLocalVideoForeground = false {
        didSet {
            if isLocalVideoForeground {
                backgroundView?.setPlaceholder(text: "Local Host".localized)
                foregroundView?.setPlaceholder(text: "Remote Host".localized)
            } else {
                backgroundView?.setPlaceholder(text: "Remote Host".localized)
                foregroundView?.setPlaceholder(text: "Local Host".localized)
            }
        }
    }
        
    deinit {
        print("sss")
    }
    
    @Published var remoteUid: UInt? {
        didSet {
            foregroundView?.isHidden = !(role == .broadcaster && remoteUid != nil)
        }
    }
    
    override init() {
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.channelProfile = .liveBroadcasting
        let kit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: nil)
        Util.configPrivatization(agoraKit: kit)
        kit.setLogFile(LogUtils.sdkLogPath())
        agoraKit = kit
    }
    
    func setupRTC(configs: [String: Any],
                  localView: VideoUIView,
                  remoteView: VideoUIView) {
        self.configs = configs
        self.backgroundView = localView
        self.foregroundView = remoteView
        
        if let isFirstFrame = configs["isFirstFrame"] as? Bool, isFirstFrame == true {
            agoraKit.enableInstantMediaRendering()
            agoraKit.startMediaRenderingTracing()
        }
        
        guard let channelName = configs["channelName"] as? String, let clientRole = configs["role"] as? AgoraClientRole else {return}
        role = clientRole
        isLocalVideoForeground = role == .audience
        showUltraLowEntry = role == .audience
        showLinkStreamEntry = role == .audience
        
        self.agoraKit.addDelegate(self)
        updateClientRole(role)
        
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        let option = AgoraRtcChannelMediaOptions()
        option.publishCameraTrack = role == .broadcaster
        option.publishMicrophoneTrack = role == .broadcaster
        option.clientRoleType = role
        NetworkManager.shared.generateToken(channelName: channelName, success: { [weak self] token in
            guard let self = self else { return }
            let preloadChannel = configs["isPreloadChannel"] as? Bool
            if preloadChannel == true {
                self.agoraKit.preloadChannel(byToken: token,
                                             channelId: channelName,
                                             uid: 0)
            }
            let result = self.agoraKit.joinChannel(byToken: token, channelId: channelName, uid: 0, mediaOptions: option)
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
                LogUtils.log(message: "joinChannel call failed: \(result), please check your params", level: .error)
            }
        })
    }
    
    func updateClientRole(_ role: AgoraClientRole) {
        self.role = role
        if role == .broadcaster {
            becomeBroadcaster()
        } else {
            becomeAudience()
        }
        let option = AgoraRtcChannelMediaOptions()
        option.publishCameraTrack = role == .broadcaster
        option.publishMicrophoneTrack = role == .broadcaster
        agoraKit.updateChannel(with: option)
    }
    
    func onToggleClientRole(state: Bool) {
        let role: AgoraClientRole = state ? .broadcaster : .audience
        updateClientRole(role)
    }
    
    func onToggleUltraLowLatency(enabled: Bool) {
        updateUltraLowLatency(enabled)
    }
    
    fileprivate func updateUltraLowLatency(_ enabled: Bool) {
        if self.role == .audience {
            self.isUltraLowLatencyOn = enabled
            updateClientRole(.audience)
        }
    }
    
    func becomeBroadcaster() {
        guard let resolution = GlobalSettings.shared.getSetting(key: "resolution")?.selectedOption().value as? CGSize,
        let fps = GlobalSettings.shared.getSetting(key: "fps")?.selectedOption().value as? AgoraVideoFrameRate,
        let orientation = GlobalSettings.shared.getSetting(key: "orientation")?.selectedOption().value as? AgoraVideoOutputOrientationMode else {
            LogUtils.log(message: "invalid video configurations, failed to become broadcaster", level: .error)
            return
        }
        agoraKit.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: resolution,
                                                                             frameRate: fps,
                                                                             bitrate: AgoraVideoBitrateStandard,
                                                                             orientationMode: orientation, mirrorMode: .auto))
        
        // set up local video to render your local camera preview
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = 0
        // the view to be binded
        videoCanvas.view = localVideoCanvas()
        videoCanvas.renderMode = .hidden
        agoraKit.setupLocalVideo(videoCanvas)
        // you have to call startPreview to see local video
        agoraKit.startPreview()
        agoraKit.enableAudio()
        
        agoraKit.setClientRole(.broadcaster, options: nil)
    }
    
    /// make myself an audience
    func becomeAudience() {
        // unbind view
        agoraKit.setupLocalVideo(nil)
        // You have to provide client role options if set to audience
        let options = AgoraClientRoleOptions()
        options.audienceLatencyLevel = isUltraLowLatencyOn ? .ultraLowLatency : .lowLatency
        agoraKit.setClientRole(.audience, options: options)
    }
    
    func localVideoCanvas() -> UIView? {
        return isLocalVideoForeground ? foregroundView?.videoView : backgroundView?.videoView
    }

    func remoteVideoCanvas() -> UIView? {
        return isLocalVideoForeground ? backgroundView?.videoView : foregroundView?.videoView
    }
    
    func onDestory() {
        agoraKit.disableAudio()
        agoraKit.disableVideo()
        if isJoined {
            agoraKit.stopPreview()
            agoraKit.leaveChannel { (stats) -> Void in
                LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
            }
        }
        AgoraRtcEngineKit.destroy()
    }
    
    func leaveChannel() {
        if isJoined {
            agoraKit.stopPreview()
            agoraKit.leaveChannel { (stats) -> Void in
                LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
            }
        }
    }
        
    func onTapForegroundVideo() {
        isLocalVideoForeground.toggle()
        
        let localVideoCanvas = AgoraRtcVideoCanvas()
        localVideoCanvas.uid = 0
        localVideoCanvas.renderMode = .hidden
        localVideoCanvas.view = self.localVideoCanvas()
        
        let remoteVideoCanvas = AgoraRtcVideoCanvas()
        remoteVideoCanvas.renderMode = .hidden
        remoteVideoCanvas.view = self.remoteVideoCanvas()
        
        agoraKit.setupLocalVideo(localVideoCanvas)
        if let uid = remoteUid {
            remoteVideoCanvas.uid = uid
            agoraKit.setupRemoteVideo(remoteVideoCanvas)
        }
    }
}

extension LiveStreamingRTC: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://api-ref.agora.io/en/voice-sdk/ios/3.x/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.description)", level: .warning)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, cameraFocusDidChangedTo rect: CGRect) {
        LogUtils.log(message: "The camera has changed".localized + " \(rect)", level: .warning)
    }
    
    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
    /// cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode)", level: .error)
    }
    
    /// callback when the local user joins a specified channel.
    /// @param channel
    /// @param uid uid of local user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    ///
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        isJoined = true
        foregroundView?.statsInfo?.updateUid(uid: uid)
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        backgroundView?.statsInfo?.updateRemoteUid(remoteUid: uid)
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        
        // record remote uid
        remoteUid = uid
        // Only one remote video view is available for this
        // tutorial. Here we check if there exists a surface
        // view tagged as this uid.
        let backgroundColor = (configs["backgroundColor"] as? UInt32) ?? 0x000000
        // Only one remote video view is available for this
        // tutorial. Here we check if there exists a surface
        // view tagged as this uid.
        let videoCanvas = AgoraRtcVideoCanvas()
        // the view to be binded
        videoCanvas.uid = uid
        videoCanvas.view = remoteVideoCanvas()
        videoCanvas.backgroundColor = backgroundColor
        videoCanvas.renderMode = .fit
        agoraKit.setupRemoteVideo(videoCanvas)
    }
    
    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)
        // clear remote uid
        if remoteUid == uid {
            remoteUid = nil
        }
        
        // to unlink your view from sdk, so that your view reference will be released
        // note the video will stay at its last frame, to completely remove it
        // you will need to remove the EAGL sublayer from your binded view
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = nil
        videoCanvas.renderMode = .hidden
        agoraKit.setupRemoteVideo(videoCanvas)
    }
    
    /// Reports the statistics of the current call. The SDK triggers this callback once every two seconds after the user joins the channel.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, reportRtcStats stats: AgoraChannelStats) {
        foregroundView?.statsInfo?.updateChannelStats(stats)
    }
    
    /// Reports the statistics of the uploading local audio streams once every two seconds.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, localAudioStats stats: AgoraRtcLocalAudioStats) {
        foregroundView?.statsInfo?.updateLocalAudioStats(stats)
    }
    
    /// Reports the statistics of the video stream from each remote user/host.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteVideoStats stats: AgoraRtcRemoteVideoStats) {
        backgroundView?.statsInfo?.updateVideoStats(stats)
    }
    
    /// Reports the statistics of the audio stream from each remote user/host.
    /// @param stats stats struct for current call statistics
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteAudioStats stats: AgoraRtcRemoteAudioStats) {
        backgroundView?.statsInfo?.updateAudioStats(stats)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit,
                   videoRenderingTracingResultOfUid uid: UInt,
                   currentEvent: AgoraMediaTraceEvent,
                   tracingInfo: AgoraVideoRenderingTracingInfo) {
        backgroundView?.statsInfo?.updateFirstFrameInfo(tracingInfo)
    }
}

