//
//  ScreenShare+Socket.swift
//  APIExample
//
//  Created by XC on 2021/7/28.
//  Copyright © 2021 Agora Corp. All rights reserved.
//

import UIKit
import AGEVideoLayout
import AgoraRtcKit
import ReplayKit

#if false
class ScreenShareSocketEntry: UIViewController
{
    @IBOutlet weak var joinButton: UIButton!
    @IBOutlet weak var channelTextField: UITextField!
    let identifier = "ScreenShare+Socket"
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    @IBAction func doJoinPressed(sender: UIButton) {
        guard let channelName = channelTextField.text else {return}
        if channelName.isEmpty {
            showAlert(title: "Error", message: "please input channel name!".localized)
            return
        }
        //resign channel text field
        channelTextField.resignFirstResponder()
        
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        // create new view controller every time to ensure we get a clean vc
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else {return}
        newViewController.title = channelName
        newViewController.configs = ["channelName":channelName]
        self.navigationController?.pushViewController(newViewController, animated: true)
    }
    
    func showAlert(title: String? = nil, message: String) {
        let alertController = UIAlertController(title: title, message: message, preferredStyle: .alert)
        let action = UIAlertAction(title: "OK", style: .cancel, handler: nil)
        alertController.addAction(action)
        self.present(alertController, animated: true, completion: nil)
    }
}

class ScreenShareSocketMain: BaseViewController {
    var localVideo = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
    var remoteVideo = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
    
    @IBOutlet weak var container: AGEVideoContainer!
    @IBOutlet weak var broadcasterPickerContainer: UIView!
    var agoraKit: AgoraRtcEngineKit!
    var screenSourcePush: AgoraScreenShareSourcePush!
    // indicate if current instance has joined channel
    var isJoined: Bool = false
    var channelName: String?
    
    private let socketPort: UInt16 = 1024
    private var processing = false

