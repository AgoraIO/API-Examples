//
//  JoinChannelVC.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import UIKit
import ReplayKit
import AGEVideoLayout
import AgoraRtcKit

class ScreenShareMain: BaseViewController {
    @IBOutlet weak var infoContainerView: UIView!
    @IBOutlet weak var closeAppAudioButton: UIButton!
    
    var localVideo = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
    var remoteVideo = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)

    var isJoined: Bool = false
    var isScreenSharing: Bool = false
    var agoraKit: AgoraRtcEngineKit!
    var screenCaptureParams: AgoraScreenCaptureParameters2?
        
    override func viewDidLoad() {
        super.viewDidLoad()
        setupUI()
        
        // get channel name from configs
        guard let channelName = configs["channelName"] as? String,
              let resolution = GlobalSettings.shared.getSetting(key: "resolution")?.selectedOption().value as? CGSize,
              let fps = GlobalSettings.shared.getSetting(key: "fps")?.selectedOption().value as? AgoraVideoFrameRate,
              let orientation = GlobalSettings.shared.getSetting(key: "orientation")?.selectedOption().value as? AgoraVideoOutputOrientationMode else {return}

        // set up agora instance when view loadedlet config = AgoraRtcEngineConfig()
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area.rawValue
        // setup log file path
        let logConfig = AgoraLogConfig()
        logConfig.level = .info
        config.logConfig = logConfig
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        
        // make myself a broadcaster
        agoraKit.setChannelProfile(.liveBroadcasting)
        agoraKit.setClientRole(.broadcaster)
        
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        agoraKit.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: resolution,
                frameRate: fps,
                bitrate: AgoraVideoBitrateStandard,
                orientationMode: orientation))
        
        // set up local video to render your local camera preview
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = 0
        // the view to be binded
        videoCanvas.view = localVideo.videoView
        videoCanvas.renderMode = .hidden
        agoraKit.setupLocalVideo(videoCanvas)
        
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        agoraKit.setAudioProfile(.default, scenario: .gameStreaming)
        
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        let option = AgoraRtcChannelMediaOptions()
        let result = agoraKit.joinChannel(byToken: KeyCenter.Token, channelId: channelName, info: nil, uid: 0, options: option)
        if result != 0 {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
        }
    }
    
    func setupUI() {
        localVideo.setPlaceholder(text: "Local Host".localized)
        remoteVideo.setPlaceholder(text: "Remote Host".localized)
        
        let screenBounds = UIScreen.main.bounds
        let width = screenBounds.width / 4.0
        let y = (navigationController?.navigationBar.frame.height)! + UIApplication.shared.statusBarFrame.height
        remoteVideo.frame = CGRect(x: 0, y: y, width: screenBounds.width, height: screenBounds.height - y)
        localVideo.frame = CGRect(x: screenBounds.width - width, y: y, width: width, height: width * 4 / 3)
        self.view.addSubview(remoteVideo)
        self.view.addSubview(localVideo)
        self.view.bringSubviewToFront(infoContainerView)
        
        updateButtonTitle()
    }
    
    @IBAction func startShareBtnClick(_ sender: Any) {
        guard #available(iOS 12.0, *) else {
            self.showAlert(message: "Minimum support iOS version is 12.0")
            return
        }
        
        if isScreenSharing {
            agoraKit.stopScreenCapture()
        } else {
            if screenCaptureParams == nil {
                let screenParams = AgoraScreenCaptureParameters2()
                screenParams.captureAudio = true
                screenParams.captureVideo = true
                
                let videoParams = AgoraScreenVideoParameters()
                videoParams.dimensions = CGSize(width: 0, height: 0)
                videoParams.frameRate = 30
                screenParams.videoParams = videoParams;
                screenCaptureParams = screenParams
            }
            agoraKit.startScreenCapture(screenCaptureParams!)
            
            let pickerView = RPSystemBroadcastPickerView(frame: CGRect(x: 0, y:0, width: 50, height: 50))
            if let url = Bundle.main.url(forResource: "Agora-ScreenShare-Extension", withExtension: "appex", subdirectory: "PlugIns") {
                if let bundle = Bundle(url: url) {
                    pickerView.preferredExtension = bundle.bundleIdentifier
                    pickerView.showsMicrophoneButton = false
                    
                    // Auto click RPSystemBroadcastPickerView
                    for view in pickerView.subviews {
                        let startButton = view as! UIButton
                        startButton.sendActions(for: .allTouchEvents)
                    }
                }
            }
        }
    }
        
    @IBAction func closeAppAudioBtnClick(_ sender: Any) {
        guard let screenParams = screenCaptureParams else {return}
        screenParams.captureAudio = false
        screenParams.captureVideo = true
        agoraKit.updateScreenCapture(screenParams)
        
        updateButtonTitle()
    }
    
    func startScreenCapture() {
        isScreenSharing = true
        localVideo.isHidden = true
        remoteVideo.isHidden = true
        infoContainerView.isHidden = false
        
        updateButtonTitle()
    }
    
    func stopScreenCapture() {
        screenCaptureParams = nil
        isScreenSharing = false
        localVideo.isHidden = false
        remoteVideo.isHidden = false
        infoContainerView.isHidden = true
        
        agoraKit.setVideoSource(AgoraRtcDefaultCamera())
        updateButtonTitle()
    }
    
    func updateButtonTitle() {
        var title = (screenCaptureParams != nil) ? "Stop Share".localized : "Start Share".localized
        let rightBarButton = UIBarButtonItem(title: title, style: .plain, target: self, action: #selector(startShareBtnClick))
        self.navigationItem.rightBarButtonItem = rightBarButton
        
        if let screenParams = screenCaptureParams, screenParams.captureAudio {
            title = "Close App Audio".localized
        } else {
            title = "Open App Audio".localized
        }
        self.closeAppAudioButton.setTitle(title, for: .normal)
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

/// agora rtc engine delegate events @available(iOS 12.0, *)
extension ScreenShareMain: AgoraRtcEngineDelegate {
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
        isJoined = true
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
        videoCanvas.renderMode = .fit
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
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, localVideoStateChange state: AgoraLocalVideoStreamState, error: AgoraLocalVideoStreamError) {
        print("localVideoStateChange, state:\(state.rawValue), error:\(error.rawValue)")
        
        switch error {
        case .extensionCaptureStarted:
            print("Replaykit started")
            startScreenCapture()
            break
        case .extensionCaptureStoped:
            print("Replaykit stoped")
            stopScreenCapture()
            break
        case .extensionCaptureDisconnected:
            print("Replaykit inside socket disconnected")
            stopScreenCapture()
            break
        default: break
        }
    }
}

// MARK: -
class ScreenShareEntry : UIViewController
{
    @IBOutlet weak var channelTextField: UITextField!
        
    @IBAction func joinBtnClick(sender: UIButton) {
        guard let channelName = channelTextField.text,
                  channelName.lengthOfBytes(using: .utf8) > 0 else {return}
        channelTextField.resignFirstResponder()
        
        let identifier = "ScreenShare"
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else {return}
        newViewController.title = channelName
        newViewController.configs = ["channelName":channelName]
        self.navigationController?.pushViewController(newViewController, animated: true)
    }
}
