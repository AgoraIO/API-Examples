//
//  JoinChannelVideoRTC.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/19.
//

import AgoraRtcKit
import SwiftUI

let CAMERA_UID = UInt.random(in: 1001...2000)
let PLAYER_UID = UInt.random(in: 2001...3000)
class MediaPlayerRTC: NSObject, ObservableObject {
    private var agoraKit: AgoraRtcEngineKit!
    private var isJoined: Bool = false
    
    private var localView: VideoUIView?
    private var remoteView: VideoUIView?
    private var mediaPlayerKit: AgoraRtcMediaPlayerProtocol!
    private var playAudioTrackId: Int = 0
    private var publishTrackIndex: Int = 0
    private var channelName: String?
    @Published var trackList: [AgoraRtcMediaStreamInfo]?
    @Published var playoutVolume: CGFloat = 0
    @Published var publishVolume: CGFloat = 50
    @Published var url: String = "https://agora-adc-artifacts.s3.cn-north-1.amazonaws.com.cn/resources/sample.mp4"
    @Published var playerDurationLabel: String = "00:00"
    @Published var playerProgressSlider: Float = 0
    @Published var codecName: String = "aac"
    @Published var isPlaying: Bool = false
    @Published var isPublished: Bool = false
    
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
        guard let channelName = configs["channelName"] as? String else {return}
        self.channelName = channelName
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
        
        // prepare media player
        mediaPlayerKit = agoraKit.createMediaPlayer(with: self)
        mediaPlayerKit.setView(localView.videoView)
        mediaPlayerKit.setRenderMode(.fit)
        
        playoutVolume = CGFloat(mediaPlayerKit.getPlayoutVolume())
        publishVolume = 50
        mediaPlayerKit.adjustPublishSignalVolume(50)
        
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
        option.publishCustomAudioTrack = GlobalSettings.shared.getUserRole() == .broadcaster
        option.publishCameraTrack = GlobalSettings.shared.getUserRole() == .broadcaster
        option.publishMicrophoneTrack = GlobalSettings.shared.getUserRole() == .broadcaster
        option.clientRoleType = GlobalSettings.shared.getUserRole()
        option.autoSubscribeAudio = true
        option.autoSubscribeVideo = true
        NetworkManager.shared.generateToken(channelName: channelName,
                                            uid: CAMERA_UID,
                                            success: { token in
            let result = self.agoraKit.joinChannel(byToken: token,
                                                   channelId: channelName,
                                                   uid: CAMERA_UID,
                                                   mediaOptions: option)
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
                LogUtils.log(message: "joinChannel call failed: \(result), please check your params", level: .error)
            }
        })
        agoraKit.muteRemoteAudioStream(PLAYER_UID, mute: true)
        
        setupChannel2(channelName: channelName)
    }
    
    private func setupChannel2(channelName: String) {
        let option1 = AgoraRtcChannelMediaOptions()
        option1.autoSubscribeAudio = false
        option1.autoSubscribeVideo = false
        option1.clientRoleType = GlobalSettings.shared.getUserRole()
        option1.enableAudioRecordingOrPlayout = false
        let connection = AgoraRtcConnection()
        connection.channelId = channelName
        connection.localUid = PLAYER_UID
        NetworkManager.shared.generateToken(channelName: channelName, uid: PLAYER_UID) { token in
            let result1 = self.agoraKit.joinChannelEx(byToken: token,
                                                      connection: connection,
                                                      delegate: self,
                                                      mediaOptions: option1,
                                                      joinSuccess: nil)
            if result1 != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
                LogUtils.log(message: "joinChannel call failed: \(result1), please check your params", level: .error)
            }
            self.doOpenMediaUrl()
        }
        localView?.setInfo(text: "")
    }
    
    func doOpenMediaUrl() {
        // resign text field
        let mediaSource = AgoraMediaSource()
        mediaSource.url = url
        mediaSource.autoPlay = false
        mediaSource.enableMultiAudioTrack = true
        mediaPlayerKit.open(with: mediaSource)
    }
    
    func doPlay() {
        mediaPlayerKit.play()
        isPlaying = true
    }
    
    func doStop() {
        mediaPlayerKit.stop()
        doOpenMediaUrl()
        isPlaying = false
    }
    
    func doPause() {
        mediaPlayerKit.pause()
        isPlaying = false
    }
    
    func doPublish() {
        guard let channelName = channelName else { return }
        let option = AgoraRtcChannelMediaOptions()
        option.publishMediaPlayerVideoTrack = true
        option.publishMediaPlayerAudioTrack = true
        option.publishMediaPlayerId = Int(mediaPlayerKit.getMediaPlayerId())
        option.publishCameraTrack = false
        let connection = AgoraRtcConnection()
        connection.channelId = channelName
        connection.localUid = PLAYER_UID
        agoraKit.updateChannelEx(with: option, connection: connection)
        isPublished = true
    }
    
    func dloStopPublish() {
        guard let channelName = channelName else { return }
        let option = AgoraRtcChannelMediaOptions()
        option.publishMediaPlayerVideoTrack = false
        option.publishMediaPlayerAudioTrack = false
        option.publishMediaPlayerId = Int(mediaPlayerKit.getMediaPlayerId())
        option.publishCameraTrack = false
        let connection = AgoraRtcConnection()
        connection.channelId = channelName
        connection.localUid = PLAYER_UID
        agoraKit.updateChannelEx(with: option, connection: connection)
        isPublished = false
    }
    
    func doAdjustPlayoutVolume(value: CGFloat) {
        mediaPlayerKit.adjustPlayoutVolume(Int32(Int(value)))
    }
    
    func doAdjustPublishVolume(value: CGFloat) {
        mediaPlayerKit.adjustPublishSignalVolume(Int32(Int(value)))
    }
    
    func onClickPlayAudioTrack(value: String) {
        guard let track = trackList?.filter({ $0.codecName == value }).first else { return }
        playAudioTrackId = track.streamIndex
        mediaPlayerKit.selectMultiAudioTrack(track.streamIndex,
                                             publishTrackIndex: publishTrackIndex)
    }
    
    func onClickPublishAudioTrack(value: String) {
        guard let track = trackList?.filter({ $0.codecName == value }).first else { return }
        publishTrackIndex = track.streamIndex
        mediaPlayerKit.selectMultiAudioTrack(playAudioTrackId,
                                             publishTrackIndex: track.streamIndex)
    }
    
    private func setupCanvasView(view: UIView?) {
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.view = view
        videoCanvas.renderMode = .hidden
        videoCanvas.sourceType = .mediaPlayer
        videoCanvas.mediaPlayerId = mediaPlayerKit.getMediaPlayerId()
        agoraKit.setupLocalVideo(videoCanvas)
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
        agoraKit.destroyMediaPlayer(mediaPlayerKit)
        mediaPlayerKit = nil
        AgoraRtcEngineKit.destroy()
    }
}

