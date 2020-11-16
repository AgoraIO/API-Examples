//
//  JoinChannelVC.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/4/17.
//  Copyright © 2020 Agora Corp. All rights reserved.
//
import Cocoa
import AgoraRtcKit
import AGEVideoLayout

class JoinChannelAudioMain: BaseViewController {
    var videos: [VideoView] = []
    
    @IBOutlet weak var container: AGEVideoContainer!
    @IBOutlet weak var channelField: NSTextField!
    @IBOutlet weak var activeSpeaker: NSTextField!
    @IBOutlet weak var localUserSpeaking: NSTextField!
    @IBOutlet weak var joinBtn: NSButton!
    @IBOutlet weak var leaveBtn: NSButton!
    @IBOutlet weak var micPicker: NSPopUpButton!
    @IBOutlet weak var profilePicker: NSPopUpButton!
    @IBOutlet weak var scenarioPicker: NSPopUpButton!
    @IBOutlet weak var layoutPicker: NSPopUpButton!
    @IBOutlet weak var recordingVolumeSlider: NSSlider!
    @IBOutlet weak var deviceRecordingVolumeSlider: NSSlider!
    @IBOutlet weak var playbackVolumeSlider: NSSlider!
    @IBOutlet weak var devicePlaybackVolumeSlider: NSSlider!
    @IBOutlet weak var userPlaybackVolumeSlider: NSSlider!
    var agoraKit: AgoraRtcEngineKit!
    
