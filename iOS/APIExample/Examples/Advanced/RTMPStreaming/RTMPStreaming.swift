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

let CANVAS_WIDTH = 640
let CANVAS_HEIGHT = 480

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

class RTMPStreamingEntry : UIViewController
{
    @IBOutlet weak var joinButtonHost: AGButton!
    @IBOutlet weak var joinButtonAudience: AGButton!
    @IBOutlet weak var channelTextField: AGTextField!
    @IBOutlet var modeBtn: UIButton!
    let identifier = "RTMPStreaming"
    let hostView = "Host"
    let audienceView = "Audience"
    var mode:StreamingMode = .agoraChannel
    
    override func viewDidLoad() {
        super.viewDidLoad()
        modeBtn.setTitle("\(mode.description())", for: .normal)
    }
    
    
    func getStreamingMode(_ mode:StreamingMode) -> UIAlertAction {
        return UIAlertAction(title: "\(mode.description())", style: .default, handler: {[unowned self] action in
            switch mode {
            case .agoraChannel:
                channelTextField.placeholder = "Set Channel Name"
                break
            case .cdnUrl:
                channelTextField.placeholder = "Set CDN URL"
                break
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
        present(alert, animated: true, completion: nil)
    }
    
    @IBAction func joinAsHost(sender: AGButton) {
        guard let channelName = channelTextField.text else {return}
        //resign channel text field
        channelTextField.resignFirstResponder()
        
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        // create new view controller every time to ensure we get a clean vc
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: hostView) as? BaseViewController else {return}
        newViewController.title = channelName
        newViewController.configs = ["channelName":channelName, "mode":mode]
        self.navigationController?.pushViewController(newViewController, animated: true)
    }
    
    @IBAction func joinAsAudience(sender: AGButton) {
        guard let channelName = channelTextField.text else {return}
        //resign channel text field
        channelTextField.resignFirstResponder()
        
        let storyBoard: UIStoryboard = UIStoryboard(name: identifier, bundle: nil)
        // create new view controller every time to ensure we get a clean vc
        guard let newViewController = storyBoard.instantiateViewController(withIdentifier: audienceView) as? BaseViewController else {return}
        newViewController.title = channelName
        newViewController.configs = ["channelName":channelName, "mode":mode]
        self.navigationController?.pushViewController(newViewController, animated: true)
    }
}

class RTMPStreamingHost: BaseViewController {
    @IBOutlet weak var container: AGEVideoContainer!
    @IBOutlet weak var streamingButton: AGButton!
    @IBOutlet weak var rtcSwitcher: UISwitch!
    @IBOutlet weak var rtcSwitcherLabel: UILabel!
    @IBOutlet weak var volumeSilder: UISlider!
    
    var agoraKit: AgoraRtcEngineKit!
    var streamingUrl: String = ""
    var cdnStreaming: Bool = false
    var rtcStreaming: Bool = false
    var transcoding = AgoraLiveTranscoding.default()
    var videoViews: [UInt:VideoView] = [:]
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // layout render view
        let localVideo = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
        localVideo.setPlaceholder(text: "Local Host".localized)
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
//        config.areaCode = GlobalSettings.shared.area
        config.channelProfile = .liveBroadcasting
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        // make myself a broadcaster
        agoraKit.setClientRole(.broadcaster)
        
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        let videoConfig = AgoraVideoEncoderConfiguration(size: AgoraVideoDimension640x480,
                                                        frameRate: .fps15,
                                                        bitrate: AgoraVideoBitrateStandard,
                                                        orientationMode: .fixedPortrait, mirrorMode: .auto)
        agoraKit.setVideoEncoderConfiguration(videoConfig)
        agoraKit.setDirectCdnStreamingVideoConfiguration(videoConfig)
        agoraKit.setDirectCdnStreamingAudioProfile(.default)
        
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
            streamingUrl = "rtmp://ongoing.push.bsc.agoramde.agoraio.cn/live/\(channelName)"
            rtcSwitcher.isEnabled = false
        }
        else {
            streamingUrl = channelName
            rtcSwitcher.isHidden = true
            rtcSwitcherLabel.isHidden = true
        }
    }
    
