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

class UserModel {
    var uid: UInt = 0
    var canvasView: SampleBufferDisplayView?
    var trackId: UInt32 = 0
    var isJoin: Bool = false {
        didSet {
            self.canvasView?.videoView.isHidden = !isJoin
            self.canvasView?.videoView.reset()
        }
    }
    var isEncode: Bool = false
    var customSource: AgoraYUVImageSourcePush?
    var customEncodeSource: KFMP4Demuxer?
    
    func reset() {
        self.customSource?.stopSource()
        self.customSource = nil
        self.customEncodeSource?.cancelReading()
        self.customEncodeSource = nil
        
        self.uid = UInt(Int.random(in: 10000...99999))
        self.trackId = 0
        self.isJoin = false
        self.isEncode = false
    }
}

class CustomVideoSourcePushMulti: BaseViewController {
    @IBOutlet weak var Container: AGEVideoContainer!
    private let localUid: UInt = UInt(Int.random(in: 10000...99999))
    lazy var remoteVideos: [UserModel] = (0..<4).map({ _ in
        let model = UserModel()
        model.canvasView = SampleBufferDisplayView.createFromNib()
        model.reset()
        return model
    })
    @IBOutlet weak var createVideoTrackButton: NSButton!
    @IBOutlet weak var destoryVideoTrackButton: NSButton!
    