// agora rtc engine delegate events
extension MediaPlayerRTC: AgoraRtcEngineDelegate {
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
        if uid == CAMERA_UID || uid == PLAYER_UID {
            return
        }
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
extension MediaPlayerRTC: AgoraRtcMediaPlayerDelegate {

    func AgoraRtcMediaPlayer(_ playerKit: AgoraRtcMediaPlayerProtocol,
                             didChangedTo state: AgoraMediaPlayerState,
                             reason: AgoraMediaPlayerReason) {
        LogUtils.log(message: "player rtc channel publish helper state changed to: \(state.rawValue), error: \(reason.rawValue)", level: .info)
        DispatchQueue.main.async {[weak self] in
            guard let weakself = self else { return }
            switch state.rawValue {
            case 100: // failed
                LogUtils.log(message: "media player error: \(reason.rawValue)", level: .error)
                weakself.isPlaying = false
                
            case 2: // openCompleted
                let duration = weakself.mediaPlayerKit.getDuration()
                let format = "\(String(format: "%02d", duration / 60000)) : \(String(format: "%02d", duration % 60000 / 1000))"
                weakself.playerDurationLabel = format
                weakself.playerProgressSlider = 0
                
                if weakself.mediaPlayerKit.getStreamCount() > 0 {
                    weakself.trackList = (0...weakself.mediaPlayerKit.getStreamCount()).filter({
                        weakself.mediaPlayerKit.getStreamBy(Int32($0))?.streamType == .audio
                    }).compactMap({ weakself.mediaPlayerKit.getStreamBy(Int32($0)) })
                    weakself.playAudioTrackId = weakself.trackList?.first?.streamIndex ?? 0
                    weakself.publishTrackIndex = weakself.trackList?.first?.streamIndex ?? 0
                    weakself.codecName = weakself.trackList?.first?.codecName ?? ""
                }

            case 7: // stopped
                weakself.playerProgressSlider = 0
                weakself.playerDurationLabel = "00 : 00"
                guard let channelName = self?.channelName else { return }
                let option = AgoraRtcChannelMediaOptions()
                option.publishMediaPlayerVideoTrack = false
                option.publishMediaPlayerAudioTrack = false
                let connection = AgoraRtcConnection()
                connection.channelId = channelName
                connection.localUid = PLAYER_UID
                weakself.agoraKit.updateChannelEx(with: option, connection: connection)
                weakself.isPlaying = false

            default: break
            }
        }
    }
    
    func AgoraRtcMediaPlayer(_ playerKit: AgoraRtcMediaPlayerProtocol, didChangedTo positionMs: Int, atTimestamp timestampMs: TimeInterval) {
        let duration = Float(mediaPlayerKit.getDuration())
        var progress: Float = 0
        var left: Int = 0
        if duration > 0 {
            progress = Float(positionMs) / duration
            left = Int((mediaPlayerKit.getDuration() - positionMs)) / 1000
        }
        DispatchQueue.main.async {[weak self] in
            guard let weakself = self else { return }
            weakself.playerDurationLabel = "\(String(format: "%02d", left / 60)) : \(String(format: "%02d", left % 60))"
            weakself.playerProgressSlider = progress
        }
    }
}
