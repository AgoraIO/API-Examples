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

class ChannelMediaRelay: BaseViewController {
    var videos: [VideoView] = []
    
    @IBOutlet weak var container: AGEVideoContainer!
    @IBOutlet weak var channelField: NSTextField!
    @IBOutlet weak var joinBtn: NSButton!
    @IBOutlet weak var leaveBtn: NSButton!
    @IBOutlet weak var relayField: NSTextField!
    @IBOutlet weak var relayBtn: NSButton!
    @IBOutlet weak var stopRelayBtn: NSButton!
    var agoraKit: AgoraRtcEngineKit!
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false {
        didSet {
            channelField.isEnabled = !isJoined
            joinBtn.isHidden = isJoined
            leaveBtn.isHidden = !isJoined
            relayBtn.isEnabled = isJoined
        }
    }
    
    var isRelaying: Bool = false {
        didSet {
            stopRelayBtn.isHidden = !isRelaying
            relayBtn.isHidden = isRelaying
            relayField.isEnabled = !isRelaying
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        layoutVideos(2)
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        //config.areaCode = GlobalSettings.shared.area.rawValue
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // this is mandatory to get camera list
        agoraKit.enableVideo()
    }
    
    override func viewWillBeRemovedFromSplitView() {
        if(isJoined) {
            agoraKit.leaveChannel { (stats:AgoraChannelStats) in
                LogUtils.log(message: "Left channel", level: .info)
            }
        }
    }
    
    @IBAction func onJoinPressed(_ sender:Any) {
        // set live broadcaster mode
        agoraKit.setChannelProfile(.liveBroadcasting)
        // set myself as broadcaster to stream video/audio
        agoraKit.setClientRole(.broadcaster)
        
        // set up local video to render your local camera preview
        let localVideo = videos[0]
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = 0
        // the view to be binded
        videoCanvas.view = localVideo.videocanvas
        videoCanvas.renderMode = .hidden
        agoraKit.setupLocalVideo(videoCanvas)
        
        let resolution = Configs.Resolutions[Configs.defaultResolutionIdx]
        let fps = Configs.Fps[Configs.defaultFpsIdx]
        agoraKit.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: resolution.size(),
                                                                             frameRate: AgoraVideoFrameRate(rawValue: fps) ?? .fps15,
                                                                             bitrate: AgoraVideoBitrateStandard,
                                                                             orientationMode: .adaptative, mirrorMode: .auto))
        
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        let result = agoraKit.joinChannel(byToken: nil, channelId: channelField.stringValue, info: nil, uid: 0) {[unowned self] (channel, uid, elapsed) -> Void in
            self.isJoined = true
            localVideo.uid = uid
            LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
        }
        if result != 0 {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
        }
    }
    
    @IBAction func onLeavePressed(_ sender: Any) {
        agoraKit.leaveChannel { [unowned self] (stats:AgoraChannelStats) in
            LogUtils.log(message: "Left channel", level: .info)
            self.videos[0].uid = nil
            self.isJoined = false
        }
    }
    
    @IBAction func onStartRelayPressed(_ sender: Any) {
        let destinationChannelName = relayField.stringValue
        
        // prevent operation if target channel name is empty
        if(destinationChannelName.isEmpty) {
            self.showAlert(message: "Destination channel name is empty")
            return
        }
        
        // configure source info, channel name defaults to current, and uid defaults to local
        let config = AgoraChannelMediaRelayConfiguration()
        config.sourceInfo = AgoraChannelMediaRelayInfo(token: nil)
        
        // configure target channel info
        let destinationInfo = AgoraChannelMediaRelayInfo(token: nil)
        config.setDestinationInfo(destinationInfo, forChannelName: destinationChannelName)
        agoraKit.startChannelMediaRelay(config)
    }
    
    @IBAction func onStopRelayPressed(_ sender: Any) {
        agoraKit.stopChannelMediaRelay()
    }
    
    func layoutVideos(_ count: Int) {
        videos = []
        for i in 0...count - 1 {
            let view = VideoView.createFromNib()!
            if(i == 0) {
                view.placeholder.stringValue = "Local"
            } else {
                view.placeholder.stringValue = "Remote \(i)"
            }
            videos.append(view)
        }
        // layout render view
        container.layoutStream(views: videos)
    }
}

/// agora rtc engine delegate events
extension ChannelMediaRelay: AgoraRtcEngineDelegate {
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
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        
        // find a VideoView w/o uid assigned
        if let remoteVideo = videos.first(where: { $0.uid == nil }) {
            let videoCanvas = AgoraRtcVideoCanvas()
            videoCanvas.uid = uid
            // the view to be binded
            videoCanvas.view = remoteVideo.videocanvas
            videoCanvas.renderMode = .hidden
            agoraKit.setupRemoteVideo(videoCanvas)
            remoteVideo.uid = uid
        } else {
            LogUtils.log(message: "no video canvas available for \(uid), cancel bind", level: .warning)
        }
    }
    
    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)
        
        // to unlink your view from sdk, so that your view reference will be released
        // note the video will stay at its last frame, to completely remove it
        // you will need to remove the EAGL sublayer from your binded view
        if let remoteVideo = videos.first(where: { $0.uid == uid }) {
            let videoCanvas = AgoraRtcVideoCanvas()
            videoCanvas.uid = uid
            // the view to be binded
            videoCanvas.view = nil
            videoCanvas.renderMode = .hidden
            agoraKit.setupRemoteVideo(videoCanvas)
            remoteVideo.uid = nil
        } else {
            LogUtils.log(message: "no matching video canvas for \(uid), cancel unbind", level: .warning)
        }
    }
    
    /// callback when a media relay process state changed
    /// @param state state of media relay
    /// @param error error details if media relay reaches failure state
    func rtcEngine(_ engine: AgoraRtcEngineKit, channelMediaRelayStateDidChange state: AgoraChannelMediaRelayState, error: AgoraChannelMediaRelayError) {
        LogUtils.log(message: "channelMediaRelayStateDidChange: \(state.rawValue) error \(error.rawValue)", level: .info)
        
        switch(state){
        case .running:
            isRelaying = true
            break
        case .failure:
            showAlert(message: "Media Relay Failed: \(error.rawValue)")
            isRelaying = false
            break
        case .idle:
            isRelaying = false
            break
        default:break
        }
    }
    
    /// callback when a media relay event received
    /// @param event  event of media relay
    func rtcEngine(_ engine: AgoraRtcEngineKit, didReceive event: AgoraChannelMediaRelayEvent) {
        LogUtils.log(message: "didReceiveRelayEvent: \(event.rawValue)", level: .info)
    }
}
