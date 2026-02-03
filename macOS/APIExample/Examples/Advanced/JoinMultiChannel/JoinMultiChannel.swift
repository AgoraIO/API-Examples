//
//  JoinChannelVC.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//
import Cocoa
import AgoraRtcKit
import AGEVideoLayout

class JoinMultipleChannel: BaseViewController {
    var videos: [VideoView] = []
    var videos2: [VideoView] = []
    
    @IBOutlet weak var container: AGEVideoContainer!
    @IBOutlet weak var container2: AGEVideoContainer!
    @IBOutlet weak var stopMicrophoneRecordingSwitch: NSSwitch!
    @IBOutlet weak var takeSnapShotEx: NSButton!
    
    //    var channel1: AgoraRtcChannel?
//    var channel2: AgoraRtcChannel?
    var channel1: JoinMultiChannelMainEventListener = JoinMultiChannelMainEventListener()
    var channel2: JoinMultiChannelMainEventListener = JoinMultiChannelMainEventListener()
    var channelName1 = ""
    var channelName2 = ""
    let channel1Uid = UInt.random(in: 1001...2000)
    let channel2Uid = UInt.random(in: 2001...3000)
    var remoteUid: UInt = 0
    var agoraKit: AgoraRtcEngineKit!
    
    // indicate if current instance has joined channel1
    var isJoined: Bool = false {
        didSet {
            channelField1.isEnabled = !isJoined
            initJoinChannel1Button()
        }
    }
    /**
     --- Channel1 TextField ---
     */
    @IBOutlet weak var channelField1: Input!
    func initChannelField1() {
        channelField1.label.stringValue = "Channel".localized + "1"
        channelField1.field.placeholderString = "Channel Name".localized + "1"
    }
    /**
     --- Join Channel1 Button ---
     */
    @IBOutlet weak var joinChannel1Button: NSButton!
    func initJoinChannel1Button() {
        joinChannel1Button.title = isJoined ? "Leave Channel".localized : "Join Channel".localized
    }
    @IBAction func onJoinChannel1ButtonPressed(_ sender: NSButton) {
        channelName1 = channelField1.stringValue
        if !isJoined {
            // auto subscribe options after join channel
            // join channel1
            let mediaOptions = AgoraRtcChannelMediaOptions()
            // publish custom video track for channel 1
            mediaOptions.publishCustomAudioTrack = true
            mediaOptions.publishCameraTrack = true
            mediaOptions.autoSubscribeAudio = true
            mediaOptions.autoSubscribeVideo = true
            mediaOptions.channelProfile = .liveBroadcasting
            mediaOptions.clientRoleType = .broadcaster
            let connection1 = AgoraRtcConnection()
            connection1.channelId = channelName1
            connection1.localUid = channel1Uid
            channel1.connectionDelegate = self
            channel1.connectionId = channelName1
            NetworkManager.shared.generateToken(channelName: channelName1, uid: channel1Uid) { token in
                let result = self.agoraKit.joinChannel(byToken: token,
                                                       channelId: self.channelName1,
                                                       uid: self.channel1Uid,
                                                       mediaOptions: mediaOptions,
                                                       joinSuccess: nil)
                if result != 0 {
                    // Usually happens with invalid parameters
                    // Error code description can be found at:
                    // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                    // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
                    self.showAlert(title: "Error", message: "joinChannel1 call failed: \(result), please check your params")
                }
                else {
                    self.isJoined = true
                }
            }
            
        } else {
            let channel1 = AgoraRtcConnection()
            channel1.channelId = channelName1
            channel1.localUid = channel1Uid
            agoraKit.leaveChannelEx(channel1, leaveChannelBlock: nil)
            isJoined = false
        }
    }
    
