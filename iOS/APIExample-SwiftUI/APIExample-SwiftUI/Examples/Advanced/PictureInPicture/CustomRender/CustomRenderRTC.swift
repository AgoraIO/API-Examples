//
//  CustomRenderRTC.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/8/12.
//

import Foundation
import AgoraRtcKit

class CustomRenderRTC: NSObject, ObservableObject {
    private var uid: UInt = 0
    
    var videoFrameDelegte: AgoraVideoFrameDelegate?
    
    var rtcEngineDelegate: AgoraRtcEngineDelegate? {
        didSet {
            if let delegte = rtcEngineDelegate {
                self.rtcEngine.delegate = delegte
            }
        }
    }

    private weak var localView: PixelBufferRenderView?
    
    private lazy var remoteRenderViews: NSHashTable = {
        let table = NSHashTable<PixelBufferRenderView>(options: .weakMemory)
        return table
    }()
    
    private lazy var rtcConfig: AgoraRtcEngineConfig = {
       let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = .global
        config.channelProfile = .liveBroadcasting
        return config
    }()

    private lazy var rtcEngine: AgoraRtcEngineKit = {
        let engine = AgoraRtcEngineKit.sharedEngine(with: rtcConfig, delegate: nil)
        engine.setClientRole(.broadcaster)
        engine.enableAudio()
        engine.enableVideo()
        engine.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: CGSize(width: 960, height: 540),
                                                                           frameRate: .fps15,
                                                                           bitrate: AgoraVideoBitrateStandard,
                                                                           orientationMode: .fixedPortrait,
                                                                           mirrorMode: .auto))

        return engine
    }()
    
    public func setupRtcEngine(token: String, channelName: String, userId: UInt, clientRoleType: AgoraClientRole, localView: PixelBufferRenderView) {
        self.localView = localView
        
        rtcEngine.setDefaultAudioRouteToSpeakerphone(true)
        rtcEngine.setVideoFrameDelegate(self)
        let option = AgoraRtcChannelMediaOptions()
        option.publishCameraTrack = true
        option.publishMicrophoneTrack = true
        option.clientRoleType = clientRoleType
        self.rtcEngine.joinChannel(byToken: token, channelId: channelName, uid: userId, mediaOptions: option)
    }
    
    public func addRenderView(renderView: PixelBufferRenderView) {
        remoteRenderViews.add(renderView)
    }
        
    public func removeRenderView(renderView: PixelBufferRenderView) {
        renderView.clean()
        
        remoteRenderViews.remove(renderView)
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

extension CustomRenderRTC: AgoraVideoFrameDelegate {
    func onCapture(_ videoFrame: AgoraOutputVideoFrame, sourceType: AgoraVideoSourceType) -> Bool {
        if let localView = localView, let pixelBuffer = videoFrame.pixelBuffer {
            localView.renderVideoPixelBuffer(pixelBuffer: pixelBuffer, width: videoFrame.width, height: videoFrame.height)
        }
        
        return self.videoFrameDelegte?.onCapture?(videoFrame, sourceType: sourceType) ?? true
    }

    func onRenderVideoFrame(_ videoFrame: AgoraOutputVideoFrame, uid: UInt, channelId: String) -> Bool {
        //远端流多视图自渲染，找到对应的view进行绘制
        if let view = remoteRenderViews.allObjects.first(where: { $0.uid == uid }) {
            view.renderFromVideoFrameData(videoData: videoFrame)
        }
        
        return self.videoFrameDelegte?.onRenderVideoFrame?(videoFrame, uid: uid, channelId: channelId) ?? true
    }
}
