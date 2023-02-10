//
//  JoinChannelAudioMain.swift
//  APIExample
//
//  Created by ADMIN on 2020/5/18.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import UIKit
import AgoraRtcKit
import AGEVideoLayout

class JoinChannelAudioEntry : UIViewController
{
    @IBOutlet weak var joinButton: AGButton!
    @IBOutlet weak var channelTextField: AGTextField!
    @IBOutlet weak var scenarioBtn: UIButton!
    @IBOutlet weak var profileBtn: UIButton!
    var profile:AgoraAudioProfile = .default
    var scenario:AgoraAudioScenario = .default
    let identifier = "JoinChannelAudio"
    
    override func viewDidLoad() {
        super.viewDidLoad()

        profileBtn.setTitle("\(profile.description())", for: .normal)
        scenarioBtn.setTitle("\(scenario.description())", for: .normal)
    }
    
    @IBAction func doJoinPressed(sender: AGButton) {
        guard let channelName = channelTextField.text else {return}
        //resign channel text field
        channelTextField.resignFirstResponder()
        
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        // create new view controller every time to ensure we get a clean vc
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else {return}
        newViewController.title = channelName
        newViewController.configs = ["channelName":channelName, "audioProfile":profile, "audioScenario":scenario]
        navigationController?.pushViewController(newViewController, animated: true)
    }
    
    func getAudioProfileAction(_ profile:AgoraAudioProfile) -> UIAlertAction{
        return UIAlertAction(title: "\(profile.description())", style: .default, handler: {[unowned self] action in
            self.profile = profile
            self.profileBtn.setTitle("\(profile.description())", for: .normal)
        })
    }
    
    func getAudioScenarioAction(_ scenario:AgoraAudioScenario) -> UIAlertAction{
        return UIAlertAction(title: "\(scenario.description())", style: .default, handler: {[unowned self] action in
            self.scenario = scenario
            self.scenarioBtn.setTitle("\(scenario.description())", for: .normal)
        })
    }
    
    @IBAction func setAudioProfile(){
        let alert = UIAlertController(title: "Set Audio Profile".localized, message: nil, preferredStyle: UIDevice.current.userInterfaceIdiom == .pad ? UIAlertController.Style.alert : UIAlertController.Style.actionSheet)
        for profile in AgoraAudioProfile.allValues(){
            alert.addAction(getAudioProfileAction(profile))
        }
        alert.addCancelAction()
        present(alert, animated: true, completion: nil)
    }
    
    @IBAction func setAudioScenario(){
        let alert = UIAlertController(title: "Set Audio Scenario".localized, message: nil, preferredStyle: UIDevice.current.userInterfaceIdiom == .pad ? UIAlertController.Style.alert : UIAlertController.Style.actionSheet)
        for scenario in AgoraAudioScenario.allValues(){
            alert.addAction(getAudioScenarioAction(scenario))
        }
        alert.addCancelAction()
        present(alert, animated: true, completion: nil)
    }
}

class JoinChannelAudioMain: BaseViewController {
    var agoraKit: AgoraRtcEngineKit!
    @IBOutlet weak var container: AGEVideoContainer!
    @IBOutlet weak var recordingVolumeSlider: UISlider!
    @IBOutlet weak var playbackVolumeSlider: UISlider!
    @IBOutlet weak var inEarMonitoringSwitch: UISwitch!
    @IBOutlet weak var inEarMonitoringVolumeSlider: UISlider!
    @IBOutlet weak var scenarioBtn: UIButton!
        
    var audioViews: [UInt:VideoView] = [:]
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false
    
