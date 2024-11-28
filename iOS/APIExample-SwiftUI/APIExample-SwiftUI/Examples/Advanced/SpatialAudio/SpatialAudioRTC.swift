//
//  JoinChannelVideoRTC.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/19.
//

import AgoraRtcKit
import SwiftUI

let SCREENSIZE = UIScreen.main.bounds.size
class SpatialAudioRTC: NSObject, ObservableObject {
    private var agoraKit: AgoraRtcEngineKit!
    private var isJoined: Bool = false
    private var mediaPlayer1: AgoraRtcMediaPlayerProtocol!
    private var mediaPlayer2: AgoraRtcMediaPlayerProtocol!
    private var localSpatial: AgoraLocalSpatialAudioKit!
    private var currentAngle = 0.0
    private var currentDistance = 0.0
    private var maxDistance: CGFloat = 10
    private let forward = simd_float3(1.0, 0.0, 0.0)
    private let right = simd_float3(0.0, 1.0, 0.0)
    private let up = simd_float3(0.0, 0.0, 1.0)
    
    var remoteUser1Frame: CGRect = .zero
    var remoteUser2Frame: CGRect = .zero
    @Published var remoteUser1: UInt = 0
    @Published var remoteUser2: UInt = 0
    
    func setupRTC(configs: [String: Any], selfFrame: CGRect) {
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
        guard let channelName = configs["channelName"] as? String else {return}
        
        // make myself a broadcaster
        agoraKit.setClientRole(GlobalSettings.shared.getUserRole())
        // enable audio module
        agoraKit.enableAudio()
        
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        let localSpatialConfig = AgoraLocalSpatialAudioConfig()
        localSpatialConfig.rtcEngine = agoraKit
        localSpatial = AgoraLocalSpatialAudioKit.sharedLocalSpatialAudio(with: localSpatialConfig)
        localSpatial.setAudioRecvRange(Float(SCREENSIZE.height))
        localSpatial.setMaxAudioRecvCount(2)
        localSpatial.setDistanceUnit(1)
        
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        let option = AgoraRtcChannelMediaOptions()
        option.publishCameraTrack = false
        option.publishMicrophoneTrack = GlobalSettings.shared.getUserRole() == .broadcaster
        option.clientRoleType = GlobalSettings.shared.getUserRole()
        option.autoSubscribeAudio = true
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
        
        updatePosition(frame: selfFrame)
    }
    
    func setupVoice1Frame(frame: CGRect) {
        mediaPlayer1 = agoraKit.createMediaPlayer(with: self)
        mediaPlayer1.setLoopCount(-1)
        mediaPlayer1.open("https://webdemo.agora.io/audiomixing.mp3", startPos: 0)
        localSpatial.updatePlayerPositionInfo(Int(mediaPlayer1.getMediaPlayerId()), positionInfo: getPlayerPostion(view: frame))
        localSpatial.setPlayerAttenuation(0.2, playerId: UInt(mediaPlayer1.getMediaPlayerId()), forceSet: false)
    }
    
    func setupVoice2Frame(frame: CGRect) {
        mediaPlayer2 = agoraKit.createMediaPlayer(with: self)
        mediaPlayer2.setLoopCount(-1)
        mediaPlayer2.open("https://webdemo.agora.io/dang.mp3", startPos: 0)
        localSpatial.updatePlayerPositionInfo(Int(mediaPlayer2.getMediaPlayerId()), positionInfo: getPlayerPostion(view: frame))
        localSpatial.setPlayerAttenuation(0.2, playerId: UInt(mediaPlayer2.getMediaPlayerId()), forceSet: false)
    }
    
    func onTapMicSwitch(_ isOn: Bool) {
        let option = AgoraRtcChannelMediaOptions()
        option.publishMicrophoneTrack = isOn
        agoraKit.updateChannel(with: option)
    }
    func onTapAudioAttenuationSwitch(_ isOn: Bool, frame: CGRect, selfFrame: CGRect) {
        if isOn {
            let audioZone = AgoraSpatialAudioZone()
            audioZone.forwardLength = Float(frame.width)
            audioZone.rightLength = Float(frame.height)
            audioZone.upLength = Float(maxDistance)
            audioZone.zoneSetId = 1
            audioZone.audioAttenuation = 1
            audioZone.forward = forward
            audioZone.right = right
            audioZone.up = up
            audioZone.position = getViewCenterPostion(view: frame)
            localSpatial.setZones([audioZone])
        } else {
            localSpatial.setZones(nil)
        }
        let pos = getViewCenterPostion(view: selfFrame)
        localSpatial.updateSelfPosition(pos, axisForward: forward, axisRight: right, axisUp: up)
    }
    
