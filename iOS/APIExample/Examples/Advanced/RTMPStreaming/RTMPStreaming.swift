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
import AGEVideoLayout

let CANVAS_WIDTH = 640
let CANVAS_HEIGHT = 480

class RTMPStreamingEntry : UIViewController
{
    @IBOutlet weak var joinButton: AGButton!
    @IBOutlet weak var channelTextField: AGTextField!
    @IBOutlet weak var noteLabel: UILabel!
    let identifier = "RTMPStreaming"
    
    override func viewDidLoad() {
        super.viewDidLoad()
        noteLabel.text = "Ensure that you enable the RTMP Converter service at Agora Dashboard before using this function."
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
        NetworkManager.shared.generateToken(channelName: channelName) {
            self.navigationController?.pushViewController(newViewController, animated: true)
        }
    }
}

class RTMPStreamingMain: BaseViewController {
    @IBOutlet weak var publishButton: UIButton!
    @IBOutlet weak var rtmpTextField: UITextField!
    @IBOutlet weak var transcodingSwitch: UISwitch!
    @IBOutlet weak var transcodingLabel: UILabel!
    @IBOutlet weak var container: AGEVideoContainer!
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false {
        didSet {
            rtmpTextField.isEnabled = isJoined
            publishButton.isEnabled = isJoined
            transcodingSwitch.isEnabled = isJoined
        }
    }
    
    var isPublished: Bool = false {
        didSet {
            rtmpTextField.isEnabled = !isPublished
            transcodingSwitch.isEnabled = !isPublished
            publishButton.title = isPublished ? "stop".localized : "push".localized
        }
    }
    
    var localVideo = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
    var remoteVideo = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
    var agoraKit: AgoraRtcEngineKit!
    var remoteUid: UInt?
    var rtmpURL: String?
    var transcoding = AgoraLiveTranscoding.default()
    var retried: UInt = 0
    var unpublishing: Bool = false
    let MAX_RETRY_TIMES = 3
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // layout render view
        localVideo.setPlaceholder(text: "Local Host".localized)
        remoteVideo.setPlaceholder(text: "Remote Host".localized)
        container.layoutStream(views: [localVideo, remoteVideo])
        
        // set up agora instance when view loadedlet config = AgoraRtcEngineConfig()
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area.rawValue
        // setup log file path
        let logConfig = AgoraLogConfig()
        logConfig.level = .info
        config.logConfig = logConfig
        
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        
        guard let channelName = configs["channelName"] as? String,
              let resolution = GlobalSettings.shared.getSetting(key: "resolution")?.selectedOption().value as? CGSize,
              let fps = GlobalSettings.shared.getSetting(key: "fps")?.selectedOption().value as? AgoraVideoFrameRate,
              let orientation = GlobalSettings.shared.getSetting(key: "orientation")?.selectedOption().value as? AgoraVideoOutputOrientationMode else {return}
        
        // make myself a broadcaster
        agoraKit.setChannelProfile(.liveBroadcasting)
        agoraKit.setClientRole(.broadcaster)
        
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        agoraKit.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: resolution,
                                                                             frameRate: fps,
                                                                             bitrate: AgoraVideoBitrateStandard,
                                                                             orientationMode: orientation))
        
        // set up local video to render your local camera preview
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = UserInfo.userId
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
        let option = AgoraRtcChannelMediaOptions()
        let result = agoraKit.joinChannel(byToken: KeyCenter.Token, channelId: channelName, info: nil, uid: UserInfo.userId, options: option)
        if (result != 0) {
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
                if let rtmpURL = rtmpURL {
                    agoraKit.removePublishStreamUrl(rtmpURL)
                }
                agoraKit.leaveChannel { (stats) -> Void in
                    LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
                }
            }
        }
    }
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        view.endEditing(true)
    }
    
    /// callback when publish button hit
    @IBAction func onPublish() {
        guard let rtmpURL = rtmpTextField.text else {
            return
        }
        if(isPublished) {
            // stop rtmp streaming
            unpublishing = true
            agoraKit.stopRtmpStream(rtmpURL)
        } else {
            // resign rtmp text field
            rtmpTextField.resignFirstResponder()
            startRtmpStreaming(isTranscoding: transcodingSwitch.isOn, rtmpURL: rtmpURL)
            self.rtmpURL = rtmpURL
        }
    }
    
    func startRtmpStreaming(isTranscoding: Bool, rtmpURL: String) {
        if(isTranscoding){
            // we will use transcoding to composite multiple hosts' video
            // therefore we have to create a livetranscoding object and call before addPublishStreamUrl
            transcoding.size = CGSize(width: CANVAS_WIDTH, height: CANVAS_HEIGHT)
//            agoraKit.setLiveTranscoding(transcoding)
            agoraKit.startRtmpStream(withTranscoding: rtmpURL, transcoding: transcoding)
        }
        else{
            agoraKit.startRtmpStreamWithoutTranscoding(rtmpURL)
        }
    }
}

