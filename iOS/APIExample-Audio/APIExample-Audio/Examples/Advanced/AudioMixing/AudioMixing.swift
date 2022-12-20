//
//  AudioMixingMain.swift
//  APIExample
//
//  Created by ADMIN on 2020/5/18.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import UIKit
import AgoraRtcKit
import AGEVideoLayout

let EFFECT_ID:Int32 = 1

class AudioMixingEntry : UIViewController
{
    @IBOutlet weak var joinButton: AGButton!
    @IBOutlet weak var channelTextField: AGTextField!
    @IBOutlet weak var scenarioBtn: UIButton!
    @IBOutlet weak var profileBtn: UIButton!
    var profile:AgoraAudioProfile = .default
    var scenario:AgoraAudioScenario = .default
    let identifier = "AudioMixing"
    
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

class AudioMixingMain: BaseViewController {
    var agoraKit: AgoraRtcEngineKit!
    @IBOutlet weak var container: AGEVideoContainer!
    @IBOutlet weak var audioMixingVolumeSlider: UISlider!
    @IBOutlet weak var audioMixingPlaybackVolumeSlider: UISlider!
    @IBOutlet weak var audioMixingPublishVolumeSlider: UISlider!
    @IBOutlet weak var audioMixingProgressView: UIProgressView!
    @IBOutlet weak var audioMixingDuration: UILabel!
    @IBOutlet weak var audioEffectVolumeSlider: UISlider!
    var audioViews: [UInt:VideoView] = [:]
    var timer:Timer?
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false
    