    var mics:[AgoraRtcDeviceInfo] = [] {
        didSet {
            DispatchQueue.main.async {[unowned self] in
                self.micPicker.addItems(withTitles: self.mics.map({ (device: AgoraRtcDeviceInfo) -> String in
                    return (device.deviceName ?? "")
                }))
            }
        }
    }
    var scenarios:[AgoraAudioScenario] = [] {
        didSet {
            DispatchQueue.main.async {[unowned self] in
                self.scenarioPicker.addItems(withTitles: self.scenarios.map({ (scenario: AgoraAudioScenario) -> String in
                    return scenario.description()
                }))
            }
        }
    }
    var profiles:[AgoraAudioProfile] = [] {
        didSet {
            DispatchQueue.main.async {[unowned self] in
                self.profilePicker.addItems(withTitles: self.profiles.map({ (profile: AgoraAudioProfile) -> String in
                    return profile.description()
                }))
            }
        }
    }
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false {
        didSet {
            channelField.isEnabled = !isJoined
            joinBtn.isHidden = isJoined
            leaveBtn.isHidden = !isJoined
            layoutPicker.isEnabled = !isJoined
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        layoutVideos(2)
        
        profiles = AgoraAudioProfile.allValues()
        scenarios = AgoraAudioScenario.allValues()
        
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        //config.areaCode = GlobalSettings.shared.area.rawValue
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        
        //find device in a separate thread to avoid blocking main thread
        let queue = DispatchQueue(label: "device.enumerateDevices")
        queue.async {[unowned self] in
            self.mics = self.agoraKit.enumerateDevices(.audioRecording) ?? []
        }
    }
    
    override func viewWillBeRemovedFromSplitView() {
        if(isJoined) {
            agoraKit.leaveChannel { (stats:AgoraChannelStats) in
                LogUtils.log(message: "Left channel", level: .info)
            }
        }
    }
    
    @IBAction func onJoinPressed(_ sender:Any) {
        // use selected devices
        if let micId = mics[micPicker.indexOfSelectedItem].deviceId {
            agoraKit.setDevice(.audioRecording, deviceId: micId)
        }
        
        // disable video module in audio scene
        agoraKit.disableVideo()
        let profile = profiles[profilePicker.indexOfSelectedItem]
        let scenario = scenarios[scenarioPicker.indexOfSelectedItem]
        agoraKit.setAudioProfile(profile, scenario: scenario)
        
        // set live broadcaster mode
        agoraKit.setChannelProfile(.liveBroadcasting)
        // set myself as broadcaster to stream audio
        agoraKit.setClientRole(.broadcaster)
        
        // enable volume indicator
        agoraKit.enableAudioVolumeIndication(200, smooth: 3)
        
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        let result = agoraKit.joinChannel(byToken: nil, channelId: channelField.stringValue, info: nil, uid: 0) {[unowned self] (channel, uid, elapsed) -> Void in
            self.isJoined = true
            self.videos[0].uid = uid
            LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
        }
        if result != 0 {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
        }
    }
    
    @IBAction func onLeavePressed(_ sender: Any) {
        agoraKit.leaveChannel { [unowned self] (stats:AgoraChannelStats) in
            LogUtils.log(message: "Left channel", level: .info)
            self.videos[0].uid = nil
            self.isJoined = false
        }
    }
    
    @IBAction func onLayoutChanged(_ sender: NSPopUpButton) {
        switch(sender.indexOfSelectedItem) {
            //1x1
        case 0:
            layoutVideos(2)
            break
            //1x3
        case 1:
            layoutVideos(4)
            break
            //1x8
        case 2:
            layoutVideos(9)
            break
            //1x15
        case 3:
            layoutVideos(16)
            break
        default:
            layoutVideos(2)
        }
    }
    
    @IBAction func onUserPlayoutVolumeChanged(_ sender: NSSlider) {
        let value:Int32 = Int32(sender.intValue)
        if videos.count > 1 {
            LogUtils.log(message: "onUserPlayoutVolumeChanged \(value)", level: .info)
            //TODO
//            agoraKit.adjustUserPlaybackSignalVolume(videos[1].uid!, volume: value)
        }
    }
    
    @IBAction func onDevicePlayoutVolumeChanged(_ sender: NSSlider) {
        let value:Int32 = Int32(sender.intValue)
        LogUtils.log(message: "onDevicePlayoutVolumeChanged \(value)", level: .info)
        agoraKit.setDeviceVolume(.audioPlayout, volume: value)
    }
    
    @IBAction func onDeviceRecordingVolumeChanged(_ sender: NSSlider) {
        let value:Int32 = Int32(sender.intValue)
        LogUtils.log(message: "onDeviceRecordingVolumeChanged \(value)", level: .info)
        agoraKit.setDeviceVolume(.audioRecording, volume: value)
    }
    
    @IBAction func onRecordingVolumeChanged(_ sender: NSSlider) {
        let value:Int = Int(sender.intValue)
        LogUtils.log(message: "onRecordingVolumeChanged \(value)", level: .info)
        agoraKit.adjustRecordingSignalVolume(value)
    }
    
    @IBAction func onPlaybackVolumeChanged(_ sender: NSSlider) {
        let value:Int = Int(sender.intValue)
        LogUtils.log(message: "onPlaybackVolumeChanged \(value)", level: .info)
        agoraKit.adjustPlaybackSignalVolume(value)
    }
    
    func layoutVideos(_ count: Int) {
        videos = []
        for i in 0...count - 1 {
            let view = VideoView.createFromNib()!
            if(i == 0) {
                view.placeholder.stringValue = "Local"
                view.type = .local
                view.statsInfo = StatisticsInfo(type: .local(StatisticsInfo.LocalInfo()))
            } else {
                view.placeholder.stringValue = "Remote \(i)"
                view.type = .remote
                view.statsInfo = StatisticsInfo(type: .remote(StatisticsInfo.RemoteInfo()))
            }
            view.audioOnly = true
            videos.append(view)
        }
        // layout render view
        container.layoutStream(views: videos)
    }
}

/// agora rtc engine delegate events
extension JoinChannelAudioMain: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.rawValue)", level: .warning)
    }
    
    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode)", level: .error)
        self.showAlert(title: "Error", message: "Error \(errorCode.rawValue) occur")
    }
    
    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        
        // find a VideoView w/o uid assigned
        if let remoteVideo = videos.first(where: { $0.uid == nil }) {
            remoteVideo.uid = uid
        } else {
            LogUtils.log(message: "no video canvas available for \(uid), cancel bind", level: .warning)
        }
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
        if let remoteVideo = videos.first(where: { $0.uid == uid }) {
            remoteVideo.uid = nil
        } else {
            LogUtils.log(message: "no matching video canvas for \(uid), cancel unbind", level: .warning)
        }
    }
    
    
    /// Reports the statistics of the current call. The SDK triggers this callback once every two seconds after the user joins the channel.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, reportRtcStats stats: AgoraChannelStats) {
        videos[0].statsInfo?.updateChannelStats(stats)
    }
    
    /// Reports the statistics of the uploading local audio streams once every two seconds.
    /// @param stats stats struct
    func rtcEngine(_ engine: AgoraRtcEngineKit, localAudioStats stats: AgoraRtcLocalAudioStats) {
        videos[0].statsInfo?.updateLocalAudioStats(stats)
    }
    
    /// Reports the statistics of the audio stream from each remote user/host.
    /// @param stats stats struct for current call statistics
    func rtcEngine(_ engine: AgoraRtcEngineKit, remoteAudioStats stats: AgoraRtcRemoteAudioStats) {
        videos.first(where: { $0.uid == stats.uid })?.statsInfo?.updateAudioStats(stats)
    }
    
    /// Occurs when the most active speaker is detected.
    /// @param speakerUid The user ID of the most active speaker
    func rtcEngine(_ engine: AgoraRtcEngineKit, activeSpeaker speakerUid: UInt) {
        DispatchQueue.main.async {
            self.activeSpeaker.stringValue = (speakerUid as NSNumber).stringValue

        }
    }
    
    /// Reports which users are speaking, the speakers' volumes, and whether the local user is speaking.
    /// @params speakers volume info for all speakers
    /// @params totalVolume Total volume after audio mixing. The value range is [0,255].
    func rtcEngine(_ engine: AgoraRtcEngineKit, reportAudioVolumeIndicationOfSpeakers speakers: [AgoraRtcAudioVolumeInfo], totalVolume: Int) {
        for volumeInfo in speakers {
            if (volumeInfo.uid == 0) {
                videos[0].statsInfo?.updateVolume(volumeInfo.volume)
            } else {
                videos.first(where: { $0.uid == volumeInfo.uid })?.statsInfo?.updateVolume(volumeInfo.volume)
            }
        }
    }
}
