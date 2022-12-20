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

class AudioMixing: BaseViewController {
    let EFFECT_ID:Int32 = 1
    let EFFECT_ID_2:Int32 = 2
    var videos: [VideoView] = []
    
    @IBOutlet weak var container: AGEVideoContainer!
    @IBOutlet weak var startAudioMixingBtn: NSButton!
    @IBOutlet weak var pauseAudioMixingBtn: NSButton!
    @IBOutlet weak var resumeAudioMixingBtn: NSButton!
    @IBOutlet weak var stopAudioMixingBtn: NSButton!
    @IBOutlet weak var audioMixingProgress: NSProgressIndicator!
    @IBOutlet weak var audioMixingDuration: NSTextField!
    @IBOutlet weak var enableLoopBackRecordingBtn: NSButton!
    @IBOutlet weak var disableLoopBackRecordingBtn: NSButton!
    
    var agoraKit: AgoraRtcEngineKit!
    var timer:Timer?

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
            self.agoraKit.setAudioProfile(profile, scenario: scenario)
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
            self.agoraKit.setAudioProfile(profile, scenario: scenario)
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
     --- loopback recording volume slider
     */
    @IBOutlet weak var loopBackVolumeSlider: Slider!
    func initLoopBackVolumeSlider() {
        loopBackVolumeSlider.label.stringValue = "Loopback Recording Volume".localized
        mixingVolumeSlider.slider.minValue = 0
        mixingVolumeSlider.slider.maxValue = 100
        mixingVolumeSlider.slider.intValue = 50
        
        loopBackVolumeSlider.onSliderChanged {
            let value: Int = Int(self.loopBackVolumeSlider.slider.intValue)
            LogUtils.log(message: "onLoopBackRecordingVolumeChanged \(value)", level: .info)
            self.agoraKit.adjustLoopbackSignalVolume(value)
        }
    }
    
    /**
     --- mix volume slider
     */
    @IBOutlet weak var mixingVolumeSlider: Slider!
    func initMixingVolumeSlider() {
        mixingVolumeSlider.label.stringValue = "Mixing Volume".localized
        mixingVolumeSlider.slider.minValue = 0
        mixingVolumeSlider.slider.maxValue = 100
        mixingVolumeSlider.slider.intValue = 50
        
        mixingVolumeSlider.onSliderChanged {
            let value: Int = Int(self.mixingVolumeSlider.slider.intValue)
            LogUtils.log(message: "onAudioMixingVolumeChanged \(value)", level: .info)
            self.agoraKit.adjustAudioMixingVolume(value)
        }
    }
    
    /**
     --- Mixing Playback Volume ---
     */
    @IBOutlet weak var mixingPlaybackVolumeSlider: Slider!
    func initMixingPlaybackVolumeSlider() {
        mixingPlaybackVolumeSlider.label.stringValue = "Mixing Playback Volume".localized
        mixingPlaybackVolumeSlider.slider.minValue = 0
        mixingPlaybackVolumeSlider.slider.maxValue = 100
        mixingPlaybackVolumeSlider.slider.intValue = 50
        
        mixingPlaybackVolumeSlider.onSliderChanged {
            let value: Int = Int(self.mixingPlaybackVolumeSlider.slider.intValue)
            LogUtils.log(message: "onAudioMixingPlaybackVolumeChanged \(value)", level: .info)
            self.agoraKit.adjustAudioMixingPlayoutVolume(value)
        }
    }
    
    /**
     --- Mixing Publish Volume ---
     */
    @IBOutlet weak var mixingPublishVolumeSlider: Slider!
    func initMixingPublishVolumeSlider() {
        mixingPublishVolumeSlider.label.stringValue = "Mixing Publish Volume".localized
        mixingPublishVolumeSlider.slider.minValue = 0
        mixingPublishVolumeSlider.slider.maxValue = 100
        mixingPublishVolumeSlider.slider.intValue = 50
        
        mixingPublishVolumeSlider.onSliderChanged {
            let value: Int = Int(self.mixingPublishVolumeSlider.slider.intValue)
            LogUtils.log(message: "onAudioMixingPublishVolumeChanged \(value)", level: .info)
            self.agoraKit.adjustAudioMixingPublishVolume(value)
        }
    }