    func mediaPlayerMute(isVoice1: Bool, isMute: Bool) {
        if isVoice1 {
            _ = isMute ? mediaPlayer1.pause() : mediaPlayer1.resume()
        } else {
            _ = isMute ? mediaPlayer2.pause() : mediaPlayer2.resume()
        }
    }
    func mediaPlayerVoiceBlur(isVoice1: Bool, isOn: Bool) {
        let params = AgoraSpatialAudioParams()
        params.enable_blur = isOn
        if isVoice1 {
            mediaPlayer1.setSpatialAudioParams(params)
        } else {
            mediaPlayer2.setSpatialAudioParams(params)
        }
    }
    func mediaPlayerAirborne(isVoice1: Bool, isOn: Bool) {
        let params = AgoraSpatialAudioParams()
        params.enable_air_absorb = isOn
        if isVoice1 {
            mediaPlayer1.setSpatialAudioParams(params)
        } else {
            mediaPlayer2.setSpatialAudioParams(params)
        }
    }
    func mediaPlayerAttenuation(isVoice1: Bool, value: Float) {
        let mediaPlayerId = isVoice1 ? UInt(mediaPlayer1.getMediaPlayerId()) : UInt(mediaPlayer2.getMediaPlayerId())
        localSpatial.setPlayerAttenuation(Double(value),
                                          playerId: mediaPlayerId,
                                          forceSet: false)
    }
    
    func updatePosition(frame: CGRect) {
        let pos = getViewCenterPostion(view: frame)
        localSpatial.updateSelfPosition(pos,
                                        axisForward: forward,
                                        axisRight: right,
                                        axisUp: up)
    }
    
    private func getPlayerPostion(view: CGRect) -> AgoraRemoteVoicePositionInfo {
        let position = getViewCenterPostion(view: view)
        let positionInfo = AgoraRemoteVoicePositionInfo()
        positionInfo.position = position
        positionInfo.forward = forward
        return positionInfo
    }
    private func getViewCenterPostion(view: CGRect) -> simd_float3 {
        simd_float3(Float(view.minX + view.width * 0.5),
                    Float(view.minY + view.height * 0.5),
                    0.0)
    }
    
    func onDestory() {
        agoraKit.disableAudio()
        if isJoined {
            agoraKit.leaveChannel { (stats) -> Void in
                LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
            }
        }
        AgoraLocalSpatialAudioKit.destroy()
        AgoraRtcEngineKit.destroy()
    }
}

extension SpatialAudioRTC: AgoraRtcMediaPlayerDelegate {
    func AgoraRtcMediaPlayer(_ playerKit: AgoraRtcMediaPlayerProtocol,
                             didChangedTo state: AgoraMediaPlayerState,
                             reason: AgoraMediaPlayerReason) {
        if state == .openCompleted || state == .playBackAllLoopsCompleted || state == .playBackCompleted {
            playerKit.play()
        }
    }
}


// agora rtc engine delegate events
extension SpatialAudioRTC: AgoraRtcEngineDelegate {
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
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        if remoteUser1 == 0 {
            localSpatial.updateRemotePosition(uid, positionInfo: getPlayerPostion(view: remoteUser1Frame))
            remoteUser1 = uid
        } else if remoteUser2 == 0 {
            localSpatial.updateRemotePosition(uid, positionInfo: getPlayerPostion(view: remoteUser2Frame))
            remoteUser2 = uid
        }
    }
    
    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)
        if remoteUser1 == uid {
            remoteUser1 = 0
        } else if remoteUser2 == uid {
            remoteUser2 = 0
        }
        localSpatial.removeRemotePosition(uid)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionChangedTo state: AgoraConnectionState, reason: AgoraConnectionChangedReason) {
        LogUtils.log(message: "Connection state changed: \(state) \(reason)", level: .info)
    }
}