    // indicate if current instance has joined channel2
    var isJoined2: Bool = false {
        didSet {
            channelField2.isEnabled = !isJoined2
            stopMicrophoneRecordingSwitch.isEnabled = isJoined2
            takeSnapShotEx.isEnabled = isJoined2
            initJoinChannel2Button()
        }
    }
    /**
     --- Channel1 TextField ---
     */
    @IBOutlet weak var channelField2: Input!
    func initChannelField2() {
        channelField2.label.stringValue = "Channel".localized + "2"
        channelField2.field.placeholderString = "Channel Name".localized + "2"
    }
    /**
     --- Join Channel2 Button ---
     */
    @IBOutlet weak var joinChannel2Button: NSButton!
    func initJoinChannel2Button() {
        joinChannel2Button.title = isJoined2 ? "Leave ChannelEx".localized : "Join ChannelEx".localized
    }
    @IBAction func onJoinChannel2ButtonPressed(_ sender:NSButton) {
        channelName2 = channelField2.stringValue
        if !isJoined2 {
            // auto subscribe options after join channel
            // join channel2
            let mediaOptions = AgoraRtcChannelMediaOptions()
            // publish custom video track for channel 2
            mediaOptions.publishCameraTrack = true
            mediaOptions.publishMicrophoneTrack = true
            mediaOptions.autoSubscribeAudio = true
            mediaOptions.autoSubscribeVideo = true
            mediaOptions.channelProfile = .liveBroadcasting
            mediaOptions.clientRoleType = .broadcaster
            let connection2 = AgoraRtcConnection()
            connection2.channelId = channelName2
            connection2.localUid = channel2Uid
            channel2.connectionDelegate = self
            channel2.connectionId = channelName2
            NetworkManager.shared.generateToken(channelName: channelName2, uid: channel2Uid) { token in
                let result = self.agoraKit.joinChannelEx(byToken: token,
                                                         connection: connection2,
                                                         delegate: self.channel2,
                                                         mediaOptions: mediaOptions)
                if result != 0 {
                    // Usually happens with invalid parameters
                    // Error code description can be found at:
                    // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                    // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
                    self.showAlert(title: "Error", message: "joinChannel2 call failed: \(result), please check your params")
                }
                else{
                    self.isJoined2 = true
                }
            }
            
        } else {
            let channel2 = AgoraRtcConnection()
            channel2.channelId = channelName2
            channel2.localUid = channel2Uid
            let channelOptions = AgoraLeaveChannelOptions()
            channelOptions.stopMicrophoneRecording = stopMicrophoneRecordingSwitch.state == .on
            agoraKit.leaveChannelEx(channel2, options: channelOptions, leaveChannelBlock: nil)
            stopMicrophoneRecordingSwitch.state = .off
            isJoined2 = false
        }
    }
    @IBAction func onTakSnapshotEx(_ sender: Any) {
        guard isJoined2 else { return }
        let filePath = FileManager.default.urls(for: .downloadsDirectory, in: .userDomainMask).first?.absoluteString
        let programPath = filePath?.components(separatedBy: "/")[4] ?? ""
        let path = "/Users/\(programPath)/Downloads/\(Date()).png"
        let channel2 = AgoraRtcConnection()
        channel2.channelId = channelName2
        channel2.localUid = channel2Uid
        agoraKit.takeSnapshotEx(channel2, uid: Int(remoteUid), filePath: path)
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        layoutVideos()
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        // this is mandatory to get camera list
        agoraKit.enableVideo()

        // set proxy configuration
//        let proxySetting = GlobalSettings.shared.proxySetting.selectedOption().value
//        agoraKit.setCloudProxy(AgoraCloudProxyType.init(rawValue: UInt(proxySetting)) ?? .noneProxy)
        
        initChannelField1()
        initJoinChannel1Button()
        initChannelField2()
        initJoinChannel2Button()
        agoraKit.setClientRole(.broadcaster)
        
        let resolution = Configs.Resolutions[GlobalSettings.shared.resolutionSetting.selectedOption().value]
        let fps = Configs.Fps[GlobalSettings.shared.fpsSetting.selectedOption().value]
        // enable video module and set up video encoding configs
        agoraKit.setVideoEncoderConfiguration(
            AgoraVideoEncoderConfiguration(
                size: resolution.size(),
                frameRate: 15,
                bitrate: AgoraVideoBitrateStandard,
                orientationMode: .adaptative,
                mirrorMode: .auto
            )
        )
        // set up local video to render your local camera preview
        let localVideo = videos[0]
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = 0
        // the view to be binded
        videoCanvas.view = localVideo.videocanvas
        videoCanvas.renderMode = .hidden
        agoraKit.setupLocalVideo(videoCanvas)
        // you have to call startPreview to see local video
        agoraKit.startPreview()
    }
    
    override func viewWillBeRemovedFromSplitView() {
        AgoraRtcEngineKit.destroy()
    }

