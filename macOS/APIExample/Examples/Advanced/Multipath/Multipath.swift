import Cocoa
import AgoraRtcKit
import AGEVideoLayout

class MultipathMain: BaseViewController {
    
    var agoraKit: AgoraRtcEngineKit!
    var remoteUid: UInt = 0
    
    var videos: [VideoView] = []
    @IBOutlet weak var Container: AGEVideoContainer!
    
    let channelMediaOption = AgoraRtcChannelMediaOptions()
    
    /**
     --- Mode Select Picker ---
     */
    @IBOutlet weak var selectModePicker: Picker!
    func initSelectModePicker() {
        selectModePicker.label.stringValue = "Mode".localized
        selectModePicker.picker.addItems(withTitles: ["dynamic", "duplicate"])
        selectModePicker.picker.selectItem(at: 0)
    }
    
    /**
     --- Role Picker ---
     */
    @IBOutlet weak var selectRolePicker: Picker!
    private let roles = AgoraClientRole.allValues()
    var selectedRole: AgoraClientRole? {
        let index = self.selectRolePicker.indexOfSelectedItem
        if index >= 0 && index < roles.count {
            return roles[index]
        } else {
            return nil
        }
    }
    func initSelectRolePicker() {
        selectRolePicker.label.stringValue = "Role".localized
        selectRolePicker.picker.addItems(withTitles: roles.map { $0.description() })
        selectRolePicker.onSelectChanged { [weak self] in
            guard let self = self, let selected = self.selectedRole else { return }
            if self.isJoined {
                let mediaOption = AgoraRtcChannelMediaOptions()
                mediaOption.publishCameraTrack = selected == .broadcaster
                mediaOption.publishMicrophoneTrack = selected == .broadcaster
                mediaOption.clientRoleType = selected
                self.agoraKit.updateChannel(with: mediaOption)
                self.agoraKit.setClientRole(selected)
                _ = selected == .broadcaster ? self.agoraKit.startPreview() : self.agoraKit.stopPreview()
            }
        }
    }
    
    /**
     --- Channel TextField ---
     */
    @IBOutlet weak var channelField: Input!
    func initChannelField() {
        channelField.label.stringValue = "Channel".localized
        channelField.field.placeholderString = "Channel Name".localized
    }
    
    /**
     --- Button ---
     */
    @IBOutlet weak var joinChannelButton: NSButton!
    func initJoinChannelButton() {
        joinChannelButton.title = isJoined ? "Leave Channel".localized : "Join Channel".localized
    }
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false {
        didSet {
            channelField.isEnabled = !isJoined
            selectModePicker.isEnabled = !isJoined
            initJoinChannelButton()
        }
    }
    
