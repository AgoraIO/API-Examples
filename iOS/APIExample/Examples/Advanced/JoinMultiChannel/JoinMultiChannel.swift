//
//  JoinMultiChannel.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//
import UIKit
import AGEVideoLayout
import AgoraRtcKit

class JoinMultiChannelEntry : UIViewController
{
    @IBOutlet weak var joinButton: AGButton!
    @IBOutlet weak var channelTextField: AGTextField!
    let identifier = "JoinMultiChannel"
    
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

let CONNECTION_1_UID = UInt.random(in: 1001...2000)
let CONNECTION_2_UID = UInt.random(in: 2001...3000)

class JoinMultiChannelMain: BaseViewController {
    var localVideo = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
    var channel1RemoteVideo = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
    var channel2RemoteVideo = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
    
    @IBOutlet weak var container1: AGEVideoContainer!
    @IBOutlet weak var container2: AGEVideoContainer!
    @IBOutlet weak var label1: UILabel!
    @IBOutlet weak var label2: UILabel!
    var channel1: Channel1Delegate = Channel1Delegate()
    var channel2: Channel2Delegate = Channel2Delegate()
    var channelName1 = ""
    var channelName2 = ""
    var agoraKit: AgoraRtcEngineKit!
    
    // indicate if current instance has joined channel
    var isJoined1: Bool = false
    var isJoined2: Bool = false
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // get channel name from configs
        guard let channelName = configs["channelName"] as? String else {return}
        channelName1 = "\(channelName)"
        channelName2 = "\(channelName)-2"
        channel1.channelId = channelName1
        channel2.channelId = channelName2
        channel1.view = channel1RemoteVideo
        channel2.view = channel2RemoteVideo
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: channel1)
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        
        
        // layout render view
        localVideo.setPlaceholder(text: "Local Host".localized)
        channel1RemoteVideo.setPlaceholder(text: "\(channelName1 )\nRemote Host")
        channel2RemoteVideo.setPlaceholder(text: "\(channelName2 )\nRemote Host")
        container1.layoutStream(views: [localVideo, channel1RemoteVideo])
        container2.layoutStream(views: [channel2RemoteVideo])
        
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        agoraKit.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: AgoraVideoDimension640x360,
             frameRate: .fps30,
             bitrate: AgoraVideoBitrateStandard,
             orientationMode: .adaptative, mirrorMode: .auto))
        
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        // to preview, has to be broadcaster
        agoraKit.setClientRole(.broadcaster)
        
        // set up local video to render your local camera preview
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = 0
        // the view to be binded
        videoCanvas.view = localVideo.videoView
        videoCanvas.renderMode = .hidden
        agoraKit.setupLocalVideo(videoCanvas)
        // you have to call startPreview to see local video
        agoraKit.startPreview()
        
        // join channel1
        var mediaOptions = AgoraRtcChannelMediaOptions()
        // publish audio and camera track for channel 1
        mediaOptions.publishAudioTrack = .of(true)
        mediaOptions.publishCameraTrack = .of(true)
        mediaOptions.publishCustomAudioTrack = .of(true)
        mediaOptions.autoSubscribeVideo = .of(true)
        mediaOptions.autoSubscribeAudio = .of(true)
        mediaOptions.clientRoleType = .of((Int32)(AgoraClientRole.broadcaster.rawValue))
        var result = agoraKit.joinChannel(byToken: KeyCenter.Token, channelId: channelName1, uid: CONNECTION_1_UID, mediaOptions: mediaOptions, joinSuccess: nil)

        agoraKit.setExternalAudioSource(true, sampleRate: 44100, channels: 2, sourceNumber: 3, localPlayback: false, publish: true)
        
        if result != 0 {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            self.showAlert(title: "Error", message: "joinChannel1 call failed: \(result), please check your params")
        }
        
        // join channel2
        mediaOptions = AgoraRtcChannelMediaOptions()
        mediaOptions.clientRoleType = .of(Int32(AgoraClientRole.broadcaster.rawValue))
        mediaOptions.publishAudioTrack = .of(false)
        mediaOptions.publishCameraTrack = .of(false)
        mediaOptions.autoSubscribeVideo = .of(true)
        mediaOptions.autoSubscribeAudio = .of(true)
        mediaOptions.enableAudioRecordingOrPlayout = .of(false)
        let connection2 = AgoraRtcConnection()
        connection2.channelId = channelName2
        connection2.localUid = CONNECTION_2_UID
        result = agoraKit.joinChannelEx(byToken: KeyCenter.Token, connection: connection2, delegate: channel2, mediaOptions: mediaOptions, joinSuccess: nil)
        if result != 0 {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            self.showAlert(title: "Error", message: "joinChannel2 call failed: \(result), please check your params")
        }
    }
    
    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            agoraKit.stopPreview()
            // leave channel when exiting the view
            agoraKit.leaveChannel()
            let channel2 = AgoraRtcConnection()
            channel2.channelId = channelName2
            channel2.localUid = CONNECTION_2_UID
            agoraKit.leaveChannelEx(channel2, leaveChannelBlock: nil)
        }
    }
}