    @IBAction func onChangeRecordingVolume(_ sender:UISlider){
        let value:Int = Int(sender.value)
        print("adjustRecordingSignalVolume \(value)")
        agoraKit.adjustRecordingSignalVolume(value)
    }
    
    @IBAction func setStreaming(sender: AGButton) {
        if cdnStreaming {
            agoraKit.stopDirectCdnStreaming()
        }
        else {
            let options = AgoraDirectCdnStreamingMediaOptions()
            options.publishCameraTrack = .of(true)
            options.publishMicrophoneTrack = .of(true)
            let ret = agoraKit.startDirectCdnStreaming(self, publishUrl: streamingUrl, mediaOptions: options)
            if ret == 0 {
                streamingButton.setTitle("Streaming", for: .normal)
                streamingButton.setTitleColor(.gray, for: .normal)
            }
        }
    }
        
    @IBAction func setRtcStreaming(_ sender: UISwitch) {
        rtcStreaming = sender.isOn
        if rtcStreaming {
            guard let channelName = configs["channelName"] as? String else {return}
            let options = AgoraRtcChannelMediaOptions()
            options.publishCameraTrack = .of(true)
            options.publishAudioTrack = .of(true)
            options.clientRoleType = .of((Int32)(AgoraClientRole.broadcaster.rawValue))
            let result = agoraKit.joinChannel(byToken: KeyCenter.Token, channelId: channelName, uid: 0, mediaOptions: options)
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
            }
            agoraKit.setLiveTranscoding(transcoding)
            agoraKit.addPublishStreamUrl(streamingUrl, transcodingEnabled: true)
        }
        else {
            agoraKit.leaveChannel { (stats) -> Void in
                LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
            }
            agoraKit.startPreview()
            let options = AgoraDirectCdnStreamingMediaOptions()
            options.publishCameraTrack = .of(true)
            options.publishMicrophoneTrack = .of(true)
            agoraKit.startDirectCdnStreaming(self, publishUrl: streamingUrl, mediaOptions: options)
            guard let localView = videoViews[0] else {return}
            self.container.layoutStream(views: [localView.videoView])
        }
    }
    
    func sortedViews() -> [VideoView] {
        return Array(videoViews.values).sorted(by: { $0.uid < $1.uid })
    }
    
    func updateTranscodeLayout() {
        var index = 0
        transcoding.transcodingUsers?.removeAll()
        for view in videoViews.values {
            index += 1
            switch index {
            case 1:
                let user = AgoraLiveTranscodingUser()
                user.rect = CGRect(x: 0, y: 0, width: CANVAS_WIDTH / 2, height: CANVAS_HEIGHT / 2)
                user.uid = view.uid
                self.transcoding.add(user)
                break
            case 2:
                let user = AgoraLiveTranscodingUser()
                user.rect = CGRect(x: CANVAS_WIDTH / 2, y: 0, width: CANVAS_WIDTH / 2, height: CANVAS_HEIGHT / 2)
                user.uid = view.uid
                self.transcoding.add(user)
                break
            case 3:
                let user = AgoraLiveTranscodingUser()
                user.rect = CGRect(x: 0, y: CANVAS_HEIGHT / 2, width: CANVAS_WIDTH / 2, height: CANVAS_HEIGHT / 2)
                user.uid = view.uid
                self.transcoding.add(user)
                break
            case 4:
                let user = AgoraLiveTranscodingUser()
                user.rect = CGRect(x: CANVAS_WIDTH / 2, y: CANVAS_HEIGHT / 2, width: CANVAS_WIDTH / 2, height: CANVAS_HEIGHT / 2)
                user.uid = view.uid
                self.transcoding.add(user)
                break
            default:
                LogUtils.log(message: "igored user \(view.uid) as only 2x2 video layout supported in this demo.", level: .warning)
            }
        }
        agoraKit.setLiveTranscoding(transcoding)
    }
    
    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            // leave channel when exiting the view
            if rtcStreaming {
                agoraKit.leaveChannel { (stats) -> Void in
                    LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
                }
            }
            else if cdnStreaming {
                agoraKit.stopDirectCdnStreaming()
            }
        }
    }
}

struct CDNChannelInfo {
    let channelName: String
    let index: Int32
}

