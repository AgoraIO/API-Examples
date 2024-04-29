//
//  PictureInPicture.swift
//  APIExample
//
//  Created by 胡润辰 on 2022/4/6.
//  Copyright © 2022 Agora Corp. All rights reserved.
//

import UIKit
import AGEVideoLayout
import AgoraRtcKit
import MediaPlayer

class PictureInPictureEntry: UIViewController {
    @IBOutlet weak var joinButton: AGButton!
    @IBOutlet weak var channelTextField: AGTextField!
    let identifier = "PictureInPicture"
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    @IBAction func doJoinPressed(sender: AGButton) {
        guard let channelName = channelTextField.text else {return}
        // resign channel text field
        channelTextField.resignFirstResponder()
        
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        // create new view controller every time to ensure we get a clean vc
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else {
            return
        }
        newViewController.title = channelName
        newViewController.configs = ["channelName": channelName]
        navigationController?.pushViewController(newViewController, animated: true)
    }
}

@available(iOS 15.0, *)
class PictureInPictureMain: BaseViewController {
    var localVideo = Bundle.loadVideoView(type: .local, audioOnly: false)
    var remoteVideo = Bundle.loadView(fromNib: "VideoViewSampleBufferDisplayView", withType: SampleBufferDisplayView.self)
    var agoraKit: AgoraRtcEngineKit!
    private lazy var callViewController: AVPictureInPictureVideoCallViewController = {
        let callViewController = AVPictureInPictureVideoCallViewController()
        callViewController.preferredContentSize = view.bounds.size
        callViewController.view.backgroundColor = .clear
        callViewController.modalPresentationStyle = .overFullScreen
        return callViewController
    }()
    var pipController: AVPictureInPictureController?
    var remoteUid: UInt?
    // indicate if current instance has joined channel
    var isJoined: Bool = false
    private lazy var containerView: UIView = {
        let view = UIView()
        view.backgroundColor = .red
        return view
    }()
    
    // swiftlint: disable function_body_length
    override func viewDidLoad() {
        super.viewDidLoad()
        // layout render view
        localVideo.setPlaceholder(text: "Local Host".localized)
        remoteVideo.setPlaceholder(text: "Remote Host".localized)
        view.addSubview(containerView)
        containerView.frame = CGRect(x: 0, y: 0, width: SCREENSIZE.width, height: 280)
        containerView.addSubview(localVideo)
        containerView.addSubview(remoteVideo)
        localVideo.translatesAutoresizingMaskIntoConstraints = false
        remoteVideo.translatesAutoresizingMaskIntoConstraints = false
        localVideo.leadingAnchor.constraint(equalTo: containerView.leadingAnchor).isActive = true
        localVideo.topAnchor.constraint(equalTo: containerView.topAnchor).isActive = true
        localVideo.bottomAnchor.constraint(equalTo: containerView.bottomAnchor).isActive = true
        localVideo.widthAnchor.constraint(equalTo: containerView.widthAnchor, multiplier: 0.5).isActive = true
        remoteVideo.trailingAnchor.constraint(equalTo: containerView.trailingAnchor).isActive = true
        remoteVideo.topAnchor.constraint(equalTo: containerView.topAnchor).isActive = true
        remoteVideo.bottomAnchor.constraint(equalTo: containerView.bottomAnchor).isActive = true
        remoteVideo.widthAnchor.constraint(equalTo: containerView.widthAnchor, multiplier: 0.5).isActive = true

        pipController = AVPictureInPictureController(contentSource: .init(activeVideoCallSourceView: containerView,
                                                                          contentViewController: callViewController))
        pipController?.canStartPictureInPictureAutomaticallyFromInline = true
        pipController?.delegate = self
        
        // set up agora instance when view loadedlet config = AgoraRtcEngineConfig()
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.channelProfile = .liveBroadcasting
        config.areaCode = GlobalSettings.shared.area
        // setup log file path
        let logConfig = AgoraLogConfig()
        logConfig.level = .info
        config.logConfig = logConfig
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        // get channel name from configs
        guard let channelName = configs["channelName"] as? String,
              let resolution = GlobalSettings.shared.getSetting(key: "resolution")?.selectedOption().value as? CGSize,
              let fps = GlobalSettings.shared.getSetting(key: "fps")?.selectedOption().value as? AgoraVideoFrameRate,
              let orientation = GlobalSettings.shared.getSetting(key: "orientation")?
            .selectedOption().value as? AgoraVideoOutputOrientationMode else {
            return
        }
        // To enable MPNowPlayingInfoCenter, you need to add the following two private parameters
        agoraKit.setAudioOptionParams("{\"adm_mix_with_others\":false}")
        agoraKit.setParameters("{\"che.audio.nonmixable.option\":true}")
        
        // make myself a broadcaster
        agoraKit.setChannelProfile(.liveBroadcasting)
        agoraKit.setClientRole(GlobalSettings.shared.getUserRole())
        
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        agoraKit.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: resolution,
                                                                             frameRate: fps,
                                                                             bitrate: AgoraVideoBitrateStandard,
                                                                             orientationMode: orientation,
                                                                             mirrorMode: AgoraVideoMirrorMode.auto))
        // set up local video to render your local camera preview
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = 0
        // the view to be binded
        videoCanvas.view = localVideo.videoView
        videoCanvas.renderMode = .hidden
        agoraKit.setupLocalVideo(videoCanvas)
        
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        // Setup raw video data frame observer
        agoraKit.setVideoFrameDelegate(self)
        
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        let option = AgoraRtcChannelMediaOptions()
        NetworkManager.shared.generateToken(channelName: channelName, success: { token in
            let result = self.agoraKit.joinChannel(byToken: token, channelId: channelName, uid: 0, mediaOptions: option, joinSuccess: nil)
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
                self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
            }
        })
                
        NotificationCenter.default.addObserver(self,
                                               selector: #selector(didEnterBackgroundNotification),
                                               name: UIApplication.willResignActiveNotification,
                                               object: nil)
    }
    // swiftlint: enable function_body_length
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        setupPlayintInfoCenter()
    }
    
    private func setupPlayintInfoCenter() {
        UIApplication.shared.beginReceivingRemoteControlEvents()
        var nowPlayingInfo = [String: Any]()
        let path = Bundle.main.path(forResource: "agora-logo", ofType: "png") ?? ""
        guard let image = UIImage(contentsOfFile: path) else { return }
        let artWork = MPMediaItemArtwork(boundsSize: image.size) { _ in
            return image
        }
        nowPlayingInfo[MPMediaItemPropertyArtwork] = artWork
        nowPlayingInfo[MPMediaItemPropertyTitle] = "Song Title"
        nowPlayingInfo[MPMediaItemPropertyArtist] = "Artist Name"
        nowPlayingInfo[MPMediaItemPropertyAlbumTitle] = "Album Name"
        nowPlayingInfo[MPNowPlayingInfoPropertyIsLiveStream] = true
        MPNowPlayingInfoCenter.default().nowPlayingInfo = nowPlayingInfo
        MPNowPlayingInfoCenter.default().playbackState = .playing
    }
    
    override var canBecomeFirstResponder: Bool {
        true
    }
    
    deinit {
        NotificationCenter.default.removeObserver(self)
    }
    
    @objc
    private func didEnterBackgroundNotification() {
        onPIP(_btn: UIButton())
    }
    
    @IBAction func onPIP(_btn: UIButton) {
        if let currentPipController = pipController {
            currentPipController.startPictureInPicture()
        } else {
            showAlert(message: "PIP Support iOS 15+".localized)
        }
    }
    
    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            // leave channel when exiting the view
            if isJoined {
                if let pipController = pipController, pipController.isPictureInPictureActive {
                    pipController.stopPictureInPicture()
                }
                agoraKit.leaveChannel { (stats) -> Void in
                    LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
                }
            }
        }
    }
}

