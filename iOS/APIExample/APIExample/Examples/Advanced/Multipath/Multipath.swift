import UIKit
import AgoraRtcKit
import AGEVideoLayout

class MultipathEntry: UIViewController {
    @IBOutlet weak var joinButton: AGButton!
    @IBOutlet weak var channelTextField: AGTextField!
    @IBOutlet weak var roleSegment: UISegmentedControl!
    @IBOutlet weak var multipathModeSegment: UISegmentedControl!
        
    let identifier = "Multipath"
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    @IBAction func doJoinPressed(_ sender: AGButton) {
        guard let channelName = channelTextField.text else { return }
        // resign channel text field
        channelTextField.resignFirstResponder()
        
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        // create new view controller every time to ensure we get a clean vc
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: identifier) as? BaseViewController else {
            return
        }
        newViewController.title = channelName
        newViewController.configs = ["channelName": channelName,
                                     "role_index": roleSegment.selectedSegmentIndex,
                                     "mode_index": multipathModeSegment.selectedSegmentIndex]
        navigationController?.pushViewController(newViewController, animated: true)
    }
}

class MultipathViewController: BaseViewController {
    
    @IBOutlet weak var modeLabel: UILabel!
    
    var localVideo = Bundle.loadVideoView(type: .local, audioOnly: false)
    var remoteVideo = Bundle.loadVideoView(type: .remote, audioOnly: false)
    
    @IBOutlet weak var container: AGEVideoContainer!
    var agoraKit: AgoraRtcEngineKit!
    let channelMediaOption = AgoraRtcChannelMediaOptions()
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false
    
    override func viewDidLoad() {
        super.viewDidLoad()
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
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        
        // get channel name from configs
        guard let channelName = configs["channelName"] as? String else {return}
        guard let roleIndex = configs["role_index"] as? Int else {return}
        guard let modeIndex = configs["mode_index"] as? Int else {return}
        modeLabel.text = (modeIndex == 0) ? "dynamic" : "duplicate"
        
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        agoraKit.enableAudio()
        agoraKit.setClientRole((roleIndex == 0) ? .broadcaster : .audience)
        if (roleIndex == 0) {
            // Set video encoder configuration
            let videoConfig = AgoraVideoEncoderConfiguration()
            videoConfig.dimensions = CGSize(width: 640, height: 360)
            videoConfig.frameRate = AgoraVideoFrameRate.fps15.rawValue
            videoConfig.bitrate = AgoraVideoBitrateStandard
            videoConfig.orientationMode = .adaptative
            videoConfig.mirrorMode = .auto
            agoraKit.setVideoEncoderConfiguration(videoConfig)

            // set up local video to render your local camera preview
            let videoCanvas = AgoraRtcVideoCanvas()
            videoCanvas.uid = 0
            // the view to be binded
            videoCanvas.view = localVideo.videoView
            videoCanvas.renderMode = .hidden
            agoraKit.setupLocalVideo(videoCanvas)
            // you have to call startPreview to see local video
            agoraKit.startPreview()
        }
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        // start joining channel
        channelMediaOption.publishCameraTrack = (roleIndex == 0)
        channelMediaOption.publishMicrophoneTrack = (roleIndex == 0)
        channelMediaOption.clientRoleType = (roleIndex == 0) ? .broadcaster : .audience
        channelMediaOption.enableMultipath = true
        channelMediaOption.uplinkMultipathMode = (modeIndex == 0) ? .dynamic : .duplicate
        channelMediaOption.downlinkMultipathMode = (modeIndex == 0) ? .dynamic : .duplicate
        channelMediaOption.autoSubscribeVideo = true
        channelMediaOption.autoSubscribeAudio = true
        NetworkManager.shared.generateToken(channelName: channelName, success: { token in
            let result = self.agoraKit.joinChannel(byToken: token, channelId: channelName, uid: 0, mediaOptions: self.channelMediaOption)
            if result != 0 {
                self.showAlert(title: "Error", message: "Join channel failed: \(result), please check your params")
            }
        })
    }

    override func viewDidDisappear(_ animated: Bool) {
        super.viewDidDisappear(animated)
        agoraKit.disableAudio()
        agoraKit.disableVideo()
        if isJoined {
            agoraKit.stopPreview()
            agoraKit.leaveChannel { (stats) -> Void in
                LogUtils.log(message: "Left channel, duration: \(stats.duration)", level: .info)
            }
        }
    }
    // enabel/disable multipath
    @IBAction func onClickMultipathSwitch(_ sender: UISwitch) {
        channelMediaOption.enableMultipath = sender.isOn
        agoraKit.updateChannel(with: channelMediaOption)
        if !sender.isOn {
            localVideo.statsInfo?.updateMultipathStats(nil)
        }
    }
}

/// agora rtc engine delegate events
extension MultipathViewController: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "Warning: \(warningCode.description)", level: .warning)
    }
    
    /// callback when error occured for agora sdk
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "Error: \(errorCode)", level: .error)
        self.showAlert(title: "Error", message: "Error occurred: \(errorCode.description)")
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        self.isJoined = true
        LogUtils.log(message: "Join channel \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "Remote user joined: \(uid) \(elapsed)ms", level: .info)
        
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        videoCanvas.view = remoteVideo.videoView
        videoCanvas.renderMode = .hidden
        agoraKit.setupRemoteVideo(videoCanvas)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "Remote user left: \(uid) reason \(reason)", level: .info)
        
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        videoCanvas.view = nil
        videoCanvas.renderMode = .hidden
        agoraKit.setupRemoteVideo(videoCanvas)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, connectionChangedTo state: AgoraConnectionState, reason: AgoraConnectionChangedReason) {
        LogUtils.log(message: "Connection state changed: \(state) \(reason)", level: .info)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, reportRtcStats stats: AgoraChannelStats) {
        localVideo.statsInfo?.updateChannelStats(stats)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, localAudioStats stats: AgoraRtcLocalAudioStats) {
        localVideo.statsInfo?.updateLocalAudioStats(stats)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, multiPathStats stats: AgoraMultipathStats) {
        localVideo.statsInfo?.updateMultipathStats(stats)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteVideoStats stats: AgoraRtcRemoteVideoStats) {
        remoteVideo.statsInfo?.updateVideoStats(stats)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteAudioStats stats: AgoraRtcRemoteAudioStats) {
        remoteVideo.statsInfo?.updateAudioStats(stats)
    }
}