    func getChannelByName(_ channelName: String?) -> String? {
        if channelName1 == channelName {
            return channelName1
        } else if channelName2 == channelName {
            return channelName2
        }
        return nil
    }
    
    func layoutVideos() {
        videos = [VideoView.createFromNib()!]
        videos[0].placeholder.stringValue = "Local"
        // layout render view
        container.layoutStream(views: videos)
        
        videos2 = [VideoView.createFromNib()!, VideoView.createFromNib()!]
        videos2[0].placeholder.stringValue = "Channel1\nRemote"
        videos2[1].placeholder.stringValue = "Channel2\nRemote"
        container2.layoutStream2(views: videos2)
    }
}

/// agora rtc engine delegate events
extension JoinMultipleChannel: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://api-ref.agora.io/en/voice-sdk/ios/3.x/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.rawValue)", level: .warning)
    }
    
    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
    /// cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode)", level: .error)
        self.showAlert(title: "Error", message: "Error \(errorCode.rawValue) occur")
    }
    func rtcEngine(_ engine: AgoraRtcEngineKit, localAudioStateChanged state: AgoraAudioLocalState, reason: AgoraAudioLocalReason) {
        print("localAudioStateChanged == \(state.rawValue)")
    }
}

extension JoinMultipleChannel :JoinMultiChannelMainConnectionProtocol {
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId: String, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.description)", level: .warning)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId: String, didOccurError errorCode: AgoraErrorCode) {
        self.showAlert(title: "Error", message: "Error \(errorCode.description) occur")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId: String, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId: String, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)

        // Only one remote video view is available for this
        // tutorial. Here we check if there exists a surface
        // view tagged as this uid.
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = connectionId == channelName2 ? videos2[0].videocanvas : videos2[1].videocanvas
        videoCanvas.renderMode = .hidden
        let connection = AgoraRtcConnection()
        connection.channelId = connectionId
        connection.localUid = connectionId == channelName2 ? channel2Uid : channel1Uid
        agoraKit.setupRemoteVideoEx(videoCanvas, connection: connection)
        remoteUid = uid
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId: String, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
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
        connection.channelId = connectionId
        connection.localUid = connectionId == channelName2 ? channel2Uid : channel1Uid
        agoraKit.setupRemoteVideoEx(videoCanvas, connection: connection)
        if remoteUid == uid {
            remoteUid = 0
        }
    }
}

protocol JoinMultiChannelMainConnectionProtocol : NSObject {
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId:String, didOccurWarning warningCode: AgoraWarningCode)
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId:String, didOccurError errorCode: AgoraErrorCode)
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId:String, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int)
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId:String, didJoinedOfUid uid: UInt, elapsed: Int)
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId:String, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason)
}

/// agora rtc engine delegate events
class JoinMultiChannelMainEventListener: NSObject, AgoraRtcEngineDelegate {
    weak var connectionDelegate: JoinMultiChannelMainConnectionProtocol?
    var connectionId:String?
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en:https://api-ref.agora.io/en/voice-sdk/ios/3.x/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        if let connId = self.connectionId {
            self.connectionDelegate?.rtcEngine(engine, connectionId: connId, didOccurWarning: warningCode)
        }
    }

    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        if let connId = self.connectionId {
            self.connectionDelegate?.rtcEngine(engine, connectionId: connId, didOccurError: errorCode)
        }
    }


    internal func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        if let connId = self.connectionId {
            self.connectionDelegate?.rtcEngine(engine, connectionId: connId, didJoinChannel: channel, withUid: uid, elapsed: elapsed)
        }
    }

    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
//    func rtcChannel(_ rtcChannel: AgoraRtcChannel, didJoinedOfUid uid: UInt, elapsed: Int) {
    internal func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        if let connId = self.connectionId {
            self.connectionDelegate?.rtcEngine(engine, connectionId: connId, didJoinedOfUid: uid, elapsed: elapsed)
        }
    }

    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    internal func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        if let connId = self.connectionId {
            self.connectionDelegate?.rtcEngine(engine, connectionId: connId, didOfflineOfUid: uid, reason: reason)
        }
    }
    func rtcEngine(_ engine: AgoraRtcEngineKit, localAudioStateChanged state: AgoraAudioLocalState, reason: AgoraAudioLocalReason) {
        print("localAudioState == \(state.rawValue)")
    }
}
