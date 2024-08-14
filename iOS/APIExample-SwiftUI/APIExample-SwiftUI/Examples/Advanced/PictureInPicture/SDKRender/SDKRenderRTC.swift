//
//  SDKRenderRTC.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/8/13.
//

import Foundation

protocol SDKRenderRTCProtocol {
    /**
        @param   rtcEngine
                     rtcrtcEngine
        @abstract    Set the self-rendering rtc manager Settings
     */
    func setupRtcEngine(rtcEngine: AgoraRtcEngineKit)
    
    /**
        @param   localView
                    local render view
        @abstract   set local render view
     */
    func setLocalView(localView: VideoUIView)
    
    /**
        @param  renderView
                     render view
        @abstract    This method is used to add video stream views. Internally, the view is wrapped as a Canvas, and the uid is bound and thrown to the RTC SDK for rendering
     */
    func addRemoteRenderView(renderView: VideoUIView)
    
    /**
        @param  renderView
                     render view
        @abstract    delete a remote video stream view
     */
    func removeRemoteRenderView(renderView: VideoUIView)
}

/**
    @class      SDKRenderRTC
    @abstract   RTC management class, dealing with RTC connection, video view data management (video based on SDK rendering), channel management
 */
class SDKRenderRTC: NSObject, ObservableObject, SDKRenderRTCProtocol {
    private var uid: UInt = 0
    
    var videoFrameDelegte: AgoraVideoFrameDelegate?
    
    private weak var localView: UIView?
    
    private weak var rtcEngine: AgoraRtcEngineKit?
    
    private lazy var remoteRenderViews: NSHashTable = {
        let table = NSHashTable<VideoUIView>(options: .weakMemory)
        return table
    }()
    
    func setupRtcEngine(rtcEngine: AgoraRtcEngineKit) {
        self.rtcEngine = rtcEngine
    }
    
    func setLocalView(localView: VideoUIView) {
        guard let rtcEngine = rtcEngine else { return }
        self.localView = localView
        
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = localView.uid
        // the view to be binded
        videoCanvas.view = localView.videoView
        videoCanvas.renderMode = .hidden
        
        rtcEngine.setupLocalVideo(videoCanvas)
        // you have to call startPreview to see local video
        rtcEngine.startPreview()
    }
    
    func addRemoteRenderView(renderView: VideoUIView) {
        guard let rtcEngine = rtcEngine else { return }
        if renderView.uid == 0 { return }
        // Only one remote video view is available for this
        // tutorial. Here we check if there exists a surface
        // view tagged as this uid.
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = renderView.uid
        // the view to be binded
        videoCanvas.view = renderView.videoView
        videoCanvas.renderMode = .hidden
        rtcEngine.setupRemoteVideo(videoCanvas)

        remoteRenderViews.add(renderView)
    }
        
    func removeRemoteRenderView(renderView: VideoUIView) {
        guard let rtcEngine = rtcEngine else { return }

        renderView.uid = 0
        
        // to unlink your view from sdk, so that your view reference will be released
        // note the video will stay at its last frame, to completely remove it
        // you will need to remove the EAGL sublayer from your binded view
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = nil
        videoCanvas.renderMode = .hidden
        rtcEngine.setupRemoteVideo(videoCanvas)
        
        remoteRenderViews.remove(renderView)
    }
}
