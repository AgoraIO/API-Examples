//
//  JoinChannelVC.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//
import UIKit
import AGEVideoLayout
import AgoraRtcKit

class LiveStreamingEntry : UIViewController
{
    @IBOutlet weak var joinButton: UIButton!
    @IBOutlet weak var channelTextField: UITextField!
    let identifier = "LiveStreaming"
    var role:AgoraClientRole = .broadcaster
    private var isFirstFrame: Bool = false
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    func getRoleAction(_ role: AgoraClientRole) -> UIAlertAction{
        return UIAlertAction(title: "\(role.description())", style: .default, handler: {[unowned self] action in
            self.role = role
            self.doJoin()
        })
    }
    
    @IBAction func doOptimizeFirstFrameSwitch(_ sender: UISwitch) {
        if sender.isOn {
            let alertVC = UIAlertController(title: "After this function is enabled, it cannot be disabled and takes effect only when both the primary and secondary ends are enabled".localized,
                                            message: nil,
                                            preferredStyle: .alert)
            
            let ok = UIAlertAction(title: "Sure".localized, style: .default) { _ in
                self.isFirstFrame = true
            }
            let cancel = UIAlertAction(title: "Cancel".localized, style: .cancel) { _ in
                self.isFirstFrame = false
            }
            
            alertVC.addAction(ok)
            alertVC.addAction(cancel)
            present(alertVC, animated: true, completion: nil)
        } else {
            isFirstFrame = false
        }
    }
    
    @IBAction func doJoinPressed(sender: UIButton) {
        guard let _ = channelTextField.text else {return}
        //resign channel text field
        channelTextField.resignFirstResponder()
        
        //display role picker
        let alert = UIAlertController(title: "Pick Role".localized, message: nil, preferredStyle: UIDevice.current.userInterfaceIdiom == .pad ? UIAlertController.Style.alert : UIAlertController.Style.actionSheet)
        alert.addAction(getRoleAction(.broadcaster))
        alert.addAction(getRoleAction(.audience))
        alert.addCancelAction()
        present(alert, animated: true, completion: nil)
    }
    
    func doJoin() {
        guard let channelName = channelTextField.text else {return}
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        // create new view controller every time to ensure we get a clean vc
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else {return}
        newViewController.title = channelName
        newViewController.configs = ["channelName":channelName, "role":self.role, "isFirstFrame": isFirstFrame]
        navigationController?.pushViewController(newViewController, animated: true)
    }
}

class LiveStreamingMain: BaseViewController {
    var foregroundVideo = Bundle.loadVideoView(type: .local, audioOnly: false)
    var backgroundVideo = Bundle.loadVideoView(type: .remote, audioOnly: false)
    @IBOutlet weak var foregroundVideoContainer:UIView!
    @IBOutlet weak var backgroundVideoContainer:UIView!
    @IBOutlet weak var clientRoleToggleView:UIView!
    @IBOutlet weak var ultraLowLatencyToggleView:UIView!
    @IBOutlet weak var clientRoleToggle:UISwitch!
    @IBOutlet weak var ultraLowLatencyToggle:UISwitch!
    @IBOutlet weak var takeSnapshot: UIButton!
    @IBOutlet weak var watarMarkContainer: UIView!
    @IBOutlet weak var dualStreamContainer: UIView!
    @IBOutlet weak var dualStreamTipsLabel: UILabel!
    @IBOutlet weak var bFrameContainer: UIView!
    @IBOutlet weak var codingSegment: UISegmentedControl!
    
    var remoteUid: UInt? {
        didSet {
            foregroundVideoContainer.isHidden = !(role == .broadcaster && remoteUid != nil)
        }
    }
    var agoraKit: AgoraRtcEngineKit!
    var role: AgoraClientRole = .broadcaster {
        didSet {
            foregroundVideoContainer.isHidden = !(role == .broadcaster && remoteUid != nil)
            ultraLowLatencyToggle.isEnabled = role == .audience
            watarMarkContainer.isHidden = role == .audience
            bFrameContainer.isHidden = role == .audience
            codingSegment.isHidden = role == .audience
        }
    }
    var isLocalVideoForeground = false {
        didSet {
            if isLocalVideoForeground {
                foregroundVideo.setPlaceholder(text: "Local Host".localized)
                backgroundVideo.setPlaceholder(text: "Remote Host".localized)
            } else {
                foregroundVideo.setPlaceholder(text: "Remote Host".localized)
                backgroundVideo.setPlaceholder(text: "Local Host".localized)
            }
        }
    }
    var isUltraLowLatencyOn: Bool = false
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // layout render view
        foregroundVideoContainer.addSubview(foregroundVideo)
        backgroundVideoContainer.addSubview(backgroundVideo)
        foregroundVideo.bindFrameToSuperviewBounds()
        backgroundVideo.bindFrameToSuperviewBounds()
        
