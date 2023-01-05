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
    var isJoin: Bool = false
    var customSource: AgoraYUVImageSourcePush?
}

class CustomVideoSourcePushMultiEntry : UIViewController
{
    @IBOutlet weak var joinButton: AGButton!
    @IBOutlet weak var channelTextField: AGTextField!
    let identifier = "CustomVideoSourcePushMulti"
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    @IBAction func doJoinPressed(sender: AGButton) {
        guard let channelName = channelTextField.text else {return}
        //resign channel text field
        channelTextField.resignFirstResponder()
        
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        // create new view controller every time to ensure we get a clean vc
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else {return}
        newViewController.title = channelName
        newViewController.configs = ["channelName":channelName]
        self.navigationController?.pushViewController(newViewController, animated: true)
    }
}

class CustomVideoSourcePushMultiMain: BaseViewController {
    var localVideo = Bundle.loadView(fromNib: "VideoViewSampleBufferDisplayView",
                                     withType: SampleBufferDisplayView.self)
    lazy var remoteVideos: [UserModel] = (0..<3).map({ _ in
        let model = UserModel()
        model.uid = UInt(Int.random(in: 10000...99999))
        model.canvasView = Bundle.loadView(fromNib: "VideoViewSampleBufferDisplayView",
                                           withType: SampleBufferDisplayView.self)
        model.trackId = agoraKit.createCustomVideoTrack()
        return model
    })
    var customCamera:AgoraYUVImageSourcePush?
    
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
        
        container.layoutStream2x2(views: [localVideo] + remoteVideos.compactMap({ $0.canvasView }))
        // make myself a broadcaster
        //agoraKit.setClientRole(.broadcaster)
        
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        agoraKit.enableAudio()
        
        // setup my own camera as custom video source
        // note setupLocalVideo is not working when using pushExternalVideoFrame
        // so you will have to prepare the preview yourself
        customCamera = AgoraYUVImageSourcePush(size: CGSize(width: 320, height: 180),
                                               fileName: "sample" ,
                                               frameRate: 15)
        customCamera?.trackId = agoraKit.createCustomVideoTrack()
        customCamera?.delegate = self
        customCamera?.startSource()
        agoraKit.setExternalVideoSource(true, useTexture: true, sourceType: .videoFrame)
        
