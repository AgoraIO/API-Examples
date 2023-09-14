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

class LiveStreamingMain: BaseViewController {
    
    var agoraKit: AgoraRtcEngineKit!
    var remoteUid: UInt = 0 {
        didSet {
            snapShot.isEnabled = remoteUid != 0
        }
    }
    var isPreloadChannel: Bool = false {
        didSet {
            preloadButton.title =  isPreloadChannel ? "cancel preload".localized : "preload Channel".localized
        }
    }
    
    var videos: [VideoView] = []
    @IBOutlet weak var Container: AGEVideoContainer!
    
    /**
     --- Cameras Picker ---
     */
    @IBOutlet weak var selectCameraPicker: Picker!
    var cameras: [AgoraRtcDeviceInfo] = [] {
        didSet {
            DispatchQueue.main.async {
                self.selectCameraPicker.picker.addItems(withTitles: self.cameras.map {$0.deviceName ?? "unknown"})
            }
        }
    }
    var selectedCamera: AgoraRtcDeviceInfo? {
        let index = selectCameraPicker.indexOfSelectedItem
        if index >= 0 && index < cameras.count {
            return cameras[index]
        } else {
            return nil
        }
    }
    func initSelectCameraPicker() {
        selectCameraPicker.label.stringValue = "Camera".localized
        // find device in a separate thread to avoid blocking main thread
        let queue = DispatchQueue(label: "device.enumerateDevices")
        queue.async {[unowned self] in
            self.cameras = self.agoraKit.enumerateDevices(.videoCapture) ?? []
        }
        
        selectCameraPicker.onSelectChanged {
            if !self.isJoined {
                return
            }
            // use selected devices
            guard let cameraId = self.selectedCamera?.deviceId else {
                return
            }
            self.agoraKit.setDevice(.videoCapture, deviceId: cameraId)
        }
    }
    
    /**
     --- Resolutions Picker ---
     */
    @IBOutlet weak var selectResolutionPicker: Picker!
    var selectedResolution: Resolution? {
        let index = self.selectResolutionPicker.indexOfSelectedItem
        if index >= 0 && index < Configs.Resolutions.count {
            return Configs.Resolutions[index]
        } else {
            return nil
        }
    }
    func initSelectResolutionPicker() {
        selectResolutionPicker.label.stringValue = "Resolution".localized
        selectResolutionPicker.picker.addItems(withTitles: Configs.Resolutions.map { $0.name() })
        selectResolutionPicker.picker.selectItem(at: GlobalSettings.shared.resolutionSetting.selectedOption().value)
        
        selectResolutionPicker.onSelectChanged {
            if !self.isJoined {
                return
            }
            
            guard let resolution = self.selectedResolution,
                  let fps = self.selectedFps else {
                return
            }
            self.agoraKit.setVideoEncoderConfiguration(
                AgoraVideoEncoderConfiguration(
                    size: resolution.size(),
                    frameRate: AgoraVideoFrameRate(rawValue: fps) ?? .fps15,
                    bitrate: AgoraVideoBitrateStandard,
                    orientationMode: .adaptative,
                    mirrorMode: AgoraVideoMirrorMode.auto
                )
            )
        }
    }
    
    /**
     --- Fps Picker ---
     */
    @IBOutlet weak var selectFpsPicker: Picker!
    var selectedFps: Int? {
        let index = self.selectFpsPicker.indexOfSelectedItem
        if index >= 0 && index < Configs.Fps.count {
            return Configs.Fps[index]
        } else {
            return nil
        }
    }
    func initSelectFpsPicker() {
        selectFpsPicker.label.stringValue = "Frame Rate".localized
        selectFpsPicker.picker.addItems(withTitles: Configs.Fps.map { "\($0)fps" })
        selectFpsPicker.picker.selectItem(at: GlobalSettings.shared.fpsSetting.selectedOption().value)
        
        selectFpsPicker.onSelectChanged {
            if !self.isJoined {
                return
            }
            
            guard let resolution = self.selectedResolution,
                  let fps = self.selectedFps else {
                return
            }
            self.agoraKit.setVideoEncoderConfiguration(
                AgoraVideoEncoderConfiguration(
                    size: resolution.size(),
                    frameRate: AgoraVideoFrameRate(rawValue: fps) ?? .fps15,
                    bitrate: AgoraVideoBitrateStandard,
                    orientationMode: .adaptative,
                    mirrorMode: .auto
                )
            )
        }
    }
    
