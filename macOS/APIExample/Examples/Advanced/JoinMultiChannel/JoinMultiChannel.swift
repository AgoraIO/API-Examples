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

class JoinMultipleChannel: BaseViewController {
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
    @IBOutlet weak var channelPicker: NSPopUpButton!
    
    var channel1: AgoraRtcChannel?
    var channel2: AgoraRtcChannel?
    
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
    
    var isPublished: Bool = false {
        didSet {
            channelPicker.isEnabled = !isPublished
            publishBtn.isHidden = isPublished
            unpublishBtn.isHidden = !isPublished
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        layoutVideos()
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area.rawValue
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // this is mandatory to get camera list
        agoraKit.enableVideo()
        // set live broadcaster mode
        agoraKit.setChannelProfile(.liveBroadcasting)
        
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
        channel1?.leave()
        channel1?.destroy()
        channel2?.leave()
        channel2?.destroy()
    }
    
    @IBAction func onJoinPressed(_ sender:NSButton) {
        // auto subscribe options after join channel
        let mediaOptions = AgoraRtcChannelMediaOptions()
        mediaOptions.autoSubscribeAudio = true
        mediaOptions.autoSubscribeVideo = true
        
        var channel: AgoraRtcChannel?
        if(sender == joinBtn) {
            if(channel1 == nil) {
                channel1 = agoraKit.createRtcChannel(channelField.stringValue)
            }
            channel = channel1
        } else if(sender == joinBtn2){
            if(channel2 == nil) {
                channel2 = agoraKit.createRtcChannel(channelField2.stringValue)
            }
            channel = channel2
        }
//        label1.text = channelName1
        channel?.setRtcChannelDelegate(self)
        
        
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        let result = channel?.join(byToken: nil, info: nil, uid: 0, options: mediaOptions) ?? -1
        if result != 0 {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            self.showAlert(title: "Error", message: "joinChannel1 call failed: \(result), please check your params")
        }
    }
    
    @IBAction func onLeavePressed(_ sender: NSButton) {
        if(sender == leaveBtn) {
            channel1?.leave()
            if let channelName = channel1?.getId() {
                channelPicker.removeItem(withTitle: channelName)
            }
            channel1?.destroy()
            channel1 = nil
            isJoined = false
        } else if(sender == leaveBtn2){
            channel2?.leave()
            if let channelName = channel2?.getId() {
                channelPicker.removeItem(withTitle: channelName)
            }
            channel2?.destroy()
            channel2 = nil
            isJoined2 = false
        }
        
    }
    
    @IBAction func onPublishPressed(_ sender: Any) {
        let selectedChannelName = channelPicker.selectedItem?.title
        if let channel = getChannelByName(selectedChannelName) {
            channel.setClientRole(.broadcaster)
            channel.publish()
            isPublished = true
        }
    }
    
    
    @IBAction func onUnpublishPressed(_ sender: Any) {
        let selectedChannelName = channelPicker.selectedItem?.title
        if let channel = getChannelByName(selectedChannelName) {
            channel.setClientRole(.audience)
            channel.unpublish()
            isPublished = false
        }
    }
    
    func getChannelByName(_ channelName: String?) -> AgoraRtcChannel? {
        if(channel1?.getId() == channelName){
            return channel1
        } else if(channel2?.getId() == channelName) {
            return channel2
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

/// agora rtc engine delegate events
extension JoinMultipleChannel: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.rawValue)", level: .warning)
    }
    
    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode)", level: .error)
        self.showAlert(title: "Error", message: "Error \(errorCode.rawValue) occur")
    }
}

extension JoinMultipleChannel: AgoraRtcChannelDelegate
{
    func rtcChannelDidJoin(_ rtcChannel: AgoraRtcChannel, withUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "Join \(rtcChannel.getId() ?? "") with uid \(uid) elapsed \(elapsed)ms", level: .info)
        channelPicker.addItem(withTitle: rtcChannel.getId()!)
        if (channel1 == rtcChannel) {
            isJoined = true
        } else {
            isJoined2 = true
        }
    }
    /// callback when warning occured for a channel, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcChannel(_ rtcChannel: AgoraRtcChannel, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "channel: \(rtcChannel.getId() ?? ""), warning: \(warningCode.rawValue)", level: .warning)
    }
    
    /// callback when error occured for a channel, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// @param errorCode error code of the problem
    func rtcChannel(_ rtcChannel: AgoraRtcChannel, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode)", level: .error)
        self.showAlert(title: "Error", message: "Error \(errorCode.rawValue) occur")
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcChannel(_ rtcChannel: AgoraRtcChannel, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        
        // Only one remote video view is available for this
        // tutorial. Here we check if there exists a surface
        // view tagged as this uid.
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = channel1 == rtcChannel ? videos2[0].videocanvas : videos2[1].videocanvas
        videoCanvas.renderMode = .hidden
        // set channelId so that it knows which channel the video belongs to
        videoCanvas.channelId = rtcChannel.getId()
        agoraKit.setupRemoteVideo(videoCanvas)
    }
    
    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    func rtcChannel(_ rtcChannel: AgoraRtcChannel, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)
        
        // to unlink your view from sdk, so that your view reference will be released
        // note the video will stay at its last frame, to completely remove it
        // you will need to remove the EAGL sublayer from your binded view
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = nil
        videoCanvas.renderMode = .hidden
        // set channelId so that it knows which channel the video belongs to
        videoCanvas.channelId = rtcChannel.getId()
        agoraKit.setupRemoteVideo(videoCanvas)
    }
}
