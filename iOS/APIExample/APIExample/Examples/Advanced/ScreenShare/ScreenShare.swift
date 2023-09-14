//
//  ScreenShare.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//
import UIKit
import AGEVideoLayout
import AgoraRtcKit
import ReplayKit

class ScreenShareEntry : UIViewController
{
    @IBOutlet weak var joinButton: UIButton!
    @IBOutlet weak var channelTextField: UITextField!
    let identifier = "ScreenShare"
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    @IBAction func doJoinPressed(sender: UIButton) {
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

class ScreenShareMain: BaseViewController {
    var localVideo = Bundle.loadVideoView(type: .local, audioOnly: false)
    var remoteVideo = Bundle.loadVideoView(type: .remote, audioOnly: false)
    
    @IBOutlet weak var container: AGEVideoContainer!
    @IBOutlet weak var broadcasterPickerContainer: UIView!
    @IBOutlet weak var fpsButton: UIButton!
    
    var agoraKit: AgoraRtcEngineKit!
    private lazy var pickerView: PickerView = {
        let pickerView = PickerView()
        pickerView.dataArray = fpsDataSources.map({ "\($0)" })
        return pickerView
    }()
    private lazy var scenarioPickerView: PickerView = {
        let pickerView = PickerView()
        pickerView.dataArray = scenarioTypeDataSources.map({ "\($0.key)" })
        return pickerView
    }()
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
    private var fpsDataSources: [Int] = [15, 30, 60]
    private var scenarioTypeDataSources: [String: Int] = ["document": 1, "gaming": 2, "video": 3, "RDC": 4]
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // prepare system broadcaster picker
//        prepareSystemBroadcaster()
        
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
        
        
        // make myself a broadcaster
        agoraKit.setClientRole(GlobalSettings.shared.getUserRole())
        
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        agoraKit.enableAudio()
        let resolution = (GlobalSettings.shared.getSetting(key: "resolution")?.selectedOption().value as? CGSize) ?? .zero
        let fps = (GlobalSettings.shared.getSetting(key: "fps")?.selectedOption().value as? AgoraVideoFrameRate) ?? .fps15
        let orientation = (GlobalSettings.shared.getSetting(key: "orientation")?.selectedOption().value as? AgoraVideoOutputOrientationMode) ?? .fixedPortrait
        agoraKit.setVideoEncoderConfiguration(AgoraVideoEncoderConfiguration(size: resolution,
                                                                             frameRate: fps,
                                                                             bitrate: AgoraVideoBitrateStandard,
                                                                             orientationMode: orientation,
                                                                             mirrorMode: .auto))
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
        

        NetworkManager.shared.generateToken(channelName: channelName, uid: SCREEN_SHARE_UID, success: { token in
            let result = self.agoraKit.joinChannel(byToken: token, channelId: channelName, uid: SCREEN_SHARE_UID, mediaOptions: self.option)
            self.agoraKit.muteRemoteAudioStream(UInt(SCREEN_SHARE_BROADCASTER_UID), mute: true)
            self.agoraKit.muteRemoteVideoStream(UInt(SCREEN_SHARE_BROADCASTER_UID), mute: true)
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
            }
        })
    }
    
