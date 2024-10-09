//
//  VideoProcess.swift
//  APIExample
//
//  Created by xianing on 2021/11/12.
//  Copyright © 2021 Agora Corp. All rights reserved.
//

import UIKit
import AgoraRtcKit
import AGEVideoLayout

class VideoProcessEntry: UIViewController {
    @IBOutlet weak var channelTextField: AGTextField!
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    @IBAction func doJoinPressed(sender: AGButton) {
        guard let channelName = channelTextField.text else { return }
        channelTextField.resignFirstResponder()
        
        let identifier = "VideoProcess"
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else {
            return
        }
        newViewController.title = channelName
        newViewController.configs = ["channelName": channelName]
        navigationController?.pushViewController(newViewController, animated: true)
    }
}

class VideoProcessMain: BaseViewController {
    @IBOutlet weak var container: AGEVideoContainer!
    @IBOutlet weak var beautySwitch: UISwitch!
    @IBOutlet weak var colorEnhanceSwitch: UISwitch!
    @IBOutlet weak var virtualBgSwitch: UISwitch!
    @IBOutlet weak var virtualBgSegment: UISegmentedControl!
    @IBOutlet weak var lightenSlider: UISlider!
    @IBOutlet weak var rednessSlider: UISlider!
    @IBOutlet weak var sharpnessSlider: UISlider!
    @IBOutlet weak var smoothSlider: UISlider!
    @IBOutlet weak var strengthSlider: UISlider!
    @IBOutlet weak var skinProtectSlider: UISlider!
    @IBOutlet weak var whiteningSlider: UISlider?
    @IBOutlet weak var beautyScrollView: UIScrollView?

    var agoraKit: AgoraRtcEngineKit!
    var localVideo = Bundle.loadVideoView(type: .local, audioOnly: false)
    var remoteVideo = Bundle.loadVideoView(type: .remote, audioOnly: false)
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false
    var beautifyOption = AgoraBeautyOptions()
    var skinProtect = 0.5
    var strength = 0.5
    var whintening = 0.5
    
    private var makeupParams = [String: Any]()
    private var enableFaceShape: Bool = false
    private lazy var faceshapeOption = AgoraFaceShapeBeautyOptions()
    private var beautyShapeParames = [String: Float]()

