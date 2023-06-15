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

class MultiCameraSoureceMain: BaseViewController {
    
    var agoraKit: AgoraRtcEngineKit!
    
    var videos: [VideoView] = []
    @IBOutlet weak var Container: AGEVideoContainer!
    
    /**
     --- Cameras Picker ---
     */
    @IBOutlet weak var selectCameraPicker1: Picker!
    @IBOutlet weak var selectCameraPicker2: Picker!
    @IBOutlet weak var selectCameraPicker3: Picker!
    @IBOutlet weak var selectCameraPicker4: Picker!
    
    @IBOutlet weak var captureButton1: NSButton!
    @IBOutlet weak var publishStreamButton1: NSButton!
    @IBOutlet weak var captureButton2: NSButton!
    @IBOutlet weak var publishStreamButton2: NSButton!
    @IBOutlet weak var captureButton3: NSButton!
    @IBOutlet weak var publishStreamButton3: NSButton!
    @IBOutlet weak var captureButton4: NSButton!
    @IBOutlet weak var publishStreamButton4: NSButton!
    var isSelectedCaptureButton1: Bool = false {
        didSet {
            publishStreamButton1.isEnabled = isSelectedCaptureButton1
        }
    }
    var isSelectedCaptureButton2: Bool = false {
        didSet {
            publishStreamButton2.isEnabled = isSelectedCaptureButton2
        }
    }
    var isSelectedCaptureButton3: Bool = false {
        didSet {
            publishStreamButton3.isEnabled = isSelectedCaptureButton3
        }
    }
    var isSelectedCaptureButton4: Bool = false {
        didSet {
            publishStreamButton4.isEnabled = isSelectedCaptureButton4
        }
    }
    var isSelectedPublishButton1: Bool = false
    var isSelectedPublishButton2: Bool = false
    var isSelectedPublishButton3: Bool = false
    var isSelectedPublishButton4: Bool = false
    
    var cameras: [AgoraRtcDeviceInfo] = [] {
        didSet {
            DispatchQueue.main.async {
                self.selectCameraPicker1.picker.addItems(withTitles: self.cameras.map {$0.deviceName ?? "unknown"})
                self.selectCameraPicker2.picker.addItems(withTitles: self.cameras.map {$0.deviceName ?? "unknown"})
                self.selectCameraPicker3.picker.addItems(withTitles: self.cameras.map {$0.deviceName ?? "unknown"})
                self.selectCameraPicker4.picker.addItems(withTitles: self.cameras.map {$0.deviceName ?? "unknown"})
            }
        }
    }
    var selectedCamera1: AgoraRtcDeviceInfo? {
        let index = selectCameraPicker1.indexOfSelectedItem
        if index >= 0 && index < cameras.count {
            return cameras[index]
        } else {
            return nil
        }
    }
    var selectedCamera2: AgoraRtcDeviceInfo? {
        let index = selectCameraPicker2.indexOfSelectedItem
        if index >= 0 && index < cameras.count {
            return cameras[index]
        } else {
            return nil
        }
    }
    var selectedCamera3: AgoraRtcDeviceInfo? {
        let index = selectCameraPicker3.indexOfSelectedItem
        if index >= 0 && index < cameras.count {
            return cameras[index]
        } else {
            return nil
        }
    }
    var selectedCamera4: AgoraRtcDeviceInfo? {
        let index = selectCameraPicker4.indexOfSelectedItem
        if index >= 0 && index < cameras.count {
            return cameras[index]
        } else {
            return nil
        }
    }
    func initSelectCameraPicker() {
        selectCameraPicker1.label.stringValue = "Camera".localized + "\(1)"
        selectCameraPicker2.label.stringValue = "Camera".localized + "\(2)"
        selectCameraPicker3.label.stringValue = "Camera".localized + "\(3)"
        selectCameraPicker4.label.stringValue = "Camera".localized + "\(4)"
        // find device in a separate thread to avoid blocking main thread
        let queue = DispatchQueue(label: "device.enumerateDevices")
        queue.async {[unowned self] in
            self.cameras = self.agoraKit.enumerateDevices(.videoCapture) ?? []
        }
        selectCameraPicker1.onSelectChanged {
            if !self.isJoined {
                return
            }
            // use selected devices
            guard let cameraId = self.selectedCamera1?.deviceId else {
                return
            }
            self.agoraKit.setDevice(.videoCapture, deviceId: cameraId)
        }
        selectCameraPicker2.onSelectChanged {
            if !self.isJoined {
                return
            }
            // use selected devices
            guard let cameraId = self.selectedCamera2?.deviceId else {
                return
            }
            self.agoraKit.setDevice(.videoCapture, deviceId: cameraId)
        }
        selectCameraPicker3.onSelectChanged {
            if !self.isJoined {
                return
            }
            // use selected devices
            guard let cameraId = self.selectedCamera3?.deviceId else {
                return
            }
            self.agoraKit.setDevice(.videoCapture, deviceId: cameraId)
        }
        selectCameraPicker4.onSelectChanged {
            if !self.isJoined {
                return
            }
            // use selected devices
            guard let cameraId = self.selectedCamera4?.deviceId else {
                return
            }
            self.agoraKit.setDevice(.videoCapture, deviceId: cameraId)
        }
    }
    