        let resolution = (GlobalSettings.shared.getSetting(key: "resolution")?.selectedOption().value as? CGSize) ?? .zero
        let fps = (GlobalSettings.shared.getSetting(key: "fps")?.selectedOption().value as? AgoraVideoFrameRate) ?? .fps15
        let orientation = (GlobalSettings.shared.getSetting(key: "orientation")?.selectedOption().value as? AgoraVideoOutputOrientationMode) ?? .fixedPortrait
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
        joinChannel(uid: 999, trackId: customCamera?.trackId ?? 0)
    }
    private func joinChannel(uid: UInt, trackId: UInt32) {
        guard let channelName = configs["channelName"] as? String else {return}
        let option = AgoraRtcChannelMediaOptions()
        option.publishCustomVideoTrack = true
        option.publishMicrophoneTrack = false
        option.autoSubscribeAudio = true
        option.autoSubscribeVideo = true
        option.customVideoTrackId = Int(trackId)
        option.clientRoleType = GlobalSettings.shared.getUserRole()
        let connection = AgoraRtcConnection()
        connection.localUid = uid
        connection.channelId = channelName
        NetworkManager.shared.generateToken(channelName: channelName, uid: uid) { token in
            let result = self.agoraKit.joinChannelEx(byToken: token,
                                                connection: connection,
                                                delegate: self,
                                                mediaOptions: option,
                                                joinSuccess: nil)
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
            }
        }
    }
    
    @IBAction func onCreateVideoTrack(_ sender: Any) {
        guard let userModel = remoteVideos.first(where: { $0.isJoin == false }) else { return }
        let customCamera = AgoraYUVImageSourcePush(size: CGSize(width: 320, height: 180),
                                               fileName: "sample" ,
                                               frameRate: 15)
        customCamera.trackId = userModel.trackId
        customCamera.delegate = self
        userModel.isJoin = true
        userModel.customSource = customCamera
        customCamera.startSource()
        joinChannel(uid: userModel.uid, trackId: userModel.trackId)
    }
    @IBAction func onDestoryVideoTrack(_ sender: Any) {
        guard let channelName = configs["channelName"] as? String else {return}
        let userModel = remoteVideos.filter({ $0.isJoin == true }).last
        userModel?.isJoin = false
        userModel?.customSource?.stopSource()
        userModel?.canvasView?.videoView.reset()
        userModel?.customSource = nil
        let connection = AgoraRtcConnection()
        connection.localUid = userModel?.uid ?? 0
        connection.channelId = channelName
        agoraKit.leaveChannelEx(connection) { state in
            LogUtils.log(message: "warning: \(state.description)", level: .info)
        }
    }
    
    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            // stop capture
            customCamera?.stopSource()
            // leave channel when exiting the view
            if isJoined, let channelName = configs["channelName"] as? String {
                remoteVideos.forEach({
                    let connection = AgoraRtcConnection()
                    connection.localUid = $0.uid
                    connection.channelId = channelName
                    $0.customSource?.stopSource()
                    agoraKit.leaveChannelEx(connection) { state in
                        LogUtils.log(message: "warning: \(state.description)", level: .info)
                    }
                })
                let connection = AgoraRtcConnection()
                connection.localUid = 0
                connection.channelId = channelName
                agoraKit.leaveChannelEx(connection) { state in
                    LogUtils.log(message: "warning: \(state.description)", level: .info)
                }
                AgoraRtcEngineKit.destroy()
            }
        }
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
        LogUtils.log(message: "warning: \(warningCode.description)", level: .warning)
    }
    
    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
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
        if uid == 999 { return }
        for model in remoteVideos {
            if model.uid == uid {
                return
            }
        }
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        let userModel = remoteVideos.first(where: { $0.isJoin == false })
        videoCanvas.view = userModel?.canvasView?.videoView
        videoCanvas.renderMode = .hidden
        userModel?.uid = uid
        userModel?.isJoin = true
        agoraKit.setupRemoteVideo(videoCanvas)
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
        let userModel = remoteVideos.first(where: { $0.uid == uid })
        userModel?.isJoin = false
        userModel?.uid = UInt(Int.random(in: 10000...99999))
        userModel?.canvasView?.videoView.reset()
    }
}

/// agora camera video source, the delegate will get frame data from camera
extension CustomVideoSourcePushMultiMain: AgoraYUVImageSourcePushDelegate {
    func onVideoFrame(_ buffer: CVPixelBuffer, size: CGSize, trackId: UInt, rotation: Int32) {
        let videoFrame = AgoraVideoFrame()
        /** Video format:
         * - 1: I420
         * - 2: BGRA
         * - 3: NV21
         * - 4: RGBA
         * - 5: IMC2
         * - 7: ARGB
         * - 8: NV12
         * - 12: iOS texture (CVPixelBufferRef)
         */
        videoFrame.format = 12
        videoFrame.textureBuf = buffer
        videoFrame.rotation = Int32(rotation)
        //once we have the video frame, we can push to agora sdk
        agoraKit?.pushExternalVideoFrame(videoFrame, videoTrackId: trackId)
        
        let outputVideoFrame = AgoraOutputVideoFrame()
        outputVideoFrame.width = Int32(size.width)
        outputVideoFrame.height = Int32(size.height)
        outputVideoFrame.pixelBuffer = buffer
        outputVideoFrame.rotation = rotation
        if customCamera?.trackId ?? 0 == trackId {
            localVideo.videoView.renderVideoPixelBuffer(outputVideoFrame)
        } else {
            let userModel = remoteVideos.first(where: { $0.trackId == trackId })
            userModel?.canvasView?.videoView.renderVideoPixelBuffer(outputVideoFrame)
        }
    }
}