    override func viewDidLoad() {
        super.viewDidLoad()
        setupUI()
        
        if let beautyScrollView = beautyScrollView {
            beautyScrollView.contentSize = CGSize(width: 800, height: beautyScrollView.frame.size.height)
        }
        
        // get channel name from configs
        guard let channelName = configs["channelName"] as? String,
              let resolution = GlobalSettings.shared.getSetting(key: "resolution")?.selectedOption().value as? CGSize,
              let fps = GlobalSettings.shared.getSetting(key: "fps")?.selectedOption().value as? AgoraVideoFrameRate,
              let orientation = GlobalSettings.shared.getSetting(key: "orientation")?
            .selectedOption().value as? AgoraVideoOutputOrientationMode else {return}
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        
        // setup log file path
        let logConfig = AgoraLogConfig()
        logConfig.level = .info
        config.logConfig = logConfig
        
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // enable filters
        agoraKit.enableExtension(withVendor: "agora_video_filters_clear_vision", 
                                 extension: "clear_vision",
                                 enabled: true,
                                 sourceType: .primaryCamera)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        // make myself a broadcaster
        agoraKit.setChannelProfile(.liveBroadcasting)
        agoraKit.setClientRole(GlobalSettings.shared.getUserRole())
                
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        agoraKit.enableAudio()
        agoraKit.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: resolution,
                                                                             frameRate: fps,
                                                                             bitrate: AgoraVideoBitrateStandard,
                                                                             orientationMode: orientation,
                                                                             mirrorMode: .auto))
        
        // set up local video to render your local camera preview
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = 0
        // the view to be binded
        videoCanvas.view = localVideo.videoView
        videoCanvas.renderMode = .hidden
        agoraKit.setupLocalVideo(videoCanvas)
        agoraKit.startPreview()
        
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        let option = AgoraRtcChannelMediaOptions()
        option.publishCameraTrack = GlobalSettings.shared.getUserRole() == .broadcaster
        option.publishMicrophoneTrack = GlobalSettings.shared.getUserRole() == .broadcaster
        option.clientRoleType = GlobalSettings.shared.getUserRole()
        NetworkManager.shared.generateToken(channelName: channelName, success: { token in
            let result = self.agoraKit.joinChannel(byToken: token, channelId: channelName, uid: 0, mediaOptions: option)
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
                self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
            }
        })
    }
    
    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            // leave channel when exiting the view
            if isJoined {
                agoraKit.disableAudio()
                agoraKit.disableVideo()
                agoraKit.leaveChannel { (stats) -> Void in
                    LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
                }
            }
            AgoraRtcEngineKit.destroy()
        }
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        // close virtural
        agoraKit.enableVirtualBackground(false, backData: nil, segData: nil)
        agoraKit.disableAudio()
        agoraKit.disableVideo()
        agoraKit.leaveChannel { _ in }
    }
    
    // MARK: - UI
    func setupUI() {
        // layout render view
        localVideo.setPlaceholder(text: "Local Host".localized)
        remoteVideo.setPlaceholder(text: "Remote Host".localized)
        container.layoutStream(views: [localVideo, remoteVideo])
        
        lightenSlider.value = beautifyOption.lighteningLevel
        rednessSlider.value = beautifyOption.rednessLevel
        sharpnessSlider.value = beautifyOption.sharpnessLevel
        smoothSlider.value = beautifyOption.smoothnessLevel
        strengthSlider.value = Float(strength)
        skinProtectSlider.value = Float(skinProtect)
        whiteningSlider?.value = Float(whintening)
    }
    
    @IBAction func onChangeBeauty(_ sender: UISwitch) {
        guard agoraKit.isFeatureAvailable(onDevice: .videoPreprocessBeauty) else {
            ToastView.show(text: "The feature is unavailable in the device!")
            return
        }
            
        agoraKit.setBeautyEffectOptions(sender.isOn, options: beautifyOption)
        if let slider = whiteningSlider {
            onWhinteningSlider(slider)
        }
    }

    @IBAction func onLightenSlider(_ sender: UISlider) {
        beautifyOption.lighteningLevel = sender.value
        agoraKit.setBeautyEffectOptions(beautySwitch.isOn, options: beautifyOption)
    }
    
    @IBAction func onRednessSlider(_ sender: UISlider) {
        beautifyOption.rednessLevel = sender.value
        agoraKit.setBeautyEffectOptions(beautySwitch.isOn, options: beautifyOption)
    }
    
    @IBAction func onSharpnessSlider(_ sender: UISlider) {
        beautifyOption.sharpnessLevel = sender.value
        agoraKit.setBeautyEffectOptions(beautySwitch.isOn, options: beautifyOption)
    }
    
    @IBAction func onSmoothSlider(_ sender: UISlider) {
        beautifyOption.smoothnessLevel = sender.value
        agoraKit.setBeautyEffectOptions(beautySwitch.isOn, options: beautifyOption)
    }
    
    @IBAction func onChangeLowLightEnhance(_ sender: UISwitch) {
        let options = AgoraLowlightEnhanceOptions()
        options.level = .fast
        options.mode = .auto
        agoraKit.setLowlightEnhanceOptions(sender.isOn, options: options)
    }
    
    @IBAction func onChangeVideoDenoise(_ sender: UISwitch) {
        let options = AgoraVideoDenoiserOptions()
        options.level = .highQuality
        options.mode = .manual
        agoraKit.setVideoDenoiserOptions(sender.isOn, options: options)
    }
    
    @IBAction func onChangeColorEnhance(_ sender: UISwitch) {
        let options = AgoraColorEnhanceOptions()
        options.strengthLevel = Float(strength)
        options.skinProtectLevel = Float(skinProtect)
        agoraKit.setColorEnhanceOptions(sender.isOn, options: options)
    }
    
    @IBAction func onStrengthSlider(_ sender: UISlider) {
        strength = Double(sender.value)
        let options = AgoraColorEnhanceOptions()
        options.strengthLevel = Float(strength)
        options.skinProtectLevel = Float(skinProtect)
        agoraKit.setColorEnhanceOptions(colorEnhanceSwitch.isOn, options: options)
    }
    
    @IBAction func onSkinProtectSlider(_ sender: UISlider) {
        skinProtect = Double(sender.value)
        let options = AgoraColorEnhanceOptions()
        options.strengthLevel = Float(strength)
        options.skinProtectLevel = Float(skinProtect)
        agoraKit.setColorEnhanceOptions(colorEnhanceSwitch.isOn, options: options)
    }
    
    @IBAction func onWhinteningSlider(_ sender: UISlider) {
        let options = AgoraFilterEffectOptions()
        options.path = "built_in_whiten_filter"
        options.strength = sender.value
        whintening = Double(sender.value)
        let ret = agoraKit.setFilterEffectOptions(beautySwitch.isOn, options: options)
        print("onWhinteningSlider: \(ret), \(options.strength)")
    }
    
    @IBAction func onChangeVirtualBgSwtich(_ sender: UISwitch) {
        if sender.isOn {
            if agoraKit.isFeatureAvailable(onDevice: .videoPreprocessVirtualBackground) {
                changeVirtualBackground()
            } else {
                ToastView.show(text: "The feature is unavailable in the device!")
            }
        } else {
            changeVirtualBackground()
        }
    }
    
    @IBAction func onChangeVirtualBgSegment(_ sender: UISegmentedControl) {
        changeVirtualBackground()
    }
    
    func changeVirtualBackground() {
        let source = AgoraVirtualBackgroundSource()
        switch virtualBgSegment.selectedSegmentIndex {
        case 0:
            let imgPath = Bundle.main.path(forResource: "agora-logo", ofType: "png")
            source.backgroundSourceType = .img
            source.source = imgPath
            
        case 1:
            source.backgroundSourceType = .color
            source.color = 0xFFFFFF
            
        case 2:
            source.backgroundSourceType = .blur
            source.blurDegree = .high
            
        case 3:
            let videoPath = Bundle.main.path(forResource: "sample", ofType: "mov")
            source.backgroundSourceType = .video
            source.source = videoPath
            
        default: break
        }
        source.backgroundSourceType = virtualBgSwitch.isOn ? source.backgroundSourceType : .none
        let result = agoraKit.enableVirtualBackground(virtualBgSwitch.isOn, backData: source, segData: AgoraSegmentationProperty())
        print("result == \(result)")
    }
}