/// agora rtc engine delegate events
@available(iOS 15.0, *)
extension PictureInPictureMain: AgoraRtcEngineDelegate {
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
    
    /// callback when the local user joins a specified channel.
    /// @param channel
    /// @param uid uid of local user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        isJoined = true
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        
        remoteVideo.videoView.reset()
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
//        remoteVideo.videoView.reset()
    }
    func rtcEngine(_ engine: AgoraRtcEngineKit, 
                   remoteVideoStateChangedOfUid uid: UInt,
                   state: AgoraVideoRemoteState, reason: AgoraVideoRemoteReason,
                   elapsed: Int) {
        if reason == .remoteMuted {
            let pixelBuffer = MediaUtils.cvPixelBufferRef(from: UIImage(named: "agora-logo") ?? UIImage()).takeRetainedValue()
            let videoFrame = AgoraOutputVideoFrame()
            videoFrame.pixelBuffer = pixelBuffer
            videoFrame.width = Int32(remoteVideo.videoView.frame.width)
            videoFrame.height = Int32(remoteVideo.videoView.frame.height)
            remoteVideo.videoView.renderVideoPixelBuffer(videoFrame)
        }
    }
}

// MARK: - AgoraVideoDataFrameProtocol
@available(iOS 15.0, *)
extension PictureInPictureMain: AgoraVideoFrameDelegate {
    func onCapture(_ videoFrame: AgoraOutputVideoFrame, sourceType: AgoraVideoSourceType) -> Bool {
        true
    }
    
    func onRenderVideoFrame(_ videoFrame: AgoraOutputVideoFrame, uid: UInt, channelId: String) -> Bool {
        remoteVideo.videoView.renderVideoPixelBuffer(videoFrame)
        return true
    }
    
    func getVideoFormatPreference() -> AgoraVideoFormat {
        .cvPixelBGRA
    }
    func getRotationApplied() -> Bool {
        true
    }
}

@available(iOS 15.0, *)
extension PictureInPictureMain: AVPictureInPictureControllerDelegate {
    func pictureInPictureControllerWillStartPictureInPicture(_ pictureInPictureController: AVPictureInPictureController) {
    }

    func pictureInPictureControllerDidStartPictureInPicture(_ pictureInPictureController: AVPictureInPictureController) {
        containerView.removeFromSuperview()
        let vc = pictureInPictureController.contentSource?.activeVideoCallContentViewController
        containerView.frame.size = vc?.view.bounds.size ?? .zero
        vc?.view.addSubview(containerView)
    }

    func pictureInPictureController(_ pictureInPictureController: AVPictureInPictureController,
                                    failedToStartPictureInPictureWithError error: Error) {
    }
    
    func pictureInPictureControllerWillStopPictureInPicture(_ pictureInPictureController: AVPictureInPictureController) {
        containerView.removeFromSuperview()
        containerView.frame.size = CGSize(width: SCREENSIZE.width, height: 280)
        view.addSubview(containerView)
    }

    func pictureInPictureControllerDidStopPictureInPicture(_ pictureInPictureController: AVPictureInPictureController) {
    }
}
