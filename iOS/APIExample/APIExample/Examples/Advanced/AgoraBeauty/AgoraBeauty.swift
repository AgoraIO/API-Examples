//
//  AgoraBeauty.swift
//  APIExample
//
//  Created by xianing on 2021/11/12.
//  Copyright © 2021 Agora Corp. All rights reserved.
//

import UIKit
import AgoraRtcKit
import AGEVideoLayout

class AgoraBeautyEntry: UIViewController {
    @IBOutlet weak var channelTextField: AGTextField!
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    @IBAction func doJoinPressed(sender: AGButton) {
        guard let channelName = channelTextField.text else { return }
        channelTextField.resignFirstResponder()
        
        let identifier = "AgoraBeauty"
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else {
            return
        }
        newViewController.title = channelName
        newViewController.configs = ["channelName": channelName]
        navigationController?.pushViewController(newViewController, animated: true)
    }
}

class AgoraBeautyMain: BaseViewController {
    @IBOutlet weak var container: AGEVideoContainer!
    @IBOutlet weak var beautySwitch: UISwitch!
    @IBOutlet weak var virtualBgSwitch: UISwitch!
    @IBOutlet weak var virtualBgSegment: UISegmentedControl!
    @IBOutlet weak var lightenSlider: UISlider!
    @IBOutlet weak var rednessSlider: UISlider!
    @IBOutlet weak var sharpnessSlider: UISlider!
    @IBOutlet weak var smoothSlider: UISlider!
    @IBOutlet weak var whiteningSlider: UISlider?
    @IBOutlet weak var beautyScrollView: UIScrollView?
    
    var agoraKit: AgoraRtcEngineKit!
    var beautyManager: AgoraBeautyManager!
    var localVideo = Bundle.loadVideoView(type: .local, audioOnly: false)
    var remoteVideo = Bundle.loadVideoView(type: .remote, audioOnly: false)
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false
    var skinProtect = 0.5
    var strength = 0.5
    var whintening = 0.5
    
    private var makeupParams = [String: Any]()
    private var enableFaceShape: Bool = false
    private lazy var faceshapeOption = AgoraFaceShapeBeautyOptions()
    private var beautyShapeParames = [String: Float]()

    override func viewDidLoad() {
        super.viewDidLoad()
        
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
        
        beautyManager = AgoraBeautyManager(agoraKit: agoraKit)
        beautyManager.beautyMakeupStyle = "default makeup style".localized
        beautyManager.makeUpEnable = false
        setupUI()
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
            beautyManager.destory()
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
        
        lightenSlider.value = beautyManager.lightness
        rednessSlider.value = beautyManager.redness
        sharpnessSlider.value = beautyManager.sharpness
        smoothSlider.value = beautyManager.smoothness
        whiteningSlider?.value = Float(whintening)
    }
    
    @IBAction func onChangeBeauty(_ sender: UISwitch) {
        beautyManager.basicBeautyEnable = sender.isOn
    }

    @IBAction func onLightenSlider(_ sender: UISlider) {
        beautyManager.lightness = sender.value
    }
    
    @IBAction func onRednessSlider(_ sender: UISlider) {
        beautyManager.redness = sender.value
    }
    
    @IBAction func onSharpnessSlider(_ sender: UISlider) {
        beautyManager.sharpness = sender.value
    }
    
    @IBAction func onSmoothSlider(_ sender: UISlider) {
        beautyManager.smoothness = sender.value
    }
    