extension VideoProcessMain: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://api-ref.agora.io/en/voice-sdk/ios/3.x/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.description)", level: .warning)
    }
    
    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
    /// cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode)", level: .error)
        self.showAlert(title: "Error", message: "Error \(errorCode.description) occur")
    }
    
    /// callback when the local user joins a specified channel.
    /// @param channel
    /// @param uid uid of local user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        self.isJoined = true
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        
        // Only one remote video view is available for this
        // tutorial. Here we check if there exists a surface
        // view tagged as this uid.
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = remoteVideo.videoView
        videoCanvas.renderMode = .hidden
        videoCanvas.enableAlphaMask = true
        agoraKit.setupRemoteVideo(videoCanvas)
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
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = nil
        videoCanvas.renderMode = .hidden
        agoraKit.setupRemoteVideo(videoCanvas)
    }

    /// Reports the statistics of the current call. The SDK triggers this callback once every two seconds after the user joins the channel.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, reportRtcStats stats: AgoraChannelStats) {
        localVideo.statsInfo?.updateChannelStats(stats)
    }
    
    /// Reports the statistics of the video stream from each remote user/host.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteVideoStats stats: AgoraRtcRemoteVideoStats) {
        remoteVideo.statsInfo?.updateVideoStats(stats)
    }
    
    /// Reports the statistics of the audio stream from each remote user/host.
    /// @param stats stats struct for current call statistics
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteAudioStats stats: AgoraRtcRemoteAudioStats) {
        remoteVideo.statsInfo?.updateAudioStats(stats)
    }
}

