//
//  JoinChannelVC.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Cocoa
import AgoraRtcKit
import AGEVideoLayout

class JoinMultipleChannel: BaseViewController, AgoraRtcEngineDelegate {
    var videos: [VideoView] = []
    var videos2: [VideoView] = []
    
    @IBOutlet weak var container: AGEVideoContainer!
    @IBOutlet weak var container2: AGEVideoContainer!
    @IBOutlet weak var channelField: NSTextField!
    @IBOutlet weak var joinBtn: NSButton!
    @IBOutlet weak var leaveBtn: NSButton!
    @IBOutlet weak var channelField2: NSTextField!
    @IBOutlet weak var joinBtn2: NSButton!
    @IBOutlet weak var leaveBtn2: NSButton!
    @IBOutlet weak var publishBtn: NSButton!
    @IBOutlet weak var unpublishBtn: NSButton!
    
    var channel1: JoinMultiChannelMainEventListener = JoinMultiChannelMainEventListener()
    var channel2: JoinMultiChannelMainEventListener = JoinMultiChannelMainEventListener()
    var connectionId1:UInt32?
    var connectionId2:UInt32?
    var imageSource: AgoraYUVImageSourcePush = AgoraYUVImageSourcePush()
    
    var agoraKit: AgoraRtcEngineKit!
    
    // indicate if current instance has joined channel1
    var isJoined: Bool = false {
        didSet {
            channelField.isEnabled = !isJoined
            joinBtn.isHidden = isJoined
            leaveBtn.isHidden = !isJoined
        }
    }
    // indicate if current instance has joined channel2
    var isJoined2: Bool = false {
        didSet {
            channelField2.isEnabled = !isJoined2
            joinBtn2.isHidden = isJoined2
            leaveBtn2.isHidden = !isJoined2
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        layoutVideos()
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        //config.areaCode = GlobalSettings.shared.area.rawValue
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // this is mandatory to get camera list
        agoraKit.enableVideo()
        // set live broadcaster mode
        agoraKit.setChannelProfile(.liveBroadcasting)
        
        // setup external video source
        agoraKit.setExternalVideoSource(true, useTexture: false, pushMode: true)
        imageSource.delegate = self
        imageSource.startSource()
        
        // set up local video to render your local camera preview
        let localVideo = videos[0]
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = 0
        // the view to be binded
        videoCanvas.view = localVideo.videocanvas
        videoCanvas.renderMode = .hidden
        agoraKit.setupLocalVideo(videoCanvas)
        agoraKit.startPreview()
    }
    
    override func viewWillBeRemovedFromSplitView() {
        // leave channel when exiting the view
        agoraKit.leaveChannelEx(channelField.stringValue, connectionId: connectionId1 ?? 0, leaveChannelBlock: nil)
        agoraKit.leaveChannelEx(channelField2.stringValue, connectionId: connectionId2 ?? 0, leaveChannelBlock: nil)
    }
    
    @IBAction func onJoinPressed(_ sender:NSButton) {
        // auto subscribe options after join channel
        let mediaOptions = AgoraRtcChannelMediaOptions()
        mediaOptions.autoSubscribeAudio = true
        mediaOptions.autoSubscribeVideo = true
        
        if(sender == joinBtn) {
            if(connectionId1 == nil) {
                let connectionIdPointer = UnsafeMutablePointer<UInt32>.allocate(capacity: MemoryLayout<UInt32>.stride)
                let mediaOptions = AgoraRtcChannelMediaOptions()
                // publish audio and camera track for channel 1
                mediaOptions.channelProfile = .liveBroadcasting
                mediaOptions.clientRoleType = .broadcaster
                mediaOptions.publishCameraTrack = true
                mediaOptions.publishAudioTrack = true
                let result = agoraKit.joinChannelEx(byToken: nil, channelId: channelField.stringValue, uid: 0, connectionId: connectionIdPointer, delegate: channel1, mediaOptions: mediaOptions)
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
            }
        } else if(sender == joinBtn2){
            if(connectionId2 == nil) {
                let connectionIdPointer2 = UnsafeMutablePointer<UInt32>.allocate(capacity: MemoryLayout<UInt32>.stride)
                let mediaOptions = AgoraRtcChannelMediaOptions()
                // publish custom video track for channel 2
                mediaOptions.publishAudioTrack = false
                mediaOptions.publishCustomVideoTrack = true
                mediaOptions.channelProfile = .liveBroadcasting
                mediaOptions.clientRoleType = .broadcaster
                let result = agoraKit.joinChannelEx(byToken: nil, channelId: channelField2.stringValue, uid: 0, connectionId: connectionIdPointer2, delegate: channel2, mediaOptions: mediaOptions)
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
        }
    }
    
    @IBAction func onLeavePressed(_ sender: NSButton) {
        if(sender == leaveBtn) {
            agoraKit.leaveChannelEx(channelField.stringValue, connectionId: connectionId1 ?? 0, leaveChannelBlock: nil)
            connectionId1 = nil
            isJoined = false
        } else if(sender == leaveBtn2){
            agoraKit.leaveChannelEx(channelField2.stringValue, connectionId: connectionId2 ?? 0, leaveChannelBlock: nil)
            connectionId2 = nil
            isJoined2 = false
        }
        
    }
    
    func getConnectionIdByName(_ channelName: String?) -> UInt32? {
        if(channelField.stringValue == channelName){
            return connectionId1
        } else if(channelField2.stringValue == channelName) {
            return connectionId2
        }
        return nil
    }
    
    func layoutVideos() {
        videos = [VideoView.createFromNib()!]
        videos[0].placeholder.stringValue = "Local"
        // layout render view
        container.layoutStream(views: videos)
        
        videos2 = [VideoView.createFromNib()!, VideoView.createFromNib()!]
        videos2[0].placeholder.stringValue = "Channel1\nRemote"
        videos2[1].placeholder.stringValue = "Channel2\nRemote"
        container2.layoutStream2(views: videos2)
    }
}

extension JoinMultipleChannel : AgoraYUVImageSourcePushDelegate {
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

extension JoinMultipleChannel :JoinMultiChannelMainConnectionProtocol {
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId: UInt32, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.description)", level: .warning)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId: UInt32, didOccurError errorCode: AgoraErrorCode) {
        self.showAlert(title: "Error", message: "Error \(errorCode.description) occur")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId: UInt32, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
        if (connectionId == connectionId1) {
            isJoined = true
        } else {
            isJoined2 = true
        }
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionId: UInt32, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)

        // Only one remote video view is available for this
        // tutorial. Here we check if there exists a surface
        // view tagged as this uid.
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = connectionId == connectionId1 ? videos2[0].videocanvas : videos2[1].videocanvas
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
