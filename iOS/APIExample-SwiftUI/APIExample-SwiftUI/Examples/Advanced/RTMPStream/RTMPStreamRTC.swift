//
//  RTMStreamRTC.swift
//  APIExample-SwiftUI
//
//  Created by qinhui on 2024/10/8.
//

import Foundation
import AgoraRtcKit

class RTMPStreamRTC: NSObject, ObservableObject {
    @Published var transcodingState = true
    @Published var isJoined = false
    @Published var isPublished: Bool = false
    @Published var rtmpUrl: String = "rtmp://push.webdemo.agoraio.cn/lbhd/test"

    var agoraKit: AgoraRtcEngineKit!
    private var localView: VideoUIView?
    private var remoteView: VideoUIView?
    private var remoteUid: UInt?
    private var transcoding = AgoraLiveTranscoding.default()
    private let CANVAS_WIDTH = 640
    private let CANVAS_HEIGHT = 480

    
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
        
        guard let channelName = configs["channelName"] as? String else {return}
        
        // make myself a broadcaster
        agoraKit.setClientRole(GlobalSettings.shared.getUserRole())
        
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        agoraKit.enableAudio()
        let resolution = (GlobalSettings.shared.getSetting(key: "resolution")?.selectedOption().value as? CGSize) ?? .zero
        let fps = (GlobalSettings.shared.getSetting(key: "fps")?.selectedOption().value as? AgoraVideoFrameRate) ?? .fps15
        let orientation = (GlobalSettings.shared.getSetting(key: "orientation")?
            .selectedOption().value as? AgoraVideoOutputOrientationMode) ?? .fixedPortrait
        agoraKit.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: resolution,
                                                                             frameRate: fps.rawValue,
                                                                             bitrate: AgoraVideoBitrateStandard,
                                                                             orientationMode: orientation,
                                                                             mirrorMode: .auto))
        
        // set up local video to render your local camera preview
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = 0
        // the view to be binded
        videoCanvas.view = localView.videoView
        videoCanvas.renderMode = .hidden
        agoraKit.setupLocalVideo(videoCanvas)
        // you have to call startPreview to see local video
        agoraKit.startPreview()
        
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
            }
        })
    }
    
    func onDestory() {
        agoraKit.disableAudio()
        agoraKit.disableVideo()
        if isJoined {
            if isPublished {
                agoraKit.stopRtmpStream(rtmpUrl)
            }
            agoraKit.stopPreview()
            agoraKit.leaveChannel { (stats) -> Void in
                LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
            }
        }
        AgoraRtcEngineKit.destroy()
    }
    
    func onPublish() {
        if rtmpUrl.isEmpty {
            return
        }
        
        if isPublished {
            // stop rtmp streaming
            agoraKit.stopRtmpStream(rtmpUrl)
        } else {
            // check whether we need to enable transcoding
            let transcodingEnabled = transcodingState
            if transcodingEnabled {
                // we will use transcoding to composite multiple hosts' video
                // therefore we have to create a livetranscoding object and call before addPublishStreamUrl
                transcoding.size = CGSize(width: CANVAS_WIDTH, height: CANVAS_HEIGHT)
                agoraKit.startRtmpStream(withTranscoding: rtmpUrl, transcoding: transcoding)
                
                if let remoteUid = remoteUid {
                    // add new user onto the canvas
                    let user = AgoraLiveTranscodingUser()
                    user.rect = CGRect(x: CANVAS_WIDTH / 2, y: 0, width: CANVAS_WIDTH / 2, height: CANVAS_HEIGHT)
                    user.uid = remoteUid
                    self.transcoding.add(user)
                    // remember you need to call setLiveTranscoding again if you changed the layout
                    agoraKit.updateRtmpTranscoding(transcoding)
                }
                
            } else {
                agoraKit.startRtmpStreamWithoutTranscoding(rtmpUrl)
            }
        }
    }
}

extension RTMPStreamRTC: AgoraRtcEngineDelegate {
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
        LogUtils.log(message: "error: \(errorCode.description)", level: .error)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        self.isJoined = true
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
        
        // add transcoding user so the video stream will be involved
        // in future RTMP Stream
        let user = AgoraLiveTranscodingUser()
        user.rect = CGRect(x: 0, y: 0, width: CANVAS_WIDTH / 2, height: CANVAS_HEIGHT)
        user.uid = uid
        self.transcoding.add(user)
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        
        // only one remote video view is available for this
        // tutorial. Here we check if there exists a surface
        // view tagged as this uid.
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = remoteView?.videoView
        videoCanvas.renderMode = .hidden
        agoraKit.setupRemoteVideo(videoCanvas)
        
        // remove preivous user from the canvas
        if let existingUid = remoteUid {
            transcoding.removeUser(existingUid)
        }
        remoteUid = uid
    }
    
    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "remote user left: \(uid) reason \(reason.rawValue)", level: .info)
        
        // to unlink your view from sdk, so that your view reference will be released
        // note the video will stay at its last frame, to completely remove it
        // you will need to remove the EAGL sublayer from your binded view
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = nil
        videoCanvas.renderMode = .hidden
        agoraKit.setupRemoteVideo(videoCanvas)
        
        // check whether we have enabled transcoding
        let transcodingEnabled = transcodingState
        if transcodingEnabled {
            // remove user from canvas if current cohost left channel
            if let existingUid = remoteUid {
                transcoding.removeUser(existingUid)
            }
            remoteUid = nil
            agoraKit.updateRtmpTranscoding(transcoding)
        }
    }
    
    /// callback for state of rtmp streaming, for both good and bad state
    /// @param url rtmp streaming url
    /// @param state state of rtmp streaming
    /// @param reason
    func rtcEngine(_ engine: AgoraRtcEngineKit,
                   rtmpStreamingChangedToState url: String,
                   state: AgoraRtmpStreamingState,
                   reason: AgoraRtmpStreamingReason) {
        LogUtils.log(message: "streamStateChanged: \(url) state \(state.rawValue) error \(reason.rawValue)", level: .info)
        if state == .running {
            isPublished = true
            ToastView.show(text: "RTMP Publish Success")
        } else if state == .failure {
            
        } else if state == .idle {
            isPublished = false
            ToastView.show(text: "RTMP Publish Stopped")
        }
    }
    
    /// callback when live transcoding is properly updated
    func rtcEngineTranscodingUpdated(_ engine: AgoraRtcEngineKit) {
        LogUtils.log(message: "live transcoding updated", level: .info)
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
