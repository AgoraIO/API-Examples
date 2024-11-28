//
//  LocalCompositeGraph.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//
import UIKit
import AGEVideoLayout
import AgoraRtcKit

class LocalCompositeGraphEntry: UIViewController {
    @IBOutlet weak var joinButton: UIButton!
    @IBOutlet weak var channelTextField: UITextField!
    let identifier = "LocalCompositeGraph"
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    @IBAction func doJoinPressed(sender: UIButton) {
        guard let channelName = channelTextField.text else { return }
        // resign channel text field
        channelTextField.resignFirstResponder()
        
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        // create new view controller every time to ensure we get a clean vc
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else {return}
        newViewController.title = channelName
        newViewController.configs = ["channelName": channelName]
        navigationController?.pushViewController(newViewController, animated: true)
    }
}

class LocalCompositeGraphMain: BaseViewController {
    var localVideo = Bundle.loadVideoView(type: .local, audioOnly: false)
    
    @IBOutlet weak var container: AGEVideoContainer!
    var agoraKit: AgoraRtcEngineKit!
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false
    
    private lazy var screenParams: AgoraScreenCaptureParameters2 = {
        let params = AgoraScreenCaptureParameters2()
        params.captureVideo = true
        params.captureAudio = true
        let audioParams = AgoraScreenAudioParameters()
        audioParams.captureSignalVolume = 50
        params.audioParams = audioParams
        let videoParams = AgoraScreenVideoParameters()
        videoParams.dimensions = screenShareVideoDimension()
        videoParams.frameRate = .fps15
        videoParams.bitrate = AgoraVideoBitrateStandard
        params.videoParams = videoParams
        return params
    }()
    
    private lazy var option: AgoraRtcChannelMediaOptions = {
        let option = AgoraRtcChannelMediaOptions()
        option.clientRoleType = GlobalSettings.shared.getUserRole()
        option.publishCameraTrack = true
        option.publishMicrophoneTrack = true
        return option
    }()
    
    private var systemBroadcastPicker: RPSystemBroadcastPickerView?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // layout render view
        localVideo.setPlaceholder(text: "Local Host".localized)
        container.layoutStream(views: [localVideo])
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        
        // get channel name from configs
        guard let channelName = configs["channelName"] as? String else {return}
        
        // make myself a broadcaster
        agoraKit.setClientRole(GlobalSettings.shared.getUserRole())
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        agoraKit.enableAudio()

        // set up local video to render your local camera preview
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = 0
        // the view to be binded
        videoCanvas.view = localVideo.videoView
        videoCanvas.mirrorMode = .disabled
        videoCanvas.renderMode = .fit
        videoCanvas.sourceType = .transCoded
        agoraKit.setupLocalVideo(videoCanvas)
        // you have to call startPreview to see local video
        agoraKit.startPreview()
        
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        //start screen capture
        self.startScreenCapture()
        