/// agora rtc engine delegate events
extension RTMPStreamingMain: AgoraRtcEngineDelegate {
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
        LogUtils.log(message: "error: \(errorCode.description)", level: .error)
    }
    
    /// callback when the local user joins a specified channel.
    /// @param channel
    /// @param uid uid of local user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        isJoined = true
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
        
        // add transcoding user so the video stream will be involved
        // in future RTMP Stream
        let user = AgoraLiveTranscodingUser()
        user.rect = CGRect(x: 0, y: 0, width: CANVAS_WIDTH / 2, height: CANVAS_HEIGHT)
        user.uid = uid
        transcoding.add(user)
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        
        // only one remote video view is available for this
        // tutorial. Here we check if there exists a surface
        // view tagged as this uid.
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = remoteVideo.videoView
        videoCanvas.renderMode = .hidden
        agoraKit.setupRemoteVideo(videoCanvas)
        
        // remove preivous user from the canvas
        if let existingUid = remoteUid {
            transcoding.removeUser(existingUid)
        }
        remoteUid = uid
        
        // check whether we have enabled transcoding
        let transcodingEnabled = transcodingSwitch.isOn
        if(transcodingEnabled){
            // add new user onto the canvas
            let user = AgoraLiveTranscodingUser()
            user.rect = CGRect(x: CANVAS_WIDTH / 2, y: 0, width: CANVAS_WIDTH / 2, height: CANVAS_HEIGHT)
            user.uid = uid
            self.transcoding.add(user)
            // remember you need to call setLiveTranscoding again if you changed the layout
            agoraKit.updateRtmpTranscoding(transcoding)
        }
    }
    
    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "remote user left: \(uid) reason \(reason.rawValue)", level: .info)
        
        // to unlink your view from sdk, so that your view reference will be released
        // note the video will stay at its last frame, to completely remove it
        // you will need to remove the EAGL sublayer from your binded view
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = nil
        videoCanvas.renderMode = .hidden
        agoraKit.setupRemoteVideo(videoCanvas)
        
        // check whether we have enabled transcoding
        let transcodingEnabled = transcodingSwitch.isOn
        if(transcodingEnabled){
            // remove user from canvas if current cohost left channel
            if let existingUid = remoteUid {
                transcoding.removeUser(existingUid)
            }
            remoteUid = nil
            // remember you need to call setLiveTranscoding again if you changed the layout
            agoraKit.updateRtmpTranscoding(transcoding)
        }
    }
    
    /// callback for state of rtmp streaming, for both good and bad state
    /// @param url rtmp streaming url
    /// @param state state of rtmp streaming
    /// @param reason
    func rtcEngine(_ engine: AgoraRtcEngineKit, rtmpStreamingChangedToState url: String, state: AgoraRtmpStreamingState, errorCode: AgoraRtmpStreamingErrorCode) {
        LogUtils.log(message: "rtmp streaming: \(url) state \(state.rawValue) error \(errorCode.rawValue)", level: .info)
        guard let rtmpURL = rtmpTextField.text else {
            return
        }
        if state == .running {
            if errorCode == .streamingErrorCodeOK {
                self.showAlert(title: "Notice", message: "RTMP Publish Success")
                isPublished = true
                retried = 0
            }
        } else if state == .failure {
            agoraKit.stopRtmpStream(rtmpURL)
            if errorCode == .streamingErrorCodeInternalServerError
                || errorCode == .streamingErrorCodeStreamNotFound
                || errorCode == .streamPublishErrorNetDown
                || errorCode == .streamingErrorCodeRtmpServerError
                || errorCode == .streamingErrorCodeConnectionTimeout {
                self.showAlert(title: "Error", message: "RTMP Publish Failed: \(errorCode.rawValue)")
            }
            else{
                unpublishing = true
            }
        } else if state == .idle {
            if unpublishing {
                unpublishing = false
                self.showAlert(title: "Notice", message: "RTMP Publish Stopped")
                isPublished = false
            }
            else if retried >= MAX_RETRY_TIMES{
                retried = 0
                self.showAlert(title: "Notice", message: "RTMP Publish Stopped")
                isPublished = false
            }
            else {
                retried += 1
                startRtmpStreaming(isTranscoding: transcodingSwitch.isOn, rtmpURL: rtmpURL)
            }
        }
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, rtmpStreamingEventWithUrl url: String, eventCode: AgoraRtmpStreamingEvent) {
        if(eventCode == .urlAlreadyInUse) {
            self.showAlert(title: "Error", message: "The URL is already in Use.")
        }
    }
    
    /// callback when live transcoding is properly updated
    func rtcEngineTranscodingUpdated(_ engine: AgoraRtcEngineKit) {
        LogUtils.log(message: "live transcoding updated", level: .info)
    }
}