class Channel1Delegate: NSObject, AgoraRtcEngineDelegate {
    var channelId: String?
    var view:VideoView?
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        
        LogUtils.log(message: "local user join room channel \(channelId ?? ""): \(uid) \(elapsed)ms", level: .info)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        guard let channelId = channelId, let channel1RemoteVideo = view else {
            return
        }
        LogUtils.log(message: "remote user join room channel \(channelId): \(uid) \(elapsed)ms", level: .info)

        // Only one remote video view is available for this
        // tutorial. Here we check if there exists a surface
        // view tagged as this uid.
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = channel1RemoteVideo.videoView
        videoCanvas.renderMode = .hidden
        let connection = AgoraRtcConnection()
        connection.channelId = channelId
        connection.localUid = CONNECTION_1_UID
        engine.setupRemoteVideoEx(videoCanvas, connection: connection)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        guard let channelId = channelId else {
            return
        }
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)

        // to unlink your view from sdk, so that your view reference will be released
        // note the video will stay at its last frame, to completely remove it
        // you will need to remove the EAGL sublayer from your binded view
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = nil
        videoCanvas.renderMode = .hidden
        let connection = AgoraRtcConnection()
        connection.channelId = channelId
        connection.localUid = CONNECTION_2_UID
        engine.setupRemoteVideoEx(videoCanvas, connection: connection)
    }
}

/// agora rtc engine delegate events
class Channel2Delegate: NSObject, AgoraRtcEngineDelegate {
    var channelId:String?
    var view:VideoView?
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.description)", level: .warning)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode.description)", level: .error)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        guard let channelId = channelId, let channel2RemoteVideo = view else {
            return
        }
        LogUtils.log(message: "remote user join room channel \(channelId): \(uid) \(elapsed)ms", level: .info)

        // Only one remote video view is available for this
        // tutorial. Here we check if there exists a surface
        // view tagged as this uid.
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = channel2RemoteVideo.videoView
        videoCanvas.renderMode = .hidden
        let connection = AgoraRtcConnection()
        connection.channelId = channelId
        connection.localUid = CONNECTION_2_UID
        engine.setupRemoteVideoEx(videoCanvas, connection: connection)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        guard let channelId = channelId else {
            return
        }
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)

        // to unlink your view from sdk, so that your view reference will be released
        // note the video will stay at its last frame, to completely remove it
        // you will need to remove the EAGL sublayer from your binded view
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = nil
        videoCanvas.renderMode = .hidden
        let connection = AgoraRtcConnection()
        connection.channelId = channelId
        connection.localUid = CONNECTION_2_UID
        engine.setupRemoteVideoEx(videoCanvas, connection: connection)
    }
}
