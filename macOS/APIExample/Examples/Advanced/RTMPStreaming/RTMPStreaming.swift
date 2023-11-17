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

let CANVAS_WIDTH = 640
let CANVAS_HEIGHT = 480

class RTMPStreaming: BaseViewController {
    var videos: [VideoView] = []
    
    @IBOutlet weak var Container: AGEVideoContainer!

    var agoraKit: AgoraRtcEngineKit!
    
    var transcoding = AgoraLiveTranscoding.default()
    
    /**
     --- rtmpUrls Picker ---
     */
    @IBOutlet weak var selectRtmpUrlsPicker: Picker!
    @IBOutlet weak var removeURLBtn: NSButton!
    @IBOutlet weak var removeAllURLBtn: NSButton!
    var rtmpURLs: [String] = []
    var selectedrtmpUrl: String? {
        let index = self.selectRtmpUrlsPicker.indexOfSelectedItem
        if index >= 0 && index < rtmpURLs.count {
            return rtmpURLs[index]
        } else {
            return nil
        }
    }
    func initSelectRtmpUrlsPicker() {
        selectRtmpUrlsPicker.label.stringValue = "urls"
        selectRtmpUrlsPicker.picker.addItems(withTitles: rtmpURLs)
    }
    /// callback when remove streaming url button hit
    @IBAction func onRemoveStreamingURL(_ sender: Any) {
        guard let selectedURL = selectedrtmpUrl else { return }
        agoraKit.stopRtmpStream(selectedURL)
        rtmpURLs.remove(at: selectRtmpUrlsPicker.indexOfSelectedItem)
        selectRtmpUrlsPicker.picker.removeItem(at: selectRtmpUrlsPicker.indexOfSelectedItem)
    }
    
    /// callback when remove all streaming url button hit
    @IBAction func onRemoveAllStreamingURL(_ sender: Any) {
        for url in rtmpURLs {
            agoraKit.stopRtmpStream(url)
        }
        rtmpURLs = []
        selectRtmpUrlsPicker.picker.removeAllItems()
    }
    
    /**
     --- Channel TextField ---
     */
    @IBOutlet weak var channelField: Input!
    func initChannelField() {
        channelField.label.stringValue = "Channel".localized
        channelField.field.placeholderString = "Channel Name".localized
    }
    
    /**
     --- rtmp TextField ---
     */
    @IBOutlet weak var rtmpURLField: Input!
    @IBOutlet weak var transcodingCheckBox: NSButton!
    var transcodingEnabled: Bool {
        get {
            return transcodingCheckBox.state == .on
        }
    }
    @IBOutlet weak var addURLBtn: NSButton!
    func initRtmpURLField() {
        rtmpURLField.label.stringValue = "rtmp"
        rtmpURLField.field.placeholderString = "rtmp://"
    }
    /// callback when publish button hit
    @IBAction func onAddStreamingURL(_ sender: Any) {
        //let transcodingEnabled = transcodingCheckBox.state == .on
        let rtmpURL = rtmpURLField.stringValue
        if(rtmpURL.isEmpty || !rtmpURL.starts(with: "rtmp://")) {
            showAlert(title: "Add Streaming URL Failed", message: "RTMP URL cannot be empty or not start with 'rtmp://'")
            return
        }
        if transcodingEnabled {
            // we will use transcoding to composite multiple hosts' video
            // therefore we have to create a livetranscoding object and call before addPublishStreamUrl
            transcoding.size = CGSize(width: CANVAS_WIDTH, height: CANVAS_HEIGHT)
            agoraKit.startRtmpStream(withTranscoding: rtmpURL, transcoding: transcoding)
        } else {
            agoraKit.startRtmpStreamWithoutTranscoding(rtmpURL)
        }
        
        // update properties and UI
        rtmpURLs.append(rtmpURL)
        selectRtmpUrlsPicker.picker.addItem(withTitle: rtmpURL)
    }
    
    /**
     --- Button ---
     */
    @IBOutlet weak var joinChannelButton: NSButton!
    func initJoinChannelButton() {
        joinChannelButton.title = isJoined ? "Leave Channel".localized : "Join Channel".localized
    }
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false {
        didSet {
            channelField.isEnabled = !isJoined
            initJoinChannelButton()
        }
    }
    
