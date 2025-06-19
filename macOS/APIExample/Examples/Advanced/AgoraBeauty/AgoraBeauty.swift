//
//  VideoProcess.swift
//  APIExample
//
//  Created by Arlin on 2022/1/19.
//  Copyright © 2022 Agora Corp. All rights reserved.
//

import Cocoa
import AgoraRtcKit
import AGEVideoLayout

class AgoraBeauty: BaseViewController {
    
    @IBOutlet weak var Container: AGEVideoContainer!
    @IBOutlet weak var selectResolutionPicker: Picker!
    @IBOutlet weak var selectFpsPicker: Picker!
    @IBOutlet weak var selectLayoutPicker: Picker!
    @IBOutlet weak var virtualBackgroundSwitch: NSSwitch!
    @IBOutlet weak var selectVirtualBackgroundPicker: Picker!
    @IBOutlet weak var channelField: Input!
    @IBOutlet weak var joinChannelButton: NSButton!
    
    @IBOutlet weak var beautySwitch: NSSwitch!
    @IBOutlet weak var lightenSlider: NSSlider!
    @IBOutlet weak var ruddySlider: NSSlider!
    @IBOutlet weak var sharpSlider: NSSlider!
    @IBOutlet weak var smoothingSlider: NSSlider!
    @IBOutlet weak var whiteningSlider: NSSlider?
    
    var videos: [VideoView] = []
    let layouts = [Layout("1v1", 2), Layout("1v3", 4), Layout("1v8", 9), Layout("1v15", 16)]
    let backgroundTypes = AgoraVirtualBackgroundSourceType.allValues()
    var agoraKit: AgoraRtcEngineKit!
    var beautyManager: AgoraBeautyManager!
    var beautifyOption = AgoraBeautyOptions()
    var skinProtect = 0.5
    var strength = 0.5
    var whintening = 0.5
    
    private var makeupParams = [String: Any]()
    private var enableFaceShape: Bool = false
    private lazy var faceshapeOption = AgoraFaceShapeBeautyOptions()
    private var beautyShapeParames = [String: Float]()
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false {
        didSet {
            channelField.isEnabled = !isJoined
            selectLayoutPicker.isEnabled = !isJoined
            joinChannelButton.title = isJoined ? "Leave Channel".localized : "Join Channel".localized
        }
    }
    
    // MARK: - LifeCycle
    override func viewDidLoad() {
        super.viewDidLoad()
        self.setupAgoraKit()
        self.setupUI()
    }
    
    func setupAgoraKit() {
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // enable filters
        agoraKit.enableExtension(withVendor: "agora_video_filters_clear_vision", extension: "clear_vision", enabled: true, sourceType: .primaryCamera)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit.setChannelProfile(.liveBroadcasting)
        agoraKit.setClientRole(.broadcaster)
        
        agoraKit.enableVideo()
        setupBeautyManager()
    }
    
