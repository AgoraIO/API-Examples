//
//  JoinChannelVC.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Foundation
import UIKit
import AgoraRtcKit


class JoinChannelMain: BaseViewController {
    @IBOutlet weak var joinButton: UIButton!
    @IBOutlet weak var channelTextField: UITextField!
    
    var localVideo = VideoView(frame: CGRect.zero)
    var remoteVideo = VideoView(frame: CGRect.zero)
    
    var agoraKit: AgoraRtcEngineKit!
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false {
        didSet {
            channelTextField.isEnabled = !isJoined
            joinButton.isHidden = isJoined
        }
    }
    
    override func viewDidLoad(){
        super.viewDidLoad()
        // set up agora instance when view loaded
        agoraKit = AgoraRtcEngineKit.sharedEngine(withAppId: KeyCenter.AppId, delegate: self)
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        // leave channel when exiting the view
        if(isJoined) {
            agoraKit.leaveChannel { (stats) -> Void in
                LogUtils.log(msg: "left channel, duration: \(stats.duration)", level: .info)
            }
        }
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        guard let identifier = segue.identifier else {
            return
        }
        
        switch identifier {
        case "RenderViewController":
            let vc = segue.destination as! RenderViewController
            vc.layoutStream(views: [localVideo, remoteVideo])
        default:
            break
        }
    }
    
    /// callback when join button hit
    @IBAction func onJoin(){
        guard let channelName = channelTextField.text else {return}
        
        //hide keyboard
        channelTextField.resignFirstResponder()
        
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        agoraKit.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: AgoraVideoDimension640x360,
                frameRate: .fps15,
                bitrate: AgoraVideoBitrateStandard,
                orientationMode: .adaptative))
        
        // set up local video to render your local camera preview
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = 0
        // the view to be binded
        videoCanvas.view = localVideo.videoView
        videoCanvas.renderMode = .hidden
        agoraKit.setupLocalVideo(videoCanvas)
        
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        let result = agoraKit.joinChannel(byToken: nil, channelId: channelName, info: nil, uid: 0) {[unowned self] (channel, uid, elapsed) -> Void in
            self.isJoined = true
            LogUtils.log(msg: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
        }
        if(result != 0) {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            self.showAlert(title: "Error", msg: "joinChannel call failed: \(result), please check your params")
        }
    }
}

/// agora rtc engine delegate events
extension JoinChannelMain: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(msg: "warning: \(warningCode.description)", level: .warning)
    }
    
    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(msg: "error: \(errorCode)", level: .error)
        self.showAlert(title: "Error", msg: "Error \(errorCode.description) occur")
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(msg: "remote user join: \(uid) \(elapsed)ms", level: .info)
        
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
        LogUtils.log(msg: "remote user left: \(uid) reason \(reason)", level: .info)
        
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
}