    var agoraKit: AgoraRtcEngineKit!
    
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
    func initSelectLayoutPicker() {
        layoutVideos(2)
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
            initJoinChannelButton()
        }
    }
    
    // indicate for doing something
    var isProcessing: Bool = false {
        didSet {
//            joinChannelButton.isEnabled = !isProcessing
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do view setup here.
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit.enableVideo()
        
        createVideoTrackButton.title = "Create Video Track".localized
        destoryVideoTrackButton.title = "Destory Track".localized
        
        initSelectResolutionPicker()
        initSelectFpsPicker()
        initSelectLayoutPicker()
        initChannelField()
        initJoinChannelButton()
    }
    
    override func viewWillBeRemovedFromSplitView() {
        if isJoined {
            remoteVideos.forEach({
                let connection = AgoraRtcConnection()
                connection.localUid = $0.uid
                connection.channelId = channelField.stringValue
                agoraKit.leaveChannelEx(connection) { state in
                    LogUtils.log(message: "leaveChannelEx: \(state.description)", level: .info)
                }
                $0.reset()
            })
            let connection = AgoraRtcConnection()
            connection.localUid = localUid
            connection.channelId = channelField.stringValue
            agoraKit.leaveChannelEx(connection) { state in
                LogUtils.log(message: "leaveChannelEx : \(state.description)", level: .info)
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
            guard let resolution = selectedResolution,
                  let fps = selectedFps else {
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
                    size: resolution.size(),
                    frameRate: AgoraVideoFrameRate(rawValue: fps) ?? .fps15,
                    bitrate: AgoraVideoBitrateStandard,
                    orientationMode: .adaptative,
                    mirrorMode: .auto
                )
            )
            // start joining channel
            // 1. Users can only see each other after they join the
            // same channel successfully using the same app id.
            // 2. If app certificate is turned on at dashboard, token is needed
            // when joining channel. The channel name and uid used to calculate
            // the token has to match the ones used for channel join
            isProcessing = true
            joinChannel(uid: localUid, trackId: nil, publishEncodedVideoTrack: false)
        } else {
            LogUtils.log(message: "Left channel", level: .info)
            isProcessing = false
            isJoined = false
            remoteVideos.forEach({
                let trackId = $0.trackId
                agoraKit.destroyCustomEncodedVideoTrack(UInt(trackId))
                agoraKit.destroyCustomVideoTrack(UInt(trackId))
                $0.reset()
            })
        }
    }
    
    private func joinChannel(uid: UInt, trackId: UInt32?, publishEncodedVideoTrack: Bool) {
        let channelName = channelField.stringValue
        let option = AgoraRtcChannelMediaOptions()
        option.publishMicrophoneTrack = false
        option.publishCameraTrack = false
        
        let delegate: AgoraRtcEngineDelegate? = self
        if let trackId = trackId {
            option.publishCustomVideoTrack = !publishEncodedVideoTrack
            option.customVideoTrackId = Int(trackId)
            option.publishEncodedVideoTrack = publishEncodedVideoTrack
            option.clientRoleType = .broadcaster
            option.autoSubscribeAudio = false
            option.autoSubscribeVideo = false
//            delegate = nil
        } else {
            option.clientRoleType = .audience
            option.autoSubscribeAudio = true
            option.autoSubscribeVideo = true
        }
        
        NetworkManager.shared.generateToken(channelName: channelName, uid: uid) { token in
            let connection = AgoraRtcConnection()
            connection.localUid = uid
            connection.channelId = channelName
            let result = self.agoraKit.joinChannelEx(byToken: token,
                                                     connection: connection,
                                                     delegate: delegate,
                                                     mediaOptions: option,
                                                     joinSuccess: nil)
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
                self.showAlert(title: "Error", message: "joinChannel[\(channelName)] uid: \(connection.localUid) call failed: \(result), please check your params")
            }
        }
    }
    
    @IBAction func onClickCustomEncodeVideoTrack(_ sender: Any) {
        if(!isJoined) {
            return
        }
        guard let userModel = remoteVideos.first(where: { $0.isJoin == false }) else { return }
        if userModel.trackId == 0 {
            let encodeVideoOptions = AgoraEncodedVideoTrackOptions()
            // - VIDEO_CODEC_H264 = 2: (Default) H.264.
            encodeVideoOptions.codecType = 2
            userModel.trackId = agoraKit.createCustomEncodedVideoTrack(encodeVideoOptions)
            userModel.isEncode = true
            userModel.isJoin = true
        } else {
            return
        }
        NetworkManager.shared.download(urlString: "https://agora-adc-artifacts.s3.cn-north-1.amazonaws.com.cn/resources/sample.mp4") { response in
            guard userModel.isJoin else {return}
            let path = response["path"] as? String
            let config = KFDemuxerConfig()
            config.demuxerType = .video
            let asset = AVAsset(url: URL(fileURLWithPath: path ?? ""))
            config.asset = asset
            guard let demuxer = KFMP4Demuxer(config: config) else {return}
            userModel.customEncodeSource = demuxer
            demuxer.startReading()
            demuxer.dataCallBack = { [weak self, weak demuxer] data, sampleBuffer in
                guard let self = self, let data = data, let sampleBuffer = sampleBuffer else { return }
                let info = AgoraEncodedVideoFrameInfo()
                info.frameType = .keyFrame
                info.framesPerSecond = 30
                info.codecType = .H264
                if(userModel.trackId == 0){
                    return
                }
                self.agoraKit.pushExternalEncodedVideoFrame(data,
                                                            info: info,
                                                            videoTrackId: UInt(userModel.trackId))
                userModel.canvasView?.videoView.renderVideoSampleBuffer(sampleBuffer, size: demuxer?.videoSize ?? .zero)
            }
        } failure: { error in
            LogUtils.log(message: "download video error == \(error)", level: .error)
        }
        joinChannel(uid: userModel.uid, trackId: userModel.trackId, publishEncodedVideoTrack: true)
    }
    
    @IBAction func onClickCreateTrack(_ sender: Any) {
        if(!isJoined) {
            return
        }
        guard let userModel = remoteVideos.first(where: { $0.isJoin == false }) else { return }
        if userModel.trackId == 0 {
            userModel.trackId = agoraKit.createCustomVideoTrack()
            userModel.isEncode = false
        } else {
            return
        }
        createVideoTrack(userModel: userModel)
    }
    
    private func createVideoTrack(userModel: UserModel) {
        let customCamera = AgoraYUVImageSourcePush(size: CGSize(width: 320, height: 180),
                                                   fileName: "sample" ,
                                                   frameRate: 15,
                                                   isHDR: false)
        customCamera.trackId = userModel.trackId
        customCamera.delegate = self
        userModel.isJoin = true
        userModel.customSource = customCamera
        customCamera.startSource()
        joinChannel(uid: userModel.uid, 
                    trackId: userModel.trackId,
                    publishEncodedVideoTrack: userModel.isEncode)
    }
    
    @IBAction func onClickDestoryTrack(_ sender: Any) {
        if(!isJoined) {
            return
        }
        let channelName = channelField.stringValue
        guard let userModel = remoteVideos.filter({ $0.trackId != 0 }).last else { return }
        let trackId = UInt(userModel.trackId)
        let connection = AgoraRtcConnection()
        connection.localUid = userModel.uid
        connection.channelId = channelName
        agoraKit.destroyCustomVideoTrack(trackId)
        agoraKit.destroyCustomEncodedVideoTrack(trackId)
        agoraKit.leaveChannelEx(connection) { state in
            LogUtils.log(message: "leaveChannelEx: \(state.description)", level: .info)
        }
        
        cleanCanvas(uid: userModel.uid)
    }
    
    private func layoutVideos(_ count: Int) {
        Container.layoutStream(views: remoteVideos.compactMap({ $0.canvasView }))
    }
    
    private func cleanCanvas(uid: UInt) {
        guard let userModel = remoteVideos.first(where: { $0.uid == uid }) else { return }
        
        // to unlink your view from sdk, so that your view reference will be released
        // note the video will stay at its last frame, to completely remove it
        // you will need to remove the EAGL sublayer from your binded view
        userModel.reset()
        
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        let connect = AgoraRtcConnection()
        connect.localUid = localUid
        connect.channelId = channelField.stringValue
        agoraKit.setupRemoteVideoEx(videoCanvas, connection: connect)
    }
}

