//
//  JoinChannelVideoRTC.swift
//  APIExample-SwiftUI
//
//  Created by zhaoyongqiang on 2024/3/19.
//

import AgoraRtcKit
import SwiftUI


let CONNECTION_1_UID = UInt.random(in: 1001...2000)
let CONNECTION_2_UID = UInt.random(in: 2001...3000)
class JoinMultiChannelRTC: NSObject, ObservableObject {
    private var agoraKit: AgoraRtcEngineKit!
    private var isJoined: Bool = false
    
    private var localView: VideoUIView?
    private var remoteView: VideoUIView?
    private var remote2View: VideoUIView?
    
    private var channel1: Channel1Delegate = Channel1Delegate()
    private var channel2: Channel2Delegate = Channel2Delegate()
    private var channelName1 = ""
    private var channelName2 = ""
    
    @Published var snapshotPath: String = ""
    @Published var isShowAlert: Bool = false
    @Published var joinEx: Bool = false
    
    func setupRTC(configs: [String: Any],
                  localView: VideoUIView,
                  remoteView: VideoUIView,
                  remote2View: VideoUIView) {
        self.localView = localView
        self.remoteView = remoteView
        self.remote2View = remote2View
        
        // get channel name from configs
        guard let channelName = configs["channelName"] as? String else {return}
        channelName1 = "\(channelName)"
        channelName2 = "\(channelName)-2"
        channel1.channelId = channelName1
        channel2.channelId = channelName2
        channel1.view = remoteView.videoView
        channel2.view = remote2View.videoView
        remote2View.setPlaceholder(text: "\(channelName2 )\nRemote Host")
        channel2.userJoinExClosure = { [weak self] isJoin in
            self?.joinEx = isJoin
        }
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: channel1)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        
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
        
        joinChannel1()
        joinChannel2()
        localView.setInfo(text: "")
        remoteView.setInfo(text: "")
        remote2View.setInfo(text: "")
    }
    
   private func setupCanvasView(view: UIView?) {
        // set up local video to render your local camera preview
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = CONNECTION_1_UID
        // the view to be binded
        videoCanvas.view = view
        videoCanvas.renderMode = .hidden
        agoraKit.setupLocalVideo(videoCanvas)
        // you have to call startPreview to see local video
        agoraKit.startPreview()
    }
    
    private func joinChannel1() {
        // join channel1
        let mediaOptions = AgoraRtcChannelMediaOptions()
        // publish audio and camera track for channel 1
        mediaOptions.publishCameraTrack = false
        mediaOptions.publishMicrophoneTrack = false
        mediaOptions.autoSubscribeVideo = true
        mediaOptions.autoSubscribeAudio = true
        mediaOptions.clientRoleType = .audience
        NetworkManager.shared.generateToken(channelName: channelName1, uid: CONNECTION_1_UID) { token in
            let result = self.agoraKit.joinChannel(byToken: token,
                                                   channelId: self.channelName1,
                                                   uid: CONNECTION_1_UID,
                                                   mediaOptions: mediaOptions,
                                                   joinSuccess: nil)
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
                LogUtils.log(message: "joinChannel1 call failed: \(result), please check your params", level: .error)
            }
        }
    }
    
    private func joinChannel2() {
        // join channel2
        let mediaOptions = AgoraRtcChannelMediaOptions()
        mediaOptions.publishMicrophoneTrack = true
        mediaOptions.publishCameraTrack = true
        mediaOptions.autoSubscribeVideo = true
        mediaOptions.autoSubscribeAudio = true
        mediaOptions.clientRoleType = .broadcaster
        let connection2 = AgoraRtcConnection()
        connection2.channelId = channelName2
        connection2.localUid = CONNECTION_2_UID
        NetworkManager.shared.generateToken(channelName: channelName2, uid: CONNECTION_2_UID) { token in
            let result = self.agoraKit.joinChannelEx(byToken: token,
                                                     connection: connection2,
                                                     delegate: self.channel2,
                                                     mediaOptions: mediaOptions,
                                                     joinSuccess: nil)
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
                LogUtils.log(message: "joinChannel2 call failed: \(result), please check your params", level: .error)
            }
        }
    }
    
    func onTapTakeSnapshotEx() {
        guard channel2.remoteUid != 0 else { return }
        let path = NSTemporaryDirectory().appending("1.png")
        let connection = AgoraRtcConnection(channelId: channelName2, localUid: Int(CONNECTION_2_UID))
        agoraKit.takeSnapshotEx(connection, uid: Int(channel2.remoteUid), filePath: path)
        snapshotPath = path
        isShowAlert = true
    }
    
    func onTapLeaveChannelEx(isStop: Bool) {
        let connection2 = AgoraRtcConnection()
        connection2.channelId = channelName2
        connection2.localUid = CONNECTION_2_UID
        let channelOptions = AgoraLeaveChannelOptions()
        if isStop {
            channelOptions.stopMicrophoneRecording = true
        }
        agoraKit.leaveChannelEx(connection2, options: channelOptions, leaveChannelBlock: nil)
        channel2.remoteUid = 0
    }
    
    func onDestory() {
        agoraKit.disableAudio()
        agoraKit.disableVideo()
        if isJoined {
            agoraKit.stopPreview()
            agoraKit.leaveChannel { (stats) -> Void in
                LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
            }
            let channel2 = AgoraRtcConnection()
            channel2.channelId = channelName2
            channel2.localUid = CONNECTION_2_UID
            agoraKit.leaveChannelEx(channel2, leaveChannelBlock: nil)
        }
        AgoraRtcEngineKit.destroy()
    }
}

