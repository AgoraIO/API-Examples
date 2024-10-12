//
//  RawAudioDataRTC.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/9/27.
//

import Foundation
import AgoraRtcKit

class RawAudioDataRTC: NSObject, ObservableObject {
    private var agoraKit: AgoraRtcEngineKit!
    @Published var isJoined: Bool = false
    @Published var audioData: String = ""
    
    private var localView: VideoUIView?
    private var remoteView: VideoUIView?
    
    func setupRTC(configs: [String: Any],
                  localView: VideoUIView,
                  remoteView: VideoUIView) {
        guard let channelId = configs["channelName"] as? String else {return}
        
        self.localView = localView
        self.remoteView = remoteView
        
        agoraKit = AgoraRtcEngineKit.sharedEngine(withAppId: KeyCenter.AppId, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit.setClientRole(GlobalSettings.shared.getUserRole())
        
        // Audio4 is required to send Audio Meta Data.
        agoraKit.setParameters("{\"rtc.use_audio4\":true}")
        
        // Setup raw auido data frame observer
        agoraKit.setAudioFrameDelegate(self)
        agoraKit.enableAudio()
        NetworkManager.shared.generateToken(channelName: channelId, success: { token in
            let result = self.agoraKit.joinChannel(byToken: token, channelId: channelId, info: nil, uid: 0)
            if result != 0 {
                // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
            }
        })
    }
    
    func sendData() {
        guard let data = audioData.data(using: .utf8) else { return }
        agoraKit.sendAudioMetadata(data)
        audioData = ""
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
    
    func getAudioLabel(uid: UInt, isLocal: Bool) -> String {
        return "AUDIO ONLY\n\(isLocal ? "Local" : "Remote")\n\(uid)"
    }
}

extension RawAudioDataRTC: AgoraAudioFrameDelegate {
    func onEarMonitoringAudioFrame(_ frame: AgoraAudioFrame) -> Bool {
        true
    }
    
    func getEarMonitoringAudioParams() -> AgoraAudioParams {
        AgoraAudioParams()
    }
    
    func getMixedAudioParams() -> AgoraAudioParams {
        AgoraAudioParams()
    }
    
    func getRecordAudioParams() -> AgoraAudioParams {
        AgoraAudioParams()
    }
    
    func getPlaybackAudioParams() -> AgoraAudioParams {
        AgoraAudioParams()
    }

    func onRecordAudioFrame(_ frame: AgoraAudioFrame, channelId: String) -> Bool {
        true
    }
    
    func getObservedAudioFramePosition() -> AgoraAudioFramePosition {
        .mixed
    }
    
    func onRecord(_ frame: AgoraAudioFrame, channelId: String) -> Bool {
        return true
    }
    
    func onPlaybackAudioFrame(_ frame: AgoraAudioFrame, channelId: String) -> Bool {
        return true
    }
    
    func onMixedAudioFrame(_ frame: AgoraAudioFrame, channelId: String) -> Bool {
        return true
    }
    
    func onPlaybackAudioFrame(beforeMixing frame: AgoraAudioFrame, channelId: String, uid: UInt) -> Bool {
        return true
    }
}

// MARK: - AgoraRtcEngineDelegate
extension RawAudioDataRTC: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, audioMetadataReceived uid: UInt, metadata: Data) {
        let data = String(data: metadata, encoding: .utf8) ?? ""
        ToastView.show(text: "uid: \(uid) data: \(data)")
    }
    // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
    // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "Error occur: \(errorCode)", level: .error)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
        localView?.setPlaceholder(text: self.getAudioLabel(uid: uid, isLocal: true))
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "Remote user \(uid) joined elapsed \(elapsed)ms", level: .info)
        remoteView?.setPlaceholder(text: self.getAudioLabel(uid: uid, isLocal: false))
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "Remote user \(uid) offline with reason \(reason)", level: .info)
        remoteView?.setPlaceholder(text: self.getAudioLabel(uid: 0, isLocal: false))
    }
}
