//
//  JoinChannelVC.swift
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

class JoinMultiChannelMain: BaseViewController, AgoraRtcEngineDelegate {
    var localVideo = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
    var channel1RemoteVideo = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
    var channel2RemoteVideo = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
    
    @IBOutlet weak var container1: AGEVideoContainer!
    @IBOutlet weak var container2: AGEVideoContainer!
    @IBOutlet weak var label1: UILabel!
    @IBOutlet weak var label2: UILabel!
    var channel1: JoinMultiChannelMainEventListener = JoinMultiChannelMainEventListener()
    var channel2: JoinMultiChannelMainEventListener = JoinMultiChannelMainEventListener()
    var channelName1 = ""
    var channelName2 = ""
    var connectionId1:UInt32?
    var connectionId2:UInt32?
    var agoraKit: AgoraRtcEngineKit!
    var imageSource: AgoraYUVImageSourcePush = AgoraYUVImageSourcePush()
    
    // indicate if current instance has joined channel
    var isJoined1: Bool = false
    var isJoined2: Bool = false
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        
        // get channel name from configs
        guard let channelName = configs["channelName"] as? String else {return}
        channelName1 = "\(channelName)"
        channelName2 = "\(channelName)-2"
        
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
        
        // setup external video source
        agoraKit.setExternalVideoSource(true, useTexture: false, encodedFrame: true)
        imageSource.delegate = self
        imageSource.startSource()
        
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
        let connectionIdPointer = UnsafeMutablePointer<UInt32>.allocate(capacity: MemoryLayout<UInt32>.stride)
        var mediaOptions = AgoraRtcChannelMediaOptions()
        // publish audio and camera track for channel 1
        mediaOptions.publishAudioTrack = .of(true)
        mediaOptions.publishCameraTrack = .of(true)
        mediaOptions.channelProfile = .of((Int32)(AgoraChannelProfile.liveBroadcasting.rawValue))
        mediaOptions.clientRoleType = .of((Int32)(AgoraClientRole.broadcaster.rawValue))
        var result = agoraKit.joinChannel(byToken: nil, channelId: channelName1, uid: 0, mediaOptions: mediaOptions)
        channel1.connectionId = connectionIdPointer.pointee
        connectionId1 = connectionIdPointer.pointee
        channel1.connecitonDelegate = self
        connectionIdPointer.deallocate()
        if result != 0 {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            self.showAlert(title: "Error", message: "joinChannel1 call failed: \(result), please check your params")
        }
        
        // join channel2
        let connectionIdPointer2 = UnsafeMutablePointer<UInt32>.allocate(capacity: MemoryLayout<UInt32>.stride)
        mediaOptions = AgoraRtcChannelMediaOptions()
        // publish custom video track for channel 2
        mediaOptions.publishAudioTrack = .of(false)
        mediaOptions.publishCustomVideoTrack = .of(true)
        mediaOptions.channelProfile = .of((Int32)(AgoraChannelProfile.liveBroadcasting.rawValue))
        mediaOptions.clientRoleType = .of((Int32)(AgoraClientRole.broadcaster.rawValue))
        result = agoraKit.joinChannelEx(byToken: nil, channelId: channelName2, uid: 0, connectionId: connectionIdPointer2, delegate: channel2, mediaOptions: mediaOptions)
        channel2.connectionId = connectionIdPointer2.pointee
        connectionId2 = connectionIdPointer2.pointee
        channel2.connecitonDelegate = self
        connectionIdPointer2.deallocate()
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
            imageSource.stopSource()
            // leave channel when exiting the view
            agoraKit.leaveChannelEx(channelName1, connectionId: connectionId1 ?? 0, leaveChannelBlock: nil)
            agoraKit.leaveChannelEx(channelName2, connectionId: connectionId2 ?? 0, leaveChannelBlock: nil)
        }
    }
}

extension JoinMultiChannelMain : AgoraYUVImageSourcePushDelegate {
    func onVideoFrame(_ buffer: Data, size: CGSize, rotation: Int32) {
        guard let connectionId = connectionId2 else {return}
        let time = CMTime(seconds: CACurrentMediaTime(), preferredTimescale: 1000)
        let videoFrame = AgoraVideoFrame()
        videoFrame.format = 1
        videoFrame.dataBuf = buffer
        videoFrame.time = time
        videoFrame.strideInPixels = Int32(size.width)
        videoFrame.height = Int32(size.height)
        videoFrame.rotation = Int32(rotation)
        agoraKit.pushExternalVideoFrame(videoFrame, connectionId: connectionId)
    }
}

extension JoinMultiChannelMain :JoinMultiChannelMainConnectionProtocol {
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId: UInt32, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.description)", level: .warning)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId: UInt32, didOccurError errorCode: AgoraErrorCode) {
        self.showAlert(title: "Error", message: "Error \(errorCode.description) occur")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId: UInt32, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId: UInt32, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)

        // Only one remote video view is available for this
        // tutorial. Here we check if there exists a surface
        // view tagged as this uid.
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = connectionId == connectionId1 ? channel1RemoteVideo.videoView : channel2RemoteVideo.videoView
        videoCanvas.renderMode = .hidden
        agoraKit.setupRemoteVideoEx(videoCanvas, connectionId: connectionId)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId: UInt32, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)

        // to unlink your view from sdk, so that your view reference will be released
        // note the video will stay at its last frame, to completely remove it
        // you will need to remove the EAGL sublayer from your binded view
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = nil
        videoCanvas.renderMode = .hidden
        agoraKit.setupRemoteVideoEx(videoCanvas, connectionId: connectionId)
    }
    
    
}

protocol JoinMultiChannelMainConnectionProtocol : NSObject {
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId:UInt32, didOccurWarning warningCode: AgoraWarningCode)
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId:UInt32, didOccurError errorCode: AgoraErrorCode)
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId:UInt32, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int)
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId:UInt32, didJoinedOfUid uid: UInt, elapsed: Int)
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId:UInt32, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason)
}

/// agora rtc engine delegate events
class JoinMultiChannelMainEventListener: NSObject, AgoraRtcEngineDelegate {
    weak var connecitonDelegate:JoinMultiChannelMainConnectionProtocol?
    var connectionId:UInt32?
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        if let connId = self.connectionId {
            self.connecitonDelegate?.rtcEngine(engine, connectionId: connId, didOccurWarning: warningCode)
        }
    }

    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        if let connId = self.connectionId {
            self.connecitonDelegate?.rtcEngine(engine, connectionId: connId, didOccurError: errorCode)
        }
    }


    internal func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        if let connId = self.connectionId {
            self.connecitonDelegate?.rtcEngine(engine, connectionId: connId, didJoinChannel: channel, withUid: uid, elapsed: elapsed)
        }
    }

    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
//    func rtcChannel(_ rtcChannel: AgoraRtcChannel, didJoinedOfUid uid: UInt, elapsed: Int) {
    internal func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        if let connId = self.connectionId {
            self.connecitonDelegate?.rtcEngine(engine, connectionId: connId, didJoinedOfUid: uid, elapsed: elapsed)
        }
    }

    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    internal func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        if let connId = self.connectionId {
            self.connecitonDelegate?.rtcEngine(engine, connectionId: connId, didOfflineOfUid: uid, reason: reason)
        }
    }
}
