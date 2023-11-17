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

class ChannelMediaRelay: BaseViewController {
    var videos: [VideoView] = []
    
    @IBOutlet weak var Container: AGEVideoContainer!
    
    var agoraKit: AgoraRtcEngineKit!
    
    /**
     --- Channel TextField ---
     */
    @IBOutlet weak var channelField: Input!
    func initChannelField() {
        channelField.label.stringValue = "Channel".localized
        channelField.field.placeholderString = "Channel Name".localized
    }
    
    /**
     --- Join Button ---
     */
    @IBOutlet weak var joinChannelButton: NSButton!
    func initJoinChannelButton() {
        joinChannelButton.title = isJoined ? "Leave Channel".localized : "Join Channel".localized
    }
    
    /**
     --- Replay Channel TextField ---
     */
    @IBOutlet weak var relayChannelField: Input!
    func initRelayChannelField() {
        relayChannelField.label.stringValue = "Relay Channel".localized
        relayChannelField.field.placeholderString = "Relay Channnel Name".localized
    }
    
    /**
     --- Relay Button ---
     */
    @IBOutlet weak var relayButton: NSButton!
    func initRelayButton() {
        relayButton.title = isRelaying ? "Stop Relay".localized : "Start Relay".localized
    }
    @IBAction func onRelayPressed(_ sender: Any) {
        if isProcessing { return }
        if !isRelaying {
            let destinationChannelName = relayChannelField.stringValue
            // prevent operation if target channel name is empty
            if(destinationChannelName.isEmpty) {
                self.showAlert(message: "Destination channel name is empty")
                return
            }
            // configure source info, channel name defaults to current, and uid defaults to local
            let config = AgoraChannelMediaRelayConfiguration()
            config.sourceInfo = AgoraChannelMediaRelayInfo(token: nil)
            isProcessing = true
            // configure target channel info
            let destinationInfo = AgoraChannelMediaRelayInfo(token: nil)
            config.setDestinationInfo(destinationInfo, forChannelName: destinationChannelName)
            agoraKit.startOrUpdateChannelMediaRelay(config)
            pauseRelayButton.isEnabled = true
        } else {
            isProcessing = true
            isPauseRelaying = false
            agoraKit.stopChannelMediaRelay()
            pauseRelayButton.isEnabled = false
        }
        
    }
    
    /**
            --- Pause Relay Button ---
     */
    @IBOutlet weak var pauseRelayButton: NSButton!
    func initPauseRelayButton() {
        pauseRelayButton.title = isPauseRelaying ? "Resume Relay".localized : "Pause Relay".localized
    }
    @IBAction func onPauseRelayPressed(_ sender: Any) {
        if isProcessing, !isRelaying { return }
        if !isPauseRelaying {
            isPauseRelaying = true
            agoraKit.pauseAllChannelMediaRelay()
            
        }
        else {
            isPauseRelaying = false
            agoraKit.resumeAllChannelMediaRelay()
        }
    }

    // indicate if current instance has joined channel
    var isJoined: Bool = false {
        didSet {
            channelField.isEnabled = !isJoined
            initJoinChannelButton()
        }
    }
    
    // indicate for doing something
    var isProcessing: Bool = false {
        didSet {
            joinChannelButton.isEnabled = !isProcessing
            relayButton.isEnabled = !isProcessing
        }
    }
    
    var isRelaying: Bool = false {
        didSet {
            initRelayButton()
        }
    }
    
