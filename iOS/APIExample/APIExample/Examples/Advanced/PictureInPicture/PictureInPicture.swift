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

class PictureInPictureEntry : UIViewController
{
    @IBOutlet weak var joinButton: AGButton!
    @IBOutlet weak var channelTextField: AGTextField!
    let identifier = "PictureInPicture"
    
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
        navigationController?.pushViewController(newViewController, animated: true)
    }
}

class PictureInPictureMain: BaseViewController {
    var localVideo = Bundle.loadVideoView(type: .local, audioOnly: false)
    var remoteVideo = Bundle.loadView(fromNib: "VideoViewSampleBufferDisplayView", withType: SampleBufferDisplayView.self)
    
    @IBOutlet weak var container: AGEVideoContainer!
    var agoraKit: AgoraRtcEngineKit!
    var pipController: AgoraPictureInPictureController?
    var remoteUid: UInt?
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // layout render view
        localVideo.setPlaceholder(text: "Local Host".localized)
        remoteVideo.setPlaceholder(text: "Remote Host".localized)
        container.layoutStream(views: [localVideo, remoteVideo])
        
        pipController = AgoraPictureInPictureController(displayView: remoteVideo.videoView)
        pipController?.pipController.delegate = self
        
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
              let orientation = GlobalSettings.shared.getSetting(key: "orientation")?.selectedOption().value as? AgoraVideoOutputOrientationMode else {return}
        
        // make myself a broadcaster
        agoraKit.setChannelProfile(.liveBroadcasting)
        agoraKit.setClientRole(.broadcaster)
        
        
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
                // en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
            }
        })
    }
        
    @IBAction func onPIP(_btn: UIButton) {
        guard let currentPipController = pipController else {return}
        
        if currentPipController.pipController.isPictureInPicturePossible {
            currentPipController.pipController.startPictureInPicture()
        }

    }
    
    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            // leave channel when exiting the view
            if isJoined {
                if let pipController = pipController?.pipController, pipController.isPictureInPictureActive {
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
    /// en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
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
        remoteVideo.videoView.reset()
    }
}

// MARK: - AgoraVideoDataFrameProtocol
extension PictureInPictureMain: AgoraVideoFrameDelegate {
    func onCapture(_ videoFrame: AgoraOutputVideoFrame) -> Bool {
        true
    }
    
    func onRenderVideoFrame(_ videoFrame: AgoraOutputVideoFrame, uid: UInt, channelId: String) -> Bool {
        remoteVideo.videoView.renderVideoData(videoFrame)
        return true
    }
    
    func getVideoFormatPreference() -> AgoraVideoFormat {
        .I420
    }
}

extension PictureInPictureMain: AVPictureInPictureControllerDelegate {
    
    func pictureInPictureControllerWillStartPictureInPicture(_ pictureInPictureController: AVPictureInPictureController) {
        
    }

    func pictureInPictureControllerDidStartPictureInPicture(_ pictureInPictureController: AVPictureInPictureController) {
        
    }

    func pictureInPictureController(_ pictureInPictureController: AVPictureInPictureController, failedToStartPictureInPictureWithError error: Error) {
        
    }
    
    func pictureInPictureControllerWillStopPictureInPicture(_ pictureInPictureController: AVPictureInPictureController) {
        
    }

    func pictureInPictureControllerDidStopPictureInPicture(_ pictureInPictureController: AVPictureInPictureController) {
        
    }

//    func pictureInPictureController(_ pictureInPictureController: AVPictureInPictureController, restoreUserInterfaceForPictureInPictureStopWithCompletionHandler completionHandler: @escaping (Bool) -> Void) {
//
//    }

//    func pictureInPictureController(_ pictureInPictureController: AVPictureInPictureController) async -> Bool {
//
//    }

}
