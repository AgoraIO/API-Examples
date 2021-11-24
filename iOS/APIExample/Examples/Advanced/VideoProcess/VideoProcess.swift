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
    @IBOutlet weak var joinButton: AGButton!
    @IBOutlet weak var channelTextField: AGTextField!
    let identifier = "VideoProcess"
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    @IBAction func doJoinPressed(sender: AGButton) {
        guard let channelName = channelTextField.text else {return}
        //resign channel text field
        channelTextField.resignFirstResponder()
        
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        // create new view controller every time to ensure we get a clean vc
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else {return}
        newViewController.title = channelName
        newViewController.configs = ["channelName":channelName]
        self.navigationController?.pushViewController(newViewController, animated: true)
    }
}

class VideoProcessMain : BaseViewController
{
    var agoraKit: AgoraRtcEngineKit!
    @IBOutlet weak var container: AGEVideoContainer!
    var localVideo = Bundle.loadVideoView(type: .local, audioOnly: false)
    var remoteVideo = Bundle.loadVideoView(type: .remote, audioOnly: false)
    
    @IBOutlet weak var beauty: UISwitch!
    @IBOutlet weak var lightness: UISwitch!
    @IBOutlet weak var colorful: UISwitch!
    @IBOutlet weak var denoiser: UISwitch!
    
    @IBOutlet weak var lightenSlider: UISlider!
    @IBOutlet weak var rednessSlider: UISlider!
    @IBOutlet weak var sharpnessSlider: UISlider!
    @IBOutlet weak var smoothSlider: UISlider!
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false
    var beautifyOption = AgoraBeautyOptions()
    var skinProtect = 1.0
    var strength = 0.5
    
    override func viewDidLoad(){
        super.viewDidLoad()
        
        guard let channelName = configs["channelName"] as? String
            else { return }
        // layout render view
        localVideo.setPlaceholder(text: "Local Host".localized)
        remoteVideo.setPlaceholder(text: "Remote Host".localized)
        container.layoutStream(views: [localVideo, remoteVideo])
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        // set audio scenario
        config.audioScenario = .default
        
        // set audio filter extension
        config.eventDelegate = self
        
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        
        agoraKit.enableExtension(withVendor: "agora", extension: "beauty", enabled: true)
        
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        
        // make myself a broadcaster
        agoraKit.setClientRole(.broadcaster)
        
        // enable video module
        agoraKit.enableVideo()

        // set up local video to render your local camera preview
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = 0
        // the view to be binded
        videoCanvas.view = localVideo.videoView
        videoCanvas.renderMode = .hidden
        agoraKit.setupLocalVideo(videoCanvas)
        // you have to call startPreview to see local video
        agoraKit.startPreview()
        
        // set audio profile
        agoraKit.setAudioProfile(.default)
        
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        let option = AgoraRtcChannelMediaOptions()
        option.publishCameraTrack = .of(true)
        option.clientRoleType = .of((Int32)(AgoraClientRole.broadcaster.rawValue))
        
        let result = agoraKit.joinChannel(byToken: KeyCenter.Token, channelId: channelName, uid: 0, mediaOptions: option)
        if result != 0 {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
        }
    }
    
    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            // leave channel when exiting the view
            if isJoined {
                agoraKit.leaveChannel { (stats) -> Void in
                    LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
                }
            }
        }
    }
    
    @IBAction func onLightenSlider(_ sender:UISlider){
        beautifyOption.lighteningLevel = sender.value
        agoraKit.setBeautyEffectOptions(beauty.isOn, options: beautifyOption)
    }
    @IBAction func onRednessSlider(_ sender:UISlider){
        beautifyOption.rednessLevel = sender.value
        agoraKit.setBeautyEffectOptions(beauty.isOn, options: beautifyOption)
    }
    @IBAction func onSharpnessSlider(_ sender:UISlider){
        beautifyOption.sharpnessLevel = sender.value
        agoraKit.setBeautyEffectOptions(beauty.isOn, options: beautifyOption)
    }
    @IBAction func onSmoothSlider(_ sender:UISlider){
        beautifyOption.smoothnessLevel = sender.value
        agoraKit.setBeautyEffectOptions(beauty.isOn, options: beautifyOption)
    }
    @IBAction func onSkinProtectSlider(_ sender:UISlider){
        skinProtect = Double(sender.value)
        setColorEnhance()
    }
    @IBAction func onStrengthSlider(_ sender:UISlider){
        strength = Double(sender.value)
        setColorEnhance()
    }
    
    @IBAction func onChangeBeauty(_ sender:UISwitch){
        agoraKit.setBeautyEffectOptions(sender.isOn, options: beautifyOption)
    }
    
    /**
     * level:
     * 0 (default), better quality
     * 1, better performance
     * mode:
     * 0 (default), auto mode, sdk determine enable/disable according to environment.
     * 1, force enable.
     */
    @IBAction func onChangeLightness(_ sender:UISwitch){
        let jsonbody : [String : Any] = [
            "enable" : sender.isOn ? 1 : 0,
            "level" : 1,
            "mode" : 0,
            ]
        guard let data = try? JSONSerialization.data(withJSONObject: jsonbody, options: .prettyPrinted) else { return }
        agoraKit.setExtensionPropertyWithVendor("agora", extension: "beauty", key: "lowlight_enhance_option", value: String(data:data,encoding:.utf8)!)
    }
    /**
     * strength: [0.0, 1.0]
     * color strength
     * skinProtect: [0.0, 1.0]
     * higher skinProtect value, less impact for skin color.
     */
    @IBAction func onChangeColorful(_ sender:UISwitch){
        setColorEnhance()
    }
    
    func setColorEnhance(){
            let jsonbody : [String : Any] = [
                "enable" : colorful.isOn ? 1 : 0,
                "strength" : strength,
                "skinProtect" : skinProtect,
                ]
            guard let data = try? JSONSerialization.data(withJSONObject: jsonbody, options: .prettyPrinted) else { return }
            agoraKit.setExtensionPropertyWithVendor("agora", extension: "beauty", key: "color_enhance_option", value: String(data:data,encoding:.utf8)!)
    }
    
    /**
     * level:
     * 0 (default), medium denoise level,
     * 1, fast denoise, for fixed camera scenario
     * 2, best denoise level, for high video noise scenario
     * mode:
     * 0 (default), auto mode, sdk determine enable/disable according to environment.
     * 1, force enable.
     */
    @IBAction func onChangeDenoiser(_ sender:UISwitch){
        let jsonbody : [String : Any] = [
            "enable" : sender.isOn ? 1 : 0,
            "level" : 0,
            "mode" : 0,
            ]
        guard let data = try? JSONSerialization.data(withJSONObject: jsonbody, options: .prettyPrinted) else { return }
        agoraKit.setExtensionPropertyWithVendor("agora", extension: "beauty", key: "video_denoiser_option", value: String(data:data,encoding:.utf8)!)
        
    }
}