    // indicate for doing something
    var isProcessing: Bool = false {
        didSet {
            joinChannelButton.isEnabled = !isProcessing
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do view setup here.
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit.enableVideo()
        
        initSelectModePicker()
        initSelectRolePicker()
        initChannelField()
        initJoinChannelButton()
        layoutVideos(2)
        remoteUid = 0
    }

    func layoutVideos(_ count: Int) {
        videos = []
        for i in 0...count - 1 {
            let view = VideoView.createFromNib()!
            if(i == 0) {
                view.placeholder.stringValue = "Local"
                view.type = .local
                view.statsInfo = StatisticsInfo(type: .local(StatisticsInfo.LocalInfo()))
            } else {
                view.placeholder.stringValue = "Remote \(i)"
                view.type = .remote
                view.statsInfo = StatisticsInfo(type: .remote(StatisticsInfo.RemoteInfo()))
            }
            videos.append(view)
        }
        // layout render view
        Container.layoutStream(views: videos)
    }
    
    @IBAction func onVideoCallButtonPressed(_ sender: NSButton) {
        if !isJoined {
            // check configuration
            let channel = channelField.stringValue
            if channel.isEmpty {
                return
            }
            
            guard let cameraId = agoraKit.enumerateDevices(.videoCapture)?.first?.deviceId,
                  let micId = agoraKit.enumerateDevices(.audioRecording)?.first?.deviceId,
                  let role = selectedRole else {
                return
            }
            // set proxy configuration
//            let proxySetting = GlobalSettings.shared.proxySetting.selectedOption().value
//            agoraKit.setCloudProxy(AgoraCloudProxyType.init(rawValue: UInt(proxySetting)) ?? .noneProxy)
            
            
            agoraKit.setDevice(.videoCapture, deviceId: cameraId)
            agoraKit.setDevice(.audioRecording, deviceId: micId)
            // set myself as broadcaster to stream video/audio
            agoraKit.setClientRole(role)
            let videoConfig = AgoraVideoEncoderConfiguration()
            videoConfig.dimensions = CGSize(width: 640, height: 360)
            videoConfig.frameRate = .fps15
            videoConfig.bitrate = AgoraVideoBitrateStandard
            videoConfig.orientationMode = .adaptative
            videoConfig.mirrorMode = .auto
            agoraKit.setVideoEncoderConfiguration(videoConfig)
            
            // set up local video to render your local camera preview
            let localVideo = videos[0]
            let videoCanvas = AgoraRtcVideoCanvas()
            videoCanvas.uid = 0
            // the view to be binded
            videoCanvas.view = localVideo.videocanvas
            videoCanvas.renderMode = .hidden
            agoraKit.setupLocalVideo(videoCanvas)
            // you have to call startPreview to see local video
            if role == .broadcaster {
                agoraKit.startPreview()
            } else {
                agoraKit.stopPreview()
            }
            
            // start joining channel
            // 1. Users can only see each other after they join the
            // same channel successfully using the same app id.
            // 2. If app certificate is turned on at dashboard, token is needed
            // when joining channel. The channel name and uid used to calculate
            // the token has to match the ones used for channel join
            isProcessing = true
            channelMediaOption.publishCameraTrack = role == .broadcaster
            channelMediaOption.publishMicrophoneTrack = role == .broadcaster
            channelMediaOption.clientRoleType = role
            channelMediaOption.enableMultipath = true
            channelMediaOption.uplinkMultipathMode = (selectModePicker.picker.indexOfSelectedItem == 0) ? .dynamic : .duplicate
            channelMediaOption.downlinkMultipathMode = (selectModePicker.picker.indexOfSelectedItem == 0) ? .dynamic : .duplicate
            channelMediaOption.autoSubscribeVideo = true
            channelMediaOption.autoSubscribeAudio = true
            NetworkManager.shared.generateToken(channelName: channel, success: { token in
                let result = self.agoraKit.joinChannel(byToken: token, channelId: channel, uid: 0, mediaOptions: self.channelMediaOption)
                if result != 0 {
                    self.isProcessing = false
                    // Usually happens with invalid parameters
                    // Error code description can be found at:
                    // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                    // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
                    self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
                }
            })
        } else {
            isProcessing = true
            let videoCanvas = AgoraRtcVideoCanvas()
            videoCanvas.uid = 0
            // the view to be binded
            videoCanvas.view = nil
            videoCanvas.renderMode = .hidden
            agoraKit.setupLocalVideo(videoCanvas)
            agoraKit.leaveChannel { (stats:AgoraChannelStats) in
                LogUtils.log(message: "Left channel", level: .info)
                self.isProcessing = false
                self.videos[0].uid = nil
                self.isJoined = false
                self.videos.forEach {
                    $0.uid = nil
                    $0.statsLabel.stringValue = ""
                }
            }
        }
    }
    
    override func viewWillBeRemovedFromSplitView() {
        if isJoined {
            agoraKit.disableVideo()
            agoraKit.leaveChannel { (stats:AgoraChannelStats) in
                LogUtils.log(message: "Left channel", level: .info)
            }
        }
        AgoraRtcEngineKit.destroy()
    }
    
    @IBAction func onClickMultipathSwitch(_ sender: NSSwitch) {
        let isOn = (sender.state == .on)
        channelMediaOption.enableMultipath = isOn
        let ret = agoraKit.updateChannel(with: channelMediaOption)
        if isOn {
            videos[0].statsInfo?.updateMultipathStats(nil)
        }
        LogUtils.log(message: "updateChannel Multipath ret: \(ret) isOn: \(isOn)", level: .info)
    }
}

/// agora rtc engine delegate events
extension MultipathMain: AgoraRtcEngineDelegate {
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
        if self.isProcessing {
            self.isProcessing = false
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
            remoteUid = uid
        } else {
            LogUtils.log(message: "no video canvas available for \(uid), cancel bind", level: .warning)
            remoteUid = 0
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
        remoteUid = 0
    }
    
    /// Reports the statistics of the current call. The SDK triggers this callback once every two seconds after the user joins the channel.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, reportRtcStats stats: AgoraChannelStats) {
        videos[0].statsInfo?.updateChannelStats(stats)
    }
    
    /// Reports the statistics of the uploading local video streams once every two seconds.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, localVideoStats stats: AgoraRtcLocalVideoStats, sourceType:AgoraVideoSourceType) {
        videos[0].statsInfo?.updateLocalVideoStats(stats)
    }
    
    /// Reports the statistics of the uploading local audio streams once every two seconds.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, localAudioStats stats: AgoraRtcLocalAudioStats) {
        videos[0].statsInfo?.updateLocalAudioStats(stats)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, multiPathStats stats: AgoraMultipathStats) {
        videos[0].statsInfo?.updateMultipathStats(stats)
    }
    
    /// Reports the statistics of the video stream from each remote user/host.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteVideoStats stats: AgoraRtcRemoteVideoStats) {
        videos.first(where: { $0.uid == stats.uid })?.statsInfo?.updateVideoStats(stats)
    }
    
    /// Reports the statistics of the audio stream from each remote user/host.
    /// @param stats stats struct for current call statistics
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteAudioStats stats: AgoraRtcRemoteAudioStats) {
        videos.first(where: { $0.uid == stats.uid })?.statsInfo?.updateAudioStats(stats)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, localVideoStateChangedOf state: AgoraVideoLocalState, reason: AgoraLocalVideoStreamReason, sourceType: AgoraVideoSourceType) {
        LogUtils.log(message: "AgoraRtcEngineKit state: \(state), error \(reason.rawValue)", level: .info)
    }
}
