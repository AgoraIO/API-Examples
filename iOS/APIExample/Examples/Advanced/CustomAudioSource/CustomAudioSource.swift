//
//  CustomAudioSourceMain.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/7/28.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Foundation
import AgoraRtcKit
import AGEVideoLayout
import AVFoundation

class CustomAudioSourceEntry : UIViewController
{
    @IBOutlet weak var joinButton: AGButton!
    @IBOutlet weak var channelTextField: AGTextField!
    let identifier = "CustomAudioSource"
    
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

class CustomAudioSourceMain: BaseViewController {
    var agoraKit: AgoraRtcEngineKit!
    var exAudio: ExternalAudio = ExternalAudio.shared()
    var pcmSourcePush: AgoraPcmSourcePush?
    @IBOutlet weak var container: AGEVideoContainer!
    var audioViews: [UInt:VideoView] = [:]
    @IBOutlet weak var mic: UISwitch!

    // indicate if current instance has joined channel
    var isJoined: Bool = false
    
    override func viewDidLoad(){
        super.viewDidLoad()
        
        let sampleRate:UInt = 44100, channel:UInt = 2
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        
        guard let channelName = configs["channelName"] as? String,
              let filepath = Bundle.main.path(forResource: "output", ofType: "raw") else {
            return
        }
        
        // make myself a broadcaster
        agoraKit.setClientRole(.broadcaster)
        
        // disable video module
        agoraKit.disableVideo()
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        // setup external audio source
        exAudio.setupExternalAudio(withAgoraKit: agoraKit, sampleRate: UInt32(sampleRate), channels: UInt32(channel), audioCRMode: .exterCaptureSDKRender, ioType: .remoteIO)
        // MIGRATED
        agoraKit.setExternalAudioSource(true, sampleRate: Int(sampleRate), channels: Int(channel))
        //agoraKit.setExternalAudioSource(true, sampleRate: Int(sampleRate), channels: Int(channel), sourceNumber: 2, localPlayback: true, publish: true)
        
        pcmSourcePush = AgoraPcmSourcePush(delegate: self, filePath: filepath, sampleRate: Int(sampleRate), channelsPerFrame: Int(channel), bitPerSample: 16, samples: 441 * 20)
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        let option = AgoraRtcChannelMediaOptions()
        option.publishCameraTrack = false
        option.publishAudioTrack = false
        option.publishCustomAudioTrack = true
        option.clientRoleType = .broadcaster
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
                exAudio.stopWork()
                agoraKit.leaveChannel { (stats) -> Void in
                    LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
                }
            }
            
            if let channelName = configs["channelName"] as? String {
                if pcmConnectionId != 0 {
                    agoraKit.leaveChannelEx(channelName, connectionId: pcmConnectionId)
                    pcmConnectionId = 0
                }
            }
        }
    }
    
    var pcmConnectionId: UInt32 = 0
    
    @IBAction func pushPCM(_ sender: UISwitch) {
        if pcmConnectionId == 0 && sender.isOn {
            LogUtils.log(message: "pushPCM", level: .info)
            guard let channelName = configs["channelName"] as? String else {
                return
            }
            let option = AgoraRtcChannelMediaOptions()
            option.publishCameraTrack = false
            option.publishAudioTrack = false
            option.publishCustomAudioTrack = true
            option.enableAudioRecordingOrPlayout = false
            option.clientRoleType = .broadcaster
            let delegate = EventListener(main: self)
            let result = agoraKit.joinChannelEx(byToken: KeyCenter.Token, channelId: channelName, uid: SCREEN_SHARE_UID,
                                                connectionId: &pcmConnectionId, delegate: delegate, mediaOptions: option)
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                self.showAlert(title: "Error", message: "joinChannelEx call failed: \(result), please check your params")
            }
        } else if pcmConnectionId != 0 && !sender.isOn {
            guard let channelName = configs["channelName"] as? String else {
                return
            }
            pcmSourcePush?.stop()
            agoraKit.leaveChannelEx(channelName, connectionId: pcmConnectionId)
            pcmConnectionId = 0
        }
    }
}

class EventListener: NSObject, AgoraRtcEngineDelegate {
    
    weak var main: CustomAudioSourceMain?
    
    init(main: CustomAudioSourceMain) {
        self.main = main
    }
    
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
        self.main?.showAlert(title: "Error", message: "Error \(errorCode.description) occur")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        main?.pcmSourcePush?.start()
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didLeaveChannelWith stats: AgoraChannelStats) {
        main?.pcmSourcePush?.stop()
    }
}

extension CustomAudioSourceMain: AgoraPcmSourcePushDelegate {
    func onAudioFrame(data: Data) {
        agoraKit.pushExternalAudioFrameExNSData(data, sourceId: 1, timestamp: 0, connectionId: pcmConnectionId)
    }
}

/// agora rtc engine delegate events
extension CustomAudioSourceMain: AgoraRtcEngineDelegate {
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
        let sampleRate:UInt = 44100
        try? AVAudioSession.sharedInstance().setPreferredSampleRate(Double(sampleRate))
        self.exAudio.startWork()
        
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
        if uid == SCREEN_SHARE_UID {
            LogUtils.log(message: "Ignore pcm play uid", level: .info)
            return
        }
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
