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

class RawMediaData: BaseViewController {
    var videos: [VideoView] = []
    
    var agoraKit: AgoraRtcEngineKit!
    
    var agoraMediaDataPlugin: AgoraMediaDataPlugin?
    
    @IBOutlet weak var Container: AGEVideoContainer!
    
    /**
     --- Resolutions Picker ---
     */
    @IBOutlet weak var selectResolutionPicker: Picker!
    var selectedResolution: Resolution? {
        let index = self.selectResolutionPicker.indexOfSelectedItem
        if index >= 0 && index < Configs.Resolutions.count {
            return Configs.Resolutions[index]
        } else {
            return nil
        }
    }
    func initSelectResolutionPicker() {
        selectResolutionPicker.label.stringValue = "Resolution".localized
        selectResolutionPicker.picker.addItems(withTitles: Configs.Resolutions.map { $0.name() })
        selectResolutionPicker.picker.selectItem(at: GlobalSettings.shared.resolutionSetting.selectedOption().value)
        
        selectResolutionPicker.onSelectChanged {
            if !self.isJoined {
                return
            }
            
            guard let resolution = self.selectedResolution,
                  let fps = self.selectedFps else {
                return
            }
            self.agoraKit.setVideoEncoderConfiguration(
                AgoraVideoEncoderConfiguration(
                    size: resolution.size(),
                    frameRate: AgoraVideoFrameRate(rawValue: fps) ?? .fps15,
                    bitrate: AgoraVideoBitrateStandard,
                    orientationMode: .adaptative,
                    mirrorMode: .auto
                )
            )
        }
    }
    
    /**
     --- Fps Picker ---
     */
    @IBOutlet weak var selectFpsPicker: Picker!
    var selectedFps: Int? {
        let index = self.selectFpsPicker.indexOfSelectedItem
        if index >= 0 && index < Configs.Fps.count {
            return Configs.Fps[index]
        } else {
            return nil
        }
    }
    func initSelectFpsPicker() {
        selectFpsPicker.label.stringValue = "Frame Rate".localized
        selectFpsPicker.picker.addItems(withTitles: Configs.Fps.map { "\($0)fps" })
        selectFpsPicker.picker.selectItem(at: GlobalSettings.shared.fpsSetting.selectedOption().value)
        
        selectFpsPicker.onSelectChanged {
            if !self.isJoined {
                return
            }
            guard let resolution = self.selectedResolution,
                  let fps = self.selectedFps else {
                return
            }
            self.agoraKit.setVideoEncoderConfiguration(
                AgoraVideoEncoderConfiguration(
                    size: resolution.size(),
                    frameRate: AgoraVideoFrameRate(rawValue: fps) ?? .fps15,
                    bitrate: AgoraVideoBitrateStandard,
                    orientationMode: .adaptative,
                    mirrorMode: .auto
                )
            )
        }
    }
    
    /**
     --- Layout Picker ---
     */
    @IBOutlet weak var selectLayoutPicker: Picker!
    let layouts = [Layout("1v1", 2), Layout("1v3", 4), Layout("1v8", 9), Layout("1v15", 16)]
    var selectedLayout: Layout? {
        let index = self.selectLayoutPicker.indexOfSelectedItem
        if index >= 0 && index < layouts.count {
            return layouts[index]
        } else {
            return nil
        }
    }
    func initSelectLayoutPicker() {
        layoutVideos(2)
        selectLayoutPicker.label.stringValue = "Layout".localized
        selectLayoutPicker.picker.addItems(withTitles: layouts.map { $0.label })
        selectLayoutPicker.onSelectChanged {
            if self.isJoined {
                return
            }
            guard let layout = self.selectedLayout else { return }
            self.layoutVideos(layout.value)
        }
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
            selectLayoutPicker.isEnabled = !isJoined
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
        // Do view setup here.
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        agoraKit.enableVideo()
        
        initSelectResolutionPicker()
        initSelectFpsPicker()
        initSelectLayoutPicker()
        initChannelField()
        initJoinChannelButton()
    }
    