    // indicate for doing something
    var isProcessing: Bool = false {
        didSet {
            joinChannelButton.isEnabled = !isProcessing
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        layoutVideos(2)
        // Do view setup here.
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit.enableVideo()
        
        initSelectRtmpUrlsPicker()
        initRtmpURLField()
        initChannelField()
        initJoinChannelButton()
    }
    
    override func viewWillBeRemovedFromSplitView() {
        if isJoined {
            agoraKit.leaveChannel { (stats:AgoraChannelStats) in
                LogUtils.log(message: "Left channel", level: .info)
            }
        }
        AgoraRtcEngineKit.destroy()
    }
    
    @IBAction func onJoinPressed(_ sender:Any) {
        if !isJoined {
            // check configuration
            let channel = channelField.stringValue
            if channel.isEmpty {
                return
            }
            // set live broadcaster mode
            agoraKit.setChannelProfile(.liveBroadcasting)
            // set myself as broadcaster to stream video/audio
            agoraKit.setClientRole(.broadcaster)
            // set proxy configuration
//            let proxySetting = GlobalSettings.shared.proxySetting.selectedOption().value
//            agoraKit.setCloudProxy(AgoraCloudProxyType.init(rawValue: UInt(proxySetting)) ?? .noneProxy)
            // enable video module and set up video encoding configs
            agoraKit.setVideoEncoderConfiguration(
                AgoraVideoEncoderConfiguration(
                    size: Configs.Resolutions[GlobalSettings.shared.resolutionSetting.selectedOption().value].size(),
                    frameRate: AgoraVideoFrameRate(rawValue: Configs.Fps[GlobalSettings.shared.fpsSetting.selectedOption().value]) ?? .fps15,
                    bitrate: AgoraVideoBitrateStandard,
                    orientationMode: .adaptative,
                    mirrorMode: .auto
                )
            )
            // set up local video to render your local camera preview
            let localVideo = videos[0]
            let videoCanvas = AgoraRtcVideoCanvas()
            videoCanvas.uid = 0
            // the view to be binded
            videoCanvas.view = localVideo.videocanvas
            videoCanvas.renderMode = .hidden
            agoraKit.setupLocalVideo(videoCanvas)
            // you have to call startPreview to see local video
            agoraKit.startPreview()
            
            // start joining channel
            // 1. Users can only see each other after they join the
            // same channel successfully using the same app id.
            // 2. If app certificate is turned on at dashboard, token is needed
            // when joining channel. The channel name and uid used to calculate
            // the token has to match the ones used for channel join
            isProcessing = true
            let option = AgoraRtcChannelMediaOptions()
            option.publishCameraTrack = true
            option.clientRoleType = .broadcaster
            NetworkManager.shared.generateToken(channelName: channel, success: { token in
                let result = self.agoraKit.joinChannel(byToken: token, channelId: channel, uid: 0, mediaOptions: option)
                if result != 0 {
                    self.isProcessing = false
                    // Usually happens with invalid parameters
                    // Error code description can be found at:
                    // en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
                    // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                    self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
                }
            })
        } else {
            isProcessing = true
            agoraKit.leaveChannel { [unowned self] (stats:AgoraChannelStats) in
                self.isProcessing = false
                LogUtils.log(message: "Left channel", level: .info)
                self.videos[0].uid = nil
                self.isJoined = false
                self.videos.forEach {
                    $0.uid = nil
                    $0.statsLabel.stringValue = ""
                }
            }
        }
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
        Container.layoutStream(views: videos)
    }
}

/// agora rtc engine delegate events
extension RTMPStreaming: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://api-ref.agora.io/en/voice-sdk/ios/3.x/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.rawValue)", level: .warning)
    }
    
    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode)", level: .error)
        if isProcessing {
            isProcessing = false
        }
        self.showAlert(title: "Error", message: "Error \(errorCode.rawValue) occur")
    }
    
    /// callback when the local user joins a specified channel.
    /// @param channel
    /// @param uid uid of local user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        isProcessing = false
        isJoined = true
        let localVideo = videos[0]
        localVideo.uid = uid
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
        
        // update live transcoding
        // add new user onto the canvas
        let user = AgoraLiveTranscodingUser()
        user.rect = CGRect(x: CANVAS_WIDTH / 2, y: 0, width: CANVAS_WIDTH / 2, height: CANVAS_HEIGHT)
        user.uid = uid
        self.transcoding.add(user)
        agoraKit.updateRtmpTranscoding(transcoding)
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
        
        // remove user from canvas if current cohost left channel
        transcoding.removeUser(uid)
        agoraKit.updateRtmpTranscoding(transcoding)
    }
    
    /// callback for state of rtmp streaming, for both good and bad state
    /// @param url rtmp streaming url
    /// @param state state of rtmp streaming
    /// @param reason
    func rtcEngine(_ engine: AgoraRtcEngineKit, rtmpStreamingChangedToState url: String, state: AgoraRtmpStreamingState, reason: AgoraRtmpStreamingReason) {
        LogUtils.log(message: "rtmp streaming: \(url) state \(state.rawValue) error \(reason.rawValue)", level: .info)
        if(state == .running) {
            self.showAlert(title: "Notice", message: "\(url) Publish Success")
        } else if(state == .failure) {
//            self.showAlert(title: "Error", message: "\(url) Publish Failed: \(errCode.rawValue)")
        } else if(state == .idle) {
            self.showAlert(title: "Notice", message: "\(url) Publish Stopped")
        }
    }
    
    /// callback when live transcoding is properly updated
    func rtcEngineTranscodingUpdated(_ engine: AgoraRtcEngineKit) {
        LogUtils.log(message: "live transcoding updated", level: .info)
    }
}