    var isPauseRelaying: Bool = false {
        didSet {
            initPauseRelayButton()
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        layoutVideos(2)
        // Do view setup here.
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit.enableVideo()
        
        initRelayChannelField()
        initRelayButton()
        initPauseRelayButton()
        initChannelField()
        initJoinChannelButton()
    }
    
    override func viewWillBeRemovedFromSplitView() {
        if isJoined {
            agoraKit.leaveChannel { (stats:AgoraChannelStats) in
                LogUtils.log(message: "Left channel", level: .info)
            }
        }
        AgoraRtcEngineKit.destroy()
    }
    
    @IBAction func onJoinPressed(_ sender:Any) {
        if !isJoined {
            // check configuration
            let channel = channelField.stringValue
            if channel.isEmpty {
                return
            }
            // set live broadcaster mode
            agoraKit.setChannelProfile(.liveBroadcasting)
            // set myself as broadcaster to stream video/audio
            agoraKit.setClientRole(.broadcaster)
            
            // set proxy configuration
//            let proxySetting = GlobalSettings.shared.proxySetting.selectedOption().value
//            agoraKit.setCloudProxy(AgoraCloudProxyType.init(rawValue: UInt(proxySetting)) ?? .noneProxy)
            
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
            
            let resolution = Configs.Resolutions[GlobalSettings.shared.resolutionSetting.selectedOption().value]
            let fps = Configs.Fps[GlobalSettings.shared.fpsSetting.selectedOption().value]
            agoraKit.setVideoEncoderConfiguration(
                AgoraVideoEncoderConfiguration(
                    size: resolution.size(),
                    frameRate: AgoraVideoFrameRate(rawValue: fps) ?? .fps15,
                    bitrate: AgoraVideoBitrateStandard,
                    orientationMode: .adaptative,
                    mirrorMode: .auto
                )
            )
            
            // start joining channel
            // 1. Users can only see each other after they join the
            // same channel successfully using the same app id.
            // 2. If app certificate is turned on at dashboard, token is needed
            // when joining channel. The channel name and uid used to calculate
            // the token has to match the ones used for channel join
            isProcessing = true
            let option = AgoraRtcChannelMediaOptions()
            option.publishCameraTrack = true
            option.clientRoleType = .broadcaster
            NetworkManager.shared.generateToken(channelName: channel, success: { token in
                let result = self.agoraKit.joinChannel(byToken: token, channelId: channel, uid: 0, mediaOptions: option)
                if result != 0 {
                    self.isProcessing = false
                    // Usually happens with invalid parameters
                    // Error code description can be found at:
                    // en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
                    // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                    self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
                }
            })
        } else {
            isProcessing = true
            agoraKit.leaveChannel { [unowned self] (stats:AgoraChannelStats) in
                self.isProcessing = false
                LogUtils.log(message: "Left channel", level: .info)
                self.videos[0].uid = nil
                self.isJoined = false
                self.videos.forEach {
                    $0.uid = nil
                    $0.statsLabel.stringValue = ""
                }
            }
        }
    }
    
    func layoutVideos(_ count: Int) {
        videos = []
        for i in 0...count - 1 {
            let view = VideoView.createFromNib()!
            if(i == 0) {
                view.placeholder.stringValue = "Local"
            } else {
                view.placeholder.stringValue = "Remote \(i)"
            }
            videos.append(view)
        }
        // layout render view
        Container.layoutStream(views: videos)
    }
}

/// agora rtc engine delegate events
extension ChannelMediaRelay: AgoraRtcEngineDelegate {
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
    /// en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode)", level: .error)
        if isProcessing {
            isProcessing = false
        }
        self.showAlert(title: "Error", message: "Error \(errorCode.rawValue) occur")
    }
    
    /// callback when the local user joins a specified channel.
    /// @param channel
    /// @param uid uid of local user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        isProcessing = false
        isJoined = true
        let localVideo = videos[0]
        localVideo.uid = uid
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        
        // find a VideoView w/o uid assigned
        if let remoteVideo = videos.first(where: { $0.uid == nil }) {
            let videoCanvas = AgoraRtcVideoCanvas()
            videoCanvas.uid = uid
            // the view to be binded
            videoCanvas.view = remoteVideo.videocanvas
            videoCanvas.renderMode = .hidden
            agoraKit.setupRemoteVideo(videoCanvas)
            remoteVideo.uid = uid
        } else {
            LogUtils.log(message: "no video canvas available for \(uid), cancel bind", level: .warning)
        }
    }
    
    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)
        
        // to unlink your view from sdk, so that your view reference will be released
        // note the video will stay at its last frame, to completely remove it
        // you will need to remove the EAGL sublayer from your binded view
        if let remoteVideo = videos.first(where: { $0.uid == uid }) {
            let videoCanvas = AgoraRtcVideoCanvas()
            videoCanvas.uid = uid
            // the view to be binded
            videoCanvas.view = nil
            videoCanvas.renderMode = .hidden
            agoraKit.setupRemoteVideo(videoCanvas)
            remoteVideo.uid = nil
        } else {
            LogUtils.log(message: "no matching video canvas for \(uid), cancel unbind", level: .warning)
        }
    }
    
    /// callback when a media relay process state changed
    /// @param state state of media relay
    /// @param error error details if media relay reaches failure state
    func rtcEngine(_ engine: AgoraRtcEngineKit, channelMediaRelayStateDidChange state: AgoraChannelMediaRelayState, error: AgoraChannelMediaRelayError) {
        LogUtils.log(message: "channelMediaRelayStateDidChange: \(state.rawValue) error \(error.rawValue)", level: .info)
        isProcessing = false
        switch state {
            case .running:
                isRelaying = true
            case .failure:
                showAlert(message: "Media Relay Failed: \(error.rawValue)")
                isRelaying = false
            case .idle:
                isRelaying = false
            default:break
        }
    }
    
    /// callback when a media relay event received
    /// @param event  event of media relay
    func rtcEngine(_ engine: AgoraRtcEngineKit, didReceive event: AgoraChannelMediaRelayEvent) {
        LogUtils.log(message: "didReceiveRelayEvent: \(event.rawValue)", level: .info)
    }
}