    /**
     --- effectVolumeSlider ---
     */
    @IBOutlet weak var effectVolumeSlider: Slider!
    func initEffectVolumeSlider() {
        effectVolumeSlider.label.stringValue = "Overall Effect Volume".localized
        effectVolumeSlider.slider.minValue = 0
        effectVolumeSlider.slider.maxValue = 100
        effectVolumeSlider.slider.intValue = 50
        
        effectVolumeSlider.onSliderChanged {
            let value: Double = Double(self.effectVolumeSlider.slider.intValue)
            LogUtils.log(message: "onAudioEffectVolumeChanged \(value)", level: .info)
            self.agoraKit.setEffectsVolume(Int(value))
        }
    }
    @IBOutlet weak var playAudioEffectBtn: NSButton!
    @IBAction func onPlayEffect(_ sender:NSButton){
        if let filepath = Bundle.main.path(forResource: "audioeffect", ofType: "mp3") {
            let result = agoraKit.playEffect(EFFECT_ID, filePath: filepath, loopCount: -1, pitch: 1, pan: 0, gain: 100, publish: true)
            if result != 0 {
                self.showAlert(title: "Error", message: "playEffect call failed: \(result), please check your params")
            }
        }
    }
    @IBOutlet weak var pauseAudioEffectBtn: NSButton!
    @IBAction func onPauseEffect(_ sender:NSButton){
        let result = agoraKit.pauseEffect(EFFECT_ID)
        if result != 0 {
            self.showAlert(title: "Error", message: "pauseEffect call failed: \(result), please check your params")
        }
    }
    @IBOutlet weak var resumeAudioEffectBtn: NSButton!
    @IBAction func onResumeEffect(_ sender:NSButton){
        let result = agoraKit.resumeEffect(EFFECT_ID)
        if result != 0 {
            self.showAlert(title: "Error", message: "resumeEffect call failed: \(result), please check your params")
        }
    }
    @IBOutlet weak var stopAudioEffectBtn: NSButton!
    @IBAction func onStopEffect(_ sender:NSButton){
        let result = agoraKit.stopEffect(EFFECT_ID)
        if result != 0 {
            self.showAlert(title: "Error", message: "stopEffect call failed: \(result), please check your params")
        }
    }
    
    /**
     --- Additional Effect Volume Slider ---
     */
    @IBOutlet weak var additionalEffectVolumeSlider: Slider!
    func initAdditionalEffectVolumeSlider() {
        additionalEffectVolumeSlider.label.stringValue = "Additional Effect Volume".localized
        additionalEffectVolumeSlider.slider.minValue = 0
        additionalEffectVolumeSlider.slider.maxValue = 100
        additionalEffectVolumeSlider.slider.intValue = 50
        
        additionalEffectVolumeSlider.onSliderChanged {
            let value: Double = Double(self.additionalEffectVolumeSlider.slider.intValue)
            LogUtils.log(message: "onAudioEffectVolumeChanged \(value)", level: .info)
            self.agoraKit.setVolumeOfEffect(self.EFFECT_ID_2, withVolume: Int32(value))
        }
    }
    /**
     --- Play Additional Effect Button ---
     */
    @IBOutlet weak var playAdditionalEffectButton: NSButton!
    @IBOutlet weak var stopAdditionalEffectButton: NSButton!
    @IBAction func onPlayEffect2(_ sender:NSButton){
        if let filepath = Bundle.main.path(forResource: "effectA", ofType: "wav") {
            let result = agoraKit.playEffect(EFFECT_ID_2, filePath: filepath, loopCount: -1, pitch: 1, pan: 0, gain: 100, publish: true)
            if result != 0 {
                    self.showAlert(title: "Error", message: "playEffect call failed: \(result), please check your params")
            }
        }
    }
    @IBAction func onStopEffect2(_ sender:NSButton){
        let result = agoraKit.stopEffect(EFFECT_ID_2)
        if result != 0 {
            self.showAlert(title: "Error", message: "stopEffect call failed: \(result), please check your params")
        }
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
        
        initMixingVolumeSlider()
        initMixingPlaybackVolumeSlider()
        initMixingPublishVolumeSlider()
        initAdditionalEffectVolumeSlider()
        initEffectVolumeSlider()
        initLoopBackVolumeSlider()

        initChannelField()
        initJoinChannelButton()
    }
    
    override func viewWillBeRemovedFromSplitView() {
        if isJoined {
            agoraKit.leaveChannel { (stats:AgoraChannelStats) in
                LogUtils.log(message: "Left channel", level: .info)
            }
        }
        AgoraRtcEngineKit.destroy()
    }
    
