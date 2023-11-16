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
    
    var agoraKit: AgoraRtcEngineKit!
    var videos: [VideoView] = []
    @IBOutlet weak var Container: AGEVideoContainer!
    
    /**
     --- Audio Profile Picker ---
     */
    @IBOutlet weak var selectAudioProfilePicker: Picker!
    var audioProfiles = AgoraAudioProfile.allValues()
    var selectedProfile: AgoraAudioProfile? {
        let index = selectAudioProfilePicker.indexOfSelectedItem
        if index >= 0 && index < audioProfiles.count {
            return audioProfiles[index]
        } else {
            return nil
        }
    }
    func initSelectAudioProfilePicker() {
        selectAudioProfilePicker.label.stringValue = "Audio Profile".localized
        selectAudioProfilePicker.picker.addItems(withTitles: audioProfiles.map { $0.description() })
        
        selectAudioProfilePicker.onSelectChanged {
            if !self.isJoined {
                return
            }
            guard let profile = self.selectedProfile,
                  let scenario = self.selectedAudioScenario else {
                return
            }
            self.agoraKit.setAudioProfile(profile)
            self.agoraKit.setAudioScenario(scenario)
        }
    }
    
    /**
     --- Audio Scenario Picker ---
     */
    @IBOutlet weak var selectAudioScenarioPicker: Picker!
    var audioScenarios = AgoraAudioScenario.allValues()
    var selectedAudioScenario: AgoraAudioScenario? {
        let index = self.selectAudioScenarioPicker.indexOfSelectedItem
        if index >= 0 && index < Configs.Resolutions.count {
            return audioScenarios[index]
        } else {
            return nil
        }
    }
    func initSelectAudioScenarioPicker() {
        selectAudioScenarioPicker.label.stringValue = "Audio Scenario".localized
        selectAudioScenarioPicker.picker.addItems(withTitles: audioScenarios.map { $0.description() })
        
        selectAudioScenarioPicker.onSelectChanged {
            if !self.isJoined {
                return
            }
            guard let profile = self.selectedProfile,
                  let scenario = self.selectedAudioScenario else {
                return
            }
            self.agoraKit.setAudioProfile(profile)
            self.agoraKit.setAudioScenario(scenario)
        }
    }
    
    /**
     --- Microphones Picker ---
     */
    @IBOutlet weak var selectMicsPicker: Picker!
    var mics:[AgoraRtcDeviceInfo] = [] {
        didSet {
            DispatchQueue.main.async {
                self.selectMicsPicker.picker.addItems(withTitles: self.mics.map {$0.deviceName ?? "unknown"})
            }
        }
    }
    var selectedMicrophone: AgoraRtcDeviceInfo? {
        let index = self.selectMicsPicker.indexOfSelectedItem
        if index >= 0 && index < mics.count {
            return mics[index]
        } else {
            return nil
        }
    }
    func initSelectMicsPicker() {
        selectMicsPicker.label.stringValue = "Microphone".localized
        // find device in a separate thread to avoid blocking main thread
        let queue = DispatchQueue(label: "device.enumerateDevices")
        queue.async {[unowned self] in
            self.mics = self.agoraKit.enumerateDevices(.audioRecording) ?? []
        }
        
        selectMicsPicker.onSelectChanged {
            if !self.isJoined {
                return
            }
            // use selected devices
            guard let micId = self.selectedMicrophone?.deviceId else {
                return
            }
            self.agoraKit.setDevice(.audioRecording, deviceId: micId)
        }
    }
    
    /**
     --- Layout Picker ---
     */
    @IBOutlet weak var selectLayoutPicker: Picker!
    let layouts = [Layout("1v1", 2), Layout("1v3", 4), Layout("1v8", 9), Layout("1v15", 16)]
    var selectedLayout: Layout? {
        let index = self.selectLayoutPicker.indexOfSelectedItem
        if index >= 0 && index < layouts.count {
            return layouts[index]
        } else {
            return nil
        }
    }
    func initSelectLayoutPicker() {
        layoutVideos(2)
        selectLayoutPicker.label.stringValue = "Layout".localized
        selectLayoutPicker.picker.addItems(withTitles: layouts.map { $0.label })
        selectLayoutPicker.onSelectChanged {
            if self.isJoined {
                return
            }
            guard let layout = self.selectedLayout else { return }
            self.layoutVideos(layout.value)
        }
    }
    
    /**
     --- Device Recording Volume Slider ---
     */
    @IBOutlet weak var deviceRecordingVolumeSlider: Slider!
    func initDeviceRecordingVolumeSlider() {
        deviceRecordingVolumeSlider.label.stringValue = "Device Recording Volume".localized
        deviceRecordingVolumeSlider.slider.minValue = 0
        deviceRecordingVolumeSlider.slider.maxValue = 100
        deviceRecordingVolumeSlider.slider.intValue = 50
        
        deviceRecordingVolumeSlider.onSliderChanged {
            let volume: Int32 = Int32(self.deviceRecordingVolumeSlider.slider.intValue)
            LogUtils.log(message: "onDeviceRecordingVolumeChanged \(volume)", level: .info)
            self.agoraKit?.setDeviceVolume(.audioRecording, volume: volume)
        }
    }
    
    /**
     --- Device Recording Volume Slider ---
     */
    @IBOutlet weak var sdkRecordingVolumeSlider: Slider!
    func initSdkRecordingVolumeSlider() {
        sdkRecordingVolumeSlider.label.stringValue = "SDK Recording Volume".localized
        sdkRecordingVolumeSlider.slider.minValue = 0
        sdkRecordingVolumeSlider.slider.maxValue = 100
        sdkRecordingVolumeSlider.slider.intValue = 50
        
        sdkRecordingVolumeSlider.onSliderChanged {
            let volume: Int = Int(self.sdkRecordingVolumeSlider.slider.intValue)
            LogUtils.log(message: "onRecordingVolumeChanged \(volume)", level: .info)
            self.agoraKit?.adjustRecordingSignalVolume(volume)
        }
    }
    
    /**
     --- Device Playout Volume Slider ---
     */
    @IBOutlet weak var devicePlayoutVolumeSlider: Slider!
    func initDevicePlayoutVolumeSlider() {
        devicePlayoutVolumeSlider.label.stringValue = "Device Playout Volume".localized
        devicePlayoutVolumeSlider.slider.minValue = 0
        devicePlayoutVolumeSlider.slider.maxValue = 100
        devicePlayoutVolumeSlider.slider.intValue = 50
        
        devicePlayoutVolumeSlider.onSliderChanged {
            let volume: Int32 = Int32(self.devicePlayoutVolumeSlider.slider.intValue)
            LogUtils.log(message: "onDevicePlayoutVolumeChanged \(volume)", level: .info)
            self.agoraKit?.setDeviceVolume(.audioPlayout, volume: volume)
        }
    }
    
    /**
     --- Device Playout Volume Slider ---
     */
    @IBOutlet weak var sdkPlaybackVolumeSlider: Slider!
    func initSdkPlaybackVolumeSlider() {
        sdkPlaybackVolumeSlider.label.stringValue = "SDK Playout Volume".localized
        sdkPlaybackVolumeSlider.slider.minValue = 0
        sdkPlaybackVolumeSlider.slider.maxValue = 100
        sdkPlaybackVolumeSlider.slider.intValue = 50
        
        sdkPlaybackVolumeSlider.onSliderChanged {
            let volume: Int = Int(self.sdkPlaybackVolumeSlider.slider.intValue)
            LogUtils.log(message: "onPlaybackVolumeChanged \(volume)", level: .info)
            self.agoraKit?.adjustPlaybackSignalVolume(volume)
        }
    }
    
    /**
     --- Device Playout Volume Slider ---
     */
    @IBOutlet weak var firstUserPlaybackVolumeSlider: Slider!
    func initFirstUserPlaybackVolumeSlider() {
        firstUserPlaybackVolumeSlider.label.stringValue = "User Playback Volume".localized
        firstUserPlaybackVolumeSlider.slider.minValue = 0
        firstUserPlaybackVolumeSlider.slider.maxValue = 100
        firstUserPlaybackVolumeSlider.slider.intValue = 50
        setFirstUserPlaybackVolumeSliderEnable()
        firstUserPlaybackVolumeSlider.onSliderChanged {
            let volume: Int32 = Int32(self.firstUserPlaybackVolumeSlider.slider.intValue)
            if self.videos.count > 1 && self.videos[1].uid != nil {
                LogUtils.log(message: "onUserPlayoutVolumeChanged \(volume)", level: .info)
                self.agoraKit?.adjustUserPlaybackSignalVolume(self.videos[1].uid!, volume: volume)
            }
        }
    }
    func setFirstUserPlaybackVolumeSliderEnable() {
        firstUserPlaybackVolumeSlider.isEnabled = videos[1].uid != nil
    }
    
    /**
     --- Channel TextField ---
     */
    @IBOutlet weak var channelField: Input!
    func initChannelField() {
        channelField.label.stringValue = "Channel".localized
        channelField.field.placeholderString = "Channel Name".localized
    }
    
    /**
     --- Button ---
     */
    @IBOutlet weak var joinChannelButton: NSButton!
    func initJoinChannelButton() {
        joinChannelButton.title = isJoined ? "Leave Channel".localized : "Join Channel".localized
    }
    
    @IBOutlet weak var localUserSpeaking: NSTextField!
    @IBOutlet weak var activeSpeaker: NSTextField!
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false {
        didSet {
            channelField.isEnabled = !isJoined
            selectLayoutPicker.isEnabled = !isJoined
            initJoinChannelButton()
        }
    }
    
    // indicate for doing something
    var isProcessing: Bool = false {
        didSet {
            joinChannelButton.isEnabled = !isProcessing
        }
    }
    
    private var storagePath: String {
        let filePath = FileManager.default.urls(for: .downloadsDirectory, in: .userDomainMask).first?.absoluteString
        let programPath = filePath?.components(separatedBy: "/")[4] ?? ""
        let path = "/Users/\(programPath)/Downloads"
        return path
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do view setup here.
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        initSelectAudioProfilePicker()
        initSelectAudioScenarioPicker()
        initSelectMicsPicker()
        initSelectLayoutPicker()
        
        initDeviceRecordingVolumeSlider()
        initSdkRecordingVolumeSlider()
        initDevicePlayoutVolumeSlider()
        initSdkPlaybackVolumeSlider()
        initFirstUserPlaybackVolumeSlider()

        initChannelField()
        initJoinChannelButton()
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
        Container.layoutStream(views: videos)
    }
    
    
    @IBAction func onJoinButtonPressed(_ sender: NSButton) {
        if !isJoined {
            // check configuration
            let channel = channelField.stringValue
            if channel.isEmpty {
                return
            }
            // use selected devices
            guard let micId = selectedMicrophone?.deviceId,
                  let profile = selectedProfile,
                  let scenario = selectedAudioScenario else {
                return
            }
            agoraKit.setDevice(.audioRecording, deviceId: micId)
            // disable video module in audio scene
            agoraKit.disableVideo()
            agoraKit.enableAudio()
            agoraKit.setAudioProfile(profile, scenario: scenario)
            
            // set proxy configuration
//            let proxySetting = GlobalSettings.shared.proxySetting.selectedOption().value
//            agoraKit.setCloudProxy(AgoraCloudProxyType.init(rawValue: UInt(proxySetting)) ?? .noneProxy)
            
            // set live broadcaster mode
            agoraKit.setChannelProfile(.liveBroadcasting)
            // set myself as broadcaster to stream audio
            agoraKit.setClientRole(.broadcaster)
            // enable volume indicator
            agoraKit.enableAudioVolumeIndication(200, smooth: 3, reportVad: true)
            
            // start joining channel
            // 1. Users can only see each other after they join the
            // same channel successfully using the same app id.
            // 2. If app certificate is turned on at dashboard, token is needed
            // when joining channel. The channel name and uid used to calculate
            // the token has to match the ones used for channel join
            isProcessing = true
            let option = AgoraRtcChannelMediaOptions()
            option.publishCameraTrack = true
            option.clientRoleType = .broadcaster
            NetworkManager.shared.generateToken(channelName: channel, success: { token in
                let result = self.agoraKit.joinChannel(byToken: token, channelId: channel, uid: 0, mediaOptions: option)
                if result != 0 {
                    self.isProcessing = false
                    // Usually happens with invalid parameters
                    // Error code description can be found at:
                    // en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
                    // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                    self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
                }
            })
        } else {
            isProcessing = true
            agoraKit.leaveChannel { (stats:AgoraChannelStats) in
                LogUtils.log(message: "Left channel", level: .info)
                self.isProcessing = false
                self.videos[0].uid = nil
                self.isJoined = false
                self.videos.forEach {
                    $0.uid = nil
                    $0.statsLabel.stringValue = ""
                }
            }
        }
    }
    
    override func viewWillBeRemovedFromSplitView() {
        if isJoined {
            agoraKit.leaveChannel { (stats:AgoraChannelStats) in
                LogUtils.log(message: "Left channel", level: .info)
            }
        }
    }
}

