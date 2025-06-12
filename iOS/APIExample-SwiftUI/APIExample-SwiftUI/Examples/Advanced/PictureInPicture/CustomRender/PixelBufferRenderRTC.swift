//
//  CustomRenderRTC.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/8/12.
//

import Foundation
import AgoraRtcKit

protocol CustomRenderRTCProtocol {
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
    func setLocalView(localView: PixelBufferRenderView)
    
    /**
        @param  renderView
                     renderView
        @abstract    This method is used to add video views. Internally, the view is wrapped as a Canvas, and the uid is bound and thrown to the RTC SDK for rendering
     */
    func addRemoteRenderView(renderView: PixelBufferRenderView)
    
    /**
        @param  renderView
                     renderView
        @abstract    delete a remote video stream view
     */
    func removeRemoteRenderView(renderView: PixelBufferRenderView)
}

/**
    @class      SDKRenderRTC
    @abstract   RTC management class, handle RTC connection, video view data management (view based on PixelBufferRenderView self-rendering), channel management
 */
class PixelBufferRenderRTC: NSObject, ObservableObject , CustomRenderRTCProtocol{
    var videoFrameDelegte: AgoraVideoFrameDelegate?
    
    private var uid: UInt = 0
    private weak var localView: PixelBufferRenderView?
    private lazy var remoteRenderViews: NSHashTable<PixelBufferRenderView> = {
        let table = NSHashTable<PixelBufferRenderView>(options: .weakMemory)
        return table
    }()

    func setLocalView(localView: PixelBufferRenderView) {
        self.localView = localView
    }
    
    func setupRtcEngine(rtcEngine: AgoraRtcEngineKit) {
        rtcEngine.setVideoFrameDelegate(self)
    }
    
    func addRemoteRenderView(renderView: PixelBufferRenderView) {
        remoteRenderViews.add(renderView)
    }
        
    func removeRemoteRenderView(renderView: PixelBufferRenderView) {
        renderView.clean()
        
        remoteRenderViews.remove(renderView)
    }
}

extension PixelBufferRenderRTC: AgoraVideoFrameDelegate {
    func onCapture(_ videoFrame: AgoraOutputVideoFrame, sourceType: AgoraVideoSourceType) -> Bool {
        if let localView = localView, let pixelBuffer = videoFrame.pixelBuffer {
            localView.renderVideoPixelBuffer(pixelBuffer: pixelBuffer, width: videoFrame.width, height: videoFrame.height)
        }
        
        return self.videoFrameDelegte?.onCapture?(videoFrame, sourceType: sourceType) ?? true
    }

    func onRenderVideoFrame(_ videoFrame: AgoraOutputVideoFrame, uid: UInt, channelId: String) -> Bool {
        if let view = remoteRenderViews.allObjects.first(where: { $0.uid == uid }) {
            view.renderFromVideoFrameData(videoData: videoFrame)
        }
        
        return self.videoFrameDelegte?.onRenderVideoFrame?(videoFrame, uid: uid, channelId: channelId) ?? true
    }

}