class Channel1Delegate: NSObject, AgoraRtcEngineDelegate {
    var channelId: String?
    var view: UIView?
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        
        LogUtils.log(message: "local user join room channel \(channelId ?? ""): \(uid) \(elapsed)ms", level: .info)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        guard let channelId = channelId, let channel1RemoteVideo = view else {
            return
        }
        LogUtils.log(message: "remote user join room channel \(channelId): \(uid) \(elapsed)ms", level: .info)

        // Only one remote video view is available for this
        // tutorial. Here we check if there exists a surface
        // view tagged as this uid.
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = channel1RemoteVideo
        videoCanvas.renderMode = .hidden
        let connection = AgoraRtcConnection()
        connection.channelId = channelId
        connection.localUid = CONNECTION_1_UID
        engine.setupRemoteVideoEx(videoCanvas, connection: connection)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        guard let channelId = channelId else {
            return
        }
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)

        // to unlink your view from sdk, so that your view reference will be released
        // note the video will stay at its last frame, to completely remove it
        // you will need to remove the EAGL sublayer from your binded view
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = nil
        videoCanvas.renderMode = .hidden
        let connection = AgoraRtcConnection()
        connection.channelId = channelId
        connection.localUid = CONNECTION_2_UID
        engine.setupRemoteVideoEx(videoCanvas, connection: connection)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, localAudioStateChanged state: AgoraAudioLocalState, reason: AgoraAudioLocalReason) {
        print("localAudioStateChanged == \(state.rawValue)")
    }
}

/// agora rtc engine delegate events
class Channel2Delegate: NSObject, AgoraRtcEngineDelegate {
    var channelId: String?
    var view: UIView?
    var remoteUid: UInt = 0
    var userJoinExClosure: ((Bool) -> Void)?
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.description)", level: .warning)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode.description)", level: .error)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        guard let channelId = channelId, let channel2RemoteVideo = view else {
            return
        }
        LogUtils.log(message: "remote user join room channel \(channelId): \(uid) \(elapsed)ms", level: .info)

        // Only one remote video view is available for this
        // tutorial. Here we check if there exists a surface
        // view tagged as this uid.
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = channel2RemoteVideo
        videoCanvas.renderMode = .hidden
        let connection = AgoraRtcConnection()
        connection.channelId = channelId
        connection.localUid = CONNECTION_2_UID
        engine.setupRemoteVideoEx(videoCanvas, connection: connection)
        remoteUid = uid
        userJoinExClosure?(true)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        guard let channelId = channelId else {
            return
        }
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)

        // to unlink your view from sdk, so that your view reference will be released
        // note the video will stay at its last frame, to completely remove it
        // you will need to remove the EAGL sublayer from your binded view
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = nil
        videoCanvas.renderMode = .hidden
        let connection = AgoraRtcConnection()
        connection.channelId = channelId
        connection.localUid = CONNECTION_2_UID
        engine.setupRemoteVideoEx(videoCanvas, connection: connection)
        remoteUid = 0
        userJoinExClosure?(false)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, localAudioStateChanged state: AgoraAudioLocalState, reason: AgoraAudioLocalReason) {
        print("localAudioStateChanged == \(state.rawValue)")
    }
}
