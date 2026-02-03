//
//  LocalVideoTranscodingRTC.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/9/29.
//

import Foundation
import AgoraRtcKit

class LocalVideoTranscodingRTC: NSObject, ObservableObject {
    @Published var isJoined: Bool = false
    private var agoraKit: AgoraRtcEngineKit!
    private var localView: SampleBufferDisplayView?
    private var remoteView: VideoUIView?
    private var mediaPlayerKit: AgoraRtcMediaPlayerProtocol!

    func setupRTC(configs: [String: Any],
                  localView: SampleBufferDisplayView,
                  remoteView: VideoUIView) {
        self.localView = localView
        self.remoteView = remoteView
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        
        agoraKit.setVideoFrameDelegate(self)

        agoraKit.setLogFile(LogUtils.sdkLogPath())
        // get channel name from configs
        guard let channelName = configs["channelName"] as? String else {return}
        let fps = GlobalSettings.shared.getFps()
        let resolution = GlobalSettings.shared.getResolution()
        let orientation = GlobalSettings.shared.getOrientation()
        
        // make myself a broadcaster
        agoraKit.setClientRole(GlobalSettings.shared.getUserRole())
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        agoraKit.enableAudio()
        agoraKit.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: resolution,
                frameRate: fps.rawValue,
                bitrate: AgoraVideoBitrateStandard,
                orientationMode: orientation, mirrorMode: .auto))
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        mediaPlayerKit = agoraKit.createMediaPlayer(with: self)
        
        let streamingUrl = "https://agora-adc-artifacts.s3.cn-north-1.amazonaws.com.cn/resources/sample.mp4"
        let mediaSource = AgoraMediaSource()
        mediaSource.url = streamingUrl
        mediaPlayerKit.open(with: mediaSource)
        mediaPlayerKit.adjustPlayoutVolume(0)

        var streams: [AgoraTranscodingVideoStream] = []
        var playerStream = AgoraTranscodingVideoStream()
        playerStream.sourceType = .mediaPlayer
        playerStream.mediaPlayerId = UInt(mediaPlayerKit.getMediaPlayerId())
        playerStream.zOrder = 1
        playerStream.rect = CGRect(x: 0, y: 0, width: 200, height: 200)
        streams.append(playerStream)
        
        var cameraStream = AgoraTranscodingVideoStream()
        cameraStream.sourceType = .camera
        cameraStream.zOrder = 2
        cameraStream.rect = CGRect(x: 0, y: 0, width: 100, height: 100)
        cameraStream.mirror = true
        streams.append(cameraStream)
        
        let cameraConfig = AgoraCameraCapturerConfiguration()
        cameraConfig.cameraDirection = .rear
        cameraConfig.dimensions = CGSize(width: 100, height: 100)
        agoraKit.startCameraCapture(.camera, config: cameraConfig)
        
        let transcoderConfig = AgoraLocalTranscoderConfiguration()
        transcoderConfig.videoInputStreams = streams
        let outputConfig = AgoraVideoEncoderConfiguration()
        outputConfig.dimensions = CGSize(width: 200, height: 200)
        transcoderConfig.videoOutputConfiguration = outputConfig
        
        agoraKit.startLocalVideoTranscoder(transcoderConfig)
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        let option = AgoraRtcChannelMediaOptions()
        option.publishCameraTrack = GlobalSettings.shared.getUserRole() == .broadcaster
        option.publishMicrophoneTrack = GlobalSettings.shared.getUserRole() == .broadcaster
        option.clientRoleType = GlobalSettings.shared.getUserRole()
        NetworkManager.shared.generateToken(channelName: channelName, success: { token in
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

// MARK: - AgoraRtcEngineDelegate
extension LocalVideoTranscodingRTC: AgoraRtcEngineDelegate {
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

extension LocalVideoTranscodingRTC: AgoraVideoFrameDelegate {
    func onCapture(_ videoFrame: AgoraOutputVideoFrame, sourceType: AgoraVideoSourceType) -> Bool {
        return true
    }
    
    func onRenderVideoFrame(_ videoFrame: AgoraOutputVideoFrame, uid: UInt, channelId: String) -> Bool {
        return true
    }
    
    func onTranscodedVideoFrame(_ videoFrame: AgoraOutputVideoFrame) -> Bool {
        if videoFrame.pixelBuffer != nil {
            localView?.videoView.renderVideoPixelBuffer(videoFrame)
        } else {
            localView?.videoView.renderVideoData(videoFrame)
        }
        return true
    }
    
    func getVideoFormatPreference() -> AgoraVideoFormat {
        .cvPixelBGRA
    }
}

extension LocalVideoTranscodingRTC: AgoraRtcMediaPlayerDelegate {
    
}