extension VideoProcessMain: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.description)", level: .warning)
    }
    
    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode)", level: .error)
        self.showAlert(title: "Error", message: "Error \(errorCode.description) occur")
    }
    
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
    
    /// Reports which users are speaking, the speakers' volumes, and whether the local user is speaking.
    /// @params speakers volume info for all speakers
    /// @params totalVolume Total volume after audio mixing. The value range is [0,255].
    func rtcEngine(_ engine: AgoraRtcEngineKit, reportAudioVolumeIndicationOfSpeakers speakers: [AgoraRtcAudioVolumeInfo], totalVolume: Int) {
//        for speaker in speakers {
//            if let audioView = audioViews[speaker.uid] {
//                audioView.setInfo(text: "Volume:\(speaker.volume)")
//            }
//        }
    }
    
    /// Reports the statistics of the current call. The SDK triggers this callback once every two seconds after the user joins the channel.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, reportRtcStats stats: AgoraChannelStats) {
        localVideo.statsInfo?.updateChannelStats(stats)
    }
    
    /// Reports the statistics of the uploading local video streams once every two seconds.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, localVideoStats stats: AgoraRtcLocalVideoStats) {
        localVideo.statsInfo?.updateLocalVideoStats(stats)
    }
    
    /// Reports the statistics of the uploading local audio streams once every two seconds.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, localAudioStats stats: AgoraRtcLocalAudioStats) {
        localVideo.statsInfo?.updateLocalAudioStats(stats)
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

extension VideoProcessMain: AgoraMediaFilterEventDelegate{
    func onEvent(_ vendor: String?, extension: String?, key: String?, json_value: String?) {
        LogUtils.log(message: "onEvent: \(String(describing: key)) \(String(describing: json_value))", level: .info)
    }
}
