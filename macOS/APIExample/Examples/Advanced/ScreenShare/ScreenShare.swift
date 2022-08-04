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

class ScreenShare: BaseViewController {
    var videos: [VideoView] = []
    
    @IBOutlet weak var container: AGEVideoContainer!
    
    var agoraKit: AgoraRtcEngineKit!

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
            if self.isScreenSharing || self.isWindowSharing {
                let params = AgoraScreenCaptureParameters()
                params.frameRate = fps
                params.dimensions = resolution.size()
//                self.agoraKit.stopScreenCapture()
//                self.ag
            } else {
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
            if self.isScreenSharing || self.isWindowSharing {
                let params = AgoraScreenCaptureParameters()
                params.frameRate = fps
                params.dimensions = resolution.size()
//                self.agoraKit.update(params)
            } else {
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
    }
    
    /**
     --- DisplayHint Picker ---
     */
    @IBOutlet weak var selectDisplayHintPicker: Picker!
    var displayHints = ["Default", "Motion", "Detail"]
    func initSelectDisplayHintPicker() {
//        selectDisplayHintPicker.label.stringValue = "Display Hint".localized
//        selectDisplayHintPicker.picker.addItems(withTitles: displayHints)
//
//        selectDisplayHintPicker.onSelectChanged {
//            if !self.isJoined {
//                return
//            }
//            guard let displayHint = self.selectedDisplayHint else { return }
//            print("setScreenCapture")
//            self.agoraKit.setScreenCapture(displayHint)
//        }
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
            windowShareButton.isEnabled = !isScreenSharing
            initScreenShareButton()
            halfScreenShareButton.isEnabled = isScreenSharing
            windowThumbnailButton.isEnabled = !isScreenSharing
        }
    }
    /**
     --- Screen Share Button ---
     */
    @IBOutlet weak var screenShareButton: NSButton!
    func initScreenShareButton() {
        screenShareButton.isEnabled = isJoined
        screenShareButton.title = isScreenSharing ? "Stop Share".localized : "Display Share".localized
        screenThumbnailButton.isEnabled = isJoined
    }
    
    @IBOutlet weak var screenThumbnailButton: NSButton!
    @IBAction func onScreentThumbnailButton(_ sender: NSButton) {
        let result = agoraKit.getScreenCaptureSources(withThumbSize: NSScreen.main?.frame.size ?? .zero, iconSize: .zero, includeScreen: true)
        saveThumbnailToDesktop(result: result, type: .screen)
    }
    @IBOutlet weak var windowThumbnailButton: NSButton!
    @IBAction func onWindowThumbnailButton(_ sender: NSButton) {
        let result = agoraKit.getScreenCaptureSources(withThumbSize: selectedResolution?.size() ?? .zero, iconSize: .zero, includeScreen: true)
        saveThumbnailToDesktop(result: result, type: .window)
    }
    
