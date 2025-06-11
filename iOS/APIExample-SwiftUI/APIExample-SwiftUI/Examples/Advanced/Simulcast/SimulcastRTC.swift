import Foundation
import AgoraRtcKit

class SimulcastRTC: NSObject, ObservableObject {
    private var agoraKit: AgoraRtcEngineKit!
    private var isJoined: Bool = false
    private var localView: VideoUIView?
    private var remoteView: VideoUIView?
    let simulcastConfig = AgoraSimulcastConfig()
    private var remoteUid: UInt?
    
    func setupRTC(configs: [String: Any],
                  localView: VideoUIView,
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
        
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        // get channel name from configs
        guard let channelName = configs["channelName"] as? String,
              let roleIndex = configs["role_index"] as? Int else {return}
        
        // make myself a broadcaster
        agoraKit.setClientRole(roleIndex == 0 ? .broadcaster : .audience)
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        agoraKit.enableAudio()
        
        if roleIndex == 0 {
            // Set video encoder configuration
            let videoConfig = AgoraVideoEncoderConfiguration()
            videoConfig.dimensions = CGSize(width: 1280, height: 720)
            videoConfig.frameRate = .fps30
            videoConfig.bitrate = AgoraVideoBitrateStandard
            videoConfig.orientationMode = .adaptative
            videoConfig.mirrorMode = .auto
            agoraKit.setVideoEncoderConfiguration(videoConfig)

            // set up local video to render your local camera preview
            let videoCanvas = AgoraRtcVideoCanvas()
            videoCanvas.uid = 0
            // the view to be binded
            videoCanvas.view = localView.videoView
            videoCanvas.renderMode = .hidden
            agoraKit.setupLocalVideo(videoCanvas)
            // you have to call startPreview to see local video
            agoraKit.startPreview()
            
            setupSimulcast()
        }
        
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
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
    
    // Set up simulcast configuration with 4 different video layers
    private func setupSimulcast() {
        let layer1_index = AgoraStreamLayerIndex.layer1.rawValue
        let layer2_index = AgoraStreamLayerIndex.layer2.rawValue
        let layer3_index = AgoraStreamLayerIndex.layer3.rawValue
        let layer4_index = AgoraStreamLayerIndex.layer4.rawValue
        
        // Layer 1: 1280x720@30fps (Highest quality)
        simulcastConfig.configs[layer1_index].dimensions.width = 1280
        simulcastConfig.configs[layer1_index].dimensions.height = 720
        simulcastConfig.configs[layer1_index].framerate = 30
        simulcastConfig.configs[layer1_index].enable = true
        
        // Layer 2: 960x540@15fps (High quality)
        simulcastConfig.configs[layer2_index].dimensions.width = 960
        simulcastConfig.configs[layer2_index].dimensions.height = 540
        simulcastConfig.configs[layer2_index].framerate = 15
        simulcastConfig.configs[layer2_index].enable = true
        
        // Layer 3: 640x360@15fps (Medium quality)
        simulcastConfig.configs[layer3_index].dimensions.width = 640
        simulcastConfig.configs[layer3_index].dimensions.height = 360
        simulcastConfig.configs[layer3_index].framerate = 15
        simulcastConfig.configs[layer3_index].enable = true
        
        // Layer 4: 480x270@15fps (Low quality)
        simulcastConfig.configs[layer4_index].dimensions.width = 480
        simulcastConfig.configs[layer4_index].dimensions.height = 270
        simulcastConfig.configs[layer4_index].framerate = 15
        simulcastConfig.configs[layer4_index].enable = true
        
        let ret = agoraKit.setSimulcastConfig(simulcastConfig)
        LogUtils.log(message: "setSimulcastConfig: \(String(describing: ret))", level: .info)
    }
    
    // Update simulcast configuration for a specific layer
    func updateSimulcastConfig(layer: AgoraStreamLayerIndex, enable: Bool) {
        let layerIndex = layer.rawValue
        simulcastConfig.configs[layerIndex].enable = enable
        let ret = agoraKit.setSimulcastConfig(simulcastConfig)
        LogUtils.log(message: "updateSimulcast: \(String(describing: ret))", level: .info)
    }
    
    // Set remote video stream type for receiving specific layer
    func setRemoteVideoStream(type: AgoraVideoStreamType) {
        guard let uid = remoteUid else {
            LogUtils.log(message: "No remote user", level: .warning)
            return
        }
        
        let ret = agoraKit.setRemoteVideoStream(uid, type: type)
        LogUtils.log(message: "set remote uid: \(uid), layer:\(type), ret: \(String(describing: ret))", level: .info)
    }
}

// agora rtc engine delegate events
extension SimulcastRTC: AgoraRtcEngineDelegate {
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.description)", level: .warning)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode)", level: .error)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        self.isJoined = true
        localView?.uid = uid
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        remoteView?.uid = uid
        // Only one remote video view is available for this
        // tutorial. Here we check if there exists a surface
        // view tagged as this uid.
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = remoteView?.videoView
        videoCanvas.renderMode = .hidden
        agoraKit.setupRemoteVideo(videoCanvas)
        remoteUid = uid
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)
        remoteView?.uid = 0
        // to unlink your view from sdk, so that your view reference will be released
        // note the video will stay at its last frame, to completely remove it
        // you will need to remove the EAGL sublayer from your binded view
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = nil
        videoCanvas.renderMode = .hidden
        agoraKit.setupRemoteVideo(videoCanvas)
        remoteUid = nil
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionChangedTo state: AgoraConnectionState, reason: AgoraConnectionChangedReason) {
        LogUtils.log(message: "Connection state changed: \(state) \(reason)", level: .info)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, reportRtcStats stats: AgoraChannelStats) {
        localView?.statsInfo?.updateChannelStats(stats)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, localAudioStats stats: AgoraRtcLocalAudioStats) {
        localView?.statsInfo?.updateLocalAudioStats(stats)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteVideoStats stats: AgoraRtcRemoteVideoStats) {
        remoteView?.statsInfo?.updateVideoStats(stats)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteAudioStats stats: AgoraRtcRemoteAudioStats) {
        remoteView?.statsInfo?.updateAudioStats(stats)
    }
}