    @IBAction func onWhinteningSlider(_ sender: UISlider) {
        beautyManager.filterStrength = sender.value
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

extension AgoraBeautyMain: AgoraRtcEngineDelegate {
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

// MARK: make up setting
extension AgoraBeautyMain {
    @IBAction func onShowMakeUpAction() {
        let customAlertVC = UIViewController()
        customAlertVC.modalPresentationStyle = .overFullScreen
        customAlertVC.view.backgroundColor = .clear

        let alertView = UIView()
        alertView.translatesAutoresizingMaskIntoConstraints = false
        alertView.backgroundColor = UIColor.white
        alertView.layer.shadowColor = UIColor.black.cgColor
        alertView.layer.shadowOpacity = 0.2
        alertView.layer.shadowOffset = CGSize(width: 0, height: 2)
        alertView.layer.shadowRadius = 4

        customAlertVC.view.addSubview(alertView)

        NSLayoutConstraint.activate([
            alertView.centerXAnchor.constraint(equalTo: customAlertVC.view.centerXAnchor),
            alertView.centerYAnchor.constraint(equalTo: customAlertVC.view.centerYAnchor),
            alertView.widthAnchor.constraint(equalTo: customAlertVC.view.widthAnchor, constant: -20),
            alertView.heightAnchor.constraint(equalToConstant: 300)
        ])

        let scrollView = UIScrollView()
        scrollView.translatesAutoresizingMaskIntoConstraints = false
        alertView.addSubview(scrollView)

        NSLayoutConstraint.activate([
            scrollView.topAnchor.constraint(equalTo: alertView.topAnchor),
            scrollView.leadingAnchor.constraint(equalTo: alertView.leadingAnchor),
            scrollView.trailingAnchor.constraint(equalTo: alertView.trailingAnchor),
            scrollView.bottomAnchor.constraint(equalTo: alertView.bottomAnchor, constant: -50)
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
        
        var lastLabel: UILabel?
        let makeupList = beautyManager.makeupList
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
                let defaultStrengthValue = Double(getDefaultStrengthValueForKey(key: key))
                let sliderView = UISlider()
                label.text = String(format: "%@[%.f]", label.text ?? "none", defaultStrengthValue)
                sliderView.minimumValue = value.first ?? 0
                sliderView.maximumValue = value.last ?? 1
                sliderView.value = Float(defaultStrengthValue)
                sliderView.addTarget(self, action: #selector(makeupSliderAction(_:)), for: .valueChanged)
                valueView = sliderView
            } else if type == "switch" {
                let switchView = UISwitch()
                let state = beautyManager.makeUpEnable
                switchView.isOn = state
                switchView.addTarget(self, action: #selector(makeupSwitchAction(_:)), for: .valueChanged)
                valueView = switchView
            } else if type == "segment" {
                let value = makeupList[i]["value"] as? [String] ?? []
                let segmentView = UISegmentedControl(items: value)
                segmentView.selectedSegmentIndex = Int(getDefaultSegmentValueForKey(key: key))
                segmentView.addTarget(self, action: #selector(makeupSegmentAction(_:)), for: .valueChanged)
                valueView = segmentView
            }
            guard let valueView = valueView else {return}
            valueView.translatesAutoresizingMaskIntoConstraints = false
            contentView.addSubview(valueView)
            valueView.tag = i + 1000
            NSLayoutConstraint.activate([
                label.topAnchor.constraint(equalTo: lastLabel?.bottomAnchor ?? contentView.topAnchor, constant: lastLabel == nil ? 10 : 10),
                label.leadingAnchor.constraint(equalTo: contentView.leadingAnchor, constant: 15),
                label.trailingAnchor.constraint(equalTo: valueView.leadingAnchor, constant: -10),
                label.heightAnchor.constraint(equalToConstant: 40),
                valueView.centerYAnchor.constraint(equalTo: label.centerYAnchor),
                valueView.trailingAnchor.constraint(equalTo: contentView.trailingAnchor, constant: -10)
            ])
            lastLabel = label
        }

        let confirmButton = UIButton(type: .system)
        confirmButton.setTitle("Sure".localized, for: .normal)
        confirmButton.translatesAutoresizingMaskIntoConstraints = false
        confirmButton.addTarget(self, action: #selector(confirmAction), for: .touchUpInside)

        alertView.addSubview(confirmButton)

        NSLayoutConstraint.activate([
            confirmButton.topAnchor.constraint(equalTo: alertView.bottomAnchor, constant: -40),
            confirmButton.centerXAnchor.constraint(equalTo: alertView.centerXAnchor)
        ])

        let lastView = lastLabel ?? contentView
        NSLayoutConstraint.activate([
            contentView.bottomAnchor.constraint(equalTo: lastView.bottomAnchor, constant: 10)
        ])

        self.present(customAlertVC, animated: true, completion: nil)
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
        }
        return 0
    }
    
    
    @objc func makeupSliderAction(_ view: UISlider) {
        let index = view.tag - 1000
        let makeupList = beautyManager.makeupList
        let key = makeupList[index]["key"] as? String ?? ""
        let value = view.value
        makeupParams[key] = value

        if let label = view.superview?.viewWithTag(index + 2000) as? UILabel {
            label.text = String(format: "%@[%.f]", makeupList[index]["name"] as? String ?? "none", view.value)
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
        }
    }
    
    @objc func makeupSwitchAction(_ view: UISwitch) {
        let index = view.tag - 1000
        let makeupList = beautyManager.makeupList
        let key = makeupList[index]["key"] as? String ?? ""
        let state = view.isOn
        if state {
            beautyManager.beautyMakeupStyle = "default makeup style".localized
        }
        makeupParams[key] = state
        beautyManager.makeUpEnable = state
    }
    
    @objc func makeupSegmentAction(_ view: UISegmentedControl) {
        let index = view.tag - 1000
        let makeupList = beautyManager.makeupList
        let key = makeupList[index]["key"] as? String ?? ""
        let value = Int32(view.selectedSegmentIndex)
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
        }
    }
    
    @objc func confirmAction() {
        self.dismiss(animated: true, completion: nil)
    }
}

// MARK: face shape settings
extension AgoraBeautyMain {
    @IBAction func onShowFaceShapeAction() {
        let customAlertVC = UIViewController()
        customAlertVC.modalPresentationStyle = .overFullScreen
        customAlertVC.view.backgroundColor = .clear

        let alertView = UIView()
        alertView.translatesAutoresizingMaskIntoConstraints = false
        alertView.backgroundColor = UIColor.white
        alertView.layer.shadowColor = UIColor.black.cgColor
        alertView.layer.shadowOpacity = 0.2
        alertView.layer.shadowOffset = CGSize(width: 0, height: 2)
        alertView.layer.shadowRadius = 4

        customAlertVC.view.addSubview(alertView)

        NSLayoutConstraint.activate([
            alertView.centerXAnchor.constraint(equalTo: customAlertVC.view.centerXAnchor),
            alertView.centerYAnchor.constraint(equalTo: customAlertVC.view.centerYAnchor),
            alertView.widthAnchor.constraint(equalTo: customAlertVC.view.widthAnchor, constant: -20),
            alertView.heightAnchor.constraint(equalToConstant: 300)
        ])

        let scrollView = UIScrollView()
        scrollView.translatesAutoresizingMaskIntoConstraints = false
        alertView.addSubview(scrollView)

        NSLayoutConstraint.activate([
            scrollView.topAnchor.constraint(equalTo: alertView.topAnchor),
            scrollView.leadingAnchor.constraint(equalTo: alertView.leadingAnchor),
            scrollView.trailingAnchor.constraint(equalTo: alertView.trailingAnchor),
            scrollView.bottomAnchor.constraint(equalTo: alertView.bottomAnchor, constant: -50)
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

        var lastLabel: UILabel?
        let beautyShapeList = beautyManager.beautyShapeList
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
                let makeupList = beautyManager.makeupList
                let value = makeupList[i]["value"] as? [Float] ?? []
                let sliderView = UISlider()
                label.text = String(format: "%@[%.f]", label.text ?? "none", sliderView.value)
                sliderView.minimumValue = value.first ?? 0
                sliderView.maximumValue = value.last ?? 100
                sliderView.value = beautyShapeParames[key] ?? 0
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
            
            NSLayoutConstraint.activate([
                label.topAnchor.constraint(equalTo: lastLabel?.bottomAnchor ?? contentView.topAnchor, constant: lastLabel == nil ? 10 : 10),
                label.leadingAnchor.constraint(equalTo: contentView.leadingAnchor, constant: 15),
                label.trailingAnchor.constraint(equalTo: valueView.leadingAnchor, constant: -10),
                label.heightAnchor.constraint(equalToConstant: 40),
                valueView.centerYAnchor.constraint(equalTo: label.centerYAnchor),
                valueView.trailingAnchor.constraint(equalTo: contentView.trailingAnchor, constant: -10)
            ])
            lastLabel = label        }

        let confirmButton = UIButton(type: .system)
        confirmButton.setTitle("Sure".localized, for: .normal)
        confirmButton.translatesAutoresizingMaskIntoConstraints = false
        confirmButton.addTarget(self, action: #selector(confirmAction), for: .touchUpInside)

        alertView.addSubview(confirmButton)

        NSLayoutConstraint.activate([
            confirmButton.topAnchor.constraint(equalTo: alertView.bottomAnchor, constant: -40),
            confirmButton.centerXAnchor.constraint(equalTo: alertView.centerXAnchor)
        ])

        let lastView = lastLabel ?? contentView
        NSLayoutConstraint.activate([
            contentView.bottomAnchor.constraint(equalTo: lastView.bottomAnchor, constant: 10)
        ])

        self.present(customAlertVC, animated: true, completion: nil)
    }
    
    @objc func beautyShapeSliderAction(_ view: UISlider) {
        let index = view.tag - 1000
        let beautyShapeList = beautyManager.beautyShapeList
        let key = beautyShapeList[index]["key"] as? String ?? ""
        beautyShapeParames[key] = view.value
        
        if let label = view.superview?.viewWithTag(index + 2000) as? UILabel {
            label.text = String(format: "%@[%.f]", beautyShapeList[index]["name"] as? String ?? "none", view.value)
        }
        
        beautyManager.setBeauty(key: key, value: view.value)
    }
    
    @objc func beautyShapeSwitchAction(_ view: UISwitch) {
        enableFaceShape = view.isOn
        beautyManager.beautyShapeEnable = enableFaceShape
    }
    
    @objc func beautyShapeSegmentAction(_ view: UISegmentedControl) {
        let style = AgoraFaceShapeStyle(rawValue: UInt(view.selectedSegmentIndex)) ?? .female
        beautyManager.beautyShapeStyle = style == .male ? "Male Template Title".localized : "Female Template Title".localized
    }
}