    private func saveThumbnailToDesktop(result: [AgoraScreenCaptureSourceInfo]?, type: AgoraScreenCaptureSourceType) {
        let programPath = Bundle.main.executablePath?.components(separatedBy: "/")[2] ?? ""
        let path = "/Users/\(programPath)/Downloads/thumbnail"
        try? FileManager.default.createDirectory(atPath: path, withIntermediateDirectories: true, attributes: nil)
        result?.enumerated().forEach({ index,item in
            let url = "\(path)/\(index).png"
            guard item.type == type else { return }
            let isSucces = item.thumbImage.savePNG(to: URL(fileURLWithPath: url))
            let message = isSucces ? "Save successfully".localized : "Save failed".localized
            showAlert(message: message)
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
        print("文件: \(file)")
        let exist = manager.fileExists(atPath: file.path)
        if !exist {
            //在文件中随便写入一些内容
            let data = Data(base64Encoded:"aGVsbG8gd29ybGQ=" ,options:.ignoreUnknownCharacters)
            let createSuccess = manager.createFile(atPath: file.path, contents:data,attributes:nil)
            print("文件创建结果: \(createSuccess)")
        }
    }
    
    @IBAction func onScreenShare(_ sender: NSButton) {
        if !isScreenSharing {
            guard let resolution = self.selectedResolution,
                  let fps = self.selectedFps,
                  let screen = selectedScreen else {
                return
            }
            let params = AgoraScreenCaptureParameters()
            params.frameRate = fps
            params.dimensions = resolution.size()
            // 增加勾边功能
            params.highLightWidth = 5
            params.highLightColor = .green
            params.highLighted = true
            let result = agoraKit.startScreenCapture(byDisplayId: UInt32(screen.id), regionRect: .zero, captureParams: params)
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                self.showAlert(title: "Error", message: "startScreenCapture call failed: \(result), please check your params")
            } else {
                isScreenSharing = true
                let mediaOptions = AgoraRtcChannelMediaOptions()
                mediaOptions.publishCameraTrack = .of(false)
                mediaOptions.publishScreenTrack = .of(true)
                agoraKit.updateChannel(with: mediaOptions)
                agoraKit.startPreview()
                setupLocalPreview(isScreenSharing: true)
            }
        } else {
            agoraKit.stopScreenCapture()
            isScreenSharing = false
            let mediaOptions = AgoraRtcChannelMediaOptions()
            mediaOptions.publishCameraTrack = .of(true)
            mediaOptions.publishScreenTrack = .of(false)
            agoraKit.updateChannel(with: mediaOptions)
            agoraKit.startPreview()
            setupLocalPreview(isScreenSharing: false)
        }
    }

    /**
     --- Window Picker ---
     */
    @IBOutlet weak var selectWindowPicker: Picker!
    var selectedWindow: Window? {
        let index = self.selectWindowPicker.indexOfSelectedItem
        if index >= 0 && index < windowlist.count {
            return windowlist[index]
        } else {
            return nil
        }
    }
    func initSelectWindowPicker() {
        windowlist = windowManager.items.filter({$0.type == .window})
        selectWindowPicker.label.stringValue = "Window Share".localized
        selectWindowPicker.picker.addItems(withTitles: windowlist.map {"\($0.name ?? "Unknown")(\($0.id))"})
    }
    var isWindowSharing: Bool = false {
        didSet {
            screenShareButton.isEnabled = !isWindowSharing
            initWindowShareButton()
            halfScreenShareButton.isEnabled = isWindowSharing
            screenThumbnailButton.isEnabled = !isWindowSharing
        }
    }
    /**
     --- Window Share Button ---
     */
    @IBOutlet weak var windowShareButton: NSButton!
    func initWindowShareButton() {
        windowShareButton.isEnabled = isJoined
        windowShareButton.title = isWindowSharing ? "Stop Share".localized : "Window Share".localized
        windowThumbnailButton.isEnabled = isJoined
    }
    @IBAction func onWindowShare(_ sender: NSButton) {
        if !isWindowSharing {
            guard let resolution = self.selectedResolution,
                  let fps = self.selectedFps,
                  let window = selectedWindow else {
                return
            }
            let params = AgoraScreenCaptureParameters()
            params.frameRate = fps
            params.dimensions = resolution.size()
            // 增加勾边功能
            params.highLightWidth = 5
            params.highLightColor = .green
            params.highLighted = true

            let result = agoraKit.startScreenCapture(byWindowId: UInt32(window.id), regionRect: .zero, captureParams: params)
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                self.showAlert(title: "Error", message: "startScreenCapture call failed: \(result), please check your params")
            } else {
                isWindowSharing = true
                let mediaOptions = AgoraRtcChannelMediaOptions()
                mediaOptions.publishCameraTrack = .of(false)
                mediaOptions.publishScreenTrack = .of(true)
                agoraKit.updateChannel(with: mediaOptions)
                agoraKit.startPreview()
                setupLocalPreview(isScreenSharing: true)
            }
        } else {
            agoraKit.stopScreenCapture()
            isScreenSharing = false
            let mediaOptions = AgoraRtcChannelMediaOptions()
            mediaOptions.publishCameraTrack = .of(true)
            mediaOptions.publishScreenTrack = .of(false)
            agoraKit.updateChannel(with: mediaOptions)
            agoraKit.startPreview()
            isWindowSharing = false
            setupLocalPreview(isScreenSharing: false)
        }
    }

