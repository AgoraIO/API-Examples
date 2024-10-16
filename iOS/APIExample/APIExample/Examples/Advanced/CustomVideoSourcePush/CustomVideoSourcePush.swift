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

class CustomVideoSourcePreview: UIView {
    private var previewLayer: AVCaptureVideoPreviewLayer?
    
    func insertCaptureVideoPreviewLayer(previewLayer: AVCaptureVideoPreviewLayer) {
        self.previewLayer?.removeFromSuperlayer()
        
        previewLayer.frame = bounds
        layer.insertSublayer(previewLayer, below: layer.sublayers?.first)
        self.previewLayer = previewLayer
    }
    
    override func layoutSublayers(of layer: CALayer) {
        super.layoutSublayers(of: layer)
        previewLayer?.frame = bounds
    }
}

class CustomVideoSourcePushEntry: UIViewController {
    @IBOutlet weak var joinButton: AGButton!
    @IBOutlet weak var channelTextField: AGTextField!
    let identifier = "CustomVideoSourcePush"
    private var isHDR: Bool = false
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    @IBAction func onHDRAction(sender: UISwitch) {
        isHDR = sender.isOn
    }
    
    @IBAction func doJoinPressed(sender: AGButton) {
        guard let channelName = channelTextField.text else { return }
        // resign channel text field
        channelTextField.resignFirstResponder()
        
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        // create new view controller every time to ensure we get a clean vc
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else { return
        }
        newViewController.title = channelName
        newViewController.configs = ["channelName": channelName, "isHDR": isHDR]
        navigationController?.pushViewController(newViewController, animated: true)
    }
}

class CustomVideoSourcePushMain: BaseViewController {
    var localVideo = Bundle.loadView(fromNib: "VideoViewSampleBufferDisplayView", withType: SampleBufferDisplayView.self)
    var remoteVideo = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
    var customCamera: AgoraYUVImageSourcePush?
    private var isHDR: Bool = false
    
    @IBOutlet weak var container: AGEVideoContainer!
    var agoraKit: AgoraRtcEngineKit?
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // layout render view
        remoteVideo.setPlaceholder(text: "Remote Host".localized)
        container.layoutStream(views: [localVideo, remoteVideo])
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        
        guard let agoraKit = agoraKit else {
            return
        }
        
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        
        // get channel name from configs
        guard let channelName = configs["channelName"] as? String else {return}
        isHDR = configs["isHDR"] as? Bool ?? false
        
        // make myself a broadcaster
        // agoraKit.setClientRole(.broadcaster)
        
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        agoraKit.enableAudio()
        
        // setup my own camera as custom video source
        // note setupLocalVideo is not working when using pushExternalVideoFrame
        // so you will have to prepare the preview yourself
        customCamera = AgoraYUVImageSourcePush(size: isHDR ? CGSize(width: 1280, height: 720) : CGSize(width: 320, height: 180),
                                               fileName: isHDR ? "hlg-hdr" : "sample",
                                               frameRate: 15,
                                               isHDR: isHDR)
        customCamera?.delegate = self
        customCamera?.startSource()
        customCamera?.trackId = 0
        agoraKit.setExternalVideoSource(true, useTexture: true, sourceType: .videoFrame)
        
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
        let option = AgoraRtcChannelMediaOptions()
        option.publishCustomAudioTrack = GlobalSettings.shared.getUserRole() == .broadcaster
        option.publishCustomVideoTrack = GlobalSettings.shared.getUserRole() == .broadcaster
        option.clientRoleType = GlobalSettings.shared.getUserRole()
        NetworkManager.shared.generateToken(channelName: channelName, success: {[weak self] token in
            guard let self = self else {return}
            let result = self.agoraKit?.joinChannel(byToken: token, channelId: channelName, uid: 0, mediaOptions: option)
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
                self.showAlert(title: "Error", message: "joinChannel call failed: \(result ?? 0), please check your params")
            }
        })
    }
    
    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            // stop capture
            customCamera?.stopSource()
            // leave channel when exiting the view
            if isJoined {
                agoraKit?.disableAudio()
                agoraKit?.disableVideo()
                let option = AgoraRtcChannelMediaOptions()
                option.publishCustomAudioTrack = false
                option.publishCustomVideoTrack = false
                agoraKit?.updateChannel(with: option)
                agoraKit?.leaveChannel { (stats) -> Void in
                    LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
                }
                agoraKit = nil
            }
            AgoraRtcEngineKit.destroy()
        }
    }
}

/// agora rtc engine delegate events
extension CustomVideoSourcePushMain: AgoraRtcEngineDelegate {
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
        isJoined = true
        LogUtils.log(message: "local user join: \(uid) \(elapsed)ms", level: .info)
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        
        // Only one remote video view is available for this
        // tutorial. Here we check if there exists a surface
        // view tagged as this uid.
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = remoteVideo.videoView
        videoCanvas.renderMode = .hidden
        agoraKit?.setupRemoteVideo(videoCanvas)
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
        agoraKit?.setupRemoteVideo(videoCanvas)
    }
}

/// agora camera video source, the delegate will get frame data from camera
extension CustomVideoSourcePushMain: AgoraYUVImageSourcePushDelegate {
    func onVideoFrame(_ buffer: CVPixelBuffer, size: CGSize, trackId: UInt, rotation: Int32) {
        let videoFrame = AgoraVideoFrame()
        
        if isHDR {
            videoFrame.format = AgoraVideoFormat.cvPixelP010.rawValue
        } else {
            videoFrame.format = AgoraVideoFormat.cvPixelNV12.rawValue
        }
        videoFrame.textureBuf = buffer
        videoFrame.rotation = Int32(rotation)
        // once we have the video frame, we can push to agora sdk
        agoraKit?.pushExternalVideoFrame(videoFrame, videoTrackId: trackId)
        
        let outputVideoFrame = AgoraOutputVideoFrame()
        outputVideoFrame.width = Int32(size.width)
        outputVideoFrame.height = Int32(size.height)
        outputVideoFrame.pixelBuffer = buffer
        outputVideoFrame.rotation = rotation
        localVideo.videoView.renderVideoPixelBuffer(outputVideoFrame)
    }
}
