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
    
    var channel1: AgoraRtcChannel?
    var channel2: AgoraRtcChannel?
    
    var agoraKit: AgoraRtcEngineKit!
    
    // indicate if current instance has joined channel1
    var isJoined: Bool = false {
        didSet {
            channelField1.isEnabled = !isJoined
            initJoinChannel1Button()
        }
    }
    /**
     --- Channel1 TextField ---
     */
    @IBOutlet weak var channelField1: Input!
    func initChannelField1() {
        channelField1.label.stringValue = "Channel".localized + "1"
        channelField1.field.placeholderString = "Channel Name".localized + "1"
    }
    /**
     --- Join Channel1 Button ---
     */
    @IBOutlet weak var joinChannel1Button: NSButton!
    func initJoinChannel1Button() {
        joinChannel1Button.title = isJoined ? "Leave Channel".localized : "Join Channel".localized
    }
    @IBAction func onJoinChannel1ButtonPressed(_ sender: NSButton) {
        if !isJoined {
            // auto subscribe options after join channel
            let mediaOptions = AgoraRtcChannelMediaOptions()
            mediaOptions.autoSubscribeAudio = true
            mediaOptions.autoSubscribeVideo = true
            
            var channel: AgoraRtcChannel?
            if channel1 == nil {
                channel1 = agoraKit.createRtcChannel(channelField1.stringValue)
            }
            channel = channel1
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
        } else {
            channel1?.leave()
            if let channelName = channel1?.getId() {
                if isPublished && channelName == selectedChannel {
                    if let channel = getChannelByName(selectedChannel) {
                        channel.setClientRole(.audience)
                        channel.unpublish()
                        isPublished = false
                    }
                }
                selectChannelsPicker.picker.removeItem(withTitle: channelName)
            }
            channel1?.destroy()
            channel1 = nil
            isJoined = false
        }
    }
    
    // indicate if current instance has joined channel2
    var isJoined2: Bool = false {
        didSet {
            channelField2.isEnabled = !isJoined2
            initJoinChannel2Button()
        }
    }
    /**
     --- Channel1 TextField ---
     */
    @IBOutlet weak var channelField2: Input!
    func initChannelField2() {
        channelField2.label.stringValue = "Channel".localized + "2"
        channelField2.field.placeholderString = "Channel Name".localized + "2"
    }
    /**
     --- Join Channel1 Button ---
     */
    @IBOutlet weak var joinChannel2Button: NSButton!
    func initJoinChannel2Button() {
        joinChannel2Button.title = isJoined2 ? "Leave Channel".localized : "Join Channel".localized
    }
    @IBAction func onJoinChannel2ButtonPressed(_ sender:NSButton) {
        if !isJoined2 {
            // auto subscribe options after join channel
            let mediaOptions = AgoraRtcChannelMediaOptions()
            mediaOptions.autoSubscribeAudio = true
            mediaOptions.autoSubscribeVideo = true
            
            var channel: AgoraRtcChannel?
            if channel2 == nil {
                channel2 = agoraKit.createRtcChannel(channelField2.stringValue)
            }
            channel = channel2

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
        } else {
            channel2?.leave()
            if let channelName = channel2?.getId() {
                if isPublished && channelName == selectedChannel {
                    if let channel = getChannelByName(selectedChannel) {
                        channel.setClientRole(.audience)
                        channel.unpublish()
                        isPublished = false
                    }
                }
                selectChannelsPicker.picker.removeItem(withTitle: channelName)
            }
            channel2?.destroy()
            channel2 = nil
            isJoined2 = false
        }
    }
    
    var isPublished: Bool = false {
        didSet {
            selectChannelsPicker.isEnabled = !isPublished
            initPublishButton()
        }
    }
    /**
     --- Channels Picker ---
     */
    @IBOutlet weak var selectChannelsPicker: Picker!
    var selectedChannel: String? {
        return selectChannelsPicker.picker.selectedItem?.title
    }
    func initSelectChannelsPicker() {
        selectChannelsPicker.label.stringValue = "Channel".localized
    }
    /**
     --- Publish Button ---
     */
    @IBOutlet weak var publishButton: NSButton!
    func initPublishButton() {
        publishButton.title = isPublished ? "Unpublish".localized : "Publish".localized
    }
    @IBAction func onPublishPressed(_ sender: Any) {
        if !isPublished {
            if let channel = getChannelByName(selectedChannel) {
                channel.setClientRole(.broadcaster)
                channel.publish()
                isPublished = true
            }
        } else {
            if let channel = getChannelByName(selectedChannel) {
                channel.setClientRole(.audience)
                channel.unpublish()
                isPublished = false
            }
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

        // set proxy configuration
        let proxySetting = GlobalSettings.shared.proxySetting.selectedOption().value
        agoraKit.setCloudProxy(AgoraCloudProxyType.init(rawValue: UInt(proxySetting)) ?? .noneProxy)
        
        initChannelField1()
        initJoinChannel1Button()
        initChannelField2()
        initJoinChannel2Button()
        initSelectChannelsPicker()
        initPublishButton()
        
        // set live broadcaster mode
        agoraKit.setChannelProfile(.liveBroadcasting)
        
        let resolution = Configs.Resolutions[GlobalSettings.shared.resolutionSetting.selectedOption().value]
        let fps = Configs.Fps[GlobalSettings.shared.fpsSetting.selectedOption().value]
        // enable video module and set up video encoding configs
        agoraKit.setVideoEncoderConfiguration(
            AgoraVideoEncoderConfiguration(
                size: resolution.size(),
                frameRate: AgoraVideoFrameRate(rawValue: fps) ?? .fps15,
                bitrate: AgoraVideoBitrateStandard,
                orientationMode: .adaptative
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
        agoraKit.startPreview()
    }
    
    override func viewWillBeRemovedFromSplitView() {
        channel1?.leave()
        channel1?.destroy()
        channel2?.leave()
        channel2?.destroy()
        AgoraRtcEngineKit.destroy()
    }

    func getChannelByName(_ channelName: String?) -> AgoraRtcChannel? {
        if channel1?.getId() == channelName {
            return channel1
        } else if channel2?.getId() == channelName {
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

extension JoinMultipleChannel: AgoraRtcChannelDelegate {
    func rtcChannelDidJoin(_ rtcChannel: AgoraRtcChannel, withUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "Join \(rtcChannel.getId() ?? "") with uid \(uid) elapsed \(elapsed)ms", level: .info)
        selectChannelsPicker.picker.addItem(withTitle: rtcChannel.getId()!)
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