    override func viewDidLoad() {
        super.viewDidLoad()
        screenSourcePush = AgoraScreenShareSourcePush(delegate: self, port: socketPort)
        screenSourcePush.startCapture()
        
        // layout render view
        localVideo.setPlaceholder(text: "Local Host".localized)
        remoteVideo.setPlaceholder(text: "Remote Host".localized)
        container.layoutStream(views: [localVideo, remoteVideo])
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        agoraKit.setLogFile(LogUtils.sdkLogPath())

        // get channel name from configs
        guard let channelName = configs["channelName"] as? String else {return}
        self.channelName = channelName
        
        // make myself a broadcaster
        agoraKit.setClientRole(.broadcaster)
        
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        agoraKit.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: AgoraVideoDimension640x360,
                                                                             frameRate: .fps30,
                                                                             bitrate: AgoraVideoBitrateStandard,
                                                                             orientationMode: .adaptative, mirrorMode: .auto))
        
        // set up local video to render your local camera preview
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = 0
        // the view to be binded
        videoCanvas.view = localVideo.videoView
        videoCanvas.renderMode = .hidden
        agoraKit.setupLocalVideo(videoCanvas)
        // you have to call startPreview to see local video
        agoraKit.startPreview()

        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        let option = AgoraRtcChannelMediaOptions()
        option.publishCameraTrack = .of(true)
        option.publishAudioTrack = .of(true)
        option.clientRoleType = .of((Int32)(AgoraClientRole.broadcaster.rawValue))

        let result = agoraKit.joinChannel(byToken: KeyCenter.Token, channelId: channelName, uid: SCREEN_SHARE_UID, mediaOptions: option)
        agoraKit.muteRemoteAudioStream(UInt(SCREEN_SHARE_BROADCASTER_UID), mute: true)
        agoraKit.muteRemoteVideoStream(UInt(SCREEN_SHARE_BROADCASTER_UID), mute: true)
        if result != 0 {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
        } else {
            prepareScreenShare(channelName: channelName)
        }
    }
    
    override func viewDidDisappear(_ animated: Bool) {
        super.viewDidDisappear(animated)
        endScreenShare()
        screenSourcePush.stopCapture()
    }
    
    func prepareScreenShare(channelName: String) {
        let option = AgoraRtcChannelMediaOptions()
        option.clientRoleType = AgoraRtcIntOptional.of(Int32(AgoraClientRole.broadcaster.rawValue))
        option.autoSubscribeAudio = AgoraRtcBoolOptional.of(false)
        option.autoSubscribeVideo = AgoraRtcBoolOptional.of(false)
        option.publishAudioTrack = AgoraRtcBoolOptional.of(false)
        option.publishCustomVideoTrack = AgoraRtcBoolOptional.of(true)

        let connection = AgoraRtcConnection()
        connection.channelId = channelName
        connection.localUid = 0
        let code = agoraKit.joinChannelEx(byToken: KeyCenter.Token, connection: connection, delegate: nil, mediaOptions: option) { [weak self] channelName, uid, elapsed in
            guard let self = self else { return }
            LogUtils.log(message: "joinChannelEx uid:\(uid) channel:\(channelName)", level: .info)

            let videoConfig = AgoraVideoEncoderConfiguration()
            videoConfig.dimensions = self.screenShareVideoDimension()
            videoConfig.bitrate = AgoraVideoBitrateStandard
            videoConfig.frameRate = .fps30
            videoConfig.orientationMode = .adaptative
            videoConfig.mirrorMode = .auto
            
            self.agoraKit.setVideoEncoderConfigurationEx(videoConfig, connection: connection)
            self.agoraKit.muteRemoteAudioStream(uid, mute: true)
            self.agoraKit.muteRemoteVideoStream(uid, mute: true)
        }
        if code != 0 {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            self.showAlert(title: "Error", message: "joinChannelEx call failed: \(code), please check your params")
        }
    }
    
    private func endScreenShare() {
        guard let channel = self.channelName else {
            return
        }
        let connection = AgoraRtcConnection()
        connection.channelId = channel
        agoraKit.leaveChannelEx(connection, leaveChannelBlock: nil)
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
    
    @IBAction func onClickScreenShareButton(_ sender: Any) {
        guard let channelName = channelName else { return }
        prepareScreenShare(channelName: channelName)
        if #available(iOS 12.0, *) {
            // config screenshare extension socket port
            let userDefault = UserDefaults(suiteName: "group.sharescreen.io")
            userDefault?.setValue(socketPort, forKey: "screen_share_port")
            
            let frame = CGRect(x: 0, y:0, width: 60, height: 60)
            let systemBroadcastPicker = RPSystemBroadcastPickerView(frame: frame)
            systemBroadcastPicker.showsMicrophoneButton = false
            systemBroadcastPicker.autoresizingMask = [.flexibleTopMargin, .flexibleRightMargin]
            if let url = Bundle.main.url(forResource: "Agora-ScreenShare-Extension(Socket)", withExtension: "appex", subdirectory: "PlugIns") {
                if let bundle = Bundle(url: url) {
                    systemBroadcastPicker.preferredExtension = bundle.bundleIdentifier
                }
            }
            let button = systemBroadcastPicker.subviews.first { view in
                view.isKind(of: UIButton.self)
            }
            if let button = button {
                (button as! UIButton).sendActions(for: .touchUpInside)
            }
        } else {
            self.showAlert(message: "Minimum support iOS version is 12.0")
        }
    }
    
    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            // leave channel when exiting the view
            // deregister packet processing
            AgoraCustomEncryption.deregisterPacketProcessing(agoraKit)
            if isJoined {
                agoraKit.leaveChannel { (stats) -> Void in
                    LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
                }
            }
        }
    }
}

extension ScreenShareSocketMain: AgoraScreenShareSourcePushDelegate {
    
    func screenSourceCapture(_ capture: AgoraScreenShareSourcePush, outputBuffer pixelBuffer: CVPixelBuffer, rotation: Int32, time: CMTime) {
        if connectionId == 0 {
            if let channelName = channelName {
                if !processing {
                    processing = true
                    prepareScreenShare(channelName: channelName)
                }
            }
            return
        } else if processing {
            processing = false
        }
        let frame = AgoraVideoFrame()
        frame.format = 12
        frame.textureBuf = pixelBuffer
        frame.time = time
        frame.rotation = rotation
        agoraKit.pushExternalVideoFrame(frame, connectionId: connectionId)
    }
    
    func pushAudioAppFrame(_ inAudioFrame: UnsafeMutablePointer<UInt8>, frameSize: Int64) {
        if connectionId == 0 { return }
        let time = Date().timeIntervalSince1970 * 1000 * 1000
        agoraKit.pushExternalAudioFrameExRawData(inAudioFrame, samples: Int(frameSize)/2, sourceId: 1, timestamp: time, connectionId: connectionId)
    }
    
    func screenShareStop() {
        endScreenShare()
    }
}

/// agora rtc engine delegate events
extension ScreenShareSocketMain: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.description)", level: .warning)
    }
    
    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode)", level: .error)
        self.showAlert(title: "Error", message: "Error \(errorCode.description) occur")
    }
    
    /// callback when the local user joins a specified channel.
    /// @param channel
    /// @param uid uid of local user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        self.isJoined = true
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
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
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = nil
        videoCanvas.renderMode = .hidden
        agoraKit.setupRemoteVideo(videoCanvas)
    }
}

#endif
