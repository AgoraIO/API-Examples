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
    class PickerProps<T> {
        let min: T
        let max: T
        var value: T
        init(min: T, max: T, defaultValue: T) {
            self.min = min
            self.max = max
            self.value = defaultValue
        }
    }
    struct VoiceChangerOption {
        var beautifierPreset: AgoraVoiceBeautifierPreset?
        var effectPreset: AgoraAudioEffectPreset?
        
        init() {}
        
        init(beautifierPreset:AgoraVoiceBeautifierPreset) {
            self.beautifierPreset = beautifierPreset
        }
        
        init(effectPreset:AgoraAudioEffectPreset) {
            self.effectPreset = effectPreset
        }
        
        func description() -> String {
            if let beautifierPreset = self.beautifierPreset {
                return beautifierPreset.description()
            }
            if let effectPreset = self.effectPreset {
                return effectPreset.description()
            }
            return "Off".localized
        }
    }
    
    var videos: [VideoView] = []
    
    @IBOutlet weak var container: AGEVideoContainer!
    
    var agoraKit: AgoraRtcEngineKit!
    
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
    
    var currentAudioEffects:AgoraAudioEffectPreset = .off

    func updateAudioEffectsControls(_ effect: AgoraAudioEffectPreset?) {
        if let _effect = effect {
            currentAudioEffects = _effect
            switch effect {
            case .roomAcous3DVoice:
                updateInput(field: audioEffectParam1Field, isEnable: true, label: "Cycle(0-60)".localized, value: 10)
                updateInput(field: audioEffectParam2Field, isEnable: false)
                audioEffectBtn.isEnabled = true
            case .pitchCorrection:
                updateInput(field: audioEffectParam1Field, isEnable: true, label: "Tonic Mode(1-3)".localized, value: 1)
                updateInput(field: audioEffectParam2Field, isEnable: true, label: "Tonic Pitch(1-12)".localized, value: 4)
                audioEffectBtn.isEnabled = true
            default:
                updateInput(field: audioEffectParam1Field, isEnable: false)
                updateInput(field: audioEffectParam2Field, isEnable: false)
                audioEffectBtn.isEnabled = false
            }
        } else {
            currentAudioEffects = .off
            updateInput(field: audioEffectParam1Field, isEnable: false)
            updateInput(field: audioEffectParam2Field, isEnable: false)
            audioEffectBtn.isEnabled = false
        }
    }
    /**
     --- chat Beautifier Picker ---
     */
    @IBOutlet weak var selectChatBeautifierPicker: Picker!
    let chatBeautifiers: [VoiceChangerOption] = [VoiceChangerOption(), VoiceChangerOption(beautifierPreset:.presetChatBeautifierFresh), VoiceChangerOption(beautifierPreset:.presetChatBeautifierVitality), VoiceChangerOption(beautifierPreset:.presetChatBeautifierMagnetic)]
    var selectedChatBeautifier: VoiceChangerOption? {
        let index = self.selectChatBeautifierPicker.indexOfSelectedItem
        if index >= 0 && index < chatBeautifiers.count {
            return chatBeautifiers[index]
        } else {
            return nil
        }
    }
    func initSelectChatBeautifierPicker() {
        selectChatBeautifierPicker.isEnabled = false
        selectChatBeautifierPicker.label.stringValue = "Chat Beautifier".localized
        selectChatBeautifierPicker.picker.addItems(withTitles: chatBeautifiers.map { $0.description() })
        selectChatBeautifierPicker.onSelectChanged {
            if !self.isJoined {
                return
            }
            guard let option = self.selectedChatBeautifier else { return }
            self.updateVoiceChangerOption(sender: self.selectChatBeautifierPicker.picker, option: option)
        }
    }
    
    /**
     --- Timbre Transformation Picker ---
     */
    @IBOutlet weak var selectTimbreTransformationPicker: Picker!
    let timbreTransformations: [VoiceChangerOption] = [VoiceChangerOption(), VoiceChangerOption(beautifierPreset:.timbreTransformationVigorous), VoiceChangerOption(beautifierPreset:.timbreTransformationDeep), VoiceChangerOption(beautifierPreset:.timbreTransformationMellow), VoiceChangerOption(beautifierPreset:.timbreTransformationFalsetto), VoiceChangerOption(beautifierPreset:.timbreTransformationFull), VoiceChangerOption(beautifierPreset:.timbreTransformationClear), VoiceChangerOption(beautifierPreset:.timbreTransformationResounding), VoiceChangerOption(beautifierPreset:.timbreTransformatRinging)]
    var selectedTimbreTransformation: VoiceChangerOption? {
        let index = self.selectTimbreTransformationPicker.indexOfSelectedItem
        if index >= 0 && index < timbreTransformations.count {
            return timbreTransformations[index]
        } else {
            return nil
        }
    }
    func initSelectTimbreTransformationPicker() {
        selectTimbreTransformationPicker.isEnabled = false
        selectTimbreTransformationPicker.label.stringValue = "Timbre Transformation".localized
        selectTimbreTransformationPicker.picker.addItems(withTitles: timbreTransformations.map { $0.description() })
        selectTimbreTransformationPicker.onSelectChanged {
            if !self.isJoined {
                return
            }
            guard let option = self.selectedTimbreTransformation else { return }
            self.updateVoiceChangerOption(sender: self.selectTimbreTransformationPicker.picker, option: option)
        }
    }

    /**
     --- Voice Changer Picker ---
     */
    @IBOutlet weak var selectVoiceChangerPicker: Picker!
    let voiceChangers: [VoiceChangerOption] = [VoiceChangerOption(), VoiceChangerOption(effectPreset:.voiceChangerEffectOldMan), VoiceChangerOption(effectPreset:.voiceChangerEffectBoy), VoiceChangerOption(effectPreset:.voiceChangerEffectGirl), VoiceChangerOption(effectPreset:.voiceChangerEffectPigKin), VoiceChangerOption(effectPreset:.voiceChangerEffectHulk), VoiceChangerOption(effectPreset:.voiceChangerEffectUncle), VoiceChangerOption(effectPreset:.voiceChangerEffectSister)]
    var selectedVoiceChanger: VoiceChangerOption? {
        let index = self.selectVoiceChangerPicker.indexOfSelectedItem
        if index >= 0 && index < voiceChangers.count {
            return voiceChangers[index]
        } else {
            return nil
        }
    }
    func initSelectVoiceChangerPicker() {
        selectVoiceChangerPicker.isEnabled = false
        selectVoiceChangerPicker.label.stringValue = "Voice Changer".localized
        selectVoiceChangerPicker.picker.addItems(withTitles: voiceChangers.map { $0.description() })
        selectVoiceChangerPicker.onSelectChanged {
            if !self.isJoined {
                return
            }
            guard let option = self.selectedVoiceChanger else { return }
            self.updateVoiceChangerOption(sender: self.selectVoiceChangerPicker.picker, option: option)
        }
    }
    
    /**
     -- style Transformation Picker --
     */
    @IBOutlet weak var selectStyleTransformationPicker: Picker!
    let styleTransformations: [VoiceChangerOption] = [VoiceChangerOption(), VoiceChangerOption(effectPreset:.styleTransformationPopular), VoiceChangerOption(effectPreset:.styleTransformationRnb)]
    var selectedStyleTransformation: VoiceChangerOption? {
        let index = self.selectVoiceChangerPicker.indexOfSelectedItem
        if index >= 0 && index < styleTransformations.count {
            return styleTransformations[index]
        } else {
            return nil
        }
    }
    func initSelectStyleTransformationPicker() {
        selectStyleTransformationPicker.isEnabled = false
        selectStyleTransformationPicker.label.stringValue = "Style Transformation".localized
        selectStyleTransformationPicker.picker.addItems(withTitles: styleTransformations.map { $0.description() })
        selectStyleTransformationPicker.onSelectChanged {
            if !self.isJoined {
                return
            }
            guard let option = self.selectedStyleTransformation else { return }
            self.updateVoiceChangerOption(sender: self.selectStyleTransformationPicker.picker, option: option)
        }
    }
    
    /**
     --- room Acoustics Picker ---
     */
    @IBOutlet weak var selectRoomAcousticsPicker: Picker!
    let roomAcoustics: [VoiceChangerOption] = [VoiceChangerOption(), VoiceChangerOption(effectPreset:.roomAcousSpatial), VoiceChangerOption(effectPreset:.roomAcousEthereal), VoiceChangerOption(effectPreset:.roomAcousVocalConcer), VoiceChangerOption(effectPreset:.roomAcousticsKTV), VoiceChangerOption(effectPreset:.roomAcousStudio), VoiceChangerOption(effectPreset:.roomAcousPhonograph), VoiceChangerOption(effectPreset:.roomAcousVirtualStereo), VoiceChangerOption(effectPreset:.roomAcous3DVoice)]
    var selectedRoomAcoustics: VoiceChangerOption? {
        let index = self.selectRoomAcousticsPicker.indexOfSelectedItem
        if index >= 0 && index < roomAcoustics.count {
            return roomAcoustics[index]
        } else {
            return nil
        }
    }
    func initSelectRoomAcousticsPicker() {
        selectRoomAcousticsPicker.isEnabled = false
        selectRoomAcousticsPicker.label.stringValue = "Room Acoustics".localized
        selectRoomAcousticsPicker.picker.addItems(withTitles: roomAcoustics.map { $0.description() })
        selectRoomAcousticsPicker.onSelectChanged {
            if !self.isJoined {
                return
            }
            guard let option = self.selectedRoomAcoustics else { return }
            self.updateVoiceChangerOption(sender: self.selectRoomAcousticsPicker.picker, option: option)
        }
    }
    
    /**
     --- pitch Correction Picker ---
     */
    @IBOutlet weak var selectPitchCorrectionPicker: Picker!
    let pitchCorrections: [VoiceChangerOption] = [VoiceChangerOption(), VoiceChangerOption(effectPreset:.pitchCorrection)]
    var selectedPitchCorrection: VoiceChangerOption? {
        let index = self.selectPitchCorrectionPicker.indexOfSelectedItem
        if index >= 0 && index < pitchCorrections.count {
            return pitchCorrections[index]
        } else {
            return nil
        }
    }
    func initSelectPitchCorrectionPicker() {
        selectPitchCorrectionPicker.isEnabled = false
        selectPitchCorrectionPicker.label.stringValue = "Pitch Correction".localized
        selectPitchCorrectionPicker.picker.addItems(withTitles: pitchCorrections.map { $0.description() })
        selectPitchCorrectionPicker.onSelectChanged {
            if !self.isJoined {
                return
            }
            guard let option = self.selectedPitchCorrection else { return }
            self.updateVoiceChangerOption(sender: self.selectPitchCorrectionPicker.picker, option: option)
        }
    }
    
    /**
     --- set audio effect button ---
     */
    @IBOutlet weak var audioEffectBtn: NSButton!
    func initAudioEffectButton() {
        audioEffectBtn.title = "Set Audio Effect Params".localized
    }
    @IBAction func onAudioEffectParamsUpdate(_ sender: NSButton) {
        let param1 = audioEffectParam1Field.isEnabled ? audioEffectParam1Field.field.intValue : 0
        let param2 = audioEffectParam2Field.isEnabled ? audioEffectParam2Field.field.intValue : 0
        LogUtils.log(message: "onAudioEffectsParamUpdated \(currentAudioEffects.description()) \(param1) \(param2)", level: .info)
        agoraKit.setAudioEffectParameters(currentAudioEffects, param1: param1, param2: param2)
    }
    
    func updateInput(field: Input, isEnable: Bool, label: String = "N/A", value: Int32 = 0) {
        field.isEnabled = isEnable
        field.label.stringValue = label
        field.field.intValue = value
    }
    /**
     --- audio effice param1 ---
     */
    @IBOutlet weak var audioEffectParam1Field: Input!
    func initAudioEffectParam1Field() {
        updateInput(field: audioEffectParam1Field, isEnable: false)
    }
    
    /**
     --- audio effice param2 ---
     */
    @IBOutlet weak var audioEffectParam2Field: Input!
    func initAudioEffectParam2Field() {
        updateInput(field: audioEffectParam2Field, isEnable: false)
    }
    
    /**
     --- equalization Reverb Key Picker ---
     */
    @IBOutlet weak var equalizationReverbKeyPicker: NSPopUpButton!
    var reverbMap: [AgoraAudioReverbType: PickerProps<Double>] = [
        .dryLevel: PickerProps<Double>(min: -20, max: 10, defaultValue: 0),
        .wetLevel: PickerProps<Double>(min: -20, max: 10, defaultValue: 0),
        .roomSize: PickerProps<Double>(min: 0, max: 100, defaultValue: 0),
        .wetDelay: PickerProps<Double>(min: 0, max: 200, defaultValue: 0),
        .strength: PickerProps<Double>(min: 0, max: 100, defaultValue: 0)
    ]
    let equalizationReverbKeys: [AgoraAudioReverbType] = [.dryLevel, .wetLevel, .roomSize, .wetDelay, .strength]
    var selectedEqualizationReverbKey: AgoraAudioReverbType? {
        let index = self.equalizationReverbKeyPicker.indexOfSelectedItem
        if index >= 0 && index < equalizationReverbKeys.count {
            return equalizationReverbKeys[index]
        } else {
            return nil
        }
    }
    func initEqualizationReverbKeyPicker() {
        equalizationReverbKeyPicker.addItems(withTitles: equalizationReverbKeys.map { $0.description() })
    }
    @IBAction func onLocalVoiceEqualizationReverbKey(_ sender: NSPopUpButton) {
        guard let reverbType = selectedEqualizationReverbKey,
              let props = reverbMap[reverbType] else { return }
        equalizationReverbValueSlider.minValue = props.min
        equalizationReverbValueSlider.maxValue = props.max
        equalizationReverbValueSlider.doubleValue = props.value
    }
    /**
     --- equalizationReverbValue Slider ---
     */
    @IBOutlet weak var equalizationReverbValueSlider: NSSlider!
    @IBAction func onLocalVoiceReverbValue(_ sender:NSSlider) {
        guard let reverbType = selectedEqualizationReverbKey,
              let props = reverbMap[reverbType] else { return }
        let value = Int(sender.doubleValue)
        props.value = Double(sender.intValue)
        LogUtils.log(message: "onLocalVoiceReverbValue \(reverbType.description()) \(value)", level: .info)
        agoraKit.setLocalVoiceReverbOf(reverbType, withValue: value)
    }
    
    /**
     --- Voice Pitch Slider ---
     */
    @IBOutlet weak var voicePitchSlider: Slider!
    func initVoicePitchSlider() {
        voicePitchSlider.isEnabled = false
        voicePitchSlider.label.stringValue = "Voice Pitch".localized
        voicePitchSlider.slider.minValue = 0.5
        voicePitchSlider.slider.maxValue = 2.0
        voicePitchSlider.slider.doubleValue = 1.0
        
        voicePitchSlider.onSliderChanged {
            LogUtils.log(message: "onLocalVoicePitch \(self.voicePitchSlider.slider.doubleValue)", level: .info)
            self.agoraKit.setLocalVoicePitch(self.voicePitchSlider.slider.doubleValue)
        }
    }
    
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
    @IBOutlet weak var joinCHannelButton: NSButton!
    func initJoinChannelButton() {
        joinCHannelButton.title = isJoined ? "Leave Channel".localized : "Join Channel".localized
    }
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false {
        didSet {
            channelField.isEnabled = !isJoined
            selectLayoutPicker.isEnabled = !isJoined
            initJoinChannelButton()
            selectChatBeautifierPicker.isEnabled = isJoined
            selectTimbreTransformationPicker.isEnabled = isJoined
            selectVoiceChangerPicker.isEnabled = isJoined
            selectStyleTransformationPicker.isEnabled = isJoined
            selectRoomAcousticsPicker.isEnabled = isJoined
            selectPitchCorrectionPicker.isEnabled = isJoined
            voicePitchSlider.isEnabled = isJoined
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
            if !isJoined {
                updateAudioEffectsControls(nil)
            }
        }
    }
    
    // indicate for doing something
    var isProcessing: Bool = false {
        didSet {
            joinCHannelButton.isEnabled = !isProcessing
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // set up agora instance when view loaded
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        initSelectMicsPicker()
        initSelectLayoutPicker()
        initSelectChatBeautifierPicker()
        initSelectTimbreTransformationPicker()
        initSelectVoiceChangerPicker()
        initSelectStyleTransformationPicker()
        initSelectRoomAcousticsPicker()
        initSelectPitchCorrectionPicker()
        initAudioEffectParam1Field()
        initAudioEffectParam2Field()
        initAudioEffectButton()
        initEqualizationReverbKeyPicker()
        initVoicePitchSlider()
        
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
            guard let micId = selectedMicrophone?.deviceId else {
                return
            }
            agoraKit.setDevice(.audioRecording, deviceId: micId)
            // disable video module in audio scene
            agoraKit.disableVideo()
            // set proxy configuration
//            let proxySetting = GlobalSettings.shared.proxySetting.selectedOption().value
//            agoraKit.setCloudProxy(AgoraCloudProxyType.init(rawValue: UInt(proxySetting)) ?? .noneProxy)
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
            agoraKit.enableAudioVolumeIndication(200, smooth: 3, reportVad: true)
            
            // start joining channel
            // 1. Users can only see each other after they join the
            // same channel successfully using the same app id.
            // 2. If app certificate is turned on at dashboard, token is needed
            // when joining channel. The channel name and uid used to calculate
            // the token has to match the ones used for channel join
            isProcessing = true
            let option = AgoraRtcChannelMediaOptions()
            option.publishCameraTrack = false
            option.publishCustomAudioTrack = true
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
                LogUtils.log(message: "Left channel", level: .info)
                self.isProcessing = false
                self.videos[0].uid = nil
                self.isJoined = false
            }
        }
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
        let pickers = [
            selectChatBeautifierPicker.picker,
            selectTimbreTransformationPicker.picker,
            selectVoiceChangerPicker.picker,
            selectStyleTransformationPicker.picker,
            selectRoomAcousticsPicker.picker
        ]
        pickers.filter {
            $0 != sender
        }.forEach {
            $0?.selectItem(at: 0)
        }
        
        if let beautifierPreset = option.beautifierPreset {
            LogUtils.log(message: "setVoiceBeautifierPreset: \(beautifierPreset.description())", level: .info)
            agoraKit.setVoiceBeautifierPreset(beautifierPreset)
            updateAudioEffectsControls(nil)
        } else if let effectPreset = option.effectPreset {
            LogUtils.log(message: "setAudioEffectPreset: \(effectPreset.description())", level: .info)
            updateAudioEffectsControls(effectPreset)
            agoraKit.setAudioEffectPreset(effectPreset)
        } else {
            // turn off if it's an off option
            agoraKit.setVoiceBeautifierPreset(.presetOff)
            agoraKit.setAudioEffectPreset(.off)
            updateAudioEffectsControls(nil)
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
}
