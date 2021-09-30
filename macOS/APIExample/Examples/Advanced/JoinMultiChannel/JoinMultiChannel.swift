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
    
//    var channel1: AgoraRtcChannel?
//    var channel2: AgoraRtcChannel?
    var channel1: JoinMultiChannelMainEventListener = JoinMultiChannelMainEventListener()
    var channel2: JoinMultiChannelMainEventListener = JoinMultiChannelMainEventListener()
    var channelName1 = ""
    var channelName2 = ""
    var connectionId1:UInt32?
    var connectionId2:UInt32?
    
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
            let connectionIdPointer1 = UnsafeMutablePointer<UInt>.allocate(capacity: MemoryLayout<UInt>.stride)
            let mediaOptions = AgoraRtcChannelMediaOptions()
            // publish custom video track for channel 1
            mediaOptions.publishAudioTrack = .of(false)
            mediaOptions.publishCameraTrack = .of(true)
            mediaOptions.channelProfile = .of((Int32)(AgoraChannelProfile.liveBroadcasting.rawValue))
            mediaOptions.clientRoleType = .of((Int32)(AgoraClientRole.broadcaster.rawValue))
            let connection1 = AgoraRtcConnection()
            connection1.channelId = channelName1
            connection1.localUid = 0
            let result = agoraKit.joinChannelEx(byToken: nil, connection: connection1, delegate: channel1, mediaOptions: mediaOptions)
            channel1.connectionId = UInt32(connectionIdPointer1.pointee)
            connectionId1 = UInt32(connectionIdPointer1.pointee)
            channel1.connecitonDelegate = self
            connectionIdPointer1.deallocate()
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                self.showAlert(title: "Error", message: "joinChannel1 call failed: \(result), please check your params")
            }
            else {
                isJoined = true
            }
        } else {
            let channel1 = AgoraRtcConnection()
            channel1.channelId = channelName1
            agoraKit.leaveChannelEx(channel1, leaveChannelBlock: nil)
            isJoined = false
        }
    }
    
    // indicate if current instance has joined channel2
    var isJoined2: Bool = false {
        didSet {
            channelField2.isEnabled = !isJoined2
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
        joinChannel2Button.title = isJoined2 ? "Leave Channel".localized : "Join Channel".localized
    }
    @IBAction func onJoinChannel2ButtonPressed(_ sender:NSButton) {
        channelName2 = channelField2.stringValue
        if !isJoined2 {
            // auto subscribe options after join channel
            // join channel2
            let connectionIdPointer2 = UnsafeMutablePointer<UInt>.allocate(capacity: MemoryLayout<UInt>.stride)
            let mediaOptions = AgoraRtcChannelMediaOptions()
            // publish custom video track for channel 2
            mediaOptions.publishAudioTrack = .of(false)
            mediaOptions.publishCameraTrack = .of(true)
            mediaOptions.channelProfile = .of((Int32)(AgoraChannelProfile.liveBroadcasting.rawValue))
            mediaOptions.clientRoleType = .of((Int32)(AgoraClientRole.broadcaster.rawValue))
            let connection2 = AgoraRtcConnection()
            connection2.channelId = channelName2
            connection2.localUid = 0
            let result = agoraKit.joinChannelEx(byToken: nil, connection: connection2, delegate: channel1, mediaOptions: mediaOptions)
            channel2.connectionId = UInt32(connectionIdPointer2.pointee)
            connectionId2 = UInt32(connectionIdPointer2.pointee)
            channel2.connecitonDelegate = self
            connectionIdPointer2.deallocate()
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                self.showAlert(title: "Error", message: "joinChannel2 call failed: \(result), please check your params")
            }
            else{
                isJoined2 = true
            }
        } else {
            let channel2 = AgoraRtcConnection()
            channel2.channelId = channelName2
            agoraKit.leaveChannelEx(channel2, leaveChannelBlock: nil)
            isJoined2 = false
        }
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
                frameRate: AgoraVideoFrameRate(rawValue: fps) ?? .fps15,
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
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.rawValue)", level: .warning)
    }
    
    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode)", level: .error)
        self.showAlert(title: "Error", message: "Error \(errorCode.rawValue) occur")
    }
}

extension JoinMultipleChannel :JoinMultiChannelMainConnectionProtocol {
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId: UInt32, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.description)", level: .warning)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId: UInt32, didOccurError errorCode: AgoraErrorCode) {
        self.showAlert(title: "Error", message: "Error \(errorCode.description) occur")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId: UInt32, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId: UInt32, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)

        // Only one remote video view is available for this
        // tutorial. Here we check if there exists a surface
        // view tagged as this uid.
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = connectionId == connectionId1 ? videos2[0].videocanvas : videos2[1].videocanvas
        videoCanvas.renderMode = .hidden
        let connection = AgoraRtcConnection()
        agoraKit.setupRemoteVideoEx(videoCanvas, connection: connection)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId: UInt32, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
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
        agoraKit.setupRemoteVideoEx(videoCanvas, connection: connection)
    }
    
    
}

protocol JoinMultiChannelMainConnectionProtocol : NSObject {
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId:UInt32, didOccurWarning warningCode: AgoraWarningCode)
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId:UInt32, didOccurError errorCode: AgoraErrorCode)
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId:UInt32, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int)
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId:UInt32, didJoinedOfUid uid: UInt, elapsed: Int)
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId:UInt32, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason)
}

/// agora rtc engine delegate events
class JoinMultiChannelMainEventListener: NSObject, AgoraRtcEngineDelegate {
    weak var connecitonDelegate:JoinMultiChannelMainConnectionProtocol?
    var connectionId:UInt32?
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        if let connId = self.connectionId {
            self.connecitonDelegate?.rtcEngine(engine, connectionId: connId, didOccurWarning: warningCode)
        }
    }

    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        if let connId = self.connectionId {
            self.connecitonDelegate?.rtcEngine(engine, connectionId: connId, didOccurError: errorCode)
        }
    }


    internal func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        if let connId = self.connectionId {
            self.connecitonDelegate?.rtcEngine(engine, connectionId: connId, didJoinChannel: channel, withUid: uid, elapsed: elapsed)
        }
    }

    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
//    func rtcChannel(_ rtcChannel: AgoraRtcChannel, didJoinedOfUid uid: UInt, elapsed: Int) {
    internal func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        if let connId = self.connectionId {
            self.connecitonDelegate?.rtcEngine(engine, connectionId: connId, didJoinedOfUid: uid, elapsed: elapsed)
        }
    }

    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    internal func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        if let connId = self.connectionId {
            self.connecitonDelegate?.rtcEngine(engine, connectionId: connId, didOfflineOfUid: uid, reason: reason)
        }
    }
}