    override func viewDidLoad(){
        super.viewDidLoad()
        
        guard let channelName = configs["channelName"] as? String,
            let audioProfile = configs["audioProfile"] as? AgoraAudioProfile,
            let audioScenario = configs["audioScenario"] as? AgoraAudioScenario
            else { return }
        
        scenarioBtn.setTitle("\(audioScenario.description())", for: .normal)

        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        // set audio scenario
        config.audioScenario = audioScenario
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        
        // make myself a broadcaster
        agoraKit.setClientRole(GlobalSettings.shared.getUserRole())
        
        // disable video module
        agoraKit.disableVideo()
        
        agoraKit.enableAudio()
        
        let resolution = (GlobalSettings.shared.getSetting(key: "resolution")?.selectedOption().value as? CGSize) ?? .zero
        let fps = (GlobalSettings.shared.getSetting(key: "fps")?.selectedOption().value as? AgoraVideoFrameRate) ?? .fps15
        let orientation = (GlobalSettings.shared.getSetting(key: "orientation")?.selectedOption().value as? AgoraVideoOutputOrientationMode) ?? .fixedPortrait
        agoraKit.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: resolution,
                                                                             frameRate: fps,
                                                                             bitrate: AgoraVideoBitrateStandard,
                                                                             orientationMode: orientation,
                                                                             mirrorMode: .auto))
        
        // set audio profile
        agoraKit.setAudioProfile(audioProfile)
        
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        // enable volume indicator
        agoraKit.enableAudioVolumeIndication(200, smooth: 3, reportVad: true)
        
        recordingVolumeSlider.maximumValue = 400
        recordingVolumeSlider.minimumValue = 0
        recordingVolumeSlider.integerValue = 100
        
        playbackVolumeSlider.maximumValue = 400
        playbackVolumeSlider.minimumValue = 0
        playbackVolumeSlider.integerValue = 100
        
        inEarMonitoringVolumeSlider.maximumValue = 100
        inEarMonitoringVolumeSlider.minimumValue = 0
        inEarMonitoringVolumeSlider.integerValue = 100
        
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        let option = AgoraRtcChannelMediaOptions()
        option.publishCameraTrack = false
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
    
    override func viewDidDisappear(_ animated: Bool) {
        super.viewDidDisappear(animated)
        // 关闭耳返
        agoraKit.enable(inEarMonitoring: false)
        agoraKit.disableAudio()
        agoraKit.disableVideo()
        if isJoined {
            agoraKit.leaveChannel { (stats) -> Void in
                LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
            }
        }
    }
    
    func sortedViews() -> [VideoView] {
        return Array(audioViews.values).sorted(by: { $0.uid < $1.uid })
    }
    @IBAction func setAudioScenario(_ sender: Any) {
        let alert = UIAlertController(title: "Set Audio Scenario".localized, message: nil, preferredStyle: UIDevice.current.userInterfaceIdiom == .pad ? UIAlertController.Style.alert : UIAlertController.Style.actionSheet)
        for scenario in AgoraAudioScenario.allValues(){
            alert.addAction(getAudioScenarioAction(scenario))
        }
        alert.addCancelAction()
        present(alert, animated: true, completion: nil)
    }
    
    func getAudioScenarioAction(_ scenario:AgoraAudioScenario) -> UIAlertAction{
        return UIAlertAction(title: "\(scenario.description())", style: .default, handler: {[unowned self] action in
            self.agoraKit.setAudioScenario(scenario)
            self.scenarioBtn.setTitle("\(scenario.description())", for: .normal)
        })
    }
    
    @IBAction func onChangeRecordingVolume(_ sender:UISlider){
        let value:Int = Int(sender.value)
        print("adjustRecordingSignalVolume \(value)")
        agoraKit.adjustRecordingSignalVolume(value)
    }
    
    @IBAction func onChangePlaybackVolume(_ sender:UISlider){
        let value:Int = Int(sender.value)
        print("adjustPlaybackSignalVolume \(value)")
        agoraKit.adjustPlaybackSignalVolume(value)
    }
    
    @IBAction func toggleInEarMonitoring(_ sender:UISwitch){
        inEarMonitoringVolumeSlider.isEnabled = sender.isOn
        agoraKit.enable(inEarMonitoring: sender.isOn)
    }
    
    @IBAction func onChangeInEarMonitoringVolume(_ sender:UISlider){
        let value:Int = Int(sender.value)
        print("setInEarMonitoringVolume \(value)")
        agoraKit.setInEarMonitoringVolume(value)
    }
}

/// agora rtc engine delegate events
extension JoinChannelAudioMain: AgoraRtcEngineDelegate {
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
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        self.isJoined = true
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
        
        //set up local audio view, this view will not show video but just a placeholder
        let view = Bundle.loadVideoView(type: .local, audioOnly: true)
        self.audioViews[0] = view
        view.setPlaceholder(text: self.getAudioLabel(uid: uid, isLocal: true))
        self.container.layoutStream3x2(views: self.sortedViews())
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)

        //set up remote audio view, this view will not show video but just a placeholder
        let view = Bundle.loadVideoView(type: .remote, audioOnly: true)
        view.uid = uid
        self.audioViews[uid] = view
        view.setPlaceholder(text: self.getAudioLabel(uid: uid, isLocal: false))
        self.container.layoutStream3x2(views: sortedViews())
        self.container.reload(level: 0, animated: true)
    }
    
    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)
        
        //remove remote audio view
        self.audioViews.removeValue(forKey: uid)
        self.container.layoutStream3x2(views: sortedViews())
        self.container.reload(level: 0, animated: true)
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
        audioViews[0]?.statsInfo?.updateChannelStats(stats)
    }
    
    /// Reports the statistics of the uploading local audio streams once every two seconds.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, localAudioStats stats: AgoraRtcLocalAudioStats) {
        audioViews[0]?.statsInfo?.updateLocalAudioStats(stats)
    }
    
    /// Reports the statistics of the audio stream from each remote user/host.
    /// @param stats stats struct for current call statistics
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteAudioStats stats: AgoraRtcRemoteAudioStats) {
        audioViews[stats.uid]?.statsInfo?.updateAudioStats(stats)
    }
}