        // start camera
        let captureConfig = AgoraCameraCapturerConfiguration()
        captureConfig.dimensions = CGSize(width: 100, height: 100)
        agoraKit.startCameraCapture(.camera, config: captureConfig)
        
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        let option = AgoraRtcChannelMediaOptions()
        option.publishCameraTrack = true
        option.publishMicrophoneTrack = true
        option.publishTranscodedVideoTrack = true
        option.clientRoleType = .broadcaster
        NetworkManager.shared.generateToken(channelName: channelName, success: { token in
            let result = self.agoraKit.joinChannel(byToken: token, channelId: channelName, uid: 0, mediaOptions: option)
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
                self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
            }
            self.startVideoTranscoder()
        })
    }

    override func viewDidDisappear(_ animated: Bool) {
        super.viewDidDisappear(animated)
        agoraKit.disableAudio()
        agoraKit.disableVideo()
        if isJoined {
            stopScreenCapture()
            agoraKit.stopPreview()
            agoraKit.leaveChannel { (stats) -> Void in
                LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
            }
        }
    }
    
    private func screenShareVideoDimension() -> CGSize {
        let screenSize = UIScreen.main.bounds
        var boundingSize = CGSize(width: 540, height: 960)
        let mW: CGFloat = boundingSize.width / screenSize.width
        let mH: CGFloat = boundingSize.height / screenSize.height
        if mH < mW {
            boundingSize.width = boundingSize.height / screenSize.height * screenSize.width
        } else if mW < mH {
            boundingSize.height = boundingSize.width / screenSize.width * screenSize.height
        }
        return boundingSize
    }
    
    private func prepareSystemBroadcaster() {
        if #available(iOS 12.0, *) {
            let frame = CGRect(x: 0, y: 0, width: 60, height: 60)
            systemBroadcastPicker = RPSystemBroadcastPickerView(frame: frame)
            systemBroadcastPicker?.showsMicrophoneButton = false
            systemBroadcastPicker?.autoresizingMask = [.flexibleTopMargin, .flexibleRightMargin]
            let bundleId = Bundle.main.bundleIdentifier ?? ""
            systemBroadcastPicker?.preferredExtension = "\(bundleId).Agora-ScreenShare-Extension"
            
        } else {
            self.showAlert(message: "Minimum support iOS version is 12.0")
        }
    }
    
    private func stopScreenCapture() {
        agoraKit.stopScreenCapture()
        option.publishScreenCaptureVideo = false
        option.publishScreenCaptureAudio = false
        agoraKit.updateChannel(with: option)
    }
    
    private func startScreenCapture() {
        guard !UIScreen.main.isCaptured else { return }
        agoraKit.startScreenCapture(screenParams)
        prepareSystemBroadcaster()
        guard let picker = systemBroadcastPicker else { return }
        for view in picker.subviews where view is UIButton {
            (view as? UIButton)?.sendActions(for: .allEvents)
            break
        }
        
        option.publishScreenCaptureVideo = true
        option.publishScreenCaptureAudio = true
        agoraKit.updateChannel(with: option)
    }
    
    private func startVideoTranscoder() {
        
        // camera capture
        let cameraStream = AgoraTranscodingVideoStream()
        cameraStream.rect = CGRect(origin: .zero, size: CGSize(width: 100, height: 100))
        cameraStream.sourceType = .camera
        
        // screen capture
        let screenStream = AgoraTranscodingVideoStream()
        screenStream.sourceType = .screen
        screenStream.rect = CGRect(origin: .zero, size: screenShareVideoDimension())
        
        let config = AgoraLocalTranscoderConfiguration()
        config.videoOutputConfiguration.dimensions = screenShareVideoDimension()
        // set transcoder config
        config.videoInputStreams = [screenStream, cameraStream]
        agoraKit.startLocalVideoTranscoder(config)
    }
}

/// agora rtc engine delegate events
extension LocalCompositeGraphMain: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://api-ref.agora.io/en/voice-sdk/ios/3.x/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.description)", level: .warning)
    }
    
    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
    /// cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode)", level: .error)
        self.showAlert(title: "Error", message: "Error \(errorCode.description) occur")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        self.isJoined = true
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
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
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = nil
        videoCanvas.renderMode = .hidden
        agoraKit.setupRemoteVideo(videoCanvas)
    }
    
    /// Reports the statistics of the current call. The SDK triggers this callback once every two seconds after the user joins the channel.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, reportRtcStats stats: AgoraChannelStats) {
        localVideo.statsInfo?.updateChannelStats(stats)
    }
    
    /// Reports the statistics of the uploading local audio streams once every two seconds.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, localAudioStats stats: AgoraRtcLocalAudioStats) {
        localVideo.statsInfo?.updateLocalAudioStats(stats)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didLocalVideoTranscoderErrorWithStream stream: AgoraTranscodingVideoStream, errorCode: AgoraVideoTranscoderError) {
        print("didLocalVideoTranscoderError: \(errorCode.rawValue), source type: \(stream.sourceType.rawValue)")
    }
}
