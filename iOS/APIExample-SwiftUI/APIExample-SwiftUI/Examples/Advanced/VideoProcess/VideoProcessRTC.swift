//
//  JoinChannelVideoRTC.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/19.
//

import AgoraRtcKit
import SwiftUI

class VideoProcessRTC: NSObject, ObservableObject {
    private var agoraKit: AgoraRtcEngineKit!
    private var isJoined: Bool = false
    
    private var localView: VideoUIView?
    private var remoteView: VideoUIView?
    
    private var beautifyOption = AgoraBeautyOptions()
    private var skinProtect = 0.5
    private var strength = 0.5
    
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
        // enable filters
        agoraKit.enableExtension(withVendor: "agora_video_filters_clear_vision",
                                 extension: "clear_vision",
                                 enabled: true,
                                 sourceType: .primaryCamera)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        
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
                frameRate: fps,
                bitrate: AgoraVideoBitrateStandard,
                orientationMode: orientation, mirrorMode: .auto))
        
        setupCanvasView(view: localView.videoView)
        
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
    
   private func setupCanvasView(view: UIView?) {
        // set up local video to render your local camera preview
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = 0
        // the view to be binded
        videoCanvas.view = view
        videoCanvas.renderMode = .hidden
        agoraKit.setupLocalVideo(videoCanvas)
        // you have to call startPreview to see local video
        agoraKit.startPreview()
    }
    
    func onChangeBeauty(_ isOn: Bool) {
        if isOn {
            if agoraKit.isFeatureAvailable(onDevice: .videoPreprocessBeauty) {
                agoraKit.setBeautyEffectOptions(true, options: beautifyOption)
            } else {
                ToastView.show(text: "The feature is unavailable in the device!")
            }
        } else {
            agoraKit.setBeautyEffectOptions(false, options: beautifyOption)
        }
    }
    
    func onLightenSlider(_ value: Float, isOn: Bool) {
        beautifyOption.lighteningLevel = value
        agoraKit.setBeautyEffectOptions(isOn, options: beautifyOption)
    }
    
    func onRednessSlider(_ value: Float, isOn: Bool) {
        beautifyOption.rednessLevel = value
        agoraKit.setBeautyEffectOptions(isOn, options: beautifyOption)
    }
    
    func onSharpnessSlider(_ value: Float, isOn: Bool) {
        beautifyOption.sharpnessLevel = value
        agoraKit.setBeautyEffectOptions(isOn, options: beautifyOption)
    }
    
    func onSmoothSlider(_ value: Float, isOn: Bool) {
        beautifyOption.smoothnessLevel = value
        agoraKit.setBeautyEffectOptions(isOn, options: beautifyOption)
    }
    
    func onChangeLowLightEnhance(_ isOn: Bool) {
        let options = AgoraLowlightEnhanceOptions()
        options.level = .fast
        options.mode = .auto
        agoraKit.setLowlightEnhanceOptions(isOn, options: options)
    }
    
    func onChangeVideoDenoise(_ isOn: Bool) {
        let options = AgoraVideoDenoiserOptions()
        options.level = .highQuality
        options.mode = .manual
        agoraKit.setVideoDenoiserOptions(isOn, options: options)
    }
    
    func onChangeColorEnhance(_ isOn: Bool) {
        let options = AgoraColorEnhanceOptions()
        options.strengthLevel = Float(strength)
        options.skinProtectLevel = Float(skinProtect)
        agoraKit.setColorEnhanceOptions(isOn, options: options)
    }
    
    func onStrengthSlider(_ value: Float, isOn: Bool) {
        strength = Double(value)
        let options = AgoraColorEnhanceOptions()
        options.strengthLevel = Float(strength)
        options.skinProtectLevel = Float(skinProtect)
        agoraKit.setColorEnhanceOptions(isOn, options: options)
    }
    
    func onSkinProtectSlider(_ value: Float, isOn: Bool) {
        skinProtect = Double(value)
        let options = AgoraColorEnhanceOptions()
        options.strengthLevel = Float(strength)
        options.skinProtectLevel = Float(skinProtect)
        agoraKit.setColorEnhanceOptions(isOn, options: options)
    }
    
    func onChangeVirtualBgSwtich(_ isOn: Bool, index: Int) {
        if isOn {
            if agoraKit.isFeatureAvailable(onDevice: .videoPreprocessVirtualBackground) {
                onChangeVirtualBackground(index: index, isOn: isOn)
            } else {
                ToastView.show(text: "The feature is unavailable in the device!")
            }
        } else {
            onChangeVirtualBackground(index: index, isOn: isOn)
        }
    }
    
    func onChangeVirtualBackground(index: Int, isOn: Bool) {
        let source = AgoraVirtualBackgroundSource()
        switch index {
        case 0:
            let imgPath = Bundle.main.path(forResource: "agora-logo", ofType: "png")
            source.backgroundSourceType = .img
            source.source = imgPath
            
        case 1:
            source.backgroundSourceType = .color
            source.color = 0xFFFFFF
            
        case 2:
            source.backgroundSourceType = .blur
            source.blurDegree = .high
            
        case 3:
            let videoPath = Bundle.main.path(forResource: "sample", ofType: "mov")
            source.backgroundSourceType = .video
            source.source = videoPath
            
        default: break
        }
        source.backgroundSourceType = isOn ? source.backgroundSourceType : .none
        let result = agoraKit.enableVirtualBackground(isOn, backData: source, segData: AgoraSegmentationProperty())
        print("result == \(result)")
    }
    
    func onDestory() {
        agoraKit.disableAudio()
        agoraKit.disableVideo()
        agoraKit.enableVirtualBackground(false, backData: nil, segData: nil)
        if isJoined {
            agoraKit.stopPreview()
            agoraKit.leaveChannel { (stats) -> Void in
                LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
            }
        }
        AgoraRtcEngineKit.destroy()
    }
}

// agora rtc engine delegate events
extension VideoProcessRTC: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://api-ref.agora.io/en/voice-sdk/ios/3.x/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.description)", level: .warning)
    }
    
    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
    /// cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode)", level: .error)
//        self.showAlert(title: "Error", message: "Error \(errorCode.description) occur")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        self.isJoined = true
        localView?.uid = uid
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
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
    }
    
    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
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
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionChangedTo state: AgoraConnectionState, reason: AgoraConnectionChangedReason) {
        LogUtils.log(message: "Connection state changed: \(state) \(reason)", level: .info)
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
