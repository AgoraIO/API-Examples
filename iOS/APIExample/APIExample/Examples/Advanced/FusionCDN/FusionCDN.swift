//
//  RTMPStreaming.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Foundation
import UIKit
import AgoraRtcKit
import AGEVideoLayout

var WIDTH = 480
var HEIGHT = 640

enum StreamingMode {
    case agoraChannel
    case cdnUrl
    func description() -> String {
        switch self {
        case .agoraChannel: return "Agora Channel".localized
        case .cdnUrl: return "CDN URL".localized
        }
    }
}

class FusionCDNEntry: BaseViewController {
    @IBOutlet weak var joinButtonHost: AGButton!
    @IBOutlet weak var joinButtonAudience: AGButton!
    @IBOutlet weak var channelTextField: AGTextField!
    @IBOutlet var modeBtn: UIButton!
    let identifier = "FusionCDN"
    let hostView = "Host"
    let audienceView = "Audience"
    var mode: StreamingMode = .agoraChannel
    
    override func viewDidLoad() {
        super.viewDidLoad()
        modeBtn.setTitle("\(mode.description())", for: .normal)
    }
    
    func getStreamingMode(_ mode: StreamingMode) -> UIAlertAction {
        return UIAlertAction(title: "\(mode.description())", style: .default, handler: { [unowned self] _ in
            switch mode {
            case .agoraChannel:
                channelTextField.placeholder = "Set Channel Name"
            case .cdnUrl:
                channelTextField.placeholder = "Set CDN URL"
            }
            self.mode = mode
            self.modeBtn.setTitle("\(mode.description())", for: .normal)
        })
    }
    
    @IBAction func setStreamingMode() {
        let alert = UIAlertController(title: "Set Streaming Mode".localized, message: nil, preferredStyle: .actionSheet)
        alert.addAction(getStreamingMode(.agoraChannel))
        alert.addAction(getStreamingMode(.cdnUrl))
        alert.addCancelAction()
//        present(alert, animated: true, completion: nil)
        presentAlertViewController(alert)
    }
    
    @IBAction func joinAsHost(sender: AGButton) {
        guard let channelName = channelTextField.text else { return }
        // resign channel text field
        channelTextField.resignFirstResponder()
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        // create new view controller every time to ensure we get a clean vc
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: hostView) as? BaseViewController else { 
            return
        }
        newViewController.title = channelName
        newViewController.configs = ["channelName": channelName, "mode": mode]
        navigationController?.pushViewController(newViewController, animated: true)
    }
    
    @IBAction func joinAsAudience(sender: AGButton) {
        guard let channelName = channelTextField.text else { return }
        // resign channel text field
        channelTextField.resignFirstResponder()
        
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        // create new view controller every time to ensure we get a clean vc
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: audienceView) as? BaseViewController else { 
            return
        }
        newViewController.title = channelName
        newViewController.configs = ["channelName": channelName, "mode": mode]
        navigationController?.pushViewController(newViewController, animated: true)
    }
}

class FusionCDNHost: BaseViewController {
    @IBOutlet weak var container: AGEVideoContainer!
    @IBOutlet weak var streamingButton: AGButton!
    @IBOutlet weak var rtcSwitcher: UISwitch!
    @IBOutlet weak var rtcSwitcherLabel: UILabel!
    @IBOutlet weak var volumeSlider: UISlider!
    
    var agoraKit: AgoraRtcEngineKit!
    var streamingUrl: String = ""
    var cdnStreaming: Bool = false
    var rtcStreaming: Bool = false
    var transcoding = AgoraLiveTranscoding.default()
    var videoViews: [UInt: VideoView] = [:]
    var videoConfig: AgoraVideoEncoderConfiguration!
    let localUid = UInt.random(in: 1001...2000)
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // layout render view
        let localVideo = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
        localVideo.setPlaceholder(text: "Local Host".localized)
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        // config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        // make myself a broadcaster
        agoraKit.setClientRole(GlobalSettings.shared.getUserRole())
        
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        agoraKit.enableAudio()
        
        guard let resolution = GlobalSettings.shared.getSetting(key: "resolution")?
            .selectedOption().value as? CGSize else {
            return
        }
        