/// agora rtc engine delegate events
extension CustomVideoSourcePushMulti: AgoraRtcEngineDelegate {
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
    /// en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
    /// cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
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
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        if let _ = remoteVideos.first(where: { $0.uid == uid }) { return }
        guard let userModel = remoteVideos.first(where: { $0.isJoin == false && $0.trackId == 0 }) else { return }
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        
        userModel.uid = uid
        userModel.isJoin = true
        
        // the view to be binded
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        videoCanvas.view = userModel.canvasView?.videoView
        videoCanvas.renderMode = .fit
        
        let connect = AgoraRtcConnection()
        connect.localUid = localUid
        connect.channelId = channelField.stringValue
        agoraKit.setupRemoteVideoEx(videoCanvas, connection: connect)
    }
    
    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        guard let _ = remoteVideos.first(where: { $0.uid == uid }) else { return }
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)
        
        cleanCanvas(uid: uid)
    }
}

/// agora camera video source, the delegate will get frame data from camera
extension CustomVideoSourcePushMulti: AgoraYUVImageSourcePushDelegate {
    func onVideoFrame(_ buffer: CVPixelBuffer, size: CGSize, trackId: UInt, rotation: Int32) {
        let videoFrame = AgoraVideoFrame()
        videoFrame.format = AgoraVideoFormat.cvPixelNV12.rawValue
        videoFrame.textureBuf = buffer
        videoFrame.rotation = Int32(rotation)
        
        let outputVideoFrame = AgoraOutputVideoFrame()
        outputVideoFrame.width = Int32(size.width)
        outputVideoFrame.height = Int32(size.height)
        outputVideoFrame.pixelBuffer = buffer
        outputVideoFrame.rotation = rotation
        let userModel = remoteVideos.first(where: { $0.trackId == trackId })
        userModel?.canvasView?.videoView.renderVideoPixelBuffer(outputVideoFrame)
        //once we have the video frame, we can push to agora sdk
        agoraKit.pushExternalVideoFrame(videoFrame, videoTrackId: trackId)
    }
}