    func prepareSystemBroadcaster() {
        if #available(iOS 12.0, *) {
            let frame = CGRect(x: 0, y:0, width: 60, height: 60)
            systemBroadcastPicker = RPSystemBroadcastPickerView(frame: frame)
            systemBroadcastPicker?.showsMicrophoneButton = false
            systemBroadcastPicker?.autoresizingMask = [.flexibleTopMargin, .flexibleRightMargin]
            let bundleId = Bundle.main.bundleIdentifier ?? ""
            systemBroadcastPicker?.preferredExtension = "\(bundleId).Agora-ScreenShare-Extension"
            
        } else {
            self.showAlert(message: "Minimum support iOS version is 12.0")
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
    
    @IBAction func clickCaptureAudio(_ sender: UISwitch) {
        screenParams.captureAudio = sender.isOn
    }
    @IBAction func clickCaptureVideo(_ sender: UISwitch) {
        screenParams.captureVideo = sender.isOn
    }
    @IBAction func captureSignalVolumeSlider(_ sender: UISlider) {
        screenParams.audioParams.captureSignalVolume = Int(sender.value * 100)
    }
    @IBAction func clickFpsButton(_ sender: UIButton) {
        pickerView.pickerViewSelectedValueClosure = { [weak self] value in
            guard let self = self else { return }
            self.fpsButton.setTitle("\(value)fps", for: .normal)
            self.screenParams.videoParams.frameRate = AgoraVideoFrameRate(rawValue: Int(value) ?? 15) ?? .fps15
        }
        AlertManager.show(view: pickerView, alertPostion: .bottom)
    }
    
    @IBAction func clickScreenScenarioType(_ sender: UIButton) {
        scenarioPickerView.pickerViewSelectedValueClosure = { [weak self] value in
            guard let self = self else { return }
            sender.setTitle("\(value)", for: .normal)
            let key = self.scenarioTypeDataSources[value] ?? 3
            let scenarioType = AgoraScreenScenarioType(rawValue: key) ?? .video
            self.agoraKit.setScreenCaptureScenario(scenarioType)
        }
        AlertManager.show(view: scenarioPickerView, alertPostion: .bottom)
    }
    func isScreenShareUid(uid: UInt) -> Bool {
        return uid >= SCREEN_SHARE_UID_MIN && uid <= SCREEN_SHARE_UID_MAX
    }
    
    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            // leave channel when exiting the view
            // deregister packet processing
            if isJoined {
                agoraKit.disableAudio()
                agoraKit.disableVideo()
                agoraKit.leaveChannel { (stats) -> Void in
                    LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
                }
                AgoraRtcEngineKit.destroy()
            }
        }
    }
    @IBAction func stopScreenCapture(_ sender: Any) {
        agoraKit.stopScreenCapture()
        option.publishScreenCaptureVideo = false
        option.publishScreenCaptureAudio = false
        option.publishCameraTrack = true
        agoraKit.updateChannel(with: option)
    }
    @IBAction func startScreenCapture(_ sender: Any) {
        agoraKit.startScreenCapture(screenParams)
        prepareSystemBroadcaster()
        guard let picker = systemBroadcastPicker else { return }
        for view in picker.subviews where view is UIButton {
            (view as? UIButton)?.sendActions(for: .allEvents)
            break
        }
    }
    @IBAction func updateScreenCapture(_ sender: Any) {
        
        agoraKit.updateScreenCapture(screenParams)
    }
    
}

/// agora rtc engine delegate events
extension ScreenShareMain: AgoraRtcEngineDelegate {
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
    func rtcEngine(_ engine: AgoraRtcEngineKit, localVideoStateChangedOf state: AgoraVideoLocalState, error: AgoraLocalVideoStreamError, sourceType: AgoraVideoSourceType) {
        switch (state, sourceType) {
        case (.capturing, .screen):
            option.publishScreenCaptureVideo = true
            option.publishScreenCaptureAudio = true
            option.publishCameraTrack = false
            agoraKit.updateChannel(with: option)
            
        default: break
        }
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
    
    /// Reports the statistics of the video stream from each remote user/host.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteVideoStats stats: AgoraRtcRemoteVideoStats) {
        remoteVideo.statsInfo?.updateVideoStats(stats)
    }
    
    /// Reports the statistics of the audio stream from each remote user/host.
    /// @param stats stats struct for current call statistics
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteAudioStats stats: AgoraRtcRemoteAudioStats) {
        remoteVideo.statsInfo?.updateAudioStats(stats)
    }
}


extension ScreenShareMain: UIPickerViewDataSource, UIPickerViewDelegate {
    func pickerView(_ pickerView: UIPickerView, rowHeightForComponent component: Int) -> CGFloat {
        return 60.0
    }

    func pickerView(_ pickerView: UIPickerView, didSelectRow row: Int, inComponent component: Int) {
        print("我选择了第"+"\(row)"+"行")
    }

    func pickerView(_ pickerView: UIPickerView, titleForRow row: Int, forComponent component: Int) -> String? {
        "\(fpsDataSources[row])fps"
    }

//    // TODO: 可以设置哪一行显示特定的样式
//    func pickerView(_ pickerView: UIPickerView, viewForRow row: Int, forComponent component: Int, reusing view: UIView?) -> UIView {
//        // 创建一个对象
//        let specificView = UIView.init()
//        specificView.frame = CGRect.init(x: 10, y: 5, width: 100, height: 60)
//        specificView.backgroundColor = UIColor.magenta
//        /**
//         创建一个标题
//         */
//        let specificLable = UILabel.init(frame: CGRect.init(x: 5, y: 0, width: 90, height: 60))
//        specificLable.text = (SourceData[row] as! String)
//        specificLable.textColor = UIColor.white
//        specificView.addSubview(specificLable)
//        return specificView
//    }

    func numberOfComponents(in pickerView: UIPickerView) -> Int {
        return 1
    }
    func pickerView(_ pickerView: UIPickerView, numberOfRowsInComponent component: Int) -> Int {
        fpsDataSources.count
    }
}
