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

class JoinChannelAudioTokenEntry : BaseViewController
{
    @IBOutlet weak var containerView: UIView!
    @IBOutlet weak var containerViewYCons: NSLayoutConstraint!
    @IBOutlet weak var joinButton: AGButton!
    @IBOutlet weak var channelTextField: AGTextField!
    @IBOutlet weak var tokenTextField: UITextField!
    @IBOutlet weak var appIdTextField: UITextField!
    @IBOutlet weak var scenarioBtn: UIButton!
    @IBOutlet weak var profileBtn: UIButton!
    var profile:AgoraAudioProfile = .default
    var scenario:AgoraAudioScenario = .default
    let identifier = "JoinChannelAudioToken"
    
    override func viewDidLoad() {
        super.viewDidLoad()

        profileBtn.setTitle("\(profile.description())", for: .normal)
        scenarioBtn.setTitle("\(scenario.description())", for: .normal)
        let tap = UITapGestureRecognizer(target: self, action: #selector(onTapViewHandler))
        view.addGestureRecognizer(tap)
        //注册键盘出现通知
        NotificationCenter.default.addObserver(self, selector: #selector(keyboardWillShow(notification:)),
                                               name:  UIApplication.keyboardWillShowNotification, object: nil)
        
        //注册键盘隐藏通知
        NotificationCenter.default.addObserver(self, selector: #selector(keyboardWillHide(notification:)),
                                               name:  UIApplication.keyboardWillHideNotification, object: nil)
    }
    @IBAction func onTapTipsButton(_ sender: Any) {
        showAlert(title: "Quick input APPID and Token methods".localized,
                  message:
                    "I: the mobile phone and Mac log in to the same Apple account. After copying the Mac, it will automatically synchronize other terminals with the same account. The mobile phone can directly click the input box to paste.\n\n II: use https://cl1p.net/ online clipboard:\n\n1.Enter in a URL that starts with cl1p.net. Example cl1p.net/uqztgjnqcalmd\n\n2.Paste in anything you want.\n\n3.On another computer enter the same URL and get your stuff.".localized,
                  textAlignment: .left
                  )
    }
    
    @objc
    private func onTapViewHandler() {
        view.endEditing(true)
    }
    // 键盘显示
    @objc
    private func keyboardWillShow(notification: Notification) {
        containerViewYCons.constant = -150
        UIView.animate(withDuration: 0.25) {
            self.view.layoutIfNeeded()
        }
    }
    // 键盘隐藏
    @objc
    private func keyboardWillHide(notification: Notification) {
        containerViewYCons.constant = -56
        UIView.animate(withDuration: 0.25, animations: {
            self.view.layoutIfNeeded()
        })
    }
    
    @IBAction func doJoinPressed(sender: AGButton) {
        if let appId = tokenTextField.text, appId.isEmpty {
            ToastView.show(text: "please input AppId!".localized)
            return
        }
        if let token = tokenTextField.text, token.isEmpty {
            ToastView.show(text: "please input Token!".localized)
            return
        }
        if let channelName = channelTextField.text, channelName.isEmpty {
            ToastView.show(text: "please input channel name!".localized)
            return
        }
        //resign channel text field
        view.endEditing(true)
        
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        // create new view controller every time to ensure we get a clean vc
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else {return}
        guard let channelName = channelTextField.text else {return}
        newViewController.title = channelName
        newViewController.configs = ["channelName": channelName,
                                     "audioProfile": profile,
                                     "audioScenario": scenario,
                                     "appId": appIdTextField.text ?? "",
                                     "token": tokenTextField.text ?? ""]
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

class JoinChannelAudioTokenMain: BaseViewController {
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
            let audioScenario = configs["audioScenario"] as? AgoraAudioScenario,
              let appId = configs["appId"] as? String,
              let token = configs["token"] as? String
            else { return }
        
        scenarioBtn.setTitle("\(audioScenario.description())", for: .normal)

        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = appId
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
        
        let result = self.agoraKit.joinChannel(byToken: token, channelId: channelName, uid: 0, mediaOptions: option)
        if result != 0 {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
            // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
        }
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
extension JoinChannelAudioTokenMain: AgoraRtcEngineDelegate {
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