extension CDNChannelInfo {
    /// static function to generate 4 channels based on given channel name
    static func AllChannelList(_ num:Int32) -> [CDNChannelInfo] {
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

class RTMPStreamingAudience: BaseViewController {
    @IBOutlet weak var container: AGEVideoContainer!
    @IBOutlet weak var rtcSwitcher: UISwitch!
    @IBOutlet weak var rtcSwitcherLabel: UILabel!
    @IBOutlet weak var cdnSelector: UIButton!
    @IBOutlet weak var cdnSelectorLabel: UILabel!
    @IBOutlet weak var volumeSilder: UISlider!
    @IBOutlet weak var volumeSilderLabel: UILabel!
    
    var playerVideo = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
    var agoraKit: AgoraRtcEngineKit!
    var mediaPlayerKit: AgoraRtcMediaPlayerProtocol!
    var streamingUrl: String = ""
    var channelNumber: Int32 = 0
    var rtcStreaming:Bool = false
    var videoViews: [UInt:VideoView] = [:]
    
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
        agoraKit.setLogFile(LogUtils.sdkLogPath())
        // make myself a broadcaster
        agoraKit.setClientRole(.broadcaster)
        
        // enable video module and set up video encoding configs
        agoraKit.enableVideo()
        let videoConfig = AgoraVideoEncoderConfiguration(size: AgoraVideoDimension640x480,
                                                        frameRate: .fps15,
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
        videoCanvas.sourceId = mediaPlayerKit.getMediaPlayerId()
        agoraKit.setupLocalVideo(videoCanvas)
        // you have to call startPreview to see local video
        agoraKit.startPreview()
        
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        guard let mode = configs["mode"] as? StreamingMode else {return}
        guard let channelName = configs["channelName"] as? String else {return}
        if mode == .agoraChannel {
            streamingUrl = "http://ongoing.pull.agoramde.agoraio.cn/live/\(channelName).flv"
            rtcSwitcher.isEnabled = false
            let ret = mediaPlayerKit.open(withAgoraCDN: streamingUrl, startPos: 0)
            print(ret)
        }
        else {
            streamingUrl = channelName
            rtcSwitcher.isHidden = true
            rtcSwitcherLabel.isHidden = true
            mediaPlayerKit.open(streamingUrl, startPos: 0)
        }
    }
    
    func sortedViews() -> [VideoView] {
        return Array(videoViews.values).sorted(by: { $0.uid < $1.uid })
    }
    
    @IBAction func setRtcStreaming(sender: UISwitch) {
        rtcStreaming = sender.isOn
        if rtcStreaming {
            guard let channelName = configs["channelName"] as? String else {return}
            let options = AgoraRtcChannelMediaOptions()
            options.publishCameraTrack = .of(true)
            options.publishAudioTrack = .of(true)
            options.clientRoleType = .of((Int32)(AgoraClientRole.broadcaster.rawValue))
            let result = agoraKit.joinChannel(byToken: KeyCenter.Token, channelId: channelName, uid: 0, mediaOptions: options)
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
            }
            else{
                // set up local video to render your local camera preview
                let videoCanvas = AgoraRtcVideoCanvas()
                videoCanvas.uid = 0
                let localVideo = Bundle.loadVideoView(type: .local, audioOnly: false)
                // the view to be binded
                videoCanvas.view = localVideo.videoView
                videoCanvas.renderMode = .hidden
                agoraKit.setupLocalVideo(videoCanvas)
                videoViews[0] = localVideo
                // you have to call startPreview to see local video
                agoraKit.startPreview()
                cdnSelector.isEnabled = false
                volumeSilder.isHidden = false
                volumeSilderLabel.isHidden = false
            }
        }
        else {
            agoraKit.leaveChannel { (stats) -> Void in
                LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
            }
            container.layoutStream(views: [playerVideo])
            cdnSelector.isEnabled = true
            volumeSilder.isHidden = true
            volumeSilderLabel.isHidden = true
        }
    }
    
    @IBAction func onChangeRecordingVolume(_ sender:UISlider){
        let value:Int = Int(sender.value)
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
    
    func getCDNChannel(_ channel:CDNChannelInfo) -> UIAlertAction {
        return UIAlertAction(title: channel.channelName, style: .default, handler: {[unowned self] action in
            self.cdnSelector.setTitle(channel.channelName, for: .normal)
            print(self.mediaPlayerKit.getAgoraCDNIndex())
            let ret = self.mediaPlayerKit.switchAgoraCDN(channel.index)
            print(ret)
        })
    }
    
    
}


extension RTMPStreamingHost: AgoraDirectCdnStreamingEventDelegate {
    func onDirectCdnStreamingStateChanged(_ state: AgoraDirectCdnStreamingState, error: AgoraDirectCdnStreamingError, message: String?) {
        DispatchQueue.main.async {[self] in
            switch state{
            case .started:
                self.streamingButton.setTitle("Stop Streaming", for: .normal)
                self.streamingButton.setTitleColor(.red, for: .normal)
                cdnStreaming = true
                break
            case .stopped:
                self.streamingButton.setTitle("Start Live Streaming", for: .normal)
                self.streamingButton.setTitleColor(.blue, for: .normal)
                cdnStreaming = false
                break
            case .failed:
                self.showAlert(title: "Error", message: "Start Streaming failed, please go back to previous page and check the settings.")
            default:
                LogUtils.log(message: "onDirectCdnStreamingStateChanged: \(state.rawValue), \(error.rawValue), \(message!)", level: .info)
            }
            rtcSwitcher.isEnabled = cdnStreaming
        }
    }
    
}

/// agora rtc engine delegate events
extension RTMPStreamingHost: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
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
        updateTranscodeLayout()
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
        
        //remove remote audio view
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
    
    /// Reports the statistics of the uploading local video streams once every two seconds.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, localVideoStats stats: AgoraRtcLocalVideoStats) {
        videoViews[0]?.statsInfo?.updateLocalVideoStats(stats)
    }
    
