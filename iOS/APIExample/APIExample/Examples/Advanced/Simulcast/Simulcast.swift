import UIKit
import AgoraRtcKit
import AGEVideoLayout

class SimulcastEntry: UIViewController {
    @IBOutlet weak var joinButton: AGButton!
    @IBOutlet weak var channelTextField: AGTextField!
    @IBOutlet weak var roleSegment: UISegmentedControl!
        
    let identifier = "Simulcast"
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
                                     "role_index": roleSegment.selectedSegmentIndex]
        navigationController?.pushViewController(newViewController, animated: true)
    }
}

class SimulcastViewController: BaseViewController {
        
    var localVideo = Bundle.loadVideoView(type: .local, audioOnly: false)
    var remoteVideo = Bundle.loadVideoView(type: .remote, audioOnly: false)
    
    @IBOutlet weak var hostSettingContainer: UIView!
    @IBOutlet weak var audienceLayerSegment: UISegmentedControl!
    @IBOutlet weak var tipsLabel: UILabel!
    
    @IBOutlet weak var layer1Switch: UISwitch!
    
    @IBOutlet weak var layer2Switch: UISwitch!
    
    @IBOutlet weak var layer3Switch: UISwitch!
    
    @IBOutlet weak var layer4Switch: UISwitch!
        
    @IBOutlet weak var container: AGEVideoContainer!
    var agoraKit: AgoraRtcEngineKit!
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false
    
    private var remoteUid: UInt? = nil
    
    let simulcastConfig = AgoraSimulcastConfig()
    
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
        // set up view
        if roleIndex == 0 {
            hostSettingContainer.isHidden = false
            audienceLayerSegment.isHidden = true
        } else {
            hostSettingContainer.isHidden = true
            audienceLayerSegment.isHidden = false
        }
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        agoraKit.enableAudio()
        agoraKit.setClientRole((roleIndex == 0) ? .broadcaster : .audience)
        
        if (roleIndex == 0) {
            // Set video encoder configuration
            let videoConfig = AgoraVideoEncoderConfiguration()
            videoConfig.dimensions = CGSize(width: 1280, height: 720)
            videoConfig.frameRate = .fps30
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
            
            setupSimulcast()
        }
                
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        // start joining channel
        let option = AgoraRtcChannelMediaOptions()
        option.publishCameraTrack = (roleIndex == 0)
        option.publishMicrophoneTrack = (roleIndex == 0)
        option.clientRoleType = (roleIndex == 0) ? .broadcaster : .audience
        option.autoSubscribeVideo = true
        option.autoSubscribeAudio = true
        NetworkManager.shared.generateToken(channelName: channelName, success: { token in
            let result = self.agoraKit.joinChannel(byToken: token, channelId: channelName, uid: 0, mediaOptions: option)
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
    @IBAction func onClickSimulcastSwitch(_ sender: UISwitch) {
        if self.layer1Switch.isOn == true,
           self.layer2Switch.isOn == true,
           self.layer3Switch.isOn == true,
           self.layer4Switch.isOn == true
        {
            ToastView.show(text: "Maxmum 3 layers can be selected".localized)
            sender.isOn.toggle()
            return
        }
        if sender == self.layer1Switch {
            let layer1_index = AgoraStreamLayerIndex.layer1.rawValue
            simulcastConfig.configs[layer1_index].enable = sender.isOn
        } else if sender == self.layer2Switch {
            let layer2_index = AgoraStreamLayerIndex.layer2.rawValue
            simulcastConfig.configs[layer2_index].enable = sender.isOn

        } else if sender == self.layer3Switch {
            let layer3_index = AgoraStreamLayerIndex.layer3.rawValue
            simulcastConfig.configs[layer3_index].enable = sender.isOn

        } else if sender == self.layer4Switch {
            let layer4_index = AgoraStreamLayerIndex.layer4.rawValue
            simulcastConfig.configs[layer4_index].enable = sender.isOn
        }
        let ret = agoraKit.setSimulcastConfig(simulcastConfig)
        LogUtils.log(message: "updateSimulcast: \(ret) ", level: .info)
    }
    
    @IBAction func onClickLaye1rSegment(_ sender: UISegmentedControl) {
        guard let uid = remoteUid else {
            ToastView.show(text: "No remote user".localized)
            return
        }
        let type: AgoraVideoStreamType
        switch sender.selectedSegmentIndex {
        case 0:
            type = .layer1
        case 1:
            type = .layer2
        case 2:
            type = .layer3
        case 3:
            type = .layer4
        default:
            type = .layer1
        }
        let ret = agoraKit.setRemoteVideoStream(uid, type: type)
        LogUtils.log(message: "set remote uid: \(uid), layer:\(type), ret: \(ret) ", level: .info)
    }
    
    private func setupSimulcast() {
        let layer1_index = AgoraStreamLayerIndex.layer1.rawValue
        let layer2_index = AgoraStreamLayerIndex.layer2.rawValue
        let layer3_index = AgoraStreamLayerIndex.layer3.rawValue
        let layer4_index = AgoraStreamLayerIndex.layer4.rawValue
        simulcastConfig.configs[layer1_index].dimensions.width = 1280
        simulcastConfig.configs[layer1_index].dimensions.height = 720
        simulcastConfig.configs[layer1_index].framerate = 30
        simulcastConfig.configs[layer1_index].enable = layer1Switch.isOn
        
        simulcastConfig.configs[layer2_index].dimensions.width = 960
        simulcastConfig.configs[layer2_index].dimensions.height = 540
        simulcastConfig.configs[layer2_index].framerate = 15
        simulcastConfig.configs[layer2_index].enable = layer2Switch.isOn

        simulcastConfig.configs[layer3_index].dimensions.width = 640
        simulcastConfig.configs[layer3_index].dimensions.height = 360
        simulcastConfig.configs[layer3_index].framerate = 15
        simulcastConfig.configs[layer3_index].enable = layer3Switch.isOn

        simulcastConfig.configs[layer4_index].dimensions.width = 480
        simulcastConfig.configs[layer4_index].dimensions.height = 270
        simulcastConfig.configs[layer4_index].framerate = 15
        simulcastConfig.configs[layer4_index].enable = layer4Switch.isOn

        let ret = agoraKit.setSimulcastConfig(simulcastConfig)
        LogUtils.log(message: "setSimulcastConfig: \(ret) ", level: .info)
    }
}

/// agora rtc engine delegate events
extension SimulcastViewController: AgoraRtcEngineDelegate {
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
        let ret = agoraKit.setRemoteVideoStream(uid, type: .layer1)
        LogUtils.log(message: "set remote layer, ret: \(ret) ", level: .info)
        
        self.remoteUid = uid
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
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteVideoStats stats: AgoraRtcRemoteVideoStats) {
        remoteVideo.statsInfo?.updateVideoStats(stats)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteAudioStats stats: AgoraRtcRemoteAudioStats) {
        remoteVideo.statsInfo?.updateAudioStats(stats)
    }
} 