    @IBAction func onCaptureButton1(_ sender: NSButton) {
        guard let cameraId = selectedCamera1?.deviceId  else {
            return
        }
        isSelectedCaptureButton1 = !isSelectedCaptureButton1
        sender.title = isSelectedCaptureButton1 ? "Stop Capture".localized : "Start Capture".localized
        agoraKit.setDevice(.videoCapture, deviceId: cameraId)
        agoraKit.setClientRole(.broadcaster)
        let localVideo = videos[0]
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.renderMode = .hidden
        videoCanvas.uid = 0
        if isSelectedCaptureButton1 {
            videoCanvas.view = localVideo.videocanvas
            agoraKit.startPreview()
        } else {
            videoCanvas.view = nil
            agoraKit.stopPreview()
            let mediaOption = AgoraRtcChannelMediaOptions()
            mediaOption.publishCameraTrack = false
            mediaOption.publishMicrophoneTrack = false
            agoraKit.updateChannel(with: mediaOption)
            publishStreamButton1.title = "Start Publish".localized
        }
        agoraKit.setupLocalVideo(videoCanvas)
    }
    @IBAction func onPublishStreamButton1(_ sender: NSButton) {
        isSelectedPublishButton1 = !isSelectedPublishButton1
        sender.title = isSelectedPublishButton1 ? "Stop Publish".localized : "Start Publish".localized
        let mediaOption = AgoraRtcChannelMediaOptions()
        mediaOption.publishCameraTrack = isSelectedPublishButton1
        mediaOption.publishMicrophoneTrack = isSelectedPublishButton1
        mediaOption.publishSecondaryCameraTrack = false
        mediaOption.publishThirdCameraTrack = false
        mediaOption.publishFourthCameraTrack = false
        agoraKit.updateChannel(with: mediaOption)
    }
    @IBAction func onCaptureButton2(_ sender: NSButton) {
        guard let cameraId = selectedCamera2?.deviceId  else {
            return
        }
        isSelectedCaptureButton2 = !isSelectedCaptureButton2
        sender.title = isSelectedCaptureButton2 ? "Stop Capture".localized : "Start Capture".localized
        agoraKit.setDevice(.videoCapture, deviceId: cameraId)
        agoraKit.setClientRole(.broadcaster)
        let localVideo = videos[1]
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.renderMode = .hidden
        videoCanvas.uid = 0
        if isSelectedCaptureButton2 {
            videoCanvas.view = localVideo.videocanvas
            agoraKit.startPreview()
        } else {
            videoCanvas.view = nil
            agoraKit.stopPreview()
            let mediaOption = AgoraRtcChannelMediaOptions()
            mediaOption.publishCameraTrack = false
            mediaOption.publishMicrophoneTrack = false
            agoraKit.updateChannel(with: mediaOption)
            publishStreamButton2.title = "Start Publish".localized
        }
        agoraKit.setupLocalVideo(videoCanvas)
    }
    @IBAction func onPublishStreamButton2(_ sender: NSButton) {
        isSelectedPublishButton2 = !isSelectedPublishButton2
        sender.title = isSelectedPublishButton2 ? "Stop Publish".localized : "Start Publish".localized
        let mediaOption = AgoraRtcChannelMediaOptions()
        mediaOption.publishCameraTrack = false
        mediaOption.publishMicrophoneTrack = isSelectedPublishButton2
        mediaOption.publishSecondaryCameraTrack = isSelectedPublishButton2
        mediaOption.publishThirdCameraTrack = false
        mediaOption.publishFourthCameraTrack = false
        agoraKit.updateChannel(with: mediaOption)
    }
    @IBAction func onCaptureButton3(_ sender: NSButton) {
        guard let cameraId = selectedCamera3?.deviceId  else {
            return
        }
        isSelectedCaptureButton3 = !isSelectedCaptureButton3
        sender.title = isSelectedCaptureButton3 ? "Stop Capture".localized : "Start Capture".localized
        agoraKit.setDevice(.videoCapture, deviceId: cameraId)
        agoraKit.setClientRole(.broadcaster)
        let localVideo = videos[2]
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.renderMode = .hidden
        videoCanvas.uid = 0
        if isSelectedCaptureButton3 {
            videoCanvas.view = localVideo.videocanvas
            agoraKit.startPreview()
        } else {
            videoCanvas.view = nil
            agoraKit.stopPreview()
            let mediaOption = AgoraRtcChannelMediaOptions()
            mediaOption.publishCameraTrack = false
            mediaOption.publishMicrophoneTrack = false
            agoraKit.updateChannel(with: mediaOption)
            publishStreamButton3.title = "Start Publish".localized
        }
        agoraKit.setupLocalVideo(videoCanvas)
    }
    @IBAction func onPublishStreamButton3(_ sender: NSButton) {
        isSelectedPublishButton3 = !isSelectedPublishButton3
        sender.title = isSelectedPublishButton3 ? "Stop Publish".localized : "Start Publish".localized
        let mediaOption = AgoraRtcChannelMediaOptions()
        mediaOption.publishCameraTrack = false
        mediaOption.publishMicrophoneTrack = isSelectedPublishButton3
        mediaOption.publishSecondaryCameraTrack = false
        mediaOption.publishThirdCameraTrack = isSelectedPublishButton3
        mediaOption.publishFourthCameraTrack = false
        agoraKit.updateChannel(with: mediaOption)
    }
    @IBAction func onCaptureButton4(_ sender: NSButton) {
        guard let cameraId = selectedCamera4?.deviceId  else {
            return
        }
        isSelectedCaptureButton4 = !isSelectedCaptureButton4
        sender.title = isSelectedCaptureButton4 ? "Stop Capture".localized : "Start Capture".localized
        agoraKit.setDevice(.videoCapture, deviceId: cameraId)
        agoraKit.setClientRole(.broadcaster)
        let localVideo = videos[3]
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.renderMode = .hidden
        videoCanvas.uid = 0
        if isSelectedCaptureButton4 {
            videoCanvas.view = localVideo.videocanvas
            agoraKit.startPreview()
        } else {
            videoCanvas.view = nil
            agoraKit.stopPreview()
            let mediaOption = AgoraRtcChannelMediaOptions()
            mediaOption.publishCameraTrack = false
            mediaOption.publishMicrophoneTrack = false
            agoraKit.updateChannel(with: mediaOption)
            publishStreamButton4.title = "Start Publish".localized
        }
        agoraKit.setupLocalVideo(videoCanvas)
    }
    @IBAction func onPublishStreamButton4(_ sender: NSButton) {
        isSelectedPublishButton4 = !isSelectedPublishButton4
        sender.title = isSelectedPublishButton4 ? "Stop Publish".localized : "Start Publish".localized
        let mediaOption = AgoraRtcChannelMediaOptions()
        mediaOption.publishCameraTrack = false
        mediaOption.publishMicrophoneTrack = isSelectedPublishButton4
        mediaOption.publishSecondaryCameraTrack = false
        mediaOption.publishThirdCameraTrack = false
        mediaOption.publishFourthCameraTrack = isSelectedPublishButton4
        agoraKit.updateChannel(with: mediaOption)
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
        captureButton1.isEnabled = isJoined
        captureButton2.isEnabled = isJoined
        captureButton3.isEnabled = isJoined
        captureButton4.isEnabled = isJoined
        selectCameraPicker1.isEnabled = isJoined
        selectCameraPicker2.isEnabled = isJoined
        selectCameraPicker3.isEnabled = isJoined
        selectCameraPicker4.isEnabled = isJoined
        if isJoined == false {
            setupText()
        } else {
            captureButton1.title = "Stop Capture".localized
        }
    }
    
