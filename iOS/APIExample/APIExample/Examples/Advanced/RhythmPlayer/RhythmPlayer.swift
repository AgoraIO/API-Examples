//
//  RhythmPlayer.swift
//  APIExample
//
//  Created by xianing on 2021/11/23.
//  Copyright © 2021 Agora Corp. All rights reserved.
//

import UIKit
import AgoraRtcKit
import AGEVideoLayout

let DING = "https://webdemo.agora.io/ding.mp3"
let DANG = "https://webdemo.agora.io/dang.mp3"

class RhythmPlayerEntry : UIViewController
{
    @IBOutlet weak var joinButton: AGButton!
    @IBOutlet weak var channelTextField: AGTextField!
    let identifier = "RhythmPlayer"
    
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
        navigationController?.pushViewController(newViewController, animated: true)
    }
}

class RhythmPlayerMain : BaseViewController
{
    var agoraKit: AgoraRtcEngineKit!
    @IBOutlet weak var container: AGEVideoContainer!
    var localView = Bundle.loadVideoView(type: .local, audioOnly: true)
    var remoteView = Bundle.loadVideoView(type: .remote, audioOnly: true)
    
    @IBOutlet weak var play: UIButton!
    @IBOutlet weak var stop: UIButton!
    @IBOutlet weak var beatsMinute: UISlider!
    @IBOutlet weak var beatsMeasure: UISlider!
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false
    var config: AgoraRhythmPlayerConfig = AgoraRhythmPlayerConfig()
    
    override func viewDidLoad(){
        super.viewDidLoad()
        
        guard let channelName = configs["channelName"] as? String
            else { return }
        // layout render view
        container.layoutStream(views: [localView, remoteView])
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        // set audio scenario
        config.audioScenario = .default
        
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        
        agoraKit.disableVideo()
        agoraKit.enableAudio()
        
        // make myself a broadcaster
        agoraKit.setClientRole(GlobalSettings.shared.getUserRole())
        
        // set audio profile
        agoraKit.setAudioProfile(.default)
        
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        // Set default RhythmPlayerConfig
        
        
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        let option = AgoraRtcChannelMediaOptions()
        option.publishCustomAudioTrack = true
        option.publishRhythmPlayerTrack = true
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
                agoraKit.stopRhythmPlayer()
                agoraKit.leaveChannel { (stats) -> Void in
                    LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
                }
            }
        }
    }
    
    @IBAction func onPlay(_ sender: UIButton){
        agoraKit.startRhythmPlayer(DING, sound2: DANG, config: config)
        beatsMinute.isEnabled = false
        beatsMeasure.isEnabled = false
    }
    
    @IBAction func onStop(_ sender: UIButton){
        agoraKit.stopRhythmPlayer()
        beatsMinute.isEnabled = true
        beatsMeasure.isEnabled = true
    }
    
    @IBAction func onChangeBeatsMinute(_ sender: UISlider){
        config.beatsPerMinute = Int32(sender.value)
    }
    
    @IBAction func onChangeBeatsMeasure(_ sender: UISlider){
        config.beatsPerMeasure = Int32(sender.value)
    }
}

extension RhythmPlayerMain : AgoraRtcEngineDelegate {
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
        localView.setPlaceholder(text: self.getAudioLabel(uid: uid, isLocal: true))
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        remoteView.setPlaceholder(text: self.getAudioLabel(uid: uid, isLocal: false))
    }
    
    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)
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
        localView.statsInfo?.updateChannelStats(stats)
    }
    
    /// Reports the statistics of the uploading local audio streams once every two seconds.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, localAudioStats stats: AgoraRtcLocalAudioStats) {
        localView.statsInfo?.updateLocalAudioStats(stats)
    }
    
    /// Reports the statistics of the audio stream from each remote user/host.
    /// @param stats stats struct for current call statistics
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteAudioStats stats: AgoraRtcRemoteAudioStats) {
        remoteView.statsInfo?.updateAudioStats(stats)
    }
}
