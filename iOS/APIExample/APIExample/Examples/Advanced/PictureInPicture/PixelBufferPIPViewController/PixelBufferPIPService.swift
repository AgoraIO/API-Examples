//
//  PixelBufferPIPService.swift
//  PIPDemo
//
//  Created by qinhui on 2024/8/8.
//

import Foundation
import AgoraRtcKit

class PixelBufferPIPService: NSObject {
    var videoFrameDelegte: AgoraVideoFrameDelegate?
    var rtcEngineDelegate: AgoraRtcEngineDelegate?
    weak var localView: PixelBufferRenderView?
    
    var uid: UInt = 0
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
                                                                           frameRate: AgoraVideoFrameRate.fps15.rawValue,
                                                                           bitrate: AgoraVideoBitrateStandard,
                                                                           orientationMode: .fixedPortrait,
                                                                           mirrorMode: .auto))
        engine.setVideoFrameDelegate(self)
        
        return engine
    }()
    
    init(channelId: String, uid: UInt, localView: PixelBufferRenderView) {
        self.channelId = channelId
        self.uid = uid
        self.localView = localView
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
            let result = self.rtcEngine.joinChannel(byToken: token, channelId: self.channelId, uid: self.uid, mediaOptions: option)
            if result != 0 {
                ToastView.showWait(text: "joinChannel call failed: \(result), please check your params", view: nil)
            } else {
                self.localView?.uid = self.uid
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

extension PixelBufferPIPService: AgoraVideoFrameDelegate {
    func onCapture(_ videoFrame: AgoraOutputVideoFrame, sourceType: AgoraVideoSourceType) -> Bool {
        return ((self.videoFrameDelegte?.onCapture?(videoFrame, sourceType: sourceType)) != nil)
    }
    
    func onRenderVideoFrame(_ videoFrame: AgoraOutputVideoFrame, uid: UInt, channelId: String) -> Bool {
        return ((self.videoFrameDelegte?.onRenderVideoFrame?(videoFrame, uid: uid, channelId: channelId)) != nil)
    }
}

extension PixelBufferPIPService: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        rtcEngineDelegate?.rtcEngine?(engine, didJoinChannel: channel, withUid: uid, elapsed: elapsed)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        rtcEngineDelegate?.rtcEngine?(engine, didJoinedOfUid: uid, elapsed: elapsed)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        rtcEngineDelegate?.rtcEngine?(engine, didOfflineOfUid: uid, reason: reason)
    }
    
}
