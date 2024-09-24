//
//  RtcManager.swift
//  PIPDemo
//
//  Created by qinhui on 2024/8/7.
//

import Foundation
import AgoraRtcKit

class CustomViewPIPService: NSObject {
    var rtcEngineDelegate: AgoraRtcEngineDelegate?
    var videoFrameDelegte: AgoraVideoFrameDelegate?
    
    weak var localView: UIView?
    weak var remoteView: UIView?
    var channelId: String
    
    private lazy var rtcConfig: AgoraRtcEngineConfig = {
       let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = .global
        config.channelProfile = .liveBroadcasting
        return config
    }()
    
    private lazy var rtcEngine: AgoraRtcEngineKit = {
        let engine = AgoraRtcEngineKit.sharedEngine(with: rtcConfig, delegate: self)
        engine.setClientRole(.broadcaster)
        engine.enableAudio()
        engine.enableVideo()
        engine.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: CGSize(width: 960, height: 540),
                                                                           frameRate: .fps15,
                                                                           bitrate: AgoraVideoBitrateStandard, 
                                                                           orientationMode: .adaptative,
                                                                           mirrorMode: .auto))
        engine.setVideoFrameDelegate(self)
        return engine
    }()
    
    init(localView: UIView, remoteView: UIView, channelId: String) {
        self.localView = localView
        self.remoteView = remoteView
        self.channelId = channelId
        
        super.init()
        
        setupRtcEngin()
    }
    
    private func setupRtcEngin() {
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = 0
        videoCanvas.view = localView
        videoCanvas.renderMode = .hidden
        
        rtcEngine.setupLocalVideo(videoCanvas)
        rtcEngine.startPreview()
        rtcEngine.setDefaultAudioRouteToSpeakerphone(true)
        rtcEngine.setVideoFrameDelegate(self)
        
        let option = AgoraRtcChannelMediaOptions()
        option.publishCameraTrack = true
        option.publishMicrophoneTrack = true
        option.clientRoleType = .broadcaster
    
        NetworkManager.shared.generateToken(channelName: channelId, success: { [weak self] token in
            guard let self = self else { return }
            
            let result = self.rtcEngine.joinChannel(byToken: token, channelId: self.channelId, uid: 0, mediaOptions: option)
            if result != 0 {
                ToastView.showWait(text: "joinChannel call failed: \(result), please check your params", view: nil)
            }
        })
    }
    
    func disable() {
        rtcEngine.disableAudio()
        rtcEngine.disableVideo()
    }
    
    func leave() {
        rtcEngine.stopPreview()
        rtcEngine.leaveChannel(nil)
    }
    
}

extension CustomViewPIPService: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccur errorType: AgoraEncryptionErrorType) {
        rtcEngineDelegate?.rtcEngine?(engine, didOccur: errorType)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        rtcEngineDelegate?.rtcEngine?(engine, didJoinChannel: channel, withUid: uid, elapsed: elapsed)
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        // Only one remote video view is available for this
        // tutorial. Here we check if there exists a surface
        // view tagged as this uid.
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = remoteView
        videoCanvas.renderMode = .hidden
        rtcEngine.setupRemoteVideo(videoCanvas)
        
        rtcEngineDelegate?.rtcEngine?(engine, didJoinedOfUid: uid, elapsed: elapsed)
    }
    
    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = nil
        videoCanvas.renderMode = .hidden
        rtcEngine.setupRemoteVideo(videoCanvas)
        
        rtcEngineDelegate?.rtcEngine?(engine, didOfflineOfUid: uid, reason: reason)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionChangedTo state: AgoraConnectionState, reason: AgoraConnectionChangedReason) {
        rtcEngineDelegate?.rtcEngine?(engine, connectionChangedTo: state, reason: reason)
    }
    
    /// Reports the statistics of the current call. The SDK triggers this callback once every two seconds after the user joins the channel.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, reportRtcStats stats: AgoraChannelStats) {
        rtcEngineDelegate?.rtcEngine?(engine, reportRtcStats: stats)
    }
    
    /// Reports the statistics of the uploading local audio streams once every two seconds.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, localAudioStats stats: AgoraRtcLocalAudioStats) {
        rtcEngineDelegate?.rtcEngine?(engine, localAudioStats: stats)
    }
    
    /// Reports the statistics of the video stream from each remote user/host.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteVideoStats stats: AgoraRtcRemoteVideoStats) {
        rtcEngineDelegate?.rtcEngine?(engine, remoteVideoStats: stats)
    }
    
    /// Reports the statistics of the audio stream from each remote user/host.
    /// @param stats stats struct for current call statistics
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteAudioStats stats: AgoraRtcRemoteAudioStats) {
        rtcEngineDelegate?.rtcEngine?(engine, remoteAudioStats: stats)
    }
}

extension CustomViewPIPService: AgoraVideoFrameDelegate {
    func onCapture(_ videoFrame: AgoraOutputVideoFrame, sourceType: AgoraVideoSourceType) -> Bool {
        print("")
        return true
    }
    
    func onRenderVideoFrame(_ videoFrame: AgoraOutputVideoFrame, uid: UInt, channelId: String) -> Bool {
        print("")
        return true
    }
}