        WIDTH = Int(resolution.height > resolution.width ? resolution.width : resolution.height)
        HEIGHT = Int(resolution.height > resolution.width ? resolution.height : resolution.width)
        videoConfig = AgoraVideoEncoderConfiguration(size: resolution,
                                                     frameRate: AgoraVideoFrameRate.fps15,
                                                     bitrate: AgoraVideoBitrateStandard,
                                                     orientationMode: .fixedPortrait, mirrorMode: .auto)
        agoraKit.setVideoEncoderConfiguration(videoConfig)
        agoraKit.setDirectCdnStreamingVideoConfiguration(videoConfig)
        agoraKit.setDirectCdnStreamingAudioConfiguration(.default)
        transcoding.size = CGSize(width: WIDTH, height: HEIGHT)
        transcoding.videoFramerate = 15
        
        // set up local video to render your local camera preview
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = 0
        // the view to be binded
        videoCanvas.view = localVideo.videoView
        videoCanvas.renderMode = .hidden
        agoraKit.setupLocalVideo(videoCanvas)
        // you have to call startPreview to see local video
        agoraKit.startPreview()
        videoViews[0] = localVideo
        container.layoutStream(views: [localVideo])
        
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        guard let mode = configs["mode"] as? StreamingMode else {return}
        guard let channelName = configs["channelName"] as? String else {return}
        if mode == .agoraChannel {
            streamingUrl = "rtmp://push.webdemo.agoraio.cn/lbhd/\(channelName)"
            rtcSwitcher.isEnabled = false
        } else {
            streamingUrl = channelName
            rtcSwitcher.isHidden = true
            rtcSwitcherLabel.isHidden = true
        }
    }
    
    @IBAction func onChangeRecordingVolume(_ sender: UISlider) {
        let value: Int = Int(sender.value)
        print("adjustRecordingSignalVolume \(value)")
        agoraKit.adjustRecordingSignalVolume(value)
    }
    
    @IBAction func setStreaming(sender: AGButton) {
        if rtcStreaming {
            stopRtcStreaming()
            resetUI()
        } else if cdnStreaming {
            stopRskStreaming()
            resetUI()
        } else {
            switchToRskStreaming()
        }
    }
    
    private func switchToRskStreaming() {
        agoraKit.setDirectCdnStreamingVideoConfiguration(videoConfig)
        agoraKit.setDirectCdnStreamingAudioConfiguration(.default)
        let options = AgoraDirectCdnStreamingMediaOptions()
        options.publishCameraTrack = GlobalSettings.shared.getUserRole() == .broadcaster
        options.publishMicrophoneTrack = GlobalSettings.shared.getUserRole() == .broadcaster
        let ret = agoraKit.startDirectCdnStreaming(self, publishUrl: streamingUrl, mediaOptions: options)
        if ret == 0 {
            streamingButton.setTitle("Streaming", for: .normal)
            streamingButton.setTitleColor(.gray, for: .normal)
            agoraKit.startPreview()
        } else {
            stopRskStreaming()
            resetUI()
            self.showAlert(title: "Error", message: "startDirectCdnStreaming failed: \(ret)")
        }
    }
    
    private func switchToRtcStreaming() {
        guard let channelName = configs["channelName"] as? String else {return}
        let options = AgoraRtcChannelMediaOptions()
        options.publishCameraTrack = GlobalSettings.shared.getUserRole() == .broadcaster
        options.publishMicrophoneTrack = GlobalSettings.shared.getUserRole() == .broadcaster
        options.clientRoleType = .broadcaster
        NetworkManager.shared.generateToken(channelName: channelName, success: { token in
            let result = self.agoraKit.joinChannel(byToken: token,
                                                   channelId: channelName,
                                                   uid: self.localUid,
                                                   mediaOptions: options)
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
                self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
            }
        })
    }
    
    private func stopRtcStreaming() {
        rtcStreaming = false
        rtcSwitcher.isOn = false
        let option = AgoraLeaveChannelOptions()
        option.stopMicrophoneRecording = false
        agoraKit.leaveChannel(option, leaveChannelBlock: nil)
        agoraKit.stopRtmpStream(streamingUrl)
    }
    
    private func stopRskStreaming() {
        cdnStreaming = false
        rtcSwitcher.isEnabled = true
        agoraKit.stopDirectCdnStreaming()
        agoraKit.stopPreview()
    }
    
    private func resetUI() {
        rtcStreaming = false
        cdnStreaming = false
        rtcSwitcher.isOn = false
        rtcSwitcher.isEnabled = false
        streamingButton.setTitle("Start Live Streaming", for: .normal)
        streamingButton.setTitleColor(.blue, for: .normal)
    }
    
    @IBAction func setRtcStreaming(_ sender: UISwitch) {
        rtcStreaming = sender.isOn
        if rtcStreaming {
            stopRskStreaming()

        } else {
            stopRtcStreaming()
            switchToRskStreaming()
        }
    }
    
    func sortedViews() -> [VideoView] {
        Array(videoViews.values).sorted(by: { $0.uid < $1.uid })
    }
    
    func updateTranscodeLayout() {
        var index = 0
        for view in videoViews.values {
            index += 1
            switch index {
            case 2:
                let user = AgoraLiveTranscodingUser()
                user.rect = CGRect(x: WIDTH / 2, y: 0, width: WIDTH / 2, height: HEIGHT / 2)
                user.uid = view.uid
                self.transcoding.add(user)
            case 3:
                let user = AgoraLiveTranscodingUser()
                user.rect = CGRect(x: 0, y: HEIGHT / 2, width: WIDTH / 2, height: HEIGHT / 2)
                user.uid = view.uid
                self.transcoding.add(user)
            case 4:
                let user = AgoraLiveTranscodingUser()
                user.rect = CGRect(x: WIDTH / 2, y: HEIGHT / 2, width: WIDTH / 2, height: HEIGHT / 2)
                user.uid = view.uid
                self.transcoding.add(user)
            default:
                LogUtils.log(message: "igored user \(view.uid) as only 2x2 video layout supported in this demo.", level: .warning)
            }
        }
        agoraKit.updateRtmpTranscoding(transcoding)
    }
    
    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            // leave channel when exiting the view
            agoraKit.disableAudio()
            agoraKit.disableVideo()
            if rtcStreaming {
                stopRtcStreaming()
            } else if cdnStreaming {
                stopRskStreaming()
                resetUI()
            }
            AgoraRtcEngineKit.destroy()
        }
    }
}