/// agora rtc engine delegate events
extension JoinChannelAudioMain: AgoraRtcEngineDelegate {
    /// callback when warning occured for agora sdk, warning can usually be ignored, still it's nice to check out
    /// what is happening
    /// Warning code description can be found at:
    /// en: https://api-ref.agora.io/en/voice-sdk/ios/3.x/Constants/AgoraWarningCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraWarningCode.html
    /// @param warningCode warning code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurWarning warningCode: AgoraWarningCode) {
        LogUtils.log(message: "warning: \(warningCode.rawValue)", level: .warning)
    }
    
    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode)", level: .error)
        self.showAlert(title: "Error", message: "Error \(errorCode.rawValue) occur")
    }
    
    /// callback when the local user joins a specified channel.
    /// @param channel
    /// @param uid uid of local user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinChannel channel: String, withUid uid: UInt, elapsed: Int) {
        isProcessing = false
        isJoined = true
        let localVideo = videos[0]
        localVideo.uid = uid
        let config = AgoraAudioRecordingConfiguration()
        config.filePath = "\(storagePath)/audio.wav"
        config.fileRecordOption = .mic
        config.quality = .high
        config.sampleRate = 44100
//        config.codec = false
        let ret = agoraKit.startAudioRecording(withConfig: config)
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms, recording status \(ret)", level: .info)
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
        setFirstUserPlaybackVolumeSliderEnable()
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
        setFirstUserPlaybackVolumeSliderEnable()
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
