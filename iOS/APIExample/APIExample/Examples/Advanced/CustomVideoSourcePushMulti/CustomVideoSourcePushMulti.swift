//
//  CustomVideoSourcePush.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//
import UIKit
import AGEVideoLayout
import AgoraRtcKit
import AVFoundation

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

class CustomVideoSourcePushMultiEntry: UIViewController {
    @IBOutlet weak var joinButton: AGButton!
    @IBOutlet weak var channelTextField: AGTextField!
    let identifier = "CustomVideoSourcePushMulti"
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    @IBAction func doJoinPressed(sender: AGButton) {
        guard let channelName = channelTextField.text else { return }
        // resign channel text field
        channelTextField.resignFirstResponder()
        
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        // create new view controller every time to ensure we get a clean vc
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else {
            return
        }
        newViewController.title = channelName
        newViewController.configs = ["channelName": channelName]
        self.navigationController?.pushViewController(newViewController, animated: true)
    }
}

class CustomVideoSourcePushMultiMain: BaseViewController {
    private let localUid: UInt = UInt(Int.random(in: 10000...99999))
    lazy var remoteVideos: [UserModel] = (0..<4).map({ _ in
        let model = UserModel()
        model.canvasView = Bundle.loadView(fromNib: "VideoViewSampleBufferDisplayView",
                                           withType: SampleBufferDisplayView.self)
        model.reset()
        return model
    })
    
    @IBOutlet weak var container: AGEVideoContainer!
    var agoraKit: AgoraRtcEngineKit!
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // layout render view
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        
        container.layoutStream2x2(views: remoteVideos.compactMap({ $0.canvasView }))
        // make myself a broadcaster
        // agoraKit.setClientRole(.broadcaster)
        
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        agoraKit.enableAudio()
        
        let resolution = (GlobalSettings.shared.getSetting(key: "resolution")?.selectedOption().value as? CGSize) ?? .zero
        let fps = (GlobalSettings.shared.getSetting(key: "fps")?.selectedOption().value as? AgoraVideoFrameRate) ?? .fps15
        let orientation = (GlobalSettings.shared.getSetting(key: "orientation")?
            .selectedOption().value as? AgoraVideoOutputOrientationMode) ?? .fixedPortrait
        agoraKit.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: resolution,
                                                                             frameRate: fps,
                                                                             bitrate: AgoraVideoBitrateStandard,
                                                                             orientationMode: orientation,
                                                                             mirrorMode: .auto))
        
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        joinChannel(uid: localUid,
                    trackId: nil,
                    publishEncodedVideoTrack: false)
    }
    private func joinChannel(uid: UInt, trackId: UInt32?, publishEncodedVideoTrack: Bool) {
        guard let channelName = configs["channelName"] as? String else {return}
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
                self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
            }
        }
    }
    
    @IBAction func onCreateCustomEncodeVideoTrack(_ sender: Any) {
        guard let userModel = remoteVideos.first(where: { $0.isJoin == false }) else { return }
        if userModel.trackId == 0 {
            let encodeVideoOptions = AgoraEncodedVideoTrackOptions()
            // - VIDEO_CODEC_H264 = 2: (Default) H.264.
            encodeVideoOptions.codecType = 2
            userModel.trackId = agoraKit.createCustomEncodedVideoTrack(encodeVideoOptions)
            userModel.isEncode = true
            userModel.isJoin = true
        }
        let urlString = "https://agora-adc-artifacts.s3.cn-north-1.amazonaws.com.cn/resources/sample.mp4"
        NetworkManager.shared.download(urlString: urlString) { response in
            let path = response["path"] as? String
            let config = KFDemuxerConfig()
            config.demuxerType = .video
            let asset = AVAsset(url: URL(fileURLWithPath: path ?? ""))
            config.asset = asset
            let demuxer = KFMP4Demuxer(config: config)
            userModel.customEncodeSource = demuxer
            demuxer?.startReading()
            demuxer?.dataCallBack = { [weak self] data, sampleBuffer in
                guard let self = self, let data = data, let sampleBuffer = sampleBuffer else { return }
                let info = AgoraEncodedVideoFrameInfo()
                info.frameType = .keyFrame
                info.framesPerSecond = 30
                info.codecType = .H264
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
    @IBAction func onCreateVideoTrack(_ sender: Any) {
        guard let userModel = remoteVideos.first(where: { $0.isJoin == false }) else { return }
        if userModel.trackId == 0 {
            userModel.trackId = agoraKit.createCustomVideoTrack()
            userModel.isEncode = false
        }
        createVideoTrack(userModel: userModel)
    }
    
    private func createVideoTrack(userModel: UserModel) {
        let customCamera = AgoraYUVImageSourcePush(size: CGSize(width: 320, height: 180),
                                                   fileName: "sample",
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
    
    @IBAction func onDestoryVideoTrack(_ sender: Any) {
        guard let channelName = configs["channelName"] as? String else {return}
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
    
    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            // leave channel when exiting the view
            if isJoined, let channelName = configs["channelName"] as? String {
                remoteVideos.forEach({
                    let connection = AgoraRtcConnection()
                    connection.localUid = $0.uid
                    connection.channelId = channelName
                    agoraKit.leaveChannelEx(connection) { state in
                        LogUtils.log(message: "leaveChannelEx: \(state.description)", level: .info)
                    }
                    $0.reset()
                })
                let connection = AgoraRtcConnection()
                connection.localUid = localUid
                connection.channelId = channelName
                agoraKit.leaveChannelEx(connection) { state in
                    LogUtils.log(message: "leaveChannelEx: \(state.description)", level: .info)
                }
            }
            AgoraRtcEngineKit.destroy()
        }
    }
    
    private func cleanCanvas(uid: UInt) {
        guard let channelName = configs["channelName"] as? String,
              let userModel = remoteVideos.first(where: { $0.uid == uid }) else { return }
        LogUtils.log(message: "cleanCanvas: \(uid)", level: .info)
        
        // to unlink your view from sdk, so that your view reference will be released
        // note the video will stay at its last frame, to completely remove it
        // you will need to remove the EAGL sublayer from your binded view
        userModel.reset()
        
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        let connect = AgoraRtcConnection()
        connect.localUid = localUid
        connect.channelId = channelName
        agoraKit.setupRemoteVideoEx(videoCanvas, connection: connect)
    }
}

/// agora rtc engine delegate events
extension CustomVideoSourcePushMultiMain: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://api-ref.agora.io/en/voice-sdk/ios/3.x/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "didOccurWarning: \(warningCode.description)", level: .warning)
    }
    
    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
    /// cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "didOccurError: \(errorCode)", level: .error)
        self.showAlert(title: "Error", message: "didOccurError \(errorCode.description) occur")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        isJoined = true
        LogUtils.log(message: "local user join: \(uid) \(elapsed)ms", level: .info)
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        guard let channelName = configs["channelName"] as? String else {return}
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
        connect.channelId = channelName
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
extension CustomVideoSourcePushMultiMain: AgoraYUVImageSourcePushDelegate {
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
        // once we have the video frame, we can push to agora sdk
        agoraKit?.pushExternalVideoFrame(videoFrame, videoTrackId: trackId)
    }
}