private let makeupList = [
    [
        "name": "Makeup_enable_mu".localized,
        "key": "enable_mu",
        "type": "switch"
    ], [
        "name": "Makeup_browStyle".localized,
        "key": "browStyle",
        "type": "segment",
        "value": ["Makeup_browStyle_value1".localized, "Makeup_browStyle_value2".localized, "Makeup_browStyle_value3".localized]
    ], [
        "name": "Makeup_browColor".localized,
        "key": "browColor",
        "type": "segment",
        "value": ["Makeup_browColor_value1".localized, "Makeup_browColor_value2".localized, "Makeup_browColor_value3".localized]
    ], [
        "name": "Makeup_browStrength".localized,
        "key": "browStrength",
        "type": "slider",
        "value": [0, 1]
    ], [
        "name": "Makeup_lashStyle".localized,
        "key": "lashStyle",
        "type": "segment",
        "value": ["Makeup_browStyle_value1".localized, "Makeup_browStyle_value2".localized, "Makeup_browStyle_value3".localized]
    ], [
        "name": "Makeup_lashColor".localized,
        "key": "lashColor",
        "type": "segment",
        "value": ["Makeup_browColor_value1".localized, "Makeup_browColor_value2".localized, "Makeup_browColor_value3".localized]
    ], [
        "name": "Makeup_lashStrength".localized,
        "key": "lashStrength",
        "type": "slider",
        "value": [0, 1]
    ], [
        "name": "Makeup_shadowStyle".localized,
        "key": "shadowStyle",
        "type": "segment",
        "value": ["Makeup_browStyle_value1".localized, "Makeup_browStyle_value2".localized, "Makeup_browStyle_value3".localized]
    ],
    //    ["name": "Makeup_shadowColor".localized, "key": "shadowColor"],
    [
        "name": "Makeup_shadowStrength".localized,
        "key": "shadowStrength",
        "type": "slider",
        "value": [0, 1]
    ], [
        "name": "Makeup_pupilStyle".localized,
        "key": "pupilStyle",
        "type": "segment",
        "value": ["Makeup_browStyle_value1".localized, "Makeup_browStyle_value2".localized, "Makeup_browStyle_value3".localized]
    ],
    //    ["name": "Makeup_pupilColor".localized, "key": "pupilColor"],
    [
        "name": "Makeup_pupilStrength".localized,
        "key": "pupilStrength",
        "type": "slider",
        "value": [0, 1]
    ], [
        "name": "Makeup_blushStyle".localized,
        "key": "blushStyle",
        "type": "segment",
        "value": ["Makeup_browStyle_value1".localized, "Makeup_browStyle_value2".localized, "Makeup_browStyle_value3".localized]
    ], [
        "name": "Makeup_blushColor".localized,
        "key": "blushColor",
        "type": "segment",
        "value": [
            "Makeup_blushColor_value1".localized,
            "Makeup_blushColor_value2".localized,
            "Makeup_blushColor_value3".localized,
            "Makeup_blushColor_value4".localized,
            "Makeup_blushColor_value5".localized
        ]
    ], [
        "name": "Makeup_blushStrength".localized,
        "key": "blushStrength",
        "type": "slider",
        "value": [0, 1]
    ], [
        "name": "Makeup_lipStyle".localized,
        "key": "lipStyle",
        "type": "segment",
        "value": ["Makeup_browColor_value1".localized, "Makeup_browColor_value2".localized, "Makeup_browColor_value3".localized]
    ], [
        "name": "Makeup_lipColor".localized,
        "key": "lipColor",
        "type": "segment",
        "value": [
            "Makeup_blushColor_value1".localized,
            "Makeup_blushColor_value2".localized,
            "Makeup_blushColor_value3".localized,
            "Makeup_blushColor_value4".localized,
            "Makeup_blushColor_value5".localized
        ]
    ], [
        "name": "Makeup_lipStrength".localized,
        "key": "lipStrength",
        "type": "slider",
        "value": [0, 1]
    ]
]