    @IBOutlet weak var selectCanvasColor: Picker!
    private var remoteCanvasColor: UInt32 = 0xff006aff
    func initSelectCanvasColor() {
        selectCanvasColor.label.stringValue = "默认背景色"
        let colors: [String: UInt32] = ["Red".localized: 0xff0d00ff,
                                        "Blue".localized: 0x0400ffff,
                                        "Pink".localized: 0xff006aff,
                                        "Purple".localized: 0xff00d9ff,
                                        "Yellow".localized: 0xeaff00ff]
        let sortColors = colors.map({ $0.key }).sorted()
        selectCanvasColor.picker.addItems(withTitles: sortColors)
        selectCanvasColor.onSelectChanged { [weak self] in
            guard let self = self else { return }
            let index = self.selectCanvasColor.indexOfSelectedItem
            let colorTitle = sortColors[index]
            self.remoteCanvasColor = colors[colorTitle] ?? 0xff006aff
        }
    }
    /**
     --- Microphones Picker ---
     */
    @IBOutlet weak var selectMicsPicker: Picker!
    var mics: [AgoraRtcDeviceInfo] = [] {
        didSet {
            DispatchQueue.main.async {
                self.selectMicsPicker.picker.addItems(withTitles: self.mics.map {$0.deviceName ?? "unknown"})
            }
        }
    }
    var selectedMicrophone: AgoraRtcDeviceInfo? {
        let index = self.selectMicsPicker.indexOfSelectedItem
        if index >= 0 && index < mics.count {
            return mics[index]
        } else {
            return nil
        }
    }
    func initSelectMicsPicker() {
        selectMicsPicker.label.stringValue = "Microphone".localized
        // find device in a separate thread to avoid blocking main thread
        let queue = DispatchQueue(label: "device.enumerateDevices")
        queue.async {[unowned self] in
            self.mics = self.agoraKit.enumerateDevices(.audioRecording) ?? []
        }
        
        selectMicsPicker.onSelectChanged {
            if !self.isJoined {
                return
            }
            // use selected devices
            guard let micId = self.selectedMicrophone?.deviceId else {
                return
            }
            self.agoraKit.setDevice(.audioRecording, deviceId: micId)
        }
    }
    
    /**
     --- Layout Picker ---
     */
    @IBOutlet weak var selectLayoutPicker: Picker!
    let layouts = [Layout("1v1", 2), Layout("1v3", 4), Layout("1v8", 9), Layout("1v15", 16)]
    var selectedLayout: Layout? {
        let index = self.selectLayoutPicker.indexOfSelectedItem
        if index >= 0 && index < layouts.count {
            return layouts[index]
        } else {
            return nil
        }
    }
    func initSelectLayoutPicker() {
        layoutVideos(2)
        selectLayoutPicker.label.stringValue = "Layout".localized
        selectLayoutPicker.picker.addItems(withTitles: layouts.map { $0.label })
        selectLayoutPicker.onSelectChanged {
            if self.isJoined {
                return
            }
            guard let layout = self.selectedLayout else { return }
            self.layoutVideos(layout.value)
        }
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
        selectRolePicker.onSelectChanged {
            guard let selected = self.selectedRole else { return }
            if self.isJoined {
                let mediaOption = AgoraRtcChannelMediaOptions()
                mediaOption.publishCameraTrack = selected == .broadcaster
                mediaOption.publishMicrophoneTrack = selected == .broadcaster
                mediaOption.clientRoleType = selected
                self.agoraKit.updateChannel(with: mediaOption)
                self.agoraKit.setClientRole(selected)
                _ = selected == .broadcaster ? self.agoraKit.startPreview() : self.agoraKit.stopPreview()
            }
            self.waterMarkContainer.isHidden = selected == .audience
            self.bFrameContainer.isHidden = selected == .audience
            self.encoderSegment.isHidden = selected == .audience
        }
    }
    @IBOutlet weak var snapShot: NSButton!
    @IBAction func onTakeSnapshot(_ sender: Any) {
        let filePath = FileManager.default.urls(for: .downloadsDirectory, in: .userDomainMask).first?.absoluteString
        let programPath = filePath?.components(separatedBy: "/")[4] ?? ""
        let path = "/Users/\(programPath)/Downloads/1.png"
        agoraKit.takeSnapshot(Int(remoteUid), filePath: path)
    }
    
    @IBOutlet weak var waterMarkContainer: NSView!
    @IBAction func onWaterMark(_ sender: NSSwitch) {
        if sender.state == .on {
            if let filepath = Bundle.main.path(forResource: "agora-logo", ofType: "png") {
                if let url = URL(string: filepath) {
                    let waterMark = WatermarkOptions()
                    waterMark.visibleInPreview = true
                    let localVideo = self.videos[0]
                    waterMark.positionInPortraitMode = localVideo.frame.offsetBy(dx: 20, dy: 20)
                    waterMark.positionInLandscapeMode = localVideo.frame.offsetBy(dx: 20, dy: 20)
                    agoraKit.addVideoWatermark(url, options: waterMark)
                }
            }
        } else {
            agoraKit.clearVideoWatermarks()
        }
    }
    
    @IBOutlet weak var dualStreamTips: NSTextField!
    @IBAction func onDualStreaming(_ sender: NSSwitch) {
        dualStreamTips.stringValue = sender.state == .on ? "Opening".localized : "(Default: flow)".localized
        agoraKit.enableDualStreamMode(sender.state == .on)
    }
    