    override func viewDidLoad(){
        super.viewDidLoad()
        
        guard let channelName = configs["channelName"] as? String,
            let audioProfile = configs["audioProfile"] as? AgoraAudioProfile,
            let audioScenario = configs["audioScenario"] as? AgoraAudioScenario
            else {return}
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        config.audioScenario = audioScenario
        
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        
        // make myself a broadcaster
        agoraKit.setClientRole(GlobalSettings.shared.getUserRole())
        
        // update slider values
        audioMixingPlaybackVolumeSlider.setValue(Float(agoraKit.getAudioMixingPlayoutVolume()), animated: true)
        audioMixingPublishVolumeSlider.setValue(Float(agoraKit.getAudioMixingPublishVolume()), animated: true)
        audioEffectVolumeSlider.setValue(Float(agoraKit.getEffectsVolume()), animated: true)
        
        // disable video module
        agoraKit.disableVideo()
        agoraKit.enableAudio()
        
        // set audio profile/audio scenario
        agoraKit.setAudioProfile(audioProfile)
        
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        // enable volume indicator
        agoraKit.enableAudioVolumeIndication(200, smooth: 3, reportVad: false)
        
        
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        NetworkManager.shared.generateToken(channelName: channelName, success: { token in
            let result = self.agoraKit.joinChannel(byToken: token, channelId: channelName, info: nil, uid: 0) {[unowned self] (channel, uid, elapsed) -> Void in
                self.isJoined = true
                LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
                
                //set up local audio view, this view will not show video but just a placeholder
                let view = Bundle.loadVideoView(type: .local, audioOnly: true)
                self.audioViews[0] = view
                view.setPlaceholder(text: self.getAudioLabel(uid: uid, isLocal: true))
                self.container.layoutStream2x1(views: self.sortedViews())
            }
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
        }
    }
    
    func sortedViews() -> [VideoView] {
        return Array(audioViews.values).sorted(by: { $0.uid < $1.uid })
    }
    
    @IBAction func onChangeAudioMixingVolume(_ sender:UISlider){
        let value:Int = Int(sender.value)
        print("adjustAudioMixingVolume \(value)")
        agoraKit.adjustAudioMixingVolume(value)
    }
    
    @IBAction func onChangeAudioMixingPlaybackVolume(_ sender:UISlider){
        let value:Int = Int(sender.value)
        print("adjustAudioMixingPlayoutVolume \(value)")
        agoraKit.adjustAudioMixingPlayoutVolume(value)
    }
    
    @IBAction func onChangeAudioMixingPublishVolume(_ sender:UISlider){
        let value:Int = Int(sender.value)
        print("adjustAudioMixingPublishVolume \(value)")
        agoraKit.adjustAudioMixingPublishVolume(value)
    }
    
    @IBAction func onChangeAudioEffectVolume(_ sender:UISlider){
        let value:Int = Int(sender.value)
        print("setEffectsVolume \(value)")
        agoraKit.setEffectsVolume(value)
    }
    
    @IBAction func onStartAudioMixing(_ sender:UIButton){
        if let filepath = Bundle.main.path(forResource: "audiomixing", ofType: "mp3") {
            let result = agoraKit.startAudioMixing(filepath, loopback: false, cycle: -1)
            if result != 0 {
                self.showAlert(title: "Error", message: "startAudioMixing call failed: \(result), please check your params")
            }
        }
    }
    
    @IBAction func onStopAudioMixing(_ sender:UIButton){
        let result = agoraKit.stopAudioMixing()
        if result != 0 {
            self.showAlert(title: "Error", message: "stopAudioMixing call failed: \(result), please check your params")
        } else {
            stopProgressTimer()
            updateTotalDuration(reset: true)
        }
    }
    
    @IBAction func onPauseAudioMixing(_ sender:UIButton){
        let result = agoraKit.pauseAudioMixing()
        if result != 0 {
            self.showAlert(title: "Error", message: "pauseAudioMixing call failed: \(result), please check your params")
        } else {
            stopProgressTimer()
        }
    }
    
    @IBAction func onResumeAudioMixing(_ sender:UIButton){
        let result = agoraKit.resumeAudioMixing()
        if result != 0 {
            self.showAlert(title: "Error", message: "resumeAudioMixing call failed: \(result), please check your params")
        } else {
            startProgressTimer()
        }
    }
    
    func startProgressTimer() {
        // begin timer to update progress
        if(timer == nil) {
            timer = Timer.scheduledTimer(withTimeInterval: 0.5, repeats: true, block: { [weak self](timer:Timer) in
                guard let weakself = self else {return}
                let progress = Float(weakself.agoraKit.getAudioMixingCurrentPosition()) / Float(weakself.agoraKit.getAudioMixingDuration())
                weakself.audioMixingProgressView.setProgress(progress, animated: true)
            })
        }
    }
    
    func stopProgressTimer() {
        // stop timer
        if(timer != nil) {
            timer?.invalidate()
            timer = nil
        }
    }
    
    func updateTotalDuration(reset:Bool) {
        if(reset) {
            audioMixingDuration.text = "00 : 00"
        } else {
            let duration = agoraKit.getAudioMixingDuration()
            let seconds = duration / 1000
            audioMixingDuration.text = "\(String(format: "%02d", seconds / 60)) : \(String(format: "%02d", seconds % 60))"
        }
    }
    
    @IBAction func onPlayEffect(_ sender:UIButton){
        if let filepath = Bundle.main.path(forResource: "audioeffect", ofType: "mp3") {
            let result = agoraKit.playEffect(EFFECT_ID, filePath: filepath, loopCount: -1, pitch: 1, pan: 0, gain: 100, publish: true)
            if result != 0 {
                self.showAlert(title: "Error", message: "playEffect call failed: \(result), please check your params")
            }
        }
    }
    
    @IBAction func onStopEffect(_ sender:UIButton){
        let result = agoraKit.stopEffect(EFFECT_ID)
        if result != 0 {
            self.showAlert(title: "Error", message: "stopEffect call failed: \(result), please check your params")
        }
    }
    
    @IBAction func onPauseEffect(_ sender:UIButton){
        let result = agoraKit.pauseEffect(EFFECT_ID)
        if result != 0 {
            self.showAlert(title: "Error", message: "pauseEffect call failed: \(result), please check your params")
        }
    }
    
    @IBAction func onResumeEffect(_ sender:UIButton){
        let result = agoraKit.resumeEffect(EFFECT_ID)
        if result != 0 {
            self.showAlert(title: "Error", message: "resumeEffect call failed: \(result), please check your params")
        }
    }
}

/// agora rtc engine delegate events
extension AudioMixingMain: AgoraRtcEngineDelegate {
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
        self.container.layoutStream2x1(views: sortedViews())
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
        self.container.layoutStream2x1(views: sortedViews())
        self.container.reload(level: 0, animated: true)
    }
    
    /// Reports which users are speaking, the speakers' volumes, and whether the local user is speaking.
    /// @params speakers volume info for all speakers
    /// @params totalVolume Total volume after audio mixing. The value range is [0,255].
    func rtcEngine(_ engine: AgoraRtcEngineKit, reportAudioVolumeIndicationOfSpeakers speakers: [AgoraRtcAudioVolumeInfo], totalVolume: Int) {
//        for volumeInfo in speakers {
//            if let audioView = audioViews[volumeInfo.uid] {
//                audioView.setInfo(text: "Volume:\(volumeInfo.volume)")
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
    func rtcEngine(_ engine: AgoraRtcEngineKit, audioMixingStateChanged state: AgoraAudioMixingStateType, reasonCode: AgoraAudioMixingReasonCode) {
        LogUtils.log(message: "audioMixingStateChanged \(state.rawValue), code: \(reasonCode.rawValue)", level: .info)
        if state == .playing {
            startProgressTimer()
            updateTotalDuration(reset: false)
        }
    }
}