    /// Reports the statistics of the uploading local audio streams once every two seconds.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, localAudioStats stats: AgoraRtcLocalAudioStats) {
        videoViews[0]?.statsInfo?.updateLocalAudioStats(stats)
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
extension RTMPStreamingAudience: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
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
        
        //remove remote audio view
        self.videoViews.removeValue(forKey: uid)
        self.container.layoutStream2x2(views: sortedViews())
        self.container.reload(level: 0, animated: true)
    }
    
    /// Reports the statistics of the current call. The SDK triggers this callback once every two seconds after the user joins the channel.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, reportRtcStats stats: AgoraChannelStats) {
        videoViews[0]?.statsInfo?.updateChannelStats(stats)
    }
    
    /// Reports the statistics of the uploading local video streams once every two seconds.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, localVideoStats stats: AgoraRtcLocalVideoStats) {
        videoViews[0]?.statsInfo?.updateLocalVideoStats(stats)
    }
    
    /// Reports the statistics of the uploading local audio streams once every two seconds.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, localAudioStats stats: AgoraRtcLocalAudioStats) {
        videoViews[0]?.statsInfo?.updateLocalAudioStats(stats)
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

extension RTMPStreamingAudience: AgoraRtcMediaPlayerDelegate {
    func agoraRtcMediaPlayer(_ playerKit: AgoraRtcMediaPlayerProtocol, didChangedTo state: AgoraMediaPlayerState, error: AgoraMediaPlayerError) {
        LogUtils.log(message: "player rtc channel publish helper state changed to: \(state.rawValue), error: \(error.rawValue)", level: .info)
        DispatchQueue.main.async {[weak self] in
            guard let weakself = self else { return }
            switch state {
            case .failed:
                weakself.showAlert(message: "media player error: \(error.rawValue)")
                break
            case .openCompleted:
                weakself.mediaPlayerKit.play()
                guard let mode = weakself.configs["mode"] as? StreamingMode else {return}
                if (mode == .agoraChannel){
                    let num = weakself.mediaPlayerKit.getAgoraCDNCount()
                    if num > 0 {
                        weakself.channelNumber = num
                        weakself.cdnSelectorLabel.isHidden = false
                        weakself.cdnSelector.isHidden = false
                        weakself.cdnSelector.setTitle("\("CDN Channel".localized)\(1)", for: .normal)
                    }
                    weakself.rtcSwitcher.isEnabled = true
                }
                break
            case .stopped:
                break
            default: break
            }
        }
    }
}