        // set up agora instance when view loadedlet config = AgoraRtcEngineConfig()
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.channelProfile = .liveBroadcasting
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit.setLogFile(LogUtils.sdkLogPath())

        if let isFirstFrame = configs["isFirstFrame"] as? Bool, isFirstFrame == true {
            agoraKit.enableInstantMediaRendering()
            agoraKit.startMediaRenderingTracing()
        }
        
        // get channel name from configs
        guard let channelName = configs["channelName"] as? String, let clientRole = configs["role"] as? AgoraClientRole else {return}
        
        role = clientRole
        // for audience put local video in foreground
        isLocalVideoForeground = role == .audience
        // if inital role is broadcaster, do not show audience options
        clientRoleToggleView.isHidden = role == .broadcaster
        ultraLowLatencyToggleView.isHidden = role == .broadcaster
        
        // make this room live broadcasting room
        updateClientRole(role)
        
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        let option = AgoraRtcChannelMediaOptions()
        option.publishCameraTrack = true
        option.publishMicrophoneTrack = true
        option.clientRoleType = GlobalSettings.shared.getUserRole()
        NetworkManager.shared.generateToken(channelName: channelName, success: { token in
            let result = self.agoraKit.joinChannel(byToken: token, channelId: channelName, uid: 0, mediaOptions: option)
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
            }
        })
    }
    
    /// make myself a broadcaster
    func becomeBroadcaster() {
        guard let resolution = GlobalSettings.shared.getSetting(key: "resolution")?.selectedOption().value as? CGSize,
        let fps = GlobalSettings.shared.getSetting(key: "fps")?.selectedOption().value as? AgoraVideoFrameRate,
        let orientation = GlobalSettings.shared.getSetting(key: "orientation")?.selectedOption().value as? AgoraVideoOutputOrientationMode else {
            LogUtils.log(message: "invalid video configurations, failed to become broadcaster", level: .error)
            return
        }
        agoraKit.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: resolution,
                                                                             frameRate: fps,
                                                                             bitrate: AgoraVideoBitrateStandard,
                                                                             orientationMode: orientation, mirrorMode: .auto))
        
        // set up local video to render your local camera preview
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = 0
        // the view to be binded
        videoCanvas.view = localVideoCanvas()
        videoCanvas.renderMode = .hidden
        agoraKit.setupLocalVideo(videoCanvas)
        // you have to call startPreview to see local video
        agoraKit.startPreview()
        agoraKit.enableAudio()
        
        agoraKit.setClientRole(.broadcaster, options: nil)
    }
    
    /// make myself an audience
    func becomeAudience() {
        // unbind view
        agoraKit.setupLocalVideo(nil)
        // You have to provide client role options if set to audience
        let options = AgoraClientRoleOptions()
        options.audienceLatencyLevel = isUltraLowLatencyOn ? .ultraLowLatency : .lowLatency
        agoraKit.setClientRole(.audience, options: options)
    }
    
    func localVideoCanvas() -> UIView {
        return isLocalVideoForeground ? foregroundVideo.videoView : backgroundVideo.videoView
    }
    
    func remoteVideoCanvas() -> UIView {
        return isLocalVideoForeground ? backgroundVideo.videoView : foregroundVideo.videoView
    }
        
    @IBAction func onTapBFrameSwitch(_ sender: UISwitch) {
        let encoderConfig = AgoraVideoEncoderConfiguration()
        let videoOptions = AgoraAdvancedVideoOptions()
        videoOptions.compressionPreference = sender.isOn ? .quality : .lowLatency
        encoderConfig.advancedVideoOptions = videoOptions
        agoraKit.setVideoEncoderConfiguration(encoderConfig)
    }
    
    @IBAction func onTapCodingSegment(_ sender: UISegmentedControl) {
        let encoderConfig = AgoraVideoEncoderConfiguration()
        let advancedOptions = AgoraAdvancedVideoOptions()
        switch sender.selectedSegmentIndex {
        case 0:
            advancedOptions.encodingPreference = .preferAuto
            
        case 1:
            advancedOptions.encodingPreference = .prefersoftware
            
        case 2:
            advancedOptions.encodingPreference = .preferhardware
            
        default: break
        }
        encoderConfig.advancedVideoOptions = advancedOptions
        agoraKit.setVideoEncoderConfiguration(encoderConfig)
    }
    
    
    // setup watermark
    @IBAction func onTapWatermarkSwitch(_ sender: UISwitch) {
        if sender.isOn {
            if let filepath = Bundle.main.path(forResource: "agora-logo", ofType: "png") {
                if let url = URL.init(string: filepath) {
                    let waterMark = WatermarkOptions()
                    waterMark.visibleInPreview = true
                    waterMark.positionInPortraitMode = CGRect(x: 10, y: 80, width: 60, height: 60)
                    waterMark.positionInLandscapeMode = CGRect(x: 10, y: 80, width: 60, height: 60)
                    agoraKit.addVideoWatermark(url, options: waterMark)
                }
            }
        } else {
            agoraKit.clearVideoWatermarks()
        }
    }
    @IBAction func onTapDualStreamSwitch(_ sender: UISwitch) {
        agoraKit.enableDualStreamMode(sender.isOn)
        dualStreamTipsLabel.text = sender.isOn ? "已开启": "默认: 大流"
    }
    
    @IBAction func onTakeSnapshot(_ sender: Any) {
        guard let remoteUid = remoteUid else {
            showAlert(title: "remote user has not joined, and cannot take a screenshot".localized, message: "")
            return
        }
        let path = NSTemporaryDirectory().appending("1.png")
        agoraKit.takeSnapshot(Int(remoteUid), filePath: path)
        showAlert(title: "Screenshot successful".localized, message: path)
    }
    @IBAction func onTapForegroundVideo(_ sender:UIGestureRecognizer) {
        isLocalVideoForeground = !isLocalVideoForeground
        
        let localVideoCanvas = AgoraRtcVideoCanvas()
        localVideoCanvas.uid = 0
        localVideoCanvas.renderMode = .hidden
        localVideoCanvas.view = self.localVideoCanvas()
        
        let remoteVideoCanvas = AgoraRtcVideoCanvas()
        remoteVideoCanvas.renderMode = .hidden
        remoteVideoCanvas.view = self.remoteVideoCanvas()
        
        agoraKit.setupLocalVideo(localVideoCanvas)
        if let uid = remoteUid {
            remoteVideoCanvas.uid = uid
            agoraKit.setupRemoteVideo(remoteVideoCanvas)
        }
    }
    
    @IBAction func onToggleClientRole(_ sender:UISwitch) {
        let role:AgoraClientRole = sender.isOn ? .broadcaster : .audience
        updateClientRole(role)
    }
    
    fileprivate func updateClientRole(_ role:AgoraClientRole) {
        self.role = role
        if(role == .broadcaster) {
            becomeBroadcaster()
        } else {
            becomeAudience()
        }
    }
    
    @IBAction func onToggleUltraLowLatency(_ sender:UISwitch) {
        updateUltraLowLatency(sender.isOn)
    }
    
    fileprivate func updateUltraLowLatency(_ enabled:Bool) {
        if(self.role == .audience) {
            self.isUltraLowLatencyOn = enabled
            updateClientRole(.audience)
        }
    }
    
    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            // leave channel when exiting the view
            if isJoined {
                agoraKit.disableVideo()
                agoraKit.disableAudio()
                agoraKit.leaveChannel { (stats) -> Void in
                    LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
                }
                AgoraRtcEngineKit.destroy()
            }
        }
    }
}