struct CDNChannelInfo {
    let channelName: String
    let index: Int32
}

extension CDNChannelInfo {
    /// static function to generate 4 channels based on given channel name
    static func AllChannelList(_ num: Int32) -> [CDNChannelInfo] {
        var channels = [CDNChannelInfo]()
        for index in 0..<num {
            let channel = CDNChannelInfo(
                channelName: "\("CDN Channel".localized)\(index + 1)",
                index: index
            )
            channels.append(channel)
        }
        return channels
    }
}

class FusionCDNAudience: BaseViewController {
    @IBOutlet weak var container: AGEVideoContainer!
    @IBOutlet weak var rtcSwitcher: UISwitch!
    @IBOutlet weak var rtcSwitcherLabel: UILabel!
    @IBOutlet weak var cdnSelector: UIButton!
    @IBOutlet weak var cdnSelectorLabel: UILabel!
    @IBOutlet weak var volumeSlider: UISlider!
    @IBOutlet weak var volumeSliderLabel: UILabel!
    
    var playerVideo = Bundle.loadVideoView(type: .local, audioOnly: false)
    var agoraKit: AgoraRtcEngineKit!
    var mediaPlayerKit: AgoraRtcMediaPlayerProtocol!
    var streamingUrl: String = ""
    var channelNumber: Int32 = 0
    var rtcStreaming: Bool = false
    var videoViews: [UInt: VideoView] = [:]
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // layout render view
        playerVideo.setPlaceholder(text: "Player".localized)
        container.layoutStream(views: [playerVideo])
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.channelProfile = .liveBroadcasting
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        // make myself a broadcaster
        agoraKit.setClientRole(.audience)
        
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        // get channel name from configs
        guard let resolution = GlobalSettings.shared.getSetting(key: "resolution")?.selectedOption().value as? CGSize,
              let fps = GlobalSettings.shared.getSetting(key: "fps")?.selectedOption().value as? AgoraVideoFrameRate else {return}
        
        let videoConfig = AgoraVideoEncoderConfiguration(size: resolution,
                                                        frameRate: fps,
                                                        bitrate: AgoraVideoBitrateStandard,
                                                        orientationMode: .fixedPortrait, mirrorMode: .auto)
        agoraKit.setVideoEncoderConfiguration(videoConfig)
        
