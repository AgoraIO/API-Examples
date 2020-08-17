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

class CustomAudioRender: BaseViewController {
    var agoraKit: AgoraRtcEngineKit!
    var exAudio: ExternalAudio = ExternalAudio.shared()
    @IBOutlet var container: AGEVideoContainer!
    var audioViews: [UInt:VideoView] = [:]
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false
    
    override func viewDidLoad(){
        super.viewDidLoad()
        
        let sampleRate:UInt = 44100, channel:UInt = 1, sourceNumber:UInt = 2
        
        // set up agora instance when view loaded
        agoraKit = AgoraRtcEngineKit.sharedEngine(withAppId: KeyCenter.AppId, delegate: self)
        
        guard let channelName = configs["channelName"] as? String else {return}
        
        // disable video module
        agoraKit.disableVideo()
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        // set live broadcaster to send stream
        agoraKit.setChannelProfile(.liveBroadcasting)
        agoraKit.setClientRole(.broadcaster)
        
        // setup external audio source
        exAudio.setupExternalAudio(withAgoraKit: agoraKit, sampleRate: UInt32(sampleRate), channels: UInt32(channel), audioCRMode: .sdkCaptureExterRender, ioType: .remoteIO, sourceNumber: Int32(sourceNumber))
        // important!! this example is using onPlaybackAudioFrame to do custom rendering
        // by default the audio output will still be processed by SDK hence below api call is mandatory to disable that behavior
        agoraKit.adjustPlaybackSignalVolume(0)
        
        
        
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        let result = agoraKit.joinChannel(byToken: nil, channelId: channelName, info: nil, uid: 0) {[unowned self] (channel, uid, elapsed) -> Void in
            self.isJoined = true
            LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
            
            self.exAudio.startWork()
            
            //set up local audio view, this view will not show video but just a placeholder
            let view = VideoView()
            self.audioViews[uid] = view
            view.setPlaceholder(text: self.getAudioLabel(uid: uid, isLocal: true))
            self.container.layoutStream3x3(views: Array(self.audioViews.values))
        }
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
        }
    }
}

/// agora rtc engine delegate events
extension CustomAudioRender: AgoraRtcEngineDelegate {
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
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)

        //set up remote audio view, this view will not show video but just a placeholder
        let view = VideoView()
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