/// agora rtc engine delegate events
extension LiveStreamingMain: AgoraRtcEngineDelegate {
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
        
        //record remote uid
        remoteUid = uid
        
        // Only one remote video view is available for this
        // tutorial. Here we check if there exists a surface
        // view tagged as this uid.
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = remoteVideoCanvas()
        videoCanvas.renderMode = .hidden
        agoraKit.setupRemoteVideo(videoCanvas)
    }
    
    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)
        
        //clear remote uid
        if(remoteUid == uid){
            remoteUid = nil
        }
        
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
        foregroundVideo.statsInfo?.updateChannelStats(stats)
    }
    
    /// Reports the statistics of the uploading local audio streams once every two seconds.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, localAudioStats stats: AgoraRtcLocalAudioStats) {
        foregroundVideo.statsInfo?.updateLocalAudioStats(stats)
    }
    
    /// Reports the statistics of the video stream from each remote user/host.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteVideoStats stats: AgoraRtcRemoteVideoStats) {
        backgroundVideo.statsInfo?.updateVideoStats(stats)
    }
    
    /// Reports the statistics of the audio stream from each remote user/host.
    /// @param stats stats struct for current call statistics
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteAudioStats stats: AgoraRtcRemoteAudioStats) {
        backgroundVideo.statsInfo?.updateAudioStats(stats)
    }

    func rtcEngine(_ engine: AgoraRtcEngineKit, videoRenderingTracingResultOfUid uid: UInt, currentEvent: AgoraMediaTraceEvent, tracingInfo: AgoraVideoRenderingTracingInfo) {
        backgroundVideo.statsInfo?.updateFirstFrameInfo(tracingInfo)
    }
}
