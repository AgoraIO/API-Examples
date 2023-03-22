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

class VideoProcess: BaseViewController {
    
    @IBOutlet weak var Container: AGEVideoContainer!
    @IBOutlet weak var selectResolutionPicker: Picker!
    @IBOutlet weak var selectFpsPicker: Picker!
    @IBOutlet weak var selectLayoutPicker: Picker!
    @IBOutlet weak var virtualBackgroundSwitch: NSSwitch!
    @IBOutlet weak var selectVirtualBackgroundPicker: Picker!
    @IBOutlet weak var channelField: Input!
    @IBOutlet weak var joinChannelButton: NSButton!
    @IBOutlet weak var lowlightEnhanceSwitch: NSSwitch!
    @IBOutlet weak var colorEnhanceSwitch: NSSwitch!
    
    @IBOutlet weak var lowlightEnhanceLabel: NSTextField!
    @IBOutlet weak var videoDenoiseLabel: NSTextField!
    @IBOutlet weak var colorEnhanceLabel: NSTextField!
    @IBOutlet weak var strenghtLabel: NSTextField!
    @IBOutlet weak var skinProtectLabel: NSTextField!
    
    @IBOutlet weak var beautySwitch: NSSwitch!
    @IBOutlet weak var whiteningSlider: NSSlider!
    @IBOutlet weak var ruddySlider: NSSlider!
    @IBOutlet weak var sharpSlider: NSSlider!
    @IBOutlet weak var smoothingSlider: NSSlider!
    
    var videos: [VideoView] = []
    let layouts = [Layout("1v1", 2), Layout("1v3", 4), Layout("1v8", 9), Layout("1v15", 16)]
    let backgroundTypes = AgoraVirtualBackgroundSourceType.allValues()
    var agoraKit: AgoraRtcEngineKit!
    var beautifyOption = AgoraBeautyOptions()
    var skinProtect = 0.5
    var strength = 0.5
    
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
        self.setupUI()
        self.setupAgoraKit()
    }
    
    func setupAgoraKit() {
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit.setChannelProfile(.liveBroadcasting)
        agoraKit.setClientRole(.broadcaster)
        
        agoraKit.enableVideo()
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
        lowlightEnhanceLabel.stringValue = "Low light Enhancement".localized
        videoDenoiseLabel.stringValue = "Video Denoise".localized
        colorEnhanceLabel.stringValue = "Color Enhancement".localized
        strenghtLabel.stringValue = "Strength".localized
        skinProtectLabel.stringValue = "Skin Protect".localized
        
        whiteningSlider.floatValue = beautifyOption.lighteningLevel
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
                    // en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
                    // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
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
    
    @IBAction func onLowLightEnhanceSwitchChange(_ sender: NSSwitch) {
        let options = AgoraLowlightEnhanceOptions()
        options.level = .fast
        options.mode = .auto
        agoraKit.setLowlightEnhanceOptions(sender.state == .on, options: options)
    }
    
    @IBAction func onVideoDenoiseSwitchChange(_ sender: NSSwitch) {
        let options = AgoraVideoDenoiserOptions()
        options.level = .highQuality
        options.mode = .manual
        agoraKit.setVideoDenoiserOptions(sender.state == .on, options: options)
    }
    
    @IBAction func onColorEnhanceSwitchChange(_ sender: NSSwitch) {
        let options = AgoraColorEnhanceOptions()
        options.strengthLevel = Float(strength)
        options.skinProtectLevel = Float(skinProtect)
        agoraKit.setColorEnhanceOptions(sender.state == .on, options: options)
    }
    
    @IBAction func onStrengthSliderChange(_ sender: NSSlider) {
        strength = sender.doubleValue
        let options = AgoraColorEnhanceOptions()
        options.strengthLevel = Float(strength)
        options.skinProtectLevel = Float(skinProtect)
        agoraKit.setColorEnhanceOptions(colorEnhanceSwitch.state == .on, options: options)
    }
    
    @IBAction func onSkinProtectSliderChange(_ sender: NSSlider) {
        skinProtect = sender.doubleValue
        let options = AgoraColorEnhanceOptions()
        options.strengthLevel = Float(strength)
        options.skinProtectLevel = Float(skinProtect)
        agoraKit.setColorEnhanceOptions(colorEnhanceSwitch.state == .on, options: options)
    }
    
    @IBAction func onBeautySliderChange(_ sender: NSSwitch) {
        agoraKit.setBeautyEffectOptions(sender.state == .on, options: beautifyOption)
    }
    
    @IBAction func onWhiteningSliderChange(_ sender: NSSlider) {
        beautifyOption.lighteningLevel = sender.floatValue
        agoraKit.setBeautyEffectOptions(beautySwitch.state == .on, options: beautifyOption)
    }
    
    @IBAction func onRuddySliderChange(_ sender: NSSlider) {
        beautifyOption.rednessLevel = sender.floatValue
        agoraKit.setBeautyEffectOptions(beautySwitch.state == .on, options: beautifyOption)
    }
    
    @IBAction func onSharpSliderChange(_ sender: NSSlider) {
        beautifyOption.sharpnessLevel = sender.floatValue
        agoraKit.setBeautyEffectOptions(beautySwitch.state == .on, options: beautifyOption)
    }
    
    @IBAction func onSmoothingSliderChange(_ sender: NSSlider) {
        beautifyOption.smoothnessLevel = sender.floatValue
        agoraKit.setBeautyEffectOptions(beautySwitch.state == .on, options: beautifyOption)
    }
    
    @IBAction func onVirtualBackgroundSwitchChange(_ sender: NSSwitch) {
        setVirtualBackground()
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
extension VideoProcess: AgoraRtcEngineDelegate {
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

