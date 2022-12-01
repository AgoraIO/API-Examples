//
//  VoiceChanger.swift
//  APIExample
//
//  Created by 张乾泽 on 2020/7/24.
//  Copyright © 2020 Agora Corp. All rights reserved.
//

import Foundation
import UIKit
import AgoraRtcKit
import AGEVideoLayout


class VoiceChangerEntry : UIViewController
{
    @IBOutlet weak var joinButton: AGButton!
    @IBOutlet weak var channelTextField: AGTextField!
    let identifier = "VoiceChanger"
    
    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    @IBAction func doJoinPressed(sender: AGButton) {
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

class VoiceChangerMain: BaseViewController {
    var agoraKit: AgoraRtcEngineKit!
    @IBOutlet weak var chatBeautifierBtn: UIButton!
    @IBOutlet weak var timbreTransformationBtn: UIButton!
    @IBOutlet weak var voiceChangerBtn: UIButton!
    @IBOutlet weak var styleTransformationBtn: UIButton!
    @IBOutlet weak var roomAcousticsBtn: UIButton!
    @IBOutlet weak var pitchCorrectionBtn: UIButton!
    @IBOutlet weak var voiceConversionBtn: UIButton!
    @IBOutlet weak var equalizationFreqBtn: UIButton!
    @IBOutlet weak var reverbKeyBtn: UIButton!
    @IBOutlet weak var reverbValueSlider: UISlider!
    @IBOutlet weak var audioEffectParam1Slider: UISlider!
    @IBOutlet weak var audioEffectParam2Slider: UISlider!
    @IBOutlet weak var audioEffectParam1Label: UILabel!
    @IBOutlet weak var audioEffectParam2Label: UILabel!
    @IBOutlet weak var container: AGEVideoContainer!
    var audioViews: [UInt:VideoView] = [:]
    var equalizationFreq: AgoraAudioEqualizationBandFrequency = .band31
    var equalizationGain: Int = 0
    var reverbType: AgoraAudioReverbType = .dryLevel
    var reverbMap:[AgoraAudioReverbType:Int] = [
        .dryLevel:0,
        .wetLevel:0,
        .roomSize:0,
        .wetDelay:0,
        .strength:0
    ]
    var currentAudioEffects:AgoraAudioEffectPreset = .off

    // indicate if current instance has joined channel
    var isJoined: Bool = false
    
    func resetVoiceChanger() {
        chatBeautifierBtn.setTitle("Off", for: .normal)
        timbreTransformationBtn.setTitle("Off", for: .normal)
        voiceChangerBtn.setTitle("Off", for: .normal)
        styleTransformationBtn.setTitle("Off", for: .normal)
        roomAcousticsBtn.setTitle("Off", for: .normal)
        pitchCorrectionBtn.setTitle("Off", for: .normal)
        voiceConversionBtn.setTitle("Off", for: .normal)
    }

    func updateAudioEffectsControls(_ effect:AgoraAudioEffectPreset) {
        currentAudioEffects = effect
        if(effect == .roomAcous3DVoice) {
            audioEffectParam1Slider.isEnabled = true
            audioEffectParam2Slider.isEnabled = false
            audioEffectParam1Label.text = "Cycle"
            audioEffectParam2Label.text = "N/A"
            audioEffectParam1Slider.minimumValue = 0
            audioEffectParam1Slider.maximumValue = 60
            audioEffectParam1Slider.value = 10
        } else if(effect == .pitchCorrection) {
            audioEffectParam1Slider.isEnabled = true
            audioEffectParam2Slider.isEnabled = true
            audioEffectParam1Label.text = "Tonic Mode"
            audioEffectParam2Label.text = "Tonic Pitch"

            audioEffectParam1Slider.minimumValue = 1
            audioEffectParam1Slider.maximumValue = 3
            audioEffectParam1Slider.value = 1
            audioEffectParam2Slider.minimumValue = 1
            audioEffectParam2Slider.maximumValue = 12
            audioEffectParam2Slider.value = 4
        } else {
            audioEffectParam1Slider.isEnabled = false
            audioEffectParam2Slider.isEnabled = false
            audioEffectParam1Label.text = "N/A"
            audioEffectParam2Label.text = "N/A"
        }
    }
    
    func getChatBeautifierAction(_ chatBeautifier:AgoraVoiceBeautifierPreset) -> UIAlertAction{
        return UIAlertAction(title: "\(chatBeautifier.description())", style: .default, handler: {[unowned self] action in
            self.resetVoiceChanger()
            self.updateAudioEffectsControls(.off)
            //when using this method with setLocalVoiceReverbPreset,
            //the method called later overrides the one called earlier
            self.agoraKit.setVoiceBeautifierPreset(chatBeautifier)
            self.chatBeautifierBtn.setTitle("\(chatBeautifier.description())", for: .normal)
        })
    }
    
    func getTimbreTransformationAction(_ timbreTransformation:AgoraVoiceBeautifierPreset) -> UIAlertAction{
        return UIAlertAction(title: "\(timbreTransformation.description())", style: .default, handler: {[unowned self] action in
            self.resetVoiceChanger()
            self.updateAudioEffectsControls(.off)
            //when using this method with setLocalVoiceReverbPreset,
            //the method called later overrides the one called earlier
            self.agoraKit.setVoiceBeautifierPreset(timbreTransformation)
            self.timbreTransformationBtn.setTitle("\(timbreTransformation.description())", for: .normal)
        })
    }
    
    func getVoiceChangerAction(_ voiceChanger:AgoraAudioEffectPreset) -> UIAlertAction{
        return UIAlertAction(title: "\(voiceChanger.description())", style: .default, handler: {[unowned self] action in
            self.resetVoiceChanger()
            self.updateAudioEffectsControls(voiceChanger)
            //when using this method with setLocalVoiceReverbPreset,
            //the method called later overrides the one called earlier
            self.agoraKit.setAudioEffectPreset(voiceChanger)
            self.voiceChangerBtn.setTitle("\(voiceChanger.description())", for: .normal)
        })
    }
    
    func getStyleTransformationAction(_ styleTransformation:AgoraAudioEffectPreset) -> UIAlertAction{
        return UIAlertAction(title: "\(styleTransformation.description())", style: .default, handler: {[unowned self] action in
            self.resetVoiceChanger()
            self.updateAudioEffectsControls(styleTransformation)
            //when using this method with setLocalVoiceChanger,
            //the method called later overrides the one called earlier
            self.agoraKit.setAudioEffectPreset(styleTransformation)
            self.styleTransformationBtn.setTitle("\(styleTransformation.description())", for: .normal)
        })
    }
    
    func getRoomAcousticsAction(_ roomAcoustics:AgoraAudioEffectPreset) -> UIAlertAction{
        return UIAlertAction(title: "\(roomAcoustics.description())", style: .default, handler: {[unowned self] action in
            self.resetVoiceChanger()
            self.updateAudioEffectsControls(roomAcoustics)
            //when using this method with setLocalVoiceReverbPreset,
            //the method called later overrides the one called earlier
            self.agoraKit.setAudioEffectPreset(roomAcoustics)
            self.roomAcousticsBtn.setTitle("\(roomAcoustics.description())", for: .normal)
        })
    }
    
    func getPitchCorrectionAction(_ pitchCorrection:AgoraAudioEffectPreset) -> UIAlertAction{
        return UIAlertAction(title: "\(pitchCorrection.description())", style: .default, handler: {[unowned self] action in
            self.resetVoiceChanger()
            self.updateAudioEffectsControls(pitchCorrection)
            //when using this method with setLocalVoiceReverbPreset,
            //the method called later overrides the one called earlier
            self.agoraKit.setAudioEffectPreset(pitchCorrection)
            self.pitchCorrectionBtn.setTitle("\(pitchCorrection.description())", for: .normal)
        })
    }

    func getVoiceConversionAction(_ voiceConversion:AgoraVoiceConversionPreset) -> UIAlertAction{
        return UIAlertAction(title: "\(voiceConversion.description())", style: .default, handler: {[unowned self] action in
            self.resetVoiceChanger()
            self.updateAudioEffectsControls(.off)
            self.agoraKit.setVoiceConversionPreset(voiceConversion)
            self.voiceConversionBtn.setTitle("\(voiceConversion.description())", for: .normal)
        })
    }
    
    func getEqualizationFreqAction(_ freq:AgoraAudioEqualizationBandFrequency) -> UIAlertAction {
        return UIAlertAction(title: "\(freq.description())", style: .default, handler: {[unowned self] action in
            self.equalizationFreq = freq
            self.equalizationFreqBtn.setTitle("\(freq.description())", for: .normal)
            LogUtils.log(message: "onLocalVoiceEqualizationGain \(self.equalizationFreq.description()) \(self.equalizationGain)", level: .info)
            self.agoraKit.setLocalVoiceEqualizationOf(self.equalizationFreq, withGain: self.equalizationGain)
        })
    }
    
    func getReverbKeyAction(_ reverbType:AgoraAudioReverbType) -> UIAlertAction {
        return UIAlertAction(title: "\(reverbType.description())", style: .default, handler: {[unowned self] action in
            self.updateReverbValueRange(reverbKey: reverbType)
            self.reverbKeyBtn.setTitle("\(reverbType.description())", for: .normal)
        })
    }
    
    /// callback when voice changer button hit
    @IBAction func onChatBeautifier() {
        let alert = UIAlertController(title: "Set Chat Beautifier".localized, message: nil, preferredStyle: UIDevice.current.userInterfaceIdiom == .pad ? UIAlertController.Style.alert : UIAlertController.Style.actionSheet)
        alert.addAction(getChatBeautifierAction(.presetOff))
        alert.addAction(getChatBeautifierAction(.presetChatBeautifierFresh))
        alert.addAction(getChatBeautifierAction(.presetChatBeautifierVitality))
        alert.addAction(getChatBeautifierAction(.presetChatBeautifierMagnetic))
        alert.addCancelAction()
        present(alert, animated: true, completion: nil)
    }
    
    /// callback when voice changer button hit
    @IBAction func onTimbreTransformation() {
        let alert = UIAlertController(title: "Set Timbre Transformation".localized, message: nil, preferredStyle: UIDevice.current.userInterfaceIdiom == .pad ? UIAlertController.Style.alert : UIAlertController.Style.actionSheet)
        alert.addAction(getTimbreTransformationAction(.presetOff))
        alert.addAction(getTimbreTransformationAction(.timbreTransformationVigorous))
        alert.addAction(getTimbreTransformationAction(.timbreTransformationDeep))
        alert.addAction(getTimbreTransformationAction(.timbreTransformationMellow))
        alert.addAction(getTimbreTransformationAction(.timbreTransformationFalsetto))
        alert.addAction(getTimbreTransformationAction(.timbreTransformationFull))
        alert.addAction(getTimbreTransformationAction(.timbreTransformationClear))
        alert.addAction(getTimbreTransformationAction(.timbreTransformationResounding))
        alert.addAction(getTimbreTransformationAction(.timbreTransformatRinging))
        alert.addCancelAction()
        present(alert, animated: true, completion: nil)
    }
    
    /// callback when voice changer button hit
    @IBAction func onVoiceChanger() {
        let alert = UIAlertController(title: "Set Voice Changer".localized, message: nil, preferredStyle: UIDevice.current.userInterfaceIdiom == .pad ? UIAlertController.Style.alert : UIAlertController.Style.actionSheet)
        alert.addAction(getVoiceChangerAction(.off))
        alert.addAction(getVoiceChangerAction(.voiceChangerEffectUncle))
        alert.addAction(getVoiceChangerAction(.voiceChangerEffectOldMan))
        alert.addAction(getVoiceChangerAction(.voiceChangerEffectBoy))
        alert.addAction(getVoiceChangerAction(.voiceChangerEffectSister))
        alert.addAction(getVoiceChangerAction(.voiceChangerEffectGirl))
        alert.addAction(getVoiceChangerAction(.voiceChangerEffectPigKin))
        alert.addAction(getVoiceChangerAction(.voiceChangerEffectHulk))
        alert.addCancelAction()
        present(alert, animated: true, completion: nil)
    }
    
    /// callback when voice changer button hit
    @IBAction func onStyleTransformation() {
        let alert = UIAlertController(title: "Set Style Transformation".localized, message: nil, preferredStyle: UIDevice.current.userInterfaceIdiom == .pad ? UIAlertController.Style.alert : UIAlertController.Style.actionSheet)
        alert.addAction(getStyleTransformationAction(.off))
        alert.addAction(getStyleTransformationAction(.styleTransformationPopular))
        alert.addAction(getStyleTransformationAction(.styleTransformationRnb))
        alert.addCancelAction()
        present(alert, animated: true, completion: nil)
    }
    
    /// callback when voice changer button hit
    @IBAction func onRoomAcoustics() {
        let alert = UIAlertController(title: "Set Room Acoustics".localized, message: nil, preferredStyle: UIDevice.current.userInterfaceIdiom == .pad ? UIAlertController.Style.alert : UIAlertController.Style.actionSheet)
        alert.addAction(getRoomAcousticsAction(.roomAcousticsKTV))
        alert.addAction(getRoomAcousticsAction(.roomAcousVocalConcer))
        alert.addAction(getRoomAcousticsAction(.roomAcousStudio))
        alert.addAction(getRoomAcousticsAction(.roomAcousPhonograph))
        alert.addAction(getRoomAcousticsAction(.roomAcousVirtualStereo))
        alert.addAction(getRoomAcousticsAction(.roomAcousSpatial))
        alert.addAction(getRoomAcousticsAction(.roomAcousEthereal))
        alert.addAction(getRoomAcousticsAction(.roomAcous3DVoice))
        alert.addCancelAction()
        present(alert, animated: true, completion: nil)
    }

    /// callback when voice changer button hit
    @IBAction func onPitchCorrection() {
        let alert = UIAlertController(title: "Set Pitch Correction".localized, message: nil, preferredStyle: UIDevice.current.userInterfaceIdiom == .pad ? UIAlertController.Style.alert : UIAlertController.Style.actionSheet)
        alert.addAction(getPitchCorrectionAction(.pitchCorrection))
        alert.addCancelAction()
        present(alert, animated: true, completion: nil)
    }
    
    @IBAction func onVoiceConversion(_ sender: Any) {
        let alert = UIAlertController(title: "Set Voice Conversion".localized, message: nil, preferredStyle: UIDevice.current.userInterfaceIdiom == .pad ? UIAlertController.Style.alert : UIAlertController.Style.actionSheet)
        alert.addAction(getVoiceConversionAction(.off))
        alert.addAction(getVoiceConversionAction(.neutral))
        alert.addAction(getVoiceConversionAction(.sweet))
        alert.addAction(getVoiceConversionAction(.changerSolid))
        alert.addAction(getVoiceConversionAction(.changerBass))
        alert.addCancelAction()
        present(alert, animated: true, completion: nil)
    }

    @IBAction func onAudioEffectsParamUpdated(_ sender: UISlider) {
        let param1 = audioEffectParam1Slider.isEnabled ? Int32(audioEffectParam1Slider.value) : 0
        let param2 = audioEffectParam2Slider.isEnabled ? Int32(audioEffectParam2Slider.value) : 0
        LogUtils.log(message: "onAudioEffectsParamUpdated \(currentAudioEffects.description()) \(param1) \(param2)", level: .info)
        agoraKit.setAudioEffectParameters(currentAudioEffects, param1: param1, param2: param2)
    }

    @IBAction func onLocalVoicePitch(_ sender:UISlider) {
        LogUtils.log(message: "onLocalVoicePitch \(Double(sender.value))", level: .info)
        agoraKit.setLocalVoicePitch(Double(sender.value))
    }
    
    @IBAction func onLocalVoiceEqualizaitonFreq(_ sender:UIButton) {
        let alert = UIAlertController(title: "Set Band Frequency".localized, message: nil, preferredStyle: UIDevice.current.userInterfaceIdiom == .pad ? UIAlertController.Style.alert : UIAlertController.Style.actionSheet)
        alert.addAction(getEqualizationFreqAction(.band31))
        alert.addAction(getEqualizationFreqAction(.band62))
        alert.addAction(getEqualizationFreqAction(.band125))
        alert.addAction(getEqualizationFreqAction(.band250))
        alert.addAction(getEqualizationFreqAction(.band500))
        alert.addAction(getEqualizationFreqAction(.band1K))
        alert.addAction(getEqualizationFreqAction(.band2K))
        alert.addAction(getEqualizationFreqAction(.band4K))
        alert.addAction(getEqualizationFreqAction(.band8K))
        alert.addAction(getEqualizationFreqAction(.band16K))
        alert.addCancelAction()
        present(alert, animated: true, completion: nil)
    }
    
    @IBAction func onLocalVoiceEqualizationGain(_ sender:UISlider) {
        equalizationGain = Int(sender.value)
        LogUtils.log(message: "onLocalVoiceEqualizationGain \(equalizationFreq.description()) \(equalizationGain)", level: .info)
        agoraKit.setLocalVoiceEqualizationOf(equalizationFreq, withGain: equalizationGain)
    }
    
    func updateReverbValueRange(reverbKey:AgoraAudioReverbType) {
        var min:Float = 0, max:Float = 0
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
        reverbValueSlider.minimumValue = min
        reverbValueSlider.maximumValue = max
        reverbValueSlider.value = Float(reverbMap[reverbType] ?? 0)
    }
    
    @IBAction func onLocalVoiceReverbKey(_ sender:UIButton) {
        let alert = UIAlertController(title: "Set Reverb Key".localized, message: nil, preferredStyle: UIDevice.current.userInterfaceIdiom == .pad ? UIAlertController.Style.alert : UIAlertController.Style.actionSheet)
        alert.addAction(getReverbKeyAction(.dryLevel))
        alert.addAction(getReverbKeyAction(.wetLevel))
        alert.addAction(getReverbKeyAction(.roomSize))
        alert.addAction(getReverbKeyAction(.wetDelay))
        alert.addAction(getReverbKeyAction(.strength))
        alert.addCancelAction()
        present(alert, animated: true, completion: nil)
    }
    
    @IBAction func onLocalVoiceReverbValue(_ sender:UISlider) {
        let value = Int(sender.value)
        reverbMap[reverbType] = value
        LogUtils.log(message: "onLocalVoiceReverbValue \(reverbType.description()) \(value)", level: .info)
        agoraKit.setLocalVoiceReverbOf(reverbType, withValue: value)
    }
    
    override func viewDidLoad(){
        super.viewDidLoad()
        
        // set up agora instance when view loadedlet config = AgoraRtcEngineConfig()
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
//        config.areaCode = GlobalSettings.shared.area.rawValue
        // setup log file path
        let logConfig = AgoraLogConfig()
        logConfig.filePath = LogUtils.sdkLogPath()
        config.logConfig = logConfig

        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        // Configuring Privatization Parameters
        Util.configPrivatization(agoraKit: agoraKit)
        
        guard let channelName = configs["channelName"] as? String else {return}
        self.title = channelName
        
        // reset voice changer options
        resetVoiceChanger()
        equalizationFreqBtn.setTitle("\(equalizationFreq.description())", for: .normal)
        reverbKeyBtn.setTitle("\(reverbType.description())", for: .normal)
        
        // Before calling the method, you need to set the profile
        // parameter of setAudioProfile to AUDIO_PROFILE_MUSIC_HIGH_QUALITY(4)
        // or AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO(5), and to set
        // scenario parameter to AUDIO_SCENARIO_GAME_STREAMING(3).
        agoraKit.setAudioProfile(.musicHighQualityStereo, scenario: .gameStreaming)
        
        // make myself a broadcaster
        agoraKit.setChannelProfile(.liveBroadcasting)
        agoraKit.setClientRole(GlobalSettings.shared.getUserRole())
        
        // disable video module
        agoraKit.disableVideo()
        agoraKit.enableAudio()
        
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        NetworkManager.shared.generateToken(channelName: channelName, success: { token in
            let result = self.agoraKit.joinChannel(byToken: token,
                                                   channelId: channelName,
                                                   info: nil,
                                                   uid: 0) {[unowned self] (channel, uid, elapsed) -> Void in
                self.isJoined = true
                LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
            
                //set up local audio view, this view will not show video but just a placeholder
                let view = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
                self.audioViews[uid] = view
                view.setPlaceholder(text: self.getAudioLabel(uid: uid, isLocal: true))
                self.container.layoutStream2x1(views: Array(self.audioViews.values))
            }
            if result != 0 {
                // Usually happens with invalid parameters
                // Error code description can be found at:
                // en: https://api-ref.agora.io/en/voice-sdk/macos/3.x/Constants/AgoraErrorCode.html#content
                // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
                self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
            }
        })
    }
    
    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            // leave channel when exiting the view
            if isJoined {
                agoraKit.disableAudio()
                agoraKit.disableVideo()
                agoraKit.leaveChannel { (stats) -> Void in
                    LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
                }
            }
        }
    }
}