        // prepare media player
        mediaPlayerKit = agoraKit.createMediaPlayer(with: self)
        mediaPlayerKit.setView(playerVideo.videoView)

        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.view = playerVideo.videoView
        videoCanvas.renderMode = .hidden
        videoCanvas.sourceType = .mediaPlayer
        videoCanvas.mediaPlayerId = mediaPlayerKit.getMediaPlayerId()
        agoraKit.setupLocalVideo(videoCanvas)
        // you have to call startPreview to see local video
        agoraKit.startPreview()
        
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        guard let mode = configs["mode"] as? StreamingMode else {return}
        guard let channelName = configs["channelName"] as? String else {return}
        if mode == .agoraChannel {
            streamingUrl = "rtmp://pull.webdemo.agoraio.cn/lbhd/\(channelName)"
            rtcSwitcher.isEnabled = false
            let mediaSource = AgoraMediaSource()
            mediaSource.url = streamingUrl
            mediaPlayerKit.open(streamingUrl, startPos: 0)
        } else {
            streamingUrl = channelName
            rtcSwitcher.isHidden = true
            rtcSwitcherLabel.isHidden = true
            mediaPlayerKit.open(streamingUrl, startPos: 0)
        }
    }
    
    func sortedViews() -> [VideoView] {
        Array(videoViews.values).sorted(by: { $0.uid < $1.uid })
    }
    
    @IBAction func setRtcStreaming(sender: UISwitch) {
        rtcStreaming = sender.isOn
        if rtcStreaming {
            guard let channelName = configs["channelName"] as? String else {return}
            let options = AgoraRtcChannelMediaOptions()
            options.publishCameraTrack = true
            options.publishCustomAudioTrack = true
            options.clientRoleType = .broadcaster
            NetworkManager.shared.generateToken(channelName: channelName, success: { token in
                let result = self.agoraKit.joinChannel(byToken: token, channelId: channelName, uid: 0, mediaOptions: options)
                if result != 0 {
                    // Usually happens with invalid parameters
                    // Error code description can be found at:
                    // en: https://api-ref.agora.io/en/video-sdk/ios/4.x/documentation/agorartckit/agoraerrorcode
                    // cn: https://doc.shengwang.cn/api-ref/rtc/ios/error-code
                    self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
                } else {
                    // set up local video to render your local camera preview
                    let videoCanvas = AgoraRtcVideoCanvas()
                    videoCanvas.uid = 0
                    let localVideo = Bundle.loadVideoView(type: .local, audioOnly: false)
                    // the view to be binded
                    videoCanvas.view = localVideo.videoView
                    videoCanvas.renderMode = .hidden
                    self.agoraKit.setupLocalVideo(videoCanvas)
                    self.videoViews[0] = localVideo
                    // you have to call startPreview to see local video
                    self.agoraKit.startPreview()
                    self.cdnSelector.isEnabled = false
                    self.volumeSlider.isHidden = false
                    self.volumeSliderLabel.isHidden = false
                }
            })
        } else {
            let leaveChannelOption = AgoraLeaveChannelOptions()
            leaveChannelOption.stopMicrophoneRecording = false
            agoraKit.leaveChannel(leaveChannelOption) { stats in
                LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
            }
            let localVideo = videoViews[0]
            videoViews.removeAll()
            videoViews[0] = localVideo
            agoraKit.startPreview()
            container.layoutStream(views: [playerVideo])
            cdnSelector.isEnabled = true
            volumeSlider.isHidden = true
            volumeSliderLabel.isHidden = true
        }
    }
    
    @IBAction func onChangeRecordingVolume(_ sender: UISlider) {
        let value: Int = Int(sender.value)
        print("adjustRecordingSignalVolume \(value)")
        agoraKit.adjustRecordingSignalVolume(value)
    }
    
    @IBAction func setCDNChannel() {
        let alert = UIAlertController(title: "Select CDN Channel".localized, message: nil, preferredStyle: .actionSheet)
        for channel in CDNChannelInfo.AllChannelList(channelNumber) {
            alert.addAction(getCDNChannel(channel))
        }
        alert.addCancelAction()
        present(alert, animated: true, completion: nil)
    }
    
    func getCDNChannel(_ channel: CDNChannelInfo) -> UIAlertAction {
        return UIAlertAction(title: channel.channelName, style: .default, handler: { [unowned self] _ in
            self.cdnSelector.setTitle(channel.channelName, for: .normal)
            let ret = mediaPlayerKit.switchAgoraCDNLine(by: channel.index)
            print(ret)
        })
    }
    
    override func viewDidDisappear(_ animated: Bool) {
        super.viewDidDisappear(animated)
        agoraKit.disableVideo()
        agoraKit.disableAudio()
        agoraKit.leaveChannel { stats -> Void in
            LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
        }
        AgoraRtcEngineKit.destroy()
    }
}