    @IBOutlet weak var firstFrameSwitch: NSSwitch!
    @IBAction func onFirstFrameSwitch(_ sender: NSSwitch) {
        if sender.state == .on {
            let alertVC = NSAlert()
            alertVC.alertStyle = .critical
            alertVC.addButton(withTitle: "Sure".localized)
            alertVC.addButton(withTitle: "Cancel".localized)
            alertVC.messageText = "After this function is enabled, it cannot be disabled and takes effect only when both the primary and secondary ends are enabled".localized
            let response = alertVC.runModal()
            if response == .alertFirstButtonReturn {
                sender.isEnabled = false
                agoraKit.enableInstantMediaRendering()
                agoraKit.startMediaRenderingTracing()
            }
            if response == .alertSecondButtonReturn {
                sender.state = .off
            }
        }
    }
    
    @IBOutlet weak var bFrameContainer: NSView!
    @IBAction func bFrameSwitch(_ sender: NSSwitch) {
        let encoderConfig = AgoraVideoEncoderConfiguration()
        let videoOptions = AgoraAdvancedVideoOptions()
        videoOptions.compressionPreference = sender.state == .on ? .quality : .lowLatency
        encoderConfig.advancedVideoOptions = videoOptions
        agoraKit.setVideoEncoderConfiguration(encoderConfig)
    }
    
    @IBOutlet weak var encoderSegment: NSSegmentedControl!
    @IBAction func onTapEncoderSegment(_ sender: NSSegmentedControl) {
        let encoderConfig = AgoraVideoEncoderConfiguration()
        let advancedOptions = AgoraAdvancedVideoOptions()
        print(sender.indexOfSelectedItem)
        switch sender.indexOfSelectedItem {
        case 0:
            advancedOptions.encodingPreference = .preferAuto
            
        case 1:
            advancedOptions.encodingPreference = .prefersoftware
            
        case 2:
            advancedOptions.encodingPreference = .preferhardware
            
        default: break
        }
        encoderConfig.advancedVideoOptions = advancedOptions
        agoraKit.setVideoEncoderConfiguration(encoderConfig)
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
            selectLayoutPicker.isEnabled = !isJoined
            firstFrameSwitch.isEnabled = !isJoined
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
        
        initSelectCameraPicker()
        initSelectResolutionPicker()
        initSelectFpsPicker()
        initSelectMicsPicker()
        initSelectLayoutPicker()
        initSelectRolePicker()
        initChannelField()
        initJoinChannelButton()
        initSelectCanvasColor()
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
    
    @IBOutlet weak var preloadButton: NSButton!
    @IBAction func onPreloadButton(_ sender: NSButton) {
        isPreloadChannel = !isPreloadChannel
    }
    
    @IBAction func onVideoCallButtonPressed(_ sender: NSButton) {
        if !isJoined {
            // check configuration
            let channel = channelField.stringValue
            if channel.isEmpty {
                return
            }
            guard let cameraId = selectedCamera?.deviceId,
                  let resolution = selectedResolution,
                  let micId = selectedMicrophone?.deviceId,
                  let role = selectedRole,
                  let fps = selectedFps else {
                return
            }
            
            // set proxy configuration
//            let proxySetting = GlobalSettings.shared.proxySetting.selectedOption().value
//            agoraKit.setCloudProxy(AgoraCloudProxyType.init(rawValue: UInt(proxySetting)) ?? .noneProxy)
            
            agoraKit.setDevice(.videoCapture, deviceId: cameraId)
            agoraKit.setDevice(.audioRecording, deviceId: micId)
            // set myself as broadcaster to stream video/audio
            agoraKit.setClientRole(role)
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
            if role == .broadcaster {
                agoraKit.startPreview()
            }
            
            // start joining channel
            // 1. Users can only see each other after they join the
            // same channel successfully using the same app id.
            // 2. If app certificate is turned on at dashboard, token is needed
            // when joining channel. The channel name and uid used to calculate
            // the token has to match the ones used for channel join
            isProcessing = true
            let option = AgoraRtcChannelMediaOptions()
            option.publishCameraTrack = role == .broadcaster
            option.publishMicrophoneTrack = role == .broadcaster
            option.clientRoleType = role
            NetworkManager.shared.generateToken(channelName: channel, success: { token in
                if self.isPreloadChannel {
                    self.agoraKit.preloadChannel(byToken: token, channelId: channel, uid: 0)
                }
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
}

/// agora rtc engine delegate events
extension LiveStreamingMain: AgoraRtcEngineDelegate {
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
            videoCanvas.renderMode = .fit
            videoCanvas.backgroundColor = remoteCanvasColor
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
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, localVideoStateChangedOf state: AgoraVideoLocalState, error: AgoraLocalVideoStreamError, sourceType:AgoraVideoSourceType) {
        LogUtils.log(message: "AgoraRtcEngineKit state: \(state), error \(error)", level: .info)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, videoRenderingTracingResultOfUid uid: UInt, currentEvent: AgoraMediaTraceEvent, tracingInfo: AgoraVideoRenderingTracingInfo) {
        videos.first(where: { $0.uid == uid })?.statsInfo?.updateFirstFrameInfo(tracingInfo)
    }
}
