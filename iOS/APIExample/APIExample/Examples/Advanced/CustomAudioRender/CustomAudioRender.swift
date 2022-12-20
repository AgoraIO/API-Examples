//
//  CustomAudioSource.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/7/28.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Foundation
import AgoraRtcKit
import AGEVideoLayout

class CustomAudioRenderEntry : UIViewController
{
    @IBOutlet weak var joinButton: AGButton!
    @IBOutlet weak var channelTextField: AGTextField!
    let identifier = "CustomAudioRender"
    
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

class CustomAudioRenderMain: BaseViewController {
    var agoraKit: AgoraRtcEngineKit!
    var exAudio: ExternalAudio = ExternalAudio.shared()

    @IBOutlet weak var container: AGEVideoContainer!
    var audioViews: [UInt:VideoView] = [:]
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false
    
    override func viewDidLoad(){
        super.viewDidLoad()
        
        let sampleRate:UInt = 44100, channel:UInt = 1
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        
        guard let channelName = configs["channelName"] as? String else {return}
        
        // make myself a broadcaster
        agoraKit.setClientRole(GlobalSettings.shared.getUserRole())
        
        // disable video module
        agoraKit.disableVideo()
        agoraKit.enableAudio()
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        // important!! this example is using onPlaybackAudioFrame to do custom rendering
        // by default the audio output will still be processed by SDK hence below api call is mandatory to disable that behavior
//        agoraKit.adjustPlaybackSignalVolume(0)
//        agoraKit.setAudioFrameDelegate(self)
//        agoraKit.setPlaybackAudioFrameParametersWithSampleRate(Int(sampleRate), channel: Int(channel), mode: .readOnly, samplesPerCall: Int(sampleRate*channel)/100)
        
        exAudio.setupExternalAudio(withAgoraKit: agoraKit,
                                   sampleRate: UInt32(sampleRate),
                                   channels: UInt32(channel),
                                   audioCRMode: .sdkCaptureExterRender,
                                   ioType: .remoteIO)
        agoraKit.setParameters("{\"che.audio.external_render\": true}")
        agoraKit.setParameters("{\"che.audio.keep.audiosession\": true}")
        
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
    
    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            // leave channel when exiting the view
            if isJoined {
                agoraKit.disableAudio()
                agoraKit.leaveChannel { (stats) -> Void in
                    LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
                }
            }
        }
    }
}

/// agora rtc engine delegate events
extension CustomAudioRenderMain: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en:https://api-ref.agora.io/en/voice-sdk/ios/3.x/Constants/AgoraWarningCode.html
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
        let view = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
        self.audioViews[uid] = view
        view.setPlaceholder(text: self.getAudioLabel(uid: uid, isLocal: true))
        self.container.layoutStream3x3(views: Array(self.audioViews.values))
    }

    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)

        //set up remote audio view, this view will not show video but just a placeholder
        let view = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
        self.audioViews[uid] = view
        view.setPlaceholder(text: self.getAudioLabel(uid: uid, isLocal: false))
        self.container.layoutStream3x3(views: Array(self.audioViews.values))
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
        self.container.layoutStream3x3(views: Array(self.audioViews.values))
        self.container.reload(level: 0, animated: true)
    }
}
