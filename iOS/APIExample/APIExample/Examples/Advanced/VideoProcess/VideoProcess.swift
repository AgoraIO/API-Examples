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

class VideoProcessEntry : UIViewController
{
    @IBOutlet weak var channelTextField: AGTextField!
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    @IBAction func doJoinPressed(sender: AGButton) {
        guard let channelName = channelTextField.text else {return}
        channelTextField.resignFirstResponder()
        
        let identifier = "VideoProcess"
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else {return}
        newViewController.title = channelName
        newViewController.configs = ["channelName":channelName]
        navigationController?.pushViewController(newViewController, animated: true)
    }
}


class VideoProcessMain : BaseViewController
{
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
    

    var agoraKit: AgoraRtcEngineKit!
    var localVideo = Bundle.loadVideoView(type: .local, audioOnly: false)
    var remoteVideo = Bundle.loadVideoView(type: .remote, audioOnly: false)
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false
    var beautifyOption = AgoraBeautyOptions()
    var skinProtect = 0.5
    var strength = 0.5

    override func viewDidLoad(){
        super.viewDidLoad()
        setupUI()
        
        // get channel name from configs
        guard let channelName = configs["channelName"] as? String,
              let resolution = GlobalSettings.shared.getSetting(key: "resolution")?.selectedOption().value as? CGSize,
              let fps = GlobalSettings.shared.getSetting(key: "fps")?.selectedOption().value as? AgoraVideoFrameRate,
              let orientation = GlobalSettings.shared.getSetting(key: "orientation")?.selectedOption().value as? AgoraVideoOutputOrientationMode else {return}
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        
        // setup log file path
        let logConfig = AgoraLogConfig()
        logConfig.level = .info
        config.logConfig = logConfig
        
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
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
        option.publishCameraTrack = true
        option.publishMicrophoneTrack = true
        option.clientRoleType = GlobalSettings.shared.getUserRole()
        NetworkManager.shared.generateToken(channelName: channelName, success: { token in
            let result = self.agoraKit.joinChannel(byToken: token, channelId: channelName, uid: 0, mediaOptions: option)
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
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
    }
    
    @IBAction func onChangeBeauty(_ sender:UISwitch){
        agoraKit.setBeautyEffectOptions(sender.isOn, options: beautifyOption)
    }

    @IBAction func onLightenSlider(_ sender:UISlider){
        beautifyOption.lighteningLevel = sender.value
        agoraKit.setBeautyEffectOptions(beautySwitch.isOn, options: beautifyOption)
    }
    
    @IBAction func onRednessSlider(_ sender:UISlider){
        beautifyOption.rednessLevel = sender.value
        agoraKit.setBeautyEffectOptions(beautySwitch.isOn, options: beautifyOption)
    }
    
    @IBAction func onSharpnessSlider(_ sender:UISlider){
        beautifyOption.sharpnessLevel = sender.value
        agoraKit.setBeautyEffectOptions(beautySwitch.isOn, options: beautifyOption)
    }
    
    @IBAction func onSmoothSlider(_ sender:UISlider){
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
    
    @IBAction func onStrengthSlider(_ sender:UISlider){
        strength = Double(sender.value)
        let options = AgoraColorEnhanceOptions()
        options.strengthLevel = Float(strength)
        options.skinProtectLevel = Float(skinProtect)
        agoraKit.setColorEnhanceOptions(colorEnhanceSwitch.isOn, options: options)
    }
    
    @IBAction func onSkinProtectSlider(_ sender:UISlider){
        skinProtect = Double(sender.value)
        let options = AgoraColorEnhanceOptions()
        options.strengthLevel = Float(strength)
        options.skinProtectLevel = Float(skinProtect)
        agoraKit.setColorEnhanceOptions(colorEnhanceSwitch.isOn, options: options)
    }
    
    @IBAction func onChangeVirtualBgSwtich(_ sender: UISwitch) {
        changeVirtualBackground()
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
            break
        case 1:
            source.backgroundSourceType = .color
            source.color = 0xFFFFFF
            break
        case 2:
            source.backgroundSourceType = .blur
            source.blurDegree = .high;
            break
        default:
            break
        }
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
    /// en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
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
