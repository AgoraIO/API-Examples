//
//  CustomPCMAudioSourceRTC.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/9/27.
//

import Foundation
import AgoraRtcKit

struct AudioStreamInfo: Identifiable, Hashable {
    var id: UInt { uid }
    var uid: UInt
    var info: String
    var local: Bool = false
}

class CustomPCMAudioSourceRTC: NSObject, ObservableObject {
    @Published var audioViews: [AudioStreamInfo] = []
    @Published var isJoined: Bool = false
    
    private var agoraKit: AgoraRtcEngineKit!
    private var trackId: Int32 = 0
    private var pcmSourcePush: AgoraPcmSourcePush?

    var sampleRate = 48000, channel = 1, bitPerSample = 16
    let bufferSize = 6
    lazy var samples: Int = {
        return (self.sampleRate / 100) * bufferSize
    }()
    
    func setupRTC(configs: [String: Any]) {
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        
        guard let channelName = configs["channelName"] as? String,
              let filepath = Bundle.main.path(forResource: "output", ofType: "pcm") else {
            return
        }
        
        // make myself a broadcaster
        agoraKit.setClientRole(GlobalSettings.shared.getUserRole())
        
        // disable video module
        agoraKit.disableVideo()
        // enable audio module
        agoraKit.enableAudio()
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        // setup external audio source
        pcmSourcePush = AgoraPcmSourcePush(delegate: self, filePath: filepath, sampleRate: Int(sampleRate),
                                           channelsPerFrame: Int(channel), bitPerSample: bitPerSample, samples: samples)
        
        let trackConfig = AgoraAudioTrackConfig()
        trackConfig.enableLocalPlayback = true
        trackId = agoraKit.createCustomAudioTrack(.mixable, config: trackConfig)
        agoraKit.enableCustomAudioLocalPlayback(1, enabled: true)
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        let option = AgoraRtcChannelMediaOptions()
        option.publishCameraTrack = false
        option.publishMicrophoneTrack = GlobalSettings.shared.getUserRole() == .broadcaster
        option.publishCustomAudioTrack = GlobalSettings.shared.getUserRole() == .broadcaster
        option.publishCustomAudioTrackId = Int(trackId)
        option.clientRoleType = GlobalSettings.shared.getUserRole()
        NetworkManager.shared.generateToken(channelName: channelName, success: { token in
            let result = self.agoraKit.joinChannel(byToken: token, channelId: channelName, uid: 0, mediaOptions: option)
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
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
    
    func pushPCM(state: Bool) {
        if state {
            pcmSourcePush?.start()
        } else {
            pcmSourcePush?.stop()
        }
        let mediaOption = AgoraRtcChannelMediaOptions()
        mediaOption.publishCustomAudioTrack = state
        agoraKit.updateChannel(with: mediaOption)
    }
    
    func getAudioLabel(uid: UInt, isLocal: Bool) -> String {
        return "AUDIO ONLY\n\(isLocal ? "Local" : "Remote")\n\(uid)"
    }
}

extension CustomPCMAudioSourceRTC: AgoraPcmSourcePushDelegate {
    func onAudioFrame(data: UnsafeMutablePointer<UInt8>) {
        agoraKit.pushExternalAudioFrameRawData(data,
                                               samples: samples,
                                               sampleRate: Int(sampleRate),
                                               channels: Int(channel),
                                               trackId: Int(trackId),
                                               timestamp: 0)
    }
}

/// agora rtc engine delegate events
extension CustomPCMAudioSourceRTC: AgoraRtcEngineDelegate {
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
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        self.isJoined = true
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
        // set up local audio view, this view will not show video but just a placeholder
        self.audioViews.append(AudioStreamInfo(uid: uid, info: self.getAudioLabel(uid: uid, isLocal: true), local: true))
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        // set up remote audio view, this view will not show video but just a placeholder
        self.audioViews.append(AudioStreamInfo(uid: uid, info: self.getAudioLabel(uid: uid, isLocal: false)))
    }
    
    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)
        
        // remove remote audio view
        self.audioViews.removeAll { $0.uid == uid }
    }
}