    override func viewWillBeRemovedFromSplitView() {
        if isJoined {
            // deregister video observer
            let videoType:ObserverVideoType = ObserverVideoType(rawValue: ObserverVideoType.captureVideo.rawValue | ObserverVideoType.renderVideo.rawValue | ObserverVideoType.preEncodeVideo.rawValue)
            agoraMediaDataPlugin?.deregisterVideoRawDataObserver(videoType)
            
            // deregister audio observer
            let audioType:ObserverAudioType = ObserverAudioType(rawValue: ObserverAudioType.recordAudio.rawValue | ObserverAudioType.playbackAudioFrameBeforeMixing.rawValue | ObserverAudioType.mixedAudio.rawValue | ObserverAudioType.playbackAudio.rawValue) ;
            agoraMediaDataPlugin?.deregisterAudioRawDataObserver(audioType)
            
            // deregister packet observer
            let packetType:ObserverPacketType = ObserverPacketType(rawValue: ObserverPacketType.sendAudio.rawValue | ObserverPacketType.sendVideo.rawValue | ObserverPacketType.receiveAudio.rawValue | ObserverPacketType.receiveVideo.rawValue)
            agoraMediaDataPlugin?.deregisterPacketRawDataObserver(packetType)
            
            agoraKit.leaveChannel { (stats:AgoraChannelStats) in
                LogUtils.log(message: "Left channel", level: .info)
            }
        }
        AgoraRtcEngineKit.destroy()
    }
    
    func registerAgoraMediaDataPlugin() {
        if agoraMediaDataPlugin == nil {
            // setup raw media data observers
            agoraMediaDataPlugin = AgoraMediaDataPlugin(agoraKit: agoraKit)
            
            // Register audio observer
            let audioType: ObserverAudioType = ObserverAudioType(rawValue: ObserverAudioType.recordAudio.rawValue | ObserverAudioType.playbackAudioFrameBeforeMixing.rawValue | ObserverAudioType.mixedAudio.rawValue | ObserverAudioType.playbackAudio.rawValue) ;
            agoraMediaDataPlugin?.registerAudioRawDataObserver(audioType)
            agoraMediaDataPlugin?.audioDelegate = self

            // Register video observer
            let videoType: ObserverVideoType = ObserverVideoType(rawValue: ObserverVideoType.captureVideo.rawValue | ObserverVideoType.renderVideo.rawValue | ObserverVideoType.preEncodeVideo.rawValue)
            agoraMediaDataPlugin?.registerVideoRawDataObserver(videoType)
            agoraMediaDataPlugin?.videoDelegate = self;

            // Register packet observer
            let packetType: ObserverPacketType = ObserverPacketType(rawValue: ObserverPacketType.sendAudio.rawValue | ObserverPacketType.sendVideo.rawValue | ObserverPacketType.receiveAudio.rawValue | ObserverPacketType.receiveVideo.rawValue)
            agoraMediaDataPlugin?.registerPacketRawDataObserver(packetType)
            agoraMediaDataPlugin?.packetDelegate = self;
        }
        
        agoraKit.setRecordingAudioFrameParametersWithSampleRate(44100, channel: 1, mode: .readWrite, samplesPerCall: 1024)
        agoraKit.setMixedAudioFrameParametersWithSampleRate(44100, channel: 1, samplesPerCall: 1024)
        agoraKit.setPlaybackAudioFrameParametersWithSampleRate(44100, channel: 1, mode: .readWrite, samplesPerCall: 1024)
    }
    