    /**
     --- Half Screen Share Button ---
     */
    @IBOutlet weak var halfScreenShareButton: NSButton!
    func initHalfScreenShareButton() {
        halfScreenShareButton.isEnabled = isJoined
        halfScreenShareButton.title = "Share Half Screen".localized
    }
    var toggleRegionalScreening = false
    @IBAction func onStartShareHalfScreen(_ sender: Any) {
        let rect = NSScreen.main?.frame
        let region = NSMakeRect(0, 0, !toggleRegionalScreening ? rect!.width/2 : rect!.width, !toggleRegionalScreening ? rect!.height/2 : rect!.height)
        agoraKit.updateScreenCaptureRegion(region)
        toggleRegionalScreening = !toggleRegionalScreening
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
            selectLayoutPicker.isEnabled = !isJoined
            initJoinChannelButton()
            screenShareButton.isEnabled = isJoined
            windowShareButton.isEnabled = isJoined
            halfScreenShareButton.isEnabled = isJoined
            screenThumbnailButton.isEnabled = isJoined
            windowThumbnailButton.isEnabled = isJoined
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
        agoraKit.enableVideo()
        
        initSelectResolutionPicker()
        initSelectFpsPicker()
        initSelectDisplayHintPicker()
        initSelectLayoutPicker()
        initSelectScreenPicker()
        initScreenShareButton()
        initSelectWindowPicker()
        initWindowShareButton()
        initHalfScreenShareButton()
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
            guard let resolution = selectedResolution,
                  let fps = selectedFps else {
                return
            }
            
            // set live broadcaster mode
            agoraKit.setChannelProfile(.liveBroadcasting)
            // set myself as broadcaster to stream video/audio
            agoraKit.setClientRole(.broadcaster)
            // set proxy configuration
//            let proxySetting = GlobalSettings.shared.proxySetting.selectedOption().value
//            agoraKit.setCloudProxy(AgoraCloudProxyType.init(rawValue: UInt(proxySetting)) ?? .noneProxy)
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
            setupLocalPreview(isScreenSharing: false)
            
            // start joining channel
            // 1. Users can only see each other after they join the
            // same channel successfully using the same app id.
            // 2. If app certificate is turned on at dashboard, token is needed
            // when joining channel. The channel name and uid used to calculate
            // the token has to match the ones used for channel join
            isProcessing = true
            let option = AgoraRtcChannelMediaOptions()
            option.publishCameraTrack = .of(true)
            option.clientRoleType = .of((Int32)(AgoraClientRole.broadcaster.rawValue))
            let result = agoraKit.joinChannel(byToken: KeyCenter.Token, channelId: channel, uid: 0, mediaOptions: option)
            if result != 0 {
                isProcessing = false
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
            }
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
    
    func setupLocalPreview(isScreenSharing: Bool) {
        // set up local video to render your local camera preview
        let localVideo = videos[0]
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = 0
        // the view to be binded
        videoCanvas.view = localVideo.videocanvas
        videoCanvas.renderMode = .hidden
        videoCanvas.sourceType = isScreenSharing ? .screen : .camera
        // changing sourceType need to set mirrorMode accordingly
        videoCanvas.mirrorMode = isScreenSharing ? .disabled : .enabled
        agoraKit.setupLocalVideo(videoCanvas)
        // you have to call startPreview to see local video
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
extension ScreenShare: AgoraRtcEngineDelegate {
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
    
}
