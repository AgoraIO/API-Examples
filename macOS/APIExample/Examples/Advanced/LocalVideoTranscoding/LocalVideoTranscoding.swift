//
//  LocalVideoTranscoding.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//
import Cocoa
import AgoraRtcKit
import AGEVideoLayout

class LocalVideoTranscoding: BaseViewController {
    var videos: [VideoView] = []
    
    @IBOutlet weak var container: AGEVideoContainer!
    @IBOutlet weak var virtualBackgroundSwitch: NSSwitch!
    
    var agoraKit: AgoraRtcEngineKit!

    func initSelectLayoutPicker() {
        layoutVideos(2)
    }
    
    var windowManager: WindowList = WindowList()
    var windowlist:[Window] = [], screenlist:[Window] = []
    /**
     --- Screen Picker ---
     */
    @IBOutlet weak var selectScreenPicker: Picker!
    var selectedScreen: Window? {
        let index = self.selectScreenPicker.indexOfSelectedItem
        if index >= 0 && index < screenlist.count {
            return screenlist[index]
        } else {
            return nil
        }
    }
    func initSelectScreenPicker() {
        screenlist = windowManager.items.filter({$0.type == .screen})
        selectScreenPicker.label.stringValue = "Screen Share".localized
        selectScreenPicker.picker.addItems(withTitles: screenlist.map {"\($0.name ?? "Unknown")(\($0.id))"})
    }
    var isScreenSharing: Bool = false {
        didSet {
            updateScreenShareButton()
        }
    }
    /**
     --- Screen Share Button ---
     */
    @IBOutlet weak var screenShareButton: NSButton!
    func updateScreenShareButton() {
        screenShareButton.isEnabled = isJoined
        screenShareButton.title = isScreenSharing ? "Stop Share".localized : "Display Share".localized
        screenThumbnailButton.isEnabled = isJoined
    }
    
    @IBOutlet weak var screenThumbnailButton: NSButton!
    @IBAction func onScreentThumbnailButton(_ sender: NSButton) {
        let result = agoraKit.getScreenCaptureSources(withThumbSize: NSScreen.main?.frame.size ?? .zero, iconSize: .zero, includeScreen: true)
        saveThumbnailToDesktop(result: result, type: .screen)
    }
    
    private func saveThumbnailToDesktop(result: [AgoraScreenCaptureSourceInfo]?, type: AgoraScreenCaptureSourceType) {
        let filePath = FileManager.default.urls(for: .downloadsDirectory, in: .userDomainMask).first?.absoluteString
        let programPath = filePath?.components(separatedBy: "/")[4] ?? ""
        let path = "/Users/\(programPath)/Downloads/thumbnail"
        try? FileManager.default.createDirectory(atPath: path, withIntermediateDirectories: true, attributes: nil)
        var isShowAlert: Bool = false
        result?.enumerated().forEach({ index,item in
            let url = "\(path)/\(index).png"
            guard item.type == type else { return }
            let isSucces = item.thumbImage.savePNG(to: URL(fileURLWithPath: url))
            let message = isSucces ? "Save successfully".localized : "Save failed".localized
            if isShowAlert == false {
                showAlert(message: message)
                isShowAlert = true
            }
        })
    }
    
    private func createFile() {
        //在桌面上创建一个文件
        let manager = FileManager.default
        let urlForDocument = manager.urls( for: .desktopDirectory,
                                              in:.userDomainMask)
        let url = urlForDocument[0]
        createFile(name:"test.txt", fileBaseUrl: url)
    }
    //根据文件名和路径创建文件
    func createFile(name:String, fileBaseUrl:URL){
        let manager = FileManager.default
        let file = fileBaseUrl.appendingPathComponent(name)
        let exist = manager.fileExists(atPath: file.path)
        if !exist {
            //在文件中随便写入一些内容
            let data = Data(base64Encoded:"aGVsbG8gd29ybGQ=" ,options:.ignoreUnknownCharacters)
            let createSuccess = manager.createFile(atPath: file.path, contents:data,attributes:nil)
            print("文件创建结果: \(createSuccess)")
        }
    }
    
    func startScreenSharing() {
        guard !isScreenSharing, let screen = selectedScreen else {
            return
        }
        
        let params = AgoraScreenCaptureParameters()
        params.highLightWidth = 5
        params.highLightColor = .green
        params.highLighted = true
        let result = agoraKit.startScreenCapture(byDisplayId: UInt32(screen.id), regionRect: .zero, captureParams: params)
        
        if result == 0 {
            isScreenSharing = true
        } else {
            showAlert(title: "Error", message: "startScreenCapture call failed: \(result), please check your params")
        }
    }
    
    @IBAction func onScreenShare(_ sender: NSButton) {
        if !isScreenSharing {
            startScreenSharing()
            updateLocalTranscoding()
        } else {
            agoraKit.stopScreenCapture()
            isScreenSharing = false
            updateLocalTranscoding()
        }
    }
    
    @IBAction func onVirturalBackground(_ sender: NSSwitch) {
        if sender.state == .on {
            if agoraKit.isFeatureAvailable(onDevice: .videoPreprocessVirtualBackground) {
                setVirtualBackground(enabled: true)
            } else {
                showAlert(message: "The feature is unavailable in the device!")
                sender.state = .off
            }
        } else {
            setVirtualBackground(enabled: false)
        }
    }
    
