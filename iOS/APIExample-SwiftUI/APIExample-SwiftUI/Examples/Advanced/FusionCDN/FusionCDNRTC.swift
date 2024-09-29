//
//  FusionCDNRTC.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/9/29.
//

import Foundation
import AgoraRtcKit

class FusionCDNRTC: NSObject, ObservableObject {
    private var isJoined: Bool = false
    private var agoraKit: AgoraRtcEngineKit!
    private var localView: VideoUIView?
    private var remoteView: VideoUIView?
    
    func setupRTC(configs: [String: Any],
                  localView: VideoUIView,
                  remoteView: VideoUIView) {
        
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
}