    private func setupText() {
        captureButton1.title = "Start Capture".localized
        captureButton2.title = captureButton1.title
        captureButton3.title = captureButton1.title
        captureButton4.title = captureButton1.title
        publishStreamButton1.title = "Start Publish".localized
        publishStreamButton2.title = publishStreamButton1.title
        publishStreamButton3.title = publishStreamButton1.title
        publishStreamButton4.title = publishStreamButton1.title
    }
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false {
        didSet {
            channelField.isEnabled = !isJoined
            initJoinChannelButton()
            isSelectedCaptureButton1 = isJoined
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
        initChannelField()
        initJoinChannelButton()
        layoutVideos(4)
        setupText()
    }

    func layoutVideos(_ count: Int) {
        videos = []
        for i in 0...count - 1 {
            let view = VideoView.createFromNib()!
            view.placeholder.stringValue = "Camera \(i + 1)"
            view.type = .local
            view.statsInfo = StatisticsInfo(type: .local(StatisticsInfo.LocalInfo()))
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
            guard let cameraId = selectedCamera1?.deviceId  else {
                return
            }
            
            // set proxy configuration
//            let proxySetting = GlobalSettings.shared.proxySetting.selectedOption().value
//            agoraKit.setCloudProxy(AgoraCloudProxyType.init(rawValue: UInt(proxySetting)) ?? .noneProxy)
            
            agoraKit.setDevice(.videoCapture, deviceId: cameraId)
            // set myself as broadcaster to stream video/audio
            agoraKit.setClientRole(.broadcaster)
            
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
            
            // start joining channel
            // 1. Users can only see each other after they join the
            // same channel successfully using the same app id.
            // 2. If app certificate is turned on at dashboard, token is needed
            // when joining channel. The channel name and uid used to calculate
            // the token has to match the ones used for channel join
            isProcessing = true
            let option = AgoraRtcChannelMediaOptions()
            option.publishCameraTrack = false
            option.publishMicrophoneTrack = false
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
extension MultiCameraSoureceMain: AgoraRtcEngineDelegate {
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
    }
    
    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)
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
}