extension FusionCDNHost: AgoraDirectCdnStreamingEventDelegate {
    func onDirectCdnStreamingStateChanged(_ state: AgoraDirectCdnStreamingState,
                                          reason: AgoraDirectCdnStreamingReason,
                                          message: String?) {
        DispatchQueue.main.async {[self] in
            switch state {
            case .running:
                self.streamingButton.setTitle("Stop Streaming", for: .normal)
                self.streamingButton.setTitleColor(.red, for: .normal)
                cdnStreaming = true
                rtcSwitcher.isEnabled = true
                
            case .stopped:
                if rtcStreaming {
                    // switch to rtc streaming when direct cdn streaming completely stopped
                    switchToRtcStreaming()

                } else {
                    self.streamingButton.setTitle("Start Live Streaming", for: .normal)
                    self.streamingButton.setTitleColor(.blue, for: .normal)
                    cdnStreaming = false
                }

            case .failed:
                self.showAlert(title: "Error", 
                               message: "Start Streaming failed, please go back to previous page and check the settings.")
            default:
                LogUtils.log(message: "onDirectCdnStreamingStateChanged: \(state.rawValue), \(reason.rawValue), \(message ?? "")",
                             level: .info)
            }
        }
    }
    
}

/// agora rtc engine delegate events
extension FusionCDNHost: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://api-ref.agora.io/en/voice-sdk/ios/3.x/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.description)", level: .warning)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        let user = AgoraLiveTranscodingUser()
        user.rect = CGRect(x: 0, y: 0, width: WIDTH, height: HEIGHT)
        user.uid = uid
        agoraKit.startPreview()
        transcoding.add(user)
//        agoraKit.updateRtmpTranscoding(transcoding)
        if !streamingUrl.isEmpty { // join Channel success后发流
            agoraKit.startRtmpStream(withTranscoding: streamingUrl, transcoding: transcoding)
        }
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        
        let remoteVideo = Bundle.loadVideoView(type: .remote, audioOnly: false)
        remoteVideo.uid = uid
        
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = remoteVideo.videoView
        videoCanvas.renderMode = .hidden
        agoraKit.setupRemoteVideo(videoCanvas)
        
        // set up local video to render your local camera preview
        let videoCanvas1 = AgoraRtcVideoCanvas()
        videoCanvas1.uid = 0
        let localVideo = Bundle.loadVideoView(type: .local, audioOnly: false)
        // the view to be binded
        videoCanvas1.view = localVideo.videoView
        videoCanvas1.renderMode = .hidden
        agoraKit.setupLocalVideo(videoCanvas1)
        videoViews[0] = localVideo
        
        self.videoViews[uid] = remoteVideo
        self.container.layoutStream2x2(views: sortedViews())
        self.container.reload(level: 0, animated: true)
        updateTranscodeLayout()
    }
    func rtcEngine(_ engine: AgoraRtcEngineKit,
                   rtmpStreamingChangedToState url: String,
                   state: AgoraRtmpStreamingState,
                   reason: AgoraRtmpStreamingReason) {
        LogUtils.log(message: "On rtmpStreamingChangedToState, state: \(state.rawValue), errCode: \(reason.rawValue)",
                     level: .info)
    }
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, 
                   streamUnpublishedWithUrl url: String) {
        switchToRtcStreaming()
        // set up local video to render your local camera preview
//        let videoCanvas = AgoraRtcVideoCanvas()
//        videoCanvas.uid = 0
//        let localVideo = Bundle.loadVideoView(type: .local, audioOnly: false)
//        // the view to be binded
//        videoCanvas.view = localVideo.videoView
//        videoCanvas.renderMode = .hidden
//        videoViews.removeAll()
//        videoViews[0] = localVideo
//        agoraKit.setupLocalVideo(videoCanvas)
        guard let view = videoViews[0] else { return }
        self.container.layoutStream(views: [view.videoView])
    }
    
    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    func rtcEngine(_ engine: AgoraRtcEngineKit, 
                   didOfflineOfUid uid: UInt,
                   reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)
        
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = nil
        videoCanvas.renderMode = .hidden
        agoraKit.setupRemoteVideo(videoCanvas)
        
        // remove remote audio view
        self.videoViews.removeValue(forKey: uid)
        self.container.layoutStream2x2(views: sortedViews())
        self.container.reload(level: 0, animated: true)
        updateTranscodeLayout()
    }
    
    /// Reports the statistics of the current call. The SDK triggers this callback once every two seconds after the user joins the channel.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, reportRtcStats stats: AgoraChannelStats) {
        videoViews[0]?.statsInfo?.updateChannelStats(stats)
    }
        
    /// Reports the statistics of the video stream from each remote user/host.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteVideoStats stats: AgoraRtcRemoteVideoStats) {
        videoViews[stats.uid]?.statsInfo?.updateVideoStats(stats)
    }
    
    /// Reports the statistics of the audio stream from each remote user/host.
    /// @param stats stats struct for current call statistics
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteAudioStats stats: AgoraRtcRemoteAudioStats) {
        videoViews[stats.uid]?.statsInfo?.updateAudioStats(stats)
    }
}