    @IBAction func onJoinPressed(_ sender:Any) {
        if !isJoined {
            // check configuration
            let channel = channelField.stringValue
            if channel.isEmpty {
                return
            }
            guard let resolution = selectedResolution,
                  let fps = selectedFps else {
                return
            }
            // set live broadcaster mode
            agoraKit.setChannelProfile(.liveBroadcasting)
            // set proxy configuration
//            let proxySetting = GlobalSettings.shared.proxySetting.selectedOption().value
//            agoraKit.setCloudProxy(AgoraCloudProxyType.init(rawValue: UInt(proxySetting)) ?? .noneProxy)
            // set myself as broadcaster to stream video/audio
            agoraKit.setClientRole(.broadcaster)
            // enable video module and set up video encoding configs
            agoraKit.setVideoEncoderConfiguration(
                AgoraVideoEncoderConfiguration(
                    size: resolution.size(),
                    frameRate: AgoraVideoFrameRate(rawValue: fps) ?? .fps15,
                    bitrate: AgoraVideoBitrateStandard,
                    orientationMode: .adaptative,
                    mirrorMode: .auto
                )
            )
            registerAgoraMediaDataPlugin()
            
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
            option.publishCameraTrack = .of(true)
            option.clientRoleType = .of((Int32)(AgoraClientRole.broadcaster.rawValue))
            let result = agoraKit.joinChannel(byToken: KeyCenter.Token, channelId: channel, uid: 0, mediaOptions: option)
            if result != 0 {
                isProcessing = false
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
            }
        } else {
            isProcessing = true
            agoraKit.leaveChannel { [unowned self] (stats:AgoraChannelStats) in
                LogUtils.log(message: "Left channel", level: .info)
                self.isProcessing = false
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
extension RawMediaData: AgoraRtcEngineDelegate {
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
}


// audio data plugin, here you can process raw audio data
// note this all happens in CPU so it comes with a performance cost
extension RawMediaData: AgoraAudioDataPluginDelegate {
    /// Retrieves the recorded audio frame.
    func mediaDataPlugin(_ mediaDataPlugin: AgoraMediaDataPlugin, didRecord audioRawData: AgoraAudioRawData) -> AgoraAudioRawData {
        return audioRawData
    }
    
    /// Retrieves the audio playback frame for getting the audio.
    func mediaDataPlugin(_ mediaDataPlugin: AgoraMediaDataPlugin, willPlaybackAudioRawData audioRawData: AgoraAudioRawData) -> AgoraAudioRawData {
        return audioRawData
    }
    
    /// Retrieves the audio frame of a specified user before mixing.
    /// The SDK triggers this callback if isMultipleChannelFrameWanted returns false.
    func mediaDataPlugin(_ mediaDataPlugin: AgoraMediaDataPlugin, willPlaybackBeforeMixing audioRawData: AgoraAudioRawData, ofUid uid: uint) -> AgoraAudioRawData {
        return audioRawData
    }
    
    /// Retrieves the mixed recorded and playback audio frame.
    func mediaDataPlugin(_ mediaDataPlugin: AgoraMediaDataPlugin, didMixedAudioRawData audioRawData: AgoraAudioRawData) -> AgoraAudioRawData {
        return audioRawData
    }
}

// video data plugin, here you can process raw video data
// note this all happens in CPU so it comes with a performance cost
extension RawMediaData : AgoraVideoDataPluginDelegate
{
    /// Occurs each time the SDK receives a video frame captured by the local camera.
    /// After you successfully register the video frame observer, the SDK triggers this callback each time a video frame is received. In this callback, you can get the video data captured by the local camera. You can then pre-process the data according to your scenarios.
    /// After pre-processing, you can send the processed video data back to the SDK by setting the videoFrame parameter in this callback.
    func mediaDataPlugin(_ mediaDataPlugin: AgoraMediaDataPlugin, didCapturedVideoRawData videoRawData: AgoraVideoRawData) -> AgoraVideoRawData {
        return videoRawData
    }
    
    /// Occurs each time the SDK receives a video frame before sending to encoder
    /// After you successfully register the video frame observer, the SDK triggers this callback each time a video frame is going to be sent to encoder. In this callback, you can get the video data before it is sent to enoder. You can then pre-process the data according to your scenarios.
    /// After pre-processing, you can send the processed video data back to the SDK by setting the videoFrame parameter in this callback.
    func mediaDataPlugin(_ mediaDataPlugin: AgoraMediaDataPlugin, willPreEncode videoRawData: AgoraVideoRawData) -> AgoraVideoRawData {
        return videoRawData
    }
    
    /// Occurs each time the SDK receives a video frame sent by the remote user.
    ///After you successfully register the video frame observer and isMultipleChannelFrameWanted return false, the SDK triggers this callback each time a video frame is received. In this callback, you can get the video data sent by the remote user. You can then post-process the data according to your scenarios.
    ///After post-processing, you can send the processed data back to the SDK by setting the videoFrame parameter in this callback.
    func mediaDataPlugin(_ mediaDataPlugin: AgoraMediaDataPlugin, willRenderVideoRawData videoRawData: AgoraVideoRawData, ofUid uid: uint) -> AgoraVideoRawData {
        return videoRawData
    }
}

// packet data plugin, here you can process raw network packet(before decoding/encoding)
// note this all happens in CPU so it comes with a performance cost
extension RawMediaData : AgoraPacketDataPluginDelegate
{
    /// Occurs when the local user sends a video packet.
    func mediaDataPlugin(_ mediaDataPlugin: AgoraMediaDataPlugin, willSendVideoPacket videoPacket: AgoraPacketRawData) -> AgoraPacketRawData {
        return videoPacket
    }
    
    /// Occurs when the local user sends an audio packet.
    func mediaDataPlugin(_ mediaDataPlugin: AgoraMediaDataPlugin, willSendAudioPacket audioPacket: AgoraPacketRawData) -> AgoraPacketRawData {
        return audioPacket
    }
    
    /// Occurs when the local user receives a video packet.
    func mediaDataPlugin(_ mediaDataPlugin: AgoraMediaDataPlugin, didReceivedVideoPacket videoPacket: AgoraPacketRawData) -> AgoraPacketRawData {
        return videoPacket
    }
    
    /// Occurs when the local user receives an audio packet.
    func mediaDataPlugin(_ mediaDataPlugin: AgoraMediaDataPlugin, didReceivedAudioPacket audioPacket: AgoraPacketRawData) -> AgoraPacketRawData {
        return audioPacket
    }
}
