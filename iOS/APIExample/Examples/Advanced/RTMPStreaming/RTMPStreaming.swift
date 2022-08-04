//
//  RTMPStreaming.swift
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
        self.navigationController?.pushViewController(newViewController, animated: true)
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
            rtmpTextField.isHidden = !isJoined
            publishButton.isHidden = !isJoined
            transcodingLabel.isHidden = !isJoined
            transcodingSwitch.isHidden = !isJoined
        }
    }
    
    var isPublished: Bool = false {
        didSet {
            rtmpTextField.isEnabled = !isPublished
            transcodingSwitch.isEnabled = !isPublished
            publishButton.title = isPublished ? "stop" : "push"
        }
    }
    
    var localVideo = Bundle.loadVideoView(type: .local, audioOnly: false)
    var remoteVideo = Bundle.loadVideoView(type: .remote, audioOnly: false)
    var agoraKit: AgoraRtcEngineKit!
    var remoteUid: UInt?
    var rtmpURL: String?
    var transcoding = AgoraLiveTranscoding.default()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // layout render view
        localVideo.setPlaceholder(text: "Local Host".localized)
        remoteVideo.setPlaceholder(text: "Remote Host".localized)
        container.layoutStream(views: [localVideo, remoteVideo])
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        
        guard let channelName = configs["channelName"] as? String else {return}
        
        // make myself a broadcaster
        agoraKit.setClientRole(GlobalSettings.shared.getUserRole())
        
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        agoraKit.enableAudio()
        agoraKit.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: AgoraVideoDimension640x480,
                                                                             frameRate: .fps30,
                                                                             bitrate: AgoraVideoBitrateStandard,
                                                                             orientationMode: .adaptative, mirrorMode: .auto))
        
        // set up local video to render your local camera preview
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = 0
        // the view to be binded
        videoCanvas.view = localVideo.videoView
        videoCanvas.renderMode = .hidden
        agoraKit.setupLocalVideo(videoCanvas)
        // you have to call startPreview to see local video
        agoraKit.startPreview()
        
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
        option.publishMicrophoneTrack = .of(true)
        option.clientRoleType = .of((Int32)(GlobalSettings.shared.getUserRole().rawValue))
        
        let result = agoraKit.joinChannel(byToken: KeyCenter.Token, channelId: channelName, uid: 0, mediaOptions: option)
//        let result = agoraKit.joinChannel(byToken: nil, channelId: channelName, info: nil, uid: 0) { [unowned self] (channel, uid, elapsed) -> Void in
//            self.isJoined = true
//            LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
//            
//            // add transcoding user so the video stream will be involved
//            // in future RTMP Stream
//            let user = AgoraLiveTranscodingUser()
//            user.rect = CGRect(x: 0, y: 0, width: CANVAS_WIDTH / 2, height: CANVAS_HEIGHT)
//            user.uid = uid
//            self.transcoding.add(user)
//        }
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
                    agoraKit.stopRtmpStream(rtmpURL)
                }
                agoraKit.disableAudio()
                agoraKit.disableVideo()
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
            agoraKit.stopRtmpStream(rtmpURL)
        } else {
            // resign rtmp text field
            rtmpTextField.resignFirstResponder()
            
            // check whether we need to enable transcoding
            let transcodingEnabled = transcodingSwitch.isOn
            if(transcodingEnabled){
                // we will use transcoding to composite multiple hosts' video
                // therefore we have to create a livetranscoding object and call before addPublishStreamUrl
                transcoding.size = CGSize(width: CANVAS_WIDTH, height: CANVAS_HEIGHT)
                agoraKit.startRtmpStream(withTranscoding: rtmpURL, transcoding: transcoding)
                
                if let remoteUid = remoteUid {
                    // add new user onto the canvas
                    let user = AgoraLiveTranscodingUser()
                    user.rect = CGRect(x: CANVAS_WIDTH / 2, y: 0, width: CANVAS_WIDTH / 2, height: CANVAS_HEIGHT)
                    user.uid = remoteUid
                    self.transcoding.add(user)
                    // remember you need to call setLiveTranscoding again if you changed the layout
                    agoraKit.updateRtmpTranscoding(transcoding)
                }
                
            } else {
                agoraKit.startRtmpStreamWithoutTranscoding(rtmpURL)
            }
            
            self.rtmpURL = rtmpURL
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
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        self.isJoined = true
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
        
        // add transcoding user so the video stream will be involved
        // in future RTMP Stream
        let user = AgoraLiveTranscodingUser()
        user.rect = CGRect(x: 0, y: 0, width: CANVAS_WIDTH / 2, height: CANVAS_HEIGHT)
        user.uid = uid
        self.transcoding.add(user)
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
            agoraKit.updateRtmpTranscoding(transcoding)
        }
    }
    
    /// callback for state of rtmp streaming, for both good and bad state
    /// @param url rtmp streaming url
    /// @param state state of rtmp streaming
    /// @param reason
    func rtcEngine(_ engine: AgoraRtcEngineKit, rtmpStreamingChangedToState url: String, state: AgoraRtmpStreamingState, errCode: AgoraRtmpStreamingErrorCode) {
        LogUtils.log(message: "streamStateChanged: \(url) state \(state.rawValue) error \(errCode.rawValue)", level: .info)
        if(state == .running) {
            self.showAlert(title: "Notice", message: "RTMP Publish Success")
            isPublished = true
        } else if(state == .failure) {
            self.showAlert(title: "Error", message: "RTMP Publish Failed: \(errCode.rawValue)")
        } else if(state == .idle) {
            self.showAlert(title: "Notice", message: "RTMP Publish Stopped")
            isPublished = false
        }
    }
    
    /// callback when live transcoding is properly updated
    func rtcEngineTranscodingUpdated(_ engine: AgoraRtcEngineKit) {
        LogUtils.log(message: "live transcoding updated", level: .info)
    }
    
    /// Reports the statistics of the current call. The SDK triggers this callback once every two seconds after the user joins the channel.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, reportRtcStats stats: AgoraChannelStats) {
        localVideo.statsInfo?.updateChannelStats(stats)
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
