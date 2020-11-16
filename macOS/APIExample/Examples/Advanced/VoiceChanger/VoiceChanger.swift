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

class VoiceChanger: BaseViewController {
    struct VoiceChangerOption {
        var changerVal: AgoraAudioVoiceChanger?
        var reverbVal: AgoraAudioReverbPreset?
        
        init() {}
        
        init(changerVal:AgoraAudioVoiceChanger) {
            self.changerVal = changerVal
        }
        
        init(reverbVal:AgoraAudioReverbPreset) {
            self.reverbVal = reverbVal
        }
        
        func description() -> String {
            if let changerVal = self.changerVal {
                return changerVal.description()
            }
            if let reverbVal = self.reverbVal {
                return reverbVal.description()
            }
            return "Off"
        }
    }
    
    var videos: [VideoView] = []
    
    @IBOutlet weak var container: AGEVideoContainer!
    @IBOutlet weak var channelField: NSTextField!
    @IBOutlet weak var joinBtn: NSButton!
    @IBOutlet weak var leaveBtn: NSButton!
    @IBOutlet weak var micPicker: NSPopUpButton!
    @IBOutlet weak var chatBeautifierPicker: NSPopUpButton!
    @IBOutlet weak var timbreTransformationPicker: NSPopUpButton!
    @IBOutlet weak var voiceChangerPicker: NSPopUpButton!
    @IBOutlet weak var styleTransformationPicker: NSPopUpButton!
    @IBOutlet weak var roomAcousticsPicker: NSPopUpButton!
    @IBOutlet weak var equalizationPitchSlider: NSSlider!
    @IBOutlet weak var equalization31hzPicker: NSSlider!
    @IBOutlet weak var equalization62hzPicker: NSSlider!
    @IBOutlet weak var equalization125hzPicker: NSSlider!
    @IBOutlet weak var equalization250hzPicker: NSSlider!
    @IBOutlet weak var equalization500hzPicker: NSSlider!
    @IBOutlet weak var equalization1khzPicker: NSSlider!
    @IBOutlet weak var equalization2khzPicker: NSSlider!
    @IBOutlet weak var equalization4khzPicker: NSSlider!
    @IBOutlet weak var equalization8khzPicker: NSSlider!
    @IBOutlet weak var equalization16khzPicker: NSSlider!
    @IBOutlet weak var equalizationReverbKeyPicker: NSPopUpButton!
    @IBOutlet weak var equalizationReverbValueSlider: NSSlider!
    @IBOutlet weak var layoutPicker: NSPopUpButton!
    
    var reverbMap:[AgoraAudioReverbType:Int] = [
        .dryLevel:0,
        .wetLevel:0,
        .roomSize:0,
        .wetDelay:0,
        .strength:0
    ]

    let chatBeautifiers:[VoiceChangerOption] = [VoiceChangerOption(), VoiceChangerOption(changerVal:.generalBeautyVoiceFemaleFresh), VoiceChangerOption(changerVal:.generalBeautyVoiceFemaleVitality), VoiceChangerOption(changerVal:.generalBeautyVoiceMaleMagnetic)]
    let timbreTransformations:[VoiceChangerOption] = [VoiceChangerOption(), VoiceChangerOption(changerVal:.voiceBeautyVigorous), VoiceChangerOption(changerVal:.voiceBeautyDeep), VoiceChangerOption(changerVal:.voiceBeautyMellow), VoiceChangerOption(changerVal:.voiceBeautyFalsetto), VoiceChangerOption(changerVal:.voiceBeautyFull), VoiceChangerOption(changerVal:.voiceBeautyClear), VoiceChangerOption(changerVal:.voiceBeautyResounding), VoiceChangerOption(changerVal:.voiceBeautyRinging)]
    let voiceChangers:[VoiceChangerOption] = [VoiceChangerOption(), VoiceChangerOption(changerVal:.voiceChangerOldMan), VoiceChangerOption(changerVal:.voiceChangerBabyBoy), VoiceChangerOption(changerVal:.voiceChangerBabyGirl), VoiceChangerOption(changerVal:.voiceChangerZhuBaJie), VoiceChangerOption(changerVal:.voiceChangerHulk), VoiceChangerOption(reverbVal:.fxUncle), VoiceChangerOption(reverbVal:.fxSister)]
    let styleTransformations:[VoiceChangerOption] = [VoiceChangerOption(), VoiceChangerOption(reverbVal:.fxPopular), VoiceChangerOption(reverbVal:.popular), VoiceChangerOption(reverbVal:.fxRNB), VoiceChangerOption(reverbVal:.rnB), VoiceChangerOption(reverbVal:.rock), VoiceChangerOption(reverbVal:.hipHop)]
    let roomAcoustics:[VoiceChangerOption] = [VoiceChangerOption(), VoiceChangerOption(changerVal:.voiceBeautySpacial), VoiceChangerOption(changerVal:.voiceChangerEthereal), VoiceChangerOption(reverbVal:.fxVocalConcert), VoiceChangerOption(reverbVal:.vocalConcert), VoiceChangerOption(reverbVal:.fxKTV), VoiceChangerOption(reverbVal:.KTV), VoiceChangerOption(reverbVal:.fxStudio), VoiceChangerOption(reverbVal:.studio), VoiceChangerOption(reverbVal:.fxPhonograph)
        //TODO
        /*, VoiceChangerOption(reverbVal:.virtualStereo)*/]
    