    private func setupBeautyManager() {
        beautyManager = AgoraBeautyManager(agoraKit: agoraKit)
        beautyManager.beautyMakeupStyle = "default makeup style".localized
        beautyManager.makeUpEnable = false
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
    
    // MARK: - UI
    func setupUI() {
        channelField.label.stringValue = "Channel".localized
        channelField.field.placeholderString = "Channel Name".localized
        joinChannelButton.title = isJoined ? "Leave Channel".localized : "Join Channel".localized
        
        lightenSlider.floatValue = beautifyOption.lighteningLevel
        ruddySlider.floatValue = beautifyOption.rednessLevel
        sharpSlider.floatValue = beautifyOption.sharpnessLevel
        smoothingSlider.floatValue = beautifyOption.smoothnessLevel
        
        initSelectResolutionPicker()
        initSelectFpsPicker()
        initSelectLayoutPicker()
        initSelectBackgroundPicker()
    }
    
    @IBAction func onJoinButtonPressed(_ sender: NSButton) {
        if !isJoined {
            let channel = channelField.stringValue
            guard !channel.isEmpty,
                  let resolution = selectedResolution(),
                  let fps = selectedFps() else {
                      return
                  }
            
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
            
            setVirtualBackground()
            
            // start joining channel
            // 1. Users can only see each other after they join the
            // same channel successfully using the same app id.
            // 2. If app certificate is turned on at dashboard, token is needed
            // when joining channel. The channel name and uid used to calculate
            // the token has to match the ones used for channel join
            let option = AgoraRtcChannelMediaOptions()
            option.publishCameraTrack = true
            NetworkManager.shared.generateToken(channelName: channel, success: { token in
                let result = self.agoraKit.joinChannel(byToken: token, channelId: channel, uid: 0, mediaOptions: option)
                if result != 0 {
                    // Usually happens with invalid parameters
                    // Error code description can be found at:
                    // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                    // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
                    self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
                }
            })
            
        } else {
            agoraKit.stopPreview()
            agoraKit.leaveChannel { (stats:AgoraChannelStats) in
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
    
    @IBAction func onBeautySliderChange(_ sender: NSSwitch) {
        if sender.state == .on {
            if agoraKit.isFeatureAvailable(onDevice: .videoPreprocessBeauty) {
                agoraKit.setBeautyEffectOptions(sender.state == .on, options: beautifyOption)
            } else {
                showAlert(message: "The feature is unavailable in the device!")
            }
        } else {
            agoraKit.setBeautyEffectOptions(sender.state == .on, options: beautifyOption)
        }
    }
    
    @IBAction func onLightenSliderChange(_ sender: NSSlider) {
        beautyManager.lightness = sender.floatValue
    }
    
    @IBAction func onRuddySliderChange(_ sender: NSSlider) {
        beautyManager.redness = sender.floatValue
    }
    
    @IBAction func onSharpSliderChange(_ sender: NSSlider) {
        beautyManager.sharpness = sender.floatValue
    }
    
    @IBAction func onSmoothingSliderChange(_ sender: NSSlider) {
        beautyManager.smoothness = sender.floatValue
    }
    
    @IBAction func onWhinteningSliderChange(_ sender: NSSlider) {
        beautyManager.filterStrength = sender.floatValue
    }
    
    @IBAction func onVirtualBackgroundSwitchChange(_ sender: NSSwitch) {
        if sender.state == .on {
            if agoraKit.isFeatureAvailable(onDevice: .videoPreprocessVirtualBackground) {
                setVirtualBackground()
            } else {
                showAlert(message: "The feature is unavailable in the device!")
            }
        } else {
            setVirtualBackground()
        }
    }
    
    func setVirtualBackground(){
        let backgroundSource = AgoraVirtualBackgroundSource()
        backgroundSource.backgroundSourceType = selectedBackgroundType() ?? .img
        switch backgroundSource.backgroundSourceType {
        case .color:
            backgroundSource.color = 0xFFFFFF
            break
        case .img:
            if let resourcePath = Bundle.main.resourcePath {
                let imgPath = resourcePath + "/" + "bg.jpg"
                backgroundSource.source = imgPath
            }
            break
        case .blur:
            backgroundSource.blurDegree = .high
            break
            
        case .video:
            let videoPath = Bundle.main.path(forResource: "sample", ofType: "mov")
            backgroundSource.backgroundSourceType = .video
            backgroundSource.source = videoPath
            
        default:
            break
        }
        backgroundSource.backgroundSourceType = virtualBackgroundSwitch.state == .on ? backgroundSource.backgroundSourceType : .none
        agoraKit.enableVirtualBackground(virtualBackgroundSwitch.state == .on,
                                         backData: backgroundSource,
                                         segData: AgoraSegmentationProperty())
    }
    
    func initSelectBackgroundPicker() {
        selectVirtualBackgroundPicker.label.stringValue = "Virtual Background".localized
        selectVirtualBackgroundPicker.picker.addItems(withTitles: backgroundTypes.map { $0.description() })

        selectVirtualBackgroundPicker.onSelectChanged {
            guard self.selectedBackgroundType() != nil else { return }
            self.setVirtualBackground()
        }
    }
    
    func selectedBackgroundType() ->AgoraVirtualBackgroundSourceType? {
        let index = selectVirtualBackgroundPicker.indexOfSelectedItem
        if index >= 0 && index < backgroundTypes.count {
            return backgroundTypes[index]
        } else {
            return nil
        }
    }
    
    // MARK: Vedio Setting
    func initSelectResolutionPicker() {
        selectResolutionPicker.label.stringValue = "Resolution".localized
        selectResolutionPicker.picker.addItems(withTitles: Configs.Resolutions.map { $0.name() })
        selectResolutionPicker.picker.selectItem(at: GlobalSettings.shared.resolutionSetting.selectedOption().value)
        
        selectResolutionPicker.onSelectChanged {
            if !self.isJoined {
                return
            }
            
            guard let resolution = self.selectedResolution(),
                  let fps = self.selectedFps() else {
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
    
    func selectedResolution() -> Resolution? {
        let index = self.selectResolutionPicker.indexOfSelectedItem
        if index >= 0 && index < Configs.Resolutions.count {
            return Configs.Resolutions[index]
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
            
            guard let resolution = self.selectedResolution(),
                  let fps = self.selectedFps() else {
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
    
    func selectedFps() -> Int? {
        let index = self.selectFpsPicker.indexOfSelectedItem
        if index >= 0 && index < Configs.Fps.count {
            return Configs.Fps[index]
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
            guard let layout = self.selectedLayout() else { return }
            self.layoutVideos(layout.value)
        }
    }
    
    func selectedLayout() ->Layout? {
        let index = self.selectLayoutPicker.indexOfSelectedItem
        if index >= 0 && index < layouts.count {
            return layouts[index]
        } else {
            return nil
        }
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
}

/// agora rtc engine delegate events
extension AgoraBeauty: AgoraRtcEngineDelegate {
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
    
    /// callback when the local user joins a specified channel.
    /// @param channel
    /// @param uid uid of local user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
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
            videoCanvas.enableAlphaMask = true
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
    
    /// Reports the statistics of the current call. The SDK triggers this callback once every two seconds after the user joins the channel.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, reportRtcStats stats: AgoraChannelStats) {
        videos[0].statsInfo?.updateChannelStats(stats)
    }
    
    /// Reports the statistics of the uploading local video streams once every two seconds.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, localVideoStats stats: AgoraRtcLocalVideoStats, sourceType: AgoraVideoSourceType) {
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
    
    /// Reports the video background substitution success or failed.
    /// @param enabled whether background substitution is enabled.
    /// @param reason The reason of the background substitution callback. See [AgoraVideoBackgroundSourceStateReason](AgoraVideoBackgroundSourceStateReason).
    
//    func rtcEngine(_ engine: AgoraRtcEngineKit, virtualBackgroundSourceEnabled enabled: Bool, reason: AgoraVirtualBackgroundSourceStateReason) {
//        if reason != .vbsStateReasonSuccess {
//            LogUtils.log(message: "background substitution failed to enabled for \(reason.rawValue)", level: .warning)
//        }
//    }
}

private func findViewInSuperview(_ superview: NSView?, identifier: String) -> NSView? {
    guard let superview = superview else { return nil }
    
    for subview in superview.subviews {
        if subview.identifier?.rawValue == identifier {
            return subview
        }
    }
    
    return nil
}

// MARK: make up setting
extension AgoraBeauty {
    @IBAction func onShowMakeUpAction(_ button: NSButton) {
        let customAlertVC = NSViewController()
        customAlertVC.view.wantsLayer = true
        customAlertVC.view.layer?.backgroundColor = NSColor.black.cgColor

        let alertView = NSView()
        alertView.translatesAutoresizingMaskIntoConstraints = false
        alertView.wantsLayer = true

        customAlertVC.view.addSubview(alertView)

        NSLayoutConstraint.activate([
            alertView.centerXAnchor.constraint(equalTo: customAlertVC.view.centerXAnchor),
            alertView.centerYAnchor.constraint(equalTo: customAlertVC.view.centerYAnchor),
            alertView.widthAnchor.constraint(equalTo: customAlertVC.view.widthAnchor, constant: -20),
            alertView.heightAnchor.constraint(equalToConstant: 300)
        ])

        let scrollView = NSScrollView()
        scrollView.translatesAutoresizingMaskIntoConstraints = false
        alertView.addSubview(scrollView)

        NSLayoutConstraint.activate([
            scrollView.topAnchor.constraint(equalTo: alertView.topAnchor),
            scrollView.leadingAnchor.constraint(equalTo: alertView.leadingAnchor),
            scrollView.trailingAnchor.constraint(equalTo: alertView.trailingAnchor),
            scrollView.bottomAnchor.constraint(equalTo: alertView.bottomAnchor, constant: -50) 
        ])

        let contentView = NSStackView()
        contentView.translatesAutoresizingMaskIntoConstraints = false
        contentView.orientation = .vertical
        contentView.spacing = 10
        scrollView.documentView = contentView

        let makeupList = beautyManager.makeupList
        for i in 0..<makeupList.count {
            let label = NSTextField(labelWithString: makeupList[i]["name"] as? String ?? "none")
            label.translatesAutoresizingMaskIntoConstraints = false
            label.font = NSFont.systemFont(ofSize: 12)
            label.textColor = NSColor.black
            label.identifier = NSUserInterfaceItemIdentifier(rawValue: "\(i + 2000)")

            let horizontalStackView = NSStackView()
            horizontalStackView.orientation = .horizontal
//            horizontalStackView.alignment = NSLayoutConstraint.Attribute.center
            horizontalStackView.spacing = 10
            horizontalStackView.translatesAutoresizingMaskIntoConstraints = false

            horizontalStackView.addArrangedSubview(label)

            var valueView: NSView?
            let key = makeupList[i]["key"] as? String ?? ""
            let type = makeupList[i]["type"] as? String ?? ""
            if type == "slider" {
                let value = makeupList[i]["value"] as? [Float] ?? []
                let defaultStrengthValue = Double(getDefaultStrengthValueForKey(key: key))
                let sliderView = NSSlider()
                label.stringValue = String(format: "%@[%.3f]", label.stringValue, defaultStrengthValue)
                sliderView.minValue = Double(value.first ?? 0)
                sliderView.maxValue = Double(value.last ?? 1)
                sliderView.doubleValue = defaultStrengthValue
                sliderView.target = self
                sliderView.action = #selector(makeupSliderAction(_:))
                valueView = sliderView
            } else if type == "switch" {
                let switchView = NSButton(checkboxWithTitle: "", target: self, action: #selector(makeupSwitchAction(_:)))
                let state = beautyManager.makeUpEnable
                switchView.state = state ? NSControl.StateValue.on : NSControl.StateValue.off
                valueView = switchView
            } else if type == "segment" {
                let titles = makeupList[i]["title"] as? [String] ?? []
                let segmentedControl = NSSegmentedControl(labels: titles, trackingMode: .selectOne, target: self, action: #selector(makeupSegmentAction(_:)))
                let defaultValue = Int(getDefaultSegmentValueForKey(key: key))
                let values = makeupList[i]["value"] as? [Int] ?? []
                let defaultIndex = values.firstIndex(of: defaultValue) ?? 0
                segmentedControl.selectedSegment = defaultIndex
                valueView = segmentedControl
            }

            if let valueView = valueView {
                valueView.translatesAutoresizingMaskIntoConstraints = false
                horizontalStackView.addArrangedSubview(valueView)
                valueView.identifier = NSUserInterfaceItemIdentifier(rawValue: "\(i + 1000)")
            }

            contentView.addArrangedSubview(horizontalStackView)
            
            NSLayoutConstraint.activate([
                horizontalStackView.leadingAnchor.constraint(equalTo: contentView.leadingAnchor),
                horizontalStackView.trailingAnchor.constraint(equalTo: contentView.trailingAnchor),
            ])
        }

        if let window = self.view.window {
            window.contentViewController?.presentAsModalWindow(customAlertVC)
        }
    }
    
    @objc func makeupSliderAction(_ view: NSSlider) {
        let makeupList = beautyManager.makeupList
        let index = (Int(view.identifier?.rawValue ?? "") ?? 0) - 1000
        let key = makeupList[index]["key"] as? String ?? ""
        let value = view.floatValue
        makeupParams[key] = value

        if let label = findViewInSuperview(view.superview, identifier: "\(index + 2000)") as? NSTextField {
            label.stringValue = String(format: "%@[%.3f]", makeupList[index]["name"] as? String ?? "none", view.doubleValue)
        }
        
        if key == "lipStrength" {
            beautyManager.lipStrength = value
        } else if key == "blushStrength" {
            beautyManager.blushStrength = value
        } else if key == "pupilStrength" {
            beautyManager.pupilStrength = value
        } else if key == "shadowStrength" {
            beautyManager.shadowStrength = value
        } else if key == "lashStrength" {
            beautyManager.lashStrength = value
        } else if key == "browStrength" {
            beautyManager.browStrength = value
        } else if key == "wocanStrength" {
            beautyManager.wocanStrength = value
        }
    }
    
    @objc func makeupSwitchAction(_ view: NSButton) {
        let makeupList = beautyManager.makeupList
        let index = (Int(view.identifier?.rawValue ?? "") ?? 0) - 1000
        let key = makeupList[index]["key"] as? String ?? ""
        let state = view.state == .on
//        if state {
//            beautyManager.beautyMakeupStyle = "default makeup style".localized
//        }
        makeupParams[key] = state
        beautyManager.makeUpEnable = state
    }

    @objc func makeupSegmentAction(_ view: NSSegmentedControl) {
        let makeupList = beautyManager.makeupList
        let index = (Int(view.identifier?.rawValue ?? "") ?? 0) - 1000
        let key = makeupList[index]["key"] as? String ?? ""
        let values = makeupList[index]["value"] as? [Int] ?? []
        let value = Int32(values[view.selectedSegment])
        makeupParams[key] = value
        if key == "pupilStyle" {
            beautyManager.pupilStyle = value
        } else if key == "browStyle" {
            beautyManager.browStyle = value
        } else if key == "browColor" {
            beautyManager.browColor = value
        } else if key == "lashStyle" {
            beautyManager.lashStyle = value
        } else if key == "lashColor" {
            beautyManager.lashColor = value
        } else if key == "shadowStyle" {
            beautyManager.shadowStyle = value
        } else if key == "pupilStyle" {
            beautyManager.pupilStyle = value
        } else if key == "blushStyle" {
            beautyManager.blushStyle = value
        } else if key == "blushColor" {
            beautyManager.blushColor = value
        } else if key == "lipStyle" {
            beautyManager.lipStyle = value
        } else if key == "lipColor" {
            beautyManager.lipColor = value
        } else if key == "wocanStyle" {
            beautyManager.wocanStyle = value
        }
    }

    @objc func confirmAction() {
        self.dismiss(nil)
    }
    
    private func getDefaultStrengthValueForKey(key: String) -> Float {
        if key == "lipStrength" {
            return beautyManager.lipStrength
        } else if key == "blushStrength" {
            return beautyManager.blushStrength
        } else if key == "pupilStrength" {
            return beautyManager.pupilStrength
        } else if key == "shadowStrength" {
            return beautyManager.shadowStrength
        } else if key == "lashStrength" {
            return beautyManager.lashStrength
        } else if key == "browStrength" {
            return beautyManager.browStrength
        } else if key == "wocanStrength" {
            return beautyManager.wocanStrength
        }
        return 0
    }
    
    private func getDefaultSegmentValueForKey(key: String) -> Int32 {
        if key == "pupilStyle" {
            return beautyManager.pupilStyle
        } else if key == "browStyle" {
            return beautyManager.browStyle
        } else if key == "browColor" {
            return beautyManager.browColor
        } else if key == "lashStyle" {
            return beautyManager.lashStyle
        } else if key == "lashColor" {
            return beautyManager.lashColor
        } else if key == "shadowStyle" {
            return beautyManager.shadowStyle
        } else if key == "pupilStyle" {
            return beautyManager.pupilStyle
        } else if key == "blushStyle" {
            return beautyManager.blushStyle
        } else if key == "blushColor" {
            return beautyManager.blushColor
        } else if key == "lipStyle" {
            return beautyManager.lipStyle
        } else if key == "lipColor" {
            return beautyManager.lipColor
        } else if key == "wocanStyle" {
            return beautyManager.wocanStyle 
        }
        return 0
    }
    
    private func updateMakeup() {
        guard let json = try? JSONSerialization.data(withJSONObject: makeupParams, options: []),
              let jsonString = String(data: json, encoding: .utf8) else {
            print("updateMakeup fail")
            return
        }
        
        let ret = self.agoraKit.setExtensionPropertyWithVendor("agora_video_filters_clear_vision",
                                                               extension: "clear_vision",
                                                               key: "makeup_options",
                                                               value: jsonString)
        print("updateMakeup ret: \(ret) jsonString: \(jsonString)")
    }
}

private let beautyShapeList = [
    ["name": "Face Shape Enable".localized, "key": "enable", "type": "switch"],
    ["name": "Face Shape Gender".localized,
     "key": "gender",
     "type": "segment",
     "value": ["Face Shape Gender Female".localized, "Face Shape Gender Male".localized]],
    ["name": "Face Shape Intensity".localized, "key": "intensity", "type": "slider", "value": [0, 100]],
    
    ["name": "Face Shape Area Head Scale".localized, "key": "headscale", "type": "slider", "value": [0, 100]],
    ["name": "Face Shape Area Forehead".localized, "key": "forehead", "type": "slider", "value": [-100, 100]],
    ["name": "Face Shape Area Face Contour".localized, "key": "facecontour", "type": "slider", "value": [0, 100]],
    ["name": "Face Shape Area Face Length".localized, "key": "facelength", "type": "slider", "value": [-100, 100]],
    ["name": "Face Shape Area Face Width".localized, "key": "facewidth", "type": "slider", "value": [0, 100]],
    ["name": "Face Shape Area Cheek Bone".localized, "key": "cheekbone", "type": "slider", "value": [0, 100]],
    ["name": "Face Shape Area Cheek".localized, "key": "cheek", "type": "slider", "value": [0, 100]],
    ["name": "Face Shape Area Chin".localized, "key": "chin", "type": "slider", "value": [-100, 100]],
    ["name": "Face Shape Area Eye Scale".localized, "key": "eyescale", "type": "slider", "value": [0, 100]],
    ["name": "Face Shape Area Nose Length".localized, "key": "noselength", "type": "slider", "value": [-100, 100]],
    ["name": "Face Shape Area Nose Width".localized, "key": "nosewidth", "type": "slider", "value": [-100, 100]],
    ["name": "Face Shape Area Mouth Scale".localized, "key": "mouthscale", "type": "slider", "value": [-100, 100]]
]

// MARK: face shape settings
extension AgoraBeauty {
    private func setBeauty(key: String?, value: Float) {
        let areaOption = AgoraFaceShapeAreaOptions()
        switch key {
        case "intensity":
            faceshapeOption.styleIntensity = Int32(value)
            updateFaceShape()
            return
        case "headscale":
            areaOption.shapeArea = AgoraFaceShapeArea.headScale
        case "forehead":
            areaOption.shapeArea = AgoraFaceShapeArea.forehead
        case "facecontour":
            areaOption.shapeArea = AgoraFaceShapeArea.faceContour
        case "facewidth":
            areaOption.shapeArea = AgoraFaceShapeArea.faceWidth
        case "facelength":
            areaOption.shapeArea = AgoraFaceShapeArea.faceLength
        case "cheekbone":
            areaOption.shapeArea = AgoraFaceShapeArea.cheekbone
        case "cheek":
            areaOption.shapeArea = AgoraFaceShapeArea.cheek
        case "chin":
            areaOption.shapeArea = AgoraFaceShapeArea.chin
        case "eyescale":
            areaOption.shapeArea = AgoraFaceShapeArea.eyeScale
        case "noselength":
            areaOption.shapeArea = AgoraFaceShapeArea.noseLength
        case "nosewidth":
            areaOption.shapeArea = AgoraFaceShapeArea.noseWidth
        case "mouthscale":
            areaOption.shapeArea = AgoraFaceShapeArea.mouthScale
        default:
            break
        }
        areaOption.shapeIntensity = Int32(value)
        agoraKit?.setFaceShapeAreaOptions(areaOption)
        updateFaceShape()
    }
    
    @IBAction func onShowFaceShapeAction(_ button: NSButton) {
        let customAlertVC = NSViewController()
        customAlertVC.view.wantsLayer = true
        customAlertVC.view.layer?.backgroundColor = NSColor.black.cgColor

        let alertView = NSView()
        alertView.translatesAutoresizingMaskIntoConstraints = false
        alertView.wantsLayer = true

        customAlertVC.view.addSubview(alertView)

        NSLayoutConstraint.activate([
            alertView.centerXAnchor.constraint(equalTo: customAlertVC.view.centerXAnchor),
            alertView.centerYAnchor.constraint(equalTo: customAlertVC.view.centerYAnchor),
            alertView.widthAnchor.constraint(equalTo: customAlertVC.view.widthAnchor, constant: -20),
            alertView.heightAnchor.constraint(equalToConstant: 300)
        ])

        let scrollView = NSScrollView()
        scrollView.translatesAutoresizingMaskIntoConstraints = false
        alertView.addSubview(scrollView)

        NSLayoutConstraint.activate([
            scrollView.topAnchor.constraint(equalTo: alertView.topAnchor),
            scrollView.leadingAnchor.constraint(equalTo: alertView.leadingAnchor),
            scrollView.trailingAnchor.constraint(equalTo: alertView.trailingAnchor),
            scrollView.bottomAnchor.constraint(equalTo: alertView.bottomAnchor, constant: -50)
        ])

        let contentView = NSStackView()
        contentView.translatesAutoresizingMaskIntoConstraints = false
        contentView.orientation = .vertical
        contentView.spacing = 10
        scrollView.documentView = contentView

        for i in 0..<beautyShapeList.count {
            let label = NSTextField(labelWithString: beautyShapeList[i]["name"] as? String ?? "none")
            label.translatesAutoresizingMaskIntoConstraints = false
            label.font = NSFont.systemFont(ofSize: 12)
            label.textColor = NSColor.black
            label.identifier = NSUserInterfaceItemIdentifier(rawValue: "\(i + 2000)")

            let horizontalStackView = NSStackView()
            horizontalStackView.orientation = .horizontal
//            horizontalStackView.alignment = NSLayoutConstraint.Attribute.center
            horizontalStackView.spacing = 10
            horizontalStackView.translatesAutoresizingMaskIntoConstraints = false

            horizontalStackView.addArrangedSubview(label)
            
            var valueView: NSView?
            let key = beautyShapeList[i]["key"] as? String ?? ""
            let type = beautyShapeList[i]["type"] as? String ?? ""
            
            if type == "slider" {
                let value = beautyShapeList[i]["value"] as? [Float] ?? []
                let sliderView = NSSlider()
                label.stringValue = String(format: "%@[%.f]", label.stringValue, sliderView.doubleValue)
                sliderView.minValue = Double(value.first ?? 0)
                sliderView.maxValue = Double(value.last ?? 100)
                sliderView.doubleValue = Double(beautyShapeParames[key] ?? 0)
                sliderView.target = self
                sliderView.action = #selector(beautyShapeSliderAction(_:))
                valueView = sliderView
            } else if type == "switch" {
                let switchView = NSButton(checkboxWithTitle: "", target: self, action: #selector(beautyShapeSwitchAction(_:)))
                switchView.state = enableFaceShape ? .on : .off
                valueView = switchView
            } else if type == "segment" {
                let value = beautyShapeList[i]["value"] as? [String] ?? []
                let segmentView = NSSegmentedControl(labels: value, trackingMode: .selectOne, target: self, action: #selector(beautyShapeSegmentAction(_:)))
                segmentView.selectedSegment = Int(faceshapeOption.shapeStyle.rawValue)
                valueView = segmentView
            }

            if let valueView = valueView {
                valueView.translatesAutoresizingMaskIntoConstraints = false
                horizontalStackView.addArrangedSubview(valueView)
                valueView.identifier = NSUserInterfaceItemIdentifier(rawValue: "\(i + 1000)")
            }

            contentView.addArrangedSubview(horizontalStackView)
            NSLayoutConstraint.activate([
                horizontalStackView.leadingAnchor.constraint(equalTo: contentView.leadingAnchor),
                horizontalStackView.trailingAnchor.constraint(equalTo: contentView.trailingAnchor),
                horizontalStackView.widthAnchor.constraint(greaterThanOrEqualToConstant: 300)
            ])
        }

        if let window = self.view.window {
            window.contentViewController?.presentAsModalWindow(customAlertVC)
        }
    }
    
    @objc func beautyShapeSliderAction(_ slider: NSSlider) {
        let index = (Int(slider.identifier?.rawValue ?? "") ?? 0) - 1000
        let key = beautyShapeList[index]["key"] as? String ?? ""
        beautyShapeParames[key] = Float(slider.doubleValue)
        
        if let label = findViewInSuperview(slider.superview, identifier: "\(index + 2000)") as? NSTextField {
            label.stringValue = String(format: "%@[%.f]", beautyShapeList[index]["name"] as? String ?? "none", slider.doubleValue)
        }
        
        setBeauty(key: key, value: Float(slider.doubleValue))
    }
    
    @objc func beautyShapeSwitchAction(_ view: NSSwitch) {
        enableFaceShape = view.state == .on
        updateFaceShape()
    }
    
    @objc func beautyShapeSegmentAction(_ view: NSSegmentedControl) {
        faceshapeOption.shapeStyle = AgoraFaceShapeStyle(rawValue: UInt(view.selectedSegment)) ?? .female
        updateFaceShape()
    }
    
    private func updateFaceShape() {
        self.agoraKit.setFaceShapeBeautyOptions(enableFaceShape, options: faceshapeOption)
    }
}