// MARK: make up setting
extension VideoProcessMain {
    @IBAction func onShowMakeUpAction() {
        // 创建自定义视图控制器
        let customAlertVC = UIViewController()
        customAlertVC.modalPresentationStyle = .overFullScreen
        customAlertVC.view.backgroundColor = .clear

        // 自定义内容视图
        let alertView = UIView()
        alertView.translatesAutoresizingMaskIntoConstraints = false
        alertView.backgroundColor = UIColor.white
        alertView.layer.shadowColor = UIColor.black.cgColor
        alertView.layer.shadowOpacity = 0.2
        alertView.layer.shadowOffset = CGSize(width: 0, height: 2)
        alertView.layer.shadowRadius = 4

        customAlertVC.view.addSubview(alertView)

        // 设置 alertView 的约束
        NSLayoutConstraint.activate([
            alertView.centerXAnchor.constraint(equalTo: customAlertVC.view.centerXAnchor),
            alertView.centerYAnchor.constraint(equalTo: customAlertVC.view.centerYAnchor),
            alertView.widthAnchor.constraint(equalTo: customAlertVC.view.widthAnchor, constant: -20),
            alertView.heightAnchor.constraint(equalToConstant: 300)
        ])

        // 创建 scrollView
        let scrollView = UIScrollView()
        scrollView.translatesAutoresizingMaskIntoConstraints = false
        alertView.addSubview(scrollView)

        NSLayoutConstraint.activate([
            scrollView.topAnchor.constraint(equalTo: alertView.topAnchor),
            scrollView.leadingAnchor.constraint(equalTo: alertView.leadingAnchor),
            scrollView.trailingAnchor.constraint(equalTo: alertView.trailingAnchor),
            scrollView.bottomAnchor.constraint(equalTo: alertView.bottomAnchor, constant: -50) // 留出按钮位置
        ])

        let contentView = UIView()
        contentView.translatesAutoresizingMaskIntoConstraints = false
        scrollView.addSubview(contentView)

        NSLayoutConstraint.activate([
            contentView.topAnchor.constraint(equalTo: scrollView.topAnchor),
            contentView.leadingAnchor.constraint(equalTo: scrollView.leadingAnchor),
            contentView.trailingAnchor.constraint(equalTo: scrollView.trailingAnchor),
            contentView.bottomAnchor.constraint(equalTo: scrollView.bottomAnchor),
            contentView.widthAnchor.constraint(equalTo: scrollView.widthAnchor)
        ])
        

        // 添加 UILabels 和 UISliders 到 contentView
        var lastLabel: UILabel?
        for i in 0..<makeupList.count {
            let label = UILabel()
            label.translatesAutoresizingMaskIntoConstraints = false
            label.text = makeupList[i]["name"] as? String ?? "none"
            label.font = UIFont.systemFont(ofSize: 12)
            label.textColor = .black
            label.tag = i + 2000
            contentView.addSubview(label)

            var valueView: UIView?
            let key = makeupList[i]["key"] as? String ?? ""
            let type = makeupList[i]["type"] as? String ?? ""
            if type == "slider" {
                let value = makeupList[i]["value"] as? [Float] ?? []
                let sliderView = UISlider()
                sliderView.value = makeupParams[key] as? Float ?? 0
                label.text = String(format: "%@[%.f]", label.text ?? "none", sliderView.value)
                sliderView.minimumValue = value.first ?? 0
                sliderView.maximumValue = value.last ?? 1
                sliderView.addTarget(self, action: #selector(makeupSliderAction(_:)), for: .valueChanged)
                valueView = sliderView
            } else if type == "switch" {
                let switchView = UISwitch()
                switchView.isOn = makeupParams[key] as? Bool ?? false
                switchView.addTarget(self, action: #selector(makeupSwitchAction(_:)), for: .valueChanged)
                valueView = switchView
            } else if type == "segment" {
                let value = makeupList[i]["value"] as? [String] ?? []
                let segmentView = UISegmentedControl(items: value)
                segmentView.selectedSegmentIndex = makeupParams[key] as? Int ?? 0
                segmentView.addTarget(self, action: #selector(makeupSegmentAction(_:)), for: .valueChanged)
                valueView = segmentView
            }
            guard let valueView = valueView else {return}
            valueView.translatesAutoresizingMaskIntoConstraints = false
            contentView.addSubview(valueView)
            valueView.tag = i + 1000
            // 设置 label 和 valueView 的约束
            NSLayoutConstraint.activate([
                label.topAnchor.constraint(equalTo: lastLabel?.bottomAnchor ?? contentView.topAnchor, constant: lastLabel == nil ? 10 : 10),
                label.leadingAnchor.constraint(equalTo: contentView.leadingAnchor, constant: 15),
                label.trailingAnchor.constraint(equalTo: valueView.leadingAnchor, constant: -10),
                label.heightAnchor.constraint(equalToConstant: 40),
                valueView.centerYAnchor.constraint(equalTo: label.centerYAnchor),
                valueView.trailingAnchor.constraint(equalTo: contentView.trailingAnchor, constant: -10)
            ])
            lastLabel = label // 更新 lastLabel
        }

        // 添加确认按钮
        let confirmButton = UIButton(type: .system)
        confirmButton.setTitle("Sure".localized, for: .normal)
        confirmButton.translatesAutoresizingMaskIntoConstraints = false
        confirmButton.addTarget(self, action: #selector(confirmAction), for: .touchUpInside)

        alertView.addSubview(confirmButton)

        // 设置确认按钮的约束
        NSLayoutConstraint.activate([
            confirmButton.topAnchor.constraint(equalTo: alertView.bottomAnchor, constant: -40),
            confirmButton.centerXAnchor.constraint(equalTo: alertView.centerXAnchor)
        ])

        // 更新 contentView 的高度约束
        let lastView = lastLabel ?? contentView
        NSLayoutConstraint.activate([
            contentView.bottomAnchor.constraint(equalTo: lastView.bottomAnchor, constant: 10)
        ])

        // 显示自定义视图控制器
        self.present(customAlertVC, animated: true, completion: nil)
    }
    
    @objc func makeupSliderAction(_ view: UISlider) {
        let index = view.tag - 1000
        let key = makeupList[index]["key"] as? String ?? ""
        makeupParams[key] = view.value
        
        if let label = view.superview?.viewWithTag(index + 2000) as? UILabel {
            label.text = String(format: "%@[%.f]", makeupList[index]["name"] as? String ?? "none", view.value)
        }
        updateMakeup()
    }
    
    @objc func makeupSwitchAction(_ view: UISwitch) {
        let index = view.tag - 1000
        let key = makeupList[index]["key"] as? String ?? ""
        makeupParams[key] = view.isOn
        updateMakeup()
    }
    
    @objc func makeupSegmentAction(_ view: UISegmentedControl) {
        let index = view.tag - 1000
        let key = makeupList[index]["key"] as? String ?? ""
        makeupParams[key] = view.selectedSegmentIndex
        updateMakeup()
    }
    
    @objc func confirmAction() {
        // 关闭自定义视图控制器
        self.dismiss(animated: true, completion: nil)
    }
    
    private func updateMakeup() {
        guard let json = try? JSONSerialization.data(withJSONObject: makeupParams, options: .prettyPrinted),
              let jsonString = String(data: json, encoding: .utf8) else {
            print("updateMakeup fail")
            return
        }
        
        let ret = self.agoraKit.setExtensionPropertyWithVendor("agora_video_filters_clear_vision",
                                                               extension: "clear_vision",
                                                               key: "makeup_options",
                                                               value: jsonString)
//        print("updateMakeup ret: \(ret) jsonString: \(jsonString)")
    }
}

private let beautyShapeList = [
    ["name": "face_shape_enable".localized, "key": "enable", "type": "switch"],
    ["name": "face_shape_gender".localized, 
     "key": "gender",
     "type": "segment",
     "value": ["face_shape_gender_female".localized, "face_shape_gender_male".localized]],
    ["name": "face_shape_intensity".localized, "key": "intensity", "type": "slider", "value": [0, 100]],
    
    ["name": "face_shape_area_head_scale".localized, "key": "headscale", "type": "slider", "value": [0, 100]],
    ["name": "face_shape_area_forehead".localized, "key": "forehead", "type": "slider", "value": [-100, 100]],
    ["name": "face_shape_area_face_contour".localized, "key": "facecontour", "type": "slider", "value": [0, 100]],
    ["name": "face_shape_area_face_length".localized, "key": "facelength", "type": "slider", "value": [-100, 100]],
    ["name": "face_shape_area_face_width".localized, "key": "facewidth", "type": "slider", "value": [0, 100]],
    ["name": "face_shape_area_cheek_bone".localized, "key": "cheekbone", "type": "slider", "value": [0, 100]],
    ["name": "face_shape_area_cheek".localized, "key": "cheek", "type": "slider", "value": [0, 100]],
    ["name": "face_shape_area_chin".localized, "key": "chin", "type": "slider", "value": [-100, 100]],
    ["name": "face_shape_area_eye_scale".localized, "key": "eyescale", "type": "slider", "value": [0, 100]],
    ["name": "face_shape_area_nose_length".localized, "key": "noselength", "type": "slider", "value": [-100, 100]],
    ["name": "face_shape_area_nose_width".localized, "key": "nosewidth", "type": "slider", "value": [-100, 100]],
    ["name": "face_shape_area_mouth_scale".localized, "key": "mouthscale", "type": "slider", "value": [-100, 100]]
]

// MARK: face shape settings
extension VideoProcessMain {
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
    
    @IBAction func onShowFaceShapeAction() {
        // 创建自定义视图控制器
        let customAlertVC = UIViewController()
        customAlertVC.modalPresentationStyle = .overFullScreen
        customAlertVC.view.backgroundColor = .clear

        // 自定义内容视图
        let alertView = UIView()
        alertView.translatesAutoresizingMaskIntoConstraints = false
        alertView.backgroundColor = UIColor.white
        alertView.layer.shadowColor = UIColor.black.cgColor
        alertView.layer.shadowOpacity = 0.2
        alertView.layer.shadowOffset = CGSize(width: 0, height: 2)
        alertView.layer.shadowRadius = 4

        customAlertVC.view.addSubview(alertView)

        // 设置 alertView 的约束
        NSLayoutConstraint.activate([
            alertView.centerXAnchor.constraint(equalTo: customAlertVC.view.centerXAnchor),
            alertView.centerYAnchor.constraint(equalTo: customAlertVC.view.centerYAnchor),
            alertView.widthAnchor.constraint(equalTo: customAlertVC.view.widthAnchor, constant: -20),
            alertView.heightAnchor.constraint(equalToConstant: 300)
        ])

        // 创建 scrollView
        let scrollView = UIScrollView()
        scrollView.translatesAutoresizingMaskIntoConstraints = false
        alertView.addSubview(scrollView)

        NSLayoutConstraint.activate([
            scrollView.topAnchor.constraint(equalTo: alertView.topAnchor),
            scrollView.leadingAnchor.constraint(equalTo: alertView.leadingAnchor),
            scrollView.trailingAnchor.constraint(equalTo: alertView.trailingAnchor),
            scrollView.bottomAnchor.constraint(equalTo: alertView.bottomAnchor, constant: -50) // 留出按钮位置
        ])

        let contentView = UIView()
        contentView.translatesAutoresizingMaskIntoConstraints = false
        scrollView.addSubview(contentView)

        NSLayoutConstraint.activate([
            contentView.topAnchor.constraint(equalTo: scrollView.topAnchor),
            contentView.leadingAnchor.constraint(equalTo: scrollView.leadingAnchor),
            contentView.trailingAnchor.constraint(equalTo: scrollView.trailingAnchor),
            contentView.bottomAnchor.constraint(equalTo: scrollView.bottomAnchor),
            contentView.widthAnchor.constraint(equalTo: scrollView.widthAnchor)
        ])

        // 添加 UILabels 和 UISliders 到 contentView
        var lastLabel: UILabel?
        for i in 0..<beautyShapeList.count {
            let label = UILabel()
            label.translatesAutoresizingMaskIntoConstraints = false
            label.text = beautyShapeList[i]["name"]  as? String ?? "unknown"
            label.font = UIFont.systemFont(ofSize: 12)
            label.textColor = .black
            label.tag = i + 2000
            contentView.addSubview(label)
            
            var valueView: UIView?
            let key = beautyShapeList[i]["key"] as? String ?? ""
            let type = beautyShapeList[i]["type"] as? String ?? ""
            if type == "slider" {
                let value = makeupList[i]["value"] as? [Float] ?? []
                let sliderView = UISlider()
                sliderView.value = beautyShapeParames[key] ?? 0
                label.text = String(format: "%@[%.f]", label.text ?? "none", sliderView.value)
                sliderView.minimumValue = value.first ?? 0
                sliderView.maximumValue = value.last ?? 100
                sliderView.addTarget(self, action: #selector(beautyShapeSliderAction(_:)), for: .valueChanged)
                valueView = sliderView
            } else if type == "switch" {
                let switchView = UISwitch()
                switchView.isOn = enableFaceShape
                switchView.addTarget(self, action: #selector(beautyShapeSwitchAction(_:)), for: .valueChanged)
                valueView = switchView
            } else if type == "segment" {
                let value = beautyShapeList[i]["value"] as? [String] ?? []
                let segmentView = UISegmentedControl(items: value)
                segmentView.selectedSegmentIndex = Int(faceshapeOption.shapeStyle.rawValue)
                segmentView.addTarget(self, action: #selector(beautyShapeSegmentAction(_:)), for: .valueChanged)
                valueView = segmentView
            }
            guard let valueView = valueView else {return}
            valueView.translatesAutoresizingMaskIntoConstraints = false
            contentView.addSubview(valueView)
            valueView.tag = i + 1000
            
            // 设置 label 和 valueView 的约束
            NSLayoutConstraint.activate([
                label.topAnchor.constraint(equalTo: lastLabel?.bottomAnchor ?? contentView.topAnchor, constant: lastLabel == nil ? 10 : 10),
                label.leadingAnchor.constraint(equalTo: contentView.leadingAnchor, constant: 15),
                label.trailingAnchor.constraint(equalTo: valueView.leadingAnchor, constant: -10),
                label.heightAnchor.constraint(equalToConstant: 40),
                valueView.centerYAnchor.constraint(equalTo: label.centerYAnchor),
                valueView.trailingAnchor.constraint(equalTo: contentView.trailingAnchor, constant: -10)
            ])
            lastLabel = label // 更新 lastLabel
        }

        // 添加确认按钮
        let confirmButton = UIButton(type: .system)
        confirmButton.setTitle("Sure".localized, for: .normal)
        confirmButton.translatesAutoresizingMaskIntoConstraints = false
        confirmButton.addTarget(self, action: #selector(confirmAction), for: .touchUpInside)

        alertView.addSubview(confirmButton)

        // 设置确认按钮的约束
        NSLayoutConstraint.activate([
            confirmButton.topAnchor.constraint(equalTo: alertView.bottomAnchor, constant: -40),
            confirmButton.centerXAnchor.constraint(equalTo: alertView.centerXAnchor)
        ])

        // 更新 contentView 的高度约束
        let lastView = lastLabel ?? contentView
        NSLayoutConstraint.activate([
            contentView.bottomAnchor.constraint(equalTo: lastView.bottomAnchor, constant: 10)
        ])

        // 显示自定义视图控制器
        self.present(customAlertVC, animated: true, completion: nil)
    }
    
    @objc func beautyShapeSliderAction(_ view: UISlider) {
        let index = view.tag - 1000
        let key = beautyShapeList[index]["key"] as? String ?? ""
        beautyShapeParames[key] = view.value
        
        if let label = view.superview?.viewWithTag(index + 2000) as? UILabel {
            label.text = String(format: "%@[%.f]", beautyShapeList[index]["name"] as? String ?? "none", view.value)
        }
        
        setBeauty(key: key, value: view.value)
    }
    
    @objc func beautyShapeSwitchAction(_ view: UISwitch) {
        enableFaceShape = view.isOn
        updateFaceShape()
    }
    
    @objc func beautyShapeSegmentAction(_ view: UISegmentedControl) {
        faceshapeOption.shapeStyle = AgoraFaceShapeStyle(rawValue: UInt(view.selectedSegmentIndex)) ?? .female
        updateFaceShape()
    }
    
    private func updateFaceShape() {
        self.agoraKit.setFaceShapeBeautyOptions(enableFaceShape, options: faceshapeOption)
    }
}