    let equalizationFreqs:[AgoraAudioEqualizationBandFrequency] = [.band31,.band62,.band125,.band250,.band500,.band1K,.band2K,.band4K,.band8K,.band16K]
    let equalizationReverbKeys:[AgoraAudioReverbType] = [.dryLevel,.wetLevel,.roomSize,.wetDelay,.strength]
    
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
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false {
        didSet {
            channelField.isEnabled = !isJoined
            joinBtn.isHidden = isJoined
            leaveBtn.isHidden = !isJoined
            layoutPicker.isEnabled = !isJoined
            chatBeautifierPicker.isEnabled = isJoined
            timbreTransformationPicker.isEnabled = isJoined
            voiceChangerPicker.isEnabled = isJoined
            styleTransformationPicker.isEnabled = isJoined
            roomAcousticsPicker.isEnabled = isJoined
            equalizationPitchSlider.isEnabled = isJoined
            equalization31hzPicker.isEnabled = isJoined
            equalization62hzPicker.isEnabled = isJoined
            equalization125hzPicker.isEnabled = isJoined
            equalization250hzPicker.isEnabled = isJoined
            equalization500hzPicker.isEnabled = isJoined
            equalization1khzPicker.isEnabled = isJoined
            equalization2khzPicker.isEnabled = isJoined
            equalization4khzPicker.isEnabled = isJoined
            equalization8khzPicker.isEnabled = isJoined
            equalization16khzPicker.isEnabled = isJoined
            equalizationReverbKeyPicker.isEnabled = isJoined
            equalizationReverbValueSlider.isEnabled = isJoined
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        layoutVideos(2)
        
        micPicker.addItems(withTitles: mics.map({ $0.deviceName ?? ""}))
        chatBeautifierPicker.addItems(withTitles: chatBeautifiers.map({$0.description()}))
        timbreTransformationPicker.addItems(withTitles: timbreTransformations.map({$0.description()}))
        voiceChangerPicker.addItems(withTitles: voiceChangers.map({$0.description()}))
        styleTransformationPicker.addItems(withTitles: styleTransformations.map({$0.description()}))
        roomAcousticsPicker.addItems(withTitles: roomAcoustics.map({$0.description()}))
        equalizationReverbKeyPicker.addItems(withTitles: equalizationReverbKeys.map({$0.description()}))
        
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
        // Before calling the method, you need to set the profile
        // parameter of setAudioProfile to AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)
        // or AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5), and to set
        // scenario parameter to AUDIO_SCENARIO_GAME_STREAMING(3).
        agoraKit.setAudioProfile(.musicHighQualityStereo, scenario: .gameStreaming)
        
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
    
    @IBAction func onChatBeautifier(_ sender: NSPopUpButton) {
        updateVoiceChangerOption(sender: sender, option: chatBeautifiers[sender.indexOfSelectedItem])
    }
    
    @IBAction func onTimbreTransformation(_ sender: NSPopUpButton) {
        updateVoiceChangerOption(sender: sender, option: timbreTransformations[sender.indexOfSelectedItem])
    }
    
    @IBAction func onVoiceChanger(_ sender: NSPopUpButton) {
        updateVoiceChangerOption(sender: sender, option: voiceChangers[sender.indexOfSelectedItem])
    }
    
    @IBAction func onStyleTransformation(_ sender: NSPopUpButton) {
        updateVoiceChangerOption(sender: sender, option: styleTransformations[sender.indexOfSelectedItem])
    }
    
    @IBAction func onRoomAcoustic(_ sender: NSPopUpButton) {
        updateVoiceChangerOption(sender: sender, option: roomAcoustics[sender.indexOfSelectedItem])
    }
    
    @IBAction func onBand31hz(_ sender: NSSlider) {
        updateVoiceBand(frequency: .band31, gain: Int(sender.doubleValue))
    }
    
    @IBAction func onBand62hz(_ sender: NSSlider) {
        updateVoiceBand(frequency: .band62, gain: Int(sender.doubleValue))
    }
    
    @IBAction func onBand125hz(_ sender: NSSlider) {
        updateVoiceBand(frequency: .band125, gain: Int(sender.doubleValue))
    }
    
    @IBAction func onBand250hz(_ sender: NSSlider) {
        updateVoiceBand(frequency: .band250, gain: Int(sender.doubleValue))
    }
    
    @IBAction func onBand500hz(_ sender: NSSlider) {
        updateVoiceBand(frequency: .band500, gain: Int(sender.doubleValue))
    }
    
    @IBAction func onBand1khz(_ sender: NSSlider) {
        updateVoiceBand(frequency: .band1K, gain: Int(sender.doubleValue))
    }
    
    @IBAction func onBand2khz(_ sender: NSSlider) {
        updateVoiceBand(frequency: .band2K, gain: Int(sender.doubleValue))
    }
    
    @IBAction func onBand4khz(_ sender: NSSlider) {
        updateVoiceBand(frequency: .band4K, gain: Int(sender.doubleValue))
    }
    
    @IBAction func onBand8khz(_ sender: NSSlider) {
        updateVoiceBand(frequency: .band8K, gain: Int(sender.doubleValue))
    }
    
    @IBAction func onBand16khz(_ sender: NSSlider) {
        updateVoiceBand(frequency: .band16K, gain: Int(sender.doubleValue))
    }
    
    func updateVoiceBand(frequency:AgoraAudioEqualizationBandFrequency, gain:Int) {
        LogUtils.log(message: "setLocalVoiceEqualization: \(frequency.description()), gain: \(gain)", level: .info)
        agoraKit.setLocalVoiceEqualizationOf(frequency, withGain: gain)
    }
    
    func updateVoiceChangerOption(sender: NSPopUpButton, option: VoiceChangerOption) {
        let pickers = [chatBeautifierPicker, timbreTransformationPicker, voiceChangerPicker, styleTransformationPicker, roomAcousticsPicker]
        
        for picker in pickers {
            // reset all other pickers
            if(picker != sender) {
                picker?.selectItem(at: 0)
            }
        }
        
        if let changerVal = option.changerVal {
            LogUtils.log(message: "setLocalVoiceChanger: \(changerVal.description())", level: .info)
            agoraKit.setLocalVoiceChanger(changerVal)
        } else if let reverbVal = option.reverbVal {
            LogUtils.log(message: "setLocalVoiceReverbPreset: \(reverbVal.description())", level: .info)
            agoraKit.setLocalVoiceReverbPreset(reverbVal)
        } else {
            // turn off if it's an off option
            agoraKit.setLocalVoiceChanger(.voiceChangerOff)
            agoraKit.setLocalVoiceReverbPreset(.off)
        }
    }
    
    @IBAction func onLocalVoicePitch(_ sender:NSSlider) {
        LogUtils.log(message: "onLocalVoicePitch \(sender.doubleValue)", level: .info)
        agoraKit.setLocalVoicePitch(sender.doubleValue)
    }
    
    @IBAction func onLocalVoiceEqualizationReverbKey(_ sender:NSPopUpButton) {
        let reverbType = equalizationReverbKeys[equalizationReverbKeyPicker.indexOfSelectedItem]
        updateReverbValueRange(reverbKey: reverbType)
    }
    
    func updateReverbValueRange(reverbKey:AgoraAudioReverbType) {
        var min:Double = 0, max:Double = 0
        switch reverbKey {
        case .dryLevel:
            min = -20
            max = 10
            break
        case .wetLevel:
            min = -20
            max = 10
            break
        case .roomSize:
            min = 0
            max = 100
            break
        case .wetDelay:
            min = 0
            max = 200
            break
        case .strength:
            min = 0
            max = 100
            break
        default: break
        }
        equalizationReverbValueSlider.minValue = min
        equalizationReverbValueSlider.maxValue = max
        equalizationReverbValueSlider.doubleValue = Double(reverbMap[reverbKey] ?? 0)
    }
    
    @IBAction func onLocalVoiceReverbValue(_ sender:NSSlider) {
        let reverbType = equalizationReverbKeys[equalizationReverbKeyPicker.indexOfSelectedItem]
        let value = Int(sender.doubleValue)
        reverbMap[reverbType] = value
        LogUtils.log(message: "onLocalVoiceReverbValue \(reverbType.description()) \(value)", level: .info)
        agoraKit.setLocalVoiceReverbOf(reverbType, withValue: value)
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
extension VoiceChanger: AgoraRtcEngineDelegate {
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