/// agora rtc engine delegate events
extension VoiceChangerMain: AgoraRtcEngineDelegate {
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
        isJoined = true
        LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)

        //set up local audio view, this view will not show video but just a placeholder
        let view = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
        audioViews[uid] = view
        view.setPlaceholder(text: self.getAudioLabel(uid: uid, isLocal: true))
        container.layoutStream2x1(views: Array(self.audioViews.values))
    }

    /// callback when a remote user is joinning the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param elapsed time elapse since current sdk instance join the channel in ms
    func rtcEngine(_ engine: AgoraRtcEngineKit, didJoinedOfUid uid: UInt, elapsed: Int) {
        LogUtils.log(message: "remote user join: \(uid) \(elapsed)ms", level: .info)
        
        //set up remote audio view, this view will not show video but just a placeholder
        let view = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
        self.audioViews[uid] = view
        view.setPlaceholder(text: self.getAudioLabel(uid: uid, isLocal: false))
        self.container.layoutStream2x1(views: Array(self.audioViews.values))
        self.container.reload(level: 0, animated: true)
    }
    
    /// callback when a remote user is leaving the channel, note audience in live broadcast mode will NOT trigger this event
    /// @param uid uid of remote joined user
    /// @param reason reason why this user left, note this event may be triggered when the remote user
    /// become an audience in live broadcasting profile
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOfflineOfUid uid: UInt, reason: AgoraUserOfflineReason) {
        LogUtils.log(message: "remote user left: \(uid) reason \(reason)", level: .info)
        
        //remove remote audio view
        self.audioViews.removeValue(forKey: uid)
        self.container.layoutStream2x1(views: Array(self.audioViews.values))
        self.container.reload(level: 0, animated: true)
    }
}