    @IBAction func onJoinPressed(_ sender:Any) {
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
            // set proxy configuration
//            let proxySetting = GlobalSettings.shared.proxySetting.selectedOption().value
//            agoraKit.setCloudProxy(AgoraCloudProxyType.init(rawValue: UInt(proxySetting)) ?? .noneProxy)
            // disable video module in audio scene
            agoraKit.disableVideo()
            agoraKit.setAudioProfile(profile, scenario: scenario)
            // set live broadcaster mode
            agoraKit.setChannelProfile(.liveBroadcasting)
            // set myself as broadcaster to stream audio
            agoraKit.setClientRole(.broadcaster)
            
            // enable volume indicator
            agoraKit.enableAudioVolumeIndication(200, smooth: 3, reportVad: false)
            
            // update slider values
            mixingPlaybackVolumeSlider.slider.doubleValue = Double(agoraKit.getAudioMixingPlayoutVolume())
            mixingPublishVolumeSlider.slider.doubleValue = Double(agoraKit.getAudioMixingPublishVolume())
            effectVolumeSlider.slider.doubleValue = Double(agoraKit.getEffectsVolume())
            additionalEffectVolumeSlider.slider.doubleValue = Double(agoraKit.getEffectsVolume())
            
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
            agoraKit.leaveChannel { [unowned self] (stats:AgoraChannelStats) in
                self.isProcessing = false
                LogUtils.log(message: "Left channel", level: .info)
                self.videos[0].uid = nil
                self.isJoined = false
                self.videos.forEach {
                    $0.uid = nil
                    $0.statsLabel.stringValue = ""
                }
            }
        }
    }
    
    func startProgressTimer(file: String) {
        // begin timer to update progress
        if timer == nil {
            timer = Timer.scheduledTimer(withTimeInterval: 0.5, repeats: true, block: { [weak self](timer:Timer) in
                guard let weakself = self else {return}
                let progress = Double(weakself.agoraKit.getAudioMixingCurrentPosition()) / Double(weakself.agoraKit.getAudioMixingDuration())
                weakself.audioMixingProgress.doubleValue = progress
                let left = weakself.agoraKit.getAudioMixingDuration() - weakself.agoraKit.getAudioMixingCurrentPosition() + 1
                let seconds = left / 1000
                weakself.audioMixingDuration.stringValue = "\(String(format: "%02d", seconds / 60)) : \(String(format: "%02d", seconds % 60))"
            })
        }
    }
    
    func stopProgressTimer() {
        // stop timer
        if timer != nil {
            timer?.invalidate()
            timer = nil
        }
    }
    
    func updateTotalDuration(reset: Bool, file: String?) {
        if reset {
            audioMixingProgress.doubleValue = 0
            audioMixingDuration.stringValue = "00 : 00"
        } else {
            let duration = agoraKit.getAudioMixingDuration()
            let seconds = duration / 1000
            audioMixingDuration.stringValue = "\(String(format: "%02d", seconds / 60)) : \(String(format: "%02d", seconds % 60))"
        }
    }
    
    @IBAction func onStartAudioMixing(_ sender: NSButton) {
        if let filepath = Bundle.main.path(forResource: "audiomixing", ofType: "mp3") {
            let result = agoraKit.startAudioMixing(filepath, loopback: false, cycle: -1)
            if result != 0 {
                self.showAlert(title: "Error", message: "startAudioMixing call failed: \(result), please check your params")
            }
        }
    }
    
    @IBAction func onStopAudioMixing(_ sender:NSButton){
        let result = agoraKit.stopAudioMixing()
        if result != 0 {
            self.showAlert(title: "Error", message: "stopAudioMixing call failed: \(result), please check your params")
        } else {
            stopProgressTimer()
            updateTotalDuration(reset: true, file: nil)
        }
    }
    
    @IBAction func onPauseAudioMixing(_ sender:NSButton){
        let result = agoraKit.pauseAudioMixing()
        if result != 0 {
            self.showAlert(title: "Error", message: "pauseAudioMixing call failed: \(result), please check your params")
        } else {
            stopProgressTimer()
        }
    }
    
    @IBAction func onResumeAudioMixing(_ sender:NSButton){
        let result = agoraKit.resumeAudioMixing()
        if result != 0 {
            self.showAlert(title: "Error", message: "resumeAudioMixing call failed: \(result), please check your params")
        } else {
            guard let filepath = Bundle.main.path(forResource: "audiomixing", ofType: "mp3") else {
                return
            }
            startProgressTimer(file: filepath)
        }
    }
    
    @IBAction func onStartLoopBackRecording(_ sender:NSButton){
        guard let micName = selectedMicrophone?.deviceName else {return}
        self.agoraKit.enableLoopbackRecording(true, deviceName: micName)
    }
    
    @IBAction func onStopLoopBackRecording(_ sender:NSButton){
        guard let micName = selectedMicrophone?.deviceName else {return}
        self.agoraKit.enableLoopbackRecording(false, deviceName: micName)
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
extension AudioMixing: AgoraRtcEngineDelegate {
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
        if isProcessing {
            isProcessing = false
        }
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
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
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
    
    func rtcEngine(_ engine: AgoraRtcEngineKit, audioMixingStateChanged state: AgoraAudioMixingStateType, reasonCode: AgoraAudioMixingReasonCode) {
        LogUtils.log(message: "audioMixingStateChanged \(state.rawValue), code: \(reasonCode.rawValue)", level: .info)
        if state == .playing {
            if let filepath = Bundle.main.path(forResource: "audiomixing", ofType: "mp3") {
                startProgressTimer(file: filepath)
                updateTotalDuration(reset: false, file: filepath)
            }
        }
    }
}