    func setVirtualBackground(enabled: Bool) {
        let backgroundSource = AgoraVirtualBackgroundSource()
        if enabled {
            backgroundSource.backgroundSourceType = .color
            backgroundSource.color = 0x000000
        } else {
            backgroundSource.backgroundSourceType = .none
        }
        let segmentationProperty = AgoraSegmentationProperty()
        let result = agoraKit.enableVirtualBackground(
            enabled,
            backData: backgroundSource,
            segData: segmentationProperty
        )
        if result != 0 {
            showAlert(title: "Error", message: "enableVirtualBackground failed: \(result)")
        }
    }
    
    private func getTranscodingConfiguration() -> AgoraLocalTranscoderConfiguration {
        let size = NSScreen.main?.visibleFrame.size ?? .zero
        let cameraSize = NSSize(width: 400, height: 300)
        let margin: CGFloat = 20
        
        let cameraStream = AgoraTranscodingVideoStream()
        cameraStream.rect = NSRect(
            origin: NSPoint(x: size.width - cameraSize.width - margin, y: size.height - cameraSize.height - margin),
            size: cameraSize
        )
        cameraStream.sourceType = .camera
        
        var inputStreams: [AgoraTranscodingVideoStream] = [cameraStream]
        
        if isScreenSharing {
            let screenStream = AgoraTranscodingVideoStream()
            screenStream.sourceType = .screen
            screenStream.rect = CGRect(origin: CGPoint(x: 0, y: 0), size: size)
            inputStreams.insert(screenStream, at: 0)
        }
        
        let config = AgoraLocalTranscoderConfiguration()
        config.videoInputStreams = inputStreams
        config.videoOutputConfiguration.dimensions = size
        
        return config
    }
    
    private func startLocalTranscoding() {
        let config = getTranscodingConfiguration()
        let result = agoraKit.startLocalVideoTranscoder(config)
        
        if result == 0 {
            let mediaOptions = AgoraRtcChannelMediaOptions()
            mediaOptions.publishTranscodedVideoTrack = true
            mediaOptions.publishCameraTrack = false
            agoraKit.updateChannel(with: mediaOptions)
        } else {
            showAlert(title: "Error", message: "startLocalVideoTranscoder failed: \(result)")
        }
    }
    
    private func updateLocalTranscoding() {
        let config = getTranscodingConfiguration()
        let result = agoraKit.updateLocalTranscoderConfiguration(config)
        
        if result != 0 {
            showAlert(title: "Error", message: "updateLocalTranscoderConfiguration failed: \(result)")
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
     --- Join Button ---
     */
    @IBOutlet weak var joinChannelButton: NSButton!
    func initJoinChannelButton() {
        joinChannelButton.title = isJoined ? "Leave Channel".localized : "Join Channel".localized
    }
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false {
        didSet {
            channelField.isEnabled = !isJoined
            initJoinChannelButton()
            screenShareButton.isEnabled = isJoined
            screenThumbnailButton.isEnabled = isJoined
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
        // prepare window manager and list
        windowManager.getList()
        // Do view setup here.
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit.enableVideo()
        
        initSelectLayoutPicker()
        initSelectScreenPicker()
        updateScreenShareButton()
        initChannelField()
        initJoinChannelButton()
    }
    
    override func viewWillBeRemovedFromSplitView() {
        if isJoined {
            agoraKit.leaveChannel { (stats:AgoraChannelStats) in
                LogUtils.log(message: "Left channel", level: .info)
            }
        }
        agoraKit.stopLocalVideoTranscoder()
        agoraKit.stopScreenCapture()
        AgoraRtcEngineKit.destroy()
    }
    
    @IBAction func onJoinPressed(_ sender:Any) {
        if !isJoined {
            let channel = channelField.stringValue
            if channel.isEmpty {
                return
            }
            agoraKit.setChannelProfile(.liveBroadcasting)
            agoraKit.setClientRole(.broadcaster)
            
            startScreenSharing()
            setupLocalPreview(transCoded: false)
            startLocalTranscoding()
            setupLocalPreview(transCoded: true)
            
            isProcessing = true
            let option = AgoraRtcChannelMediaOptions()
            option.publishCameraTrack = false
            option.publishScreenTrack = false
            option.clientRoleType = .broadcaster
            NetworkManager.shared.generateToken(channelName: channel, success: { token in
                let result = self.agoraKit.joinChannel(byToken: token, channelId: channel, uid: 0, mediaOptions: option)
                if result != 0 {
                    self.isProcessing = false
                    self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
                }
            })
        } else {
            isProcessing = true
            agoraKit.stopLocalVideoTranscoder()
            agoraKit.stopScreenCapture()
            agoraKit.stopCameraCapture(.camera)
            isScreenSharing = false
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
    
    func setupLocalPreview(transCoded: Bool) {
        let localVideo = videos[0]
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = 0
        videoCanvas.view = localVideo.videocanvas
        videoCanvas.renderMode = .fit
        videoCanvas.sourceType = transCoded ? .transCoded : .screen
        videoCanvas.mirrorMode = .disabled
        agoraKit.setupLocalVideo(videoCanvas)
        agoraKit.startPreview()
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
        container.layoutStream(views: videos)
    }
}

/// agora rtc engine delegate events
extension LocalVideoTranscoding: AgoraRtcEngineDelegate {
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
            videoCanvas.renderMode = .fit
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
            videoCanvas.renderMode = .fit
            agoraKit.setupRemoteVideo(videoCanvas)
            remoteVideo.uid = nil
        } else {
            LogUtils.log(message: "no matching video canvas for \(uid), cancel unbind", level: .warning)
        }
    }
    
}
