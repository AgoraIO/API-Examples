//
//  RawVideoDataRTC.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/9/27.
//

import Foundation
import AgoraRtcKit

class RawVideoDataRTC: NSObject, ObservableObject {
    @Published var isJoined: Bool = false
    @Published var isSnapShoting: Bool = false
    @Published var screenshot: UIImage?
    
    private var agoraKit: AgoraRtcEngineKit!
    private var localView: VideoUIView?
    private var remoteView: VideoUIView?
    
    func setupRTC(configs: [String: Any],
                  localView: VideoUIView,
                  remoteView: VideoUIView) {
        guard let channelId = configs["channelName"] as? String,
              let resolution = GlobalSettings.shared.getSetting(key: "resolution")?.selectedOption().value as? CGSize,
              let fps = GlobalSettings.shared.getSetting(key: "fps")?.selectedOption().value as? AgoraVideoFrameRate,
              let orientation = GlobalSettings.shared.getSetting(key: "orientation")?
            .selectedOption().value as? AgoraVideoOutputOrientationMode else {
            return
        }
        
        self.localView = localView
        self.remoteView = remoteView
        
        agoraKit = AgoraRtcEngineKit.sharedEngine(withAppId: KeyCenter.AppId, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit.setClientRole(GlobalSettings.shared.getUserRole())
        
        // Setup raw video data frame observer
        agoraKit.setVideoFrameDelegate(self)
        
        agoraKit.enableVideo()
        agoraKit.enableAudio()
        agoraKit.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: resolution,
                                                                             frameRate: fps.rawValue,
                                                                             bitrate: AgoraVideoBitrateStandard,
                                                                             orientationMode: orientation,
                                                                             mirrorMode: .auto))
        
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.view = localView.videoView
        videoCanvas.renderMode = .hidden
        agoraKit.setupLocalVideo(videoCanvas)
        
        agoraKit.startPreview()
        
        let option = AgoraRtcChannelMediaOptions()
        option.publishCameraTrack = GlobalSettings.shared.getUserRole() == .broadcaster
        option.publishMicrophoneTrack = GlobalSettings.shared.getUserRole() == .broadcaster
        
        NetworkManager.shared.generateToken(channelName: channelId, success: { token in
            let result = self.agoraKit.joinChannel(byToken: token, channelId: channelId, uid: 0, mediaOptions: option, joinSuccess: nil)
            if result != 0 {
                // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
            }
        })
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

extension RawVideoDataRTC: AgoraVideoFrameDelegate {
    func onCapture(_ videoFrame: AgoraOutputVideoFrame,
                   sourceType: AgoraVideoSourceType) -> Bool {
        true
    }
    
    func onRenderVideoFrame(_ videoFrame: AgoraOutputVideoFrame,
                            uid: UInt,
                            channelId: String) -> Bool {
        if isSnapShoting {
            isSnapShoting = false
            guard let pixelBuffer = videoFrame.pixelBuffer else { return true }
            let image = MediaUtils.pixelBuffer(toImage: pixelBuffer)
            DispatchQueue.main.async {
                self.screenshot = image
            }
        }
        return true
    }
    func getVideoFormatPreference() -> AgoraVideoFormat {
        .cvPixelBGRA
    }
}

// MARK: - AgoraRtcEngineDelegate
extension RawVideoDataRTC: AgoraRtcEngineDelegate {
    // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
    // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "Error occur: \(errorCode)", level: .error)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        isJoined = true
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "Remote user \(uid) joined elapsed \(elapsed)ms", level: .info)
        
        // Render remote user video frame at a UIView
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.view = remoteView?.videoView
        videoCanvas.uid = uid
        videoCanvas.renderMode = .hidden
        agoraKit.setupRemoteVideo(videoCanvas)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "Remote user \(uid) offline with reason \(reason)", level: .info)
        
        // Stop render remote user video frame
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.view = nil
        videoCanvas.uid = uid
        agoraKit.setupRemoteVideo(videoCanvas)
    }
    
    /// Reports the statistics of the current call. The SDK triggers this callback once every two seconds after the user joins the channel.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, reportRtcStats stats: AgoraChannelStats) {
        localView?.statsInfo?.updateChannelStats(stats)
    }
    
    /// Reports the statistics of the uploading local audio streams once every two seconds.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, localAudioStats stats: AgoraRtcLocalAudioStats) {
        localView?.statsInfo?.updateLocalAudioStats(stats)
    }
    
    /// Reports the statistics of the video stream from each remote user/host.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteVideoStats stats: AgoraRtcRemoteVideoStats) {
        remoteView?.statsInfo?.updateVideoStats(stats)
    }
    
    /// Reports the statistics of the audio stream from each remote user/host.
    /// @param stats stats struct for current call statistics
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteAudioStats stats: AgoraRtcRemoteAudioStats) {
        remoteView?.statsInfo?.updateAudioStats(stats)
    }
}