/// agora rtc engine delegate events
extension FusionCDNAudience: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://api-ref.agora.io/en/voice-sdk/ios/3.x/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.description)", level: .warning)
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        
        let remoteVideo = Bundle.loadVideoView(type: .remote, audioOnly: false)
        remoteVideo.uid = uid
        
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = remoteVideo.videoView
        videoCanvas.renderMode = .hidden
        agoraKit.setupRemoteVideo(videoCanvas)
        
        self.videoViews[uid] = remoteVideo
        self.container.layoutStream2x2(views: sortedViews())
        self.container.reload(level: 0, animated: true)
    }
    
    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)
        
        let videoCanvas = AgoraRtcVideoCanvas()
        videoCanvas.uid = uid
        // the view to be binded
        videoCanvas.view = nil
        videoCanvas.renderMode = .hidden
        agoraKit.setupRemoteVideo(videoCanvas)
        
        // remove remote audio view
        self.videoViews.removeValue(forKey: uid)
        self.container.layoutStream2x2(views: sortedViews())
        self.container.reload(level: 0, animated: true)
    }
    
    /// Reports the statistics of the current call. The SDK triggers this callback once every two seconds after the user joins the channel.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, reportRtcStats stats: AgoraChannelStats) {
        videoViews[0]?.statsInfo?.updateChannelStats(stats)
    }
    
    /// Reports the statistics of the video stream from each remote user/host.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteVideoStats stats: AgoraRtcRemoteVideoStats) {
        videoViews[stats.uid]?.statsInfo?.updateVideoStats(stats)
    }
    
    /// Reports the statistics of the audio stream from each remote user/host.
    /// @param stats stats struct for current call statistics
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteAudioStats stats: AgoraRtcRemoteAudioStats) {
        videoViews[stats.uid]?.statsInfo?.updateAudioStats(stats)
    }
}

extension FusionCDNAudience: AgoraRtcMediaPlayerDelegate {
    func AgoraRtcMediaPlayer(_ playerKit: AgoraRtcMediaPlayerProtocol, 
                             didChangedTo state: AgoraMediaPlayerState,
                             reason: AgoraMediaPlayerReason) {
        LogUtils.log(message: "player rtc channel publish helper state changed to: \(state.rawValue), error: \(reason.rawValue)", level: .info)
        DispatchQueue.main.async {[weak self] in
            guard let weakself = self else { return }
            switch state {
            case .failed:
                weakself.showAlert(message: "media player error: \(reason.rawValue)")

            case .openCompleted:
                weakself.mediaPlayerKit.play()
                guard let mode = weakself.configs["mode"] as? StreamingMode else {return}
                if mode == .agoraChannel {
                    let num = weakself.mediaPlayerKit.getAgoraCDNLineCount()
                    if num > 0 {
                        weakself.channelNumber = num
                        weakself.cdnSelectorLabel.isHidden = false
                        weakself.cdnSelector.isHidden = false
                        weakself.cdnSelector.setTitle("\("CDN Channel".localized)\(1)", for: .normal)
                    }
                    weakself.rtcSwitcher.isEnabled = true
                }
            case .stopped: break
            default: break
            }
        }
    }
    
    func AgoraRtcMediaPlayer(_ playerKit: AgoraRtcMediaPlayerProtocol, 
                             didOccur event: AgoraMediaPlayerEvent,
                             elapsedTime time: Int,
                             message: String?) {
        DispatchQueue.main.async { [weak self] in
            guard let weakself = self else { return }
            switch event {
            case .switchError:
                weakself.showAlert(message: "switch cdn channel error!: \(message ?? "")")

            case .switchComplete:
                weakself.showAlert(message: "switch cdn channel complete!")
                
            default: break
            }
        }
    }
}
