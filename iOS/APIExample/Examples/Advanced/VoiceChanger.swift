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
import PopMenu
import AGEVideoLayout

struct VoiceChangerItem{
    var title: String
    var value: AgoraAudioVoiceChanger
}

struct VoiceReverbItem{
    var title: String
    var value: AgoraAudioReverbPreset
}

class VoiceChanger: BaseViewController {
    var agoraKit: AgoraRtcEngineKit!
    @IBOutlet weak var voiceChanger: UIButton!
    @IBOutlet weak var voiceBeauty: UIButton!
    @IBOutlet weak var reverb: UIButton!
    @IBOutlet weak var container: AGEVideoContainer!
    var audioViews: [UInt:VideoView] = [:]
    
    var voiceChangeItems:[VoiceChangerItem] = [
        VoiceChangerItem(title: "Off", value: .voiceChangerOff),
        VoiceChangerItem(title: "Old Man", value: .voiceChangerOldMan),
        VoiceChangerItem(title: "Baby Boy", value: .voiceChangerBabyBoy),
        VoiceChangerItem(title: "Baby Girl", value: .voiceChangerBabyGirl),
        VoiceChangerItem(title: "Zhu Ba Jie", value: .voiceChangerZhuBaJie),
        VoiceChangerItem(title: "Ethereal", value: .voiceChangerEthereal),
        VoiceChangerItem(title: "Hulk", value: .voiceChangerHulk)
    ]
    
    var voiceBeautyItems:[VoiceChangerItem] = [
        VoiceChangerItem(title: "Vigorous", value: .voiceBeautyVigorous),
        VoiceChangerItem(title: "Deep", value: .voiceBeautyDeep),
        VoiceChangerItem(title: "Mellow", value: .voiceBeautyMellow),
        VoiceChangerItem(title: "Falsetto", value: .voiceBeautyFalsetto),
        VoiceChangerItem(title: "Full", value: .voiceBeautyFull),
        VoiceChangerItem(title: "Clear", value: .voiceBeautyClear),
        VoiceChangerItem(title: "Resounding", value: .voiceBeautyResounding),
        VoiceChangerItem(title: "Ringing", value: .voiceBeautyRinging),
        VoiceChangerItem(title: "Spacial", value: .voiceBeautySpacial),
        VoiceChangerItem(title: "Male Magnetic", value: .generalBeautyVoiceMaleMagnetic),
        VoiceChangerItem(title: "Female Fresh", value: .generalBeautyVoiceFemaleFresh),
        VoiceChangerItem(title: "Female Vitality", value: .generalBeautyVoiceFemaleVitality)
    ]
    
    var reverbItems:[VoiceReverbItem] = [
        VoiceReverbItem(title: "Popular", value: .popular),
        VoiceReverbItem(title: "RNB", value: .rnB),
        VoiceReverbItem(title: "Rock", value: .rock),
        VoiceReverbItem(title: "HipHop", value: .hipHop),
        VoiceReverbItem(title: "Vocal Concert", value: .vocalConcert),
        VoiceReverbItem(title: "KTV", value: .KTV),
        VoiceReverbItem(title: "Studio", value: .studio),
        VoiceReverbItem(title: "fx KTV", value: .fxKTV),
        VoiceReverbItem(title: "fx Vocal Concert", value: .fxVocalConcert),
        VoiceReverbItem(title: "fx Uncle", value: .fxUncle),
        VoiceReverbItem(title: "fx Sister", value: .fxSister),
        VoiceReverbItem(title: "fx Studio", value: .fxStudio),
        VoiceReverbItem(title: "fx Popular", value: .fxPopular),
        VoiceReverbItem(title: "fx RNB", value: .fxRNB),
        VoiceReverbItem(title: "fx Phonograph", value: .fxPhonograph),
        VoiceReverbItem(title: "fx Virtual Stereo", value: .virtualStereo)
    ]
    
    // indicate if current instance has joined channel
    var isJoined: Bool = false
    
    /// callback when voice changer button hit
    @IBAction func onVoiceChanger() {
        // create a list of voice changer options from voice changer defs
        let actions = voiceChangeItems.map { (item:VoiceChangerItem) -> PopMenuAGAction in
            let action = PopMenuAGAction(title: item.title, didSelect: {[unowned self]select in
                guard let action:PopMenuAGAction = select as? PopMenuAGAction, let val =
                action.value as? AgoraAudioVoiceChanger else {return}
                
                let result = self.agoraKit.setLocalVoiceChanger(val)
                LogUtils.log(message: "setLocalVoiceChanger \(val), result: \(result)", level: .info)
                if(result < 0) {
                    self.showAlert(message: "setLocalVoiceChanger failed: \(result)")
                }
            })
            action.value = item.value as AnyObject
            return action
        }
        self.getPrompt(actions: actions).present(sourceView: voiceChanger)
    }
    
    /// callback when voice beauty button hit
    @IBAction func onVoiceBeauty() {
        // create a list of voice beauty options from voice beauty defs
        let actions = voiceBeautyItems.map { (item:VoiceChangerItem) -> PopMenuAGAction in
            let action = PopMenuAGAction(title: item.title, didSelect: {[unowned self]select in
                guard let action:PopMenuAGAction = select as? PopMenuAGAction, let val =
                action.value as? AgoraAudioVoiceChanger else {return}
                
                let result = self.agoraKit.setLocalVoiceChanger(val)
                LogUtils.log(message: "setLocalVoiceChanger \(val), result: \(result)", level: .info)
                if(result < 0) {
                    self.showAlert(message: "setLocalVoiceChanger failed: \(result)")
                }
            })
            action.value = item.value as AnyObject
            return action
        }
        self.getPrompt(actions: actions).present(sourceView: voiceBeauty)
    }
    
    /// callback when reverb button hit
    @IBAction func onReverb() {
        // create a list of voice reverb options from voice reverb defs
        let actions = reverbItems.map { (item:VoiceReverbItem) -> PopMenuAGAction in
            let action = PopMenuAGAction(title: item.title, didSelect: {[unowned self]select in
                guard let action:PopMenuAGAction = select as? PopMenuAGAction, let val =
                action.value as? AgoraAudioReverbPreset else {return}
                
                let result = self.agoraKit.setLocalVoiceReverbPreset(val)
                LogUtils.log(message: "setLocalVoiceReverbPreset \(val), result: \(result)", level: .info)
                if(result < 0) {
                    self.showAlert(message: "setLocalVoiceReverbPreset failed: \(result)")
                }
            })
            action.value = item.value as AnyObject
            return action
        }
        self.getPrompt(actions: actions).present(sourceView: reverb)
    }
    
    /// callback when customize button hit
    @IBAction func onCustomize() {
        let storyBoard: UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
        
        guard let settings = storyBoard.instantiateViewController(withIdentifier: "settings") as? SettingsViewController else { return }
        settings.sectionNames = ["Voice Pitch"]
        settings.sections = [
            [
                SettingsSliderParam(key: "pitch", label: "Pitch", value: 1, minimumValue: 0.5, maximumValue: 2.0)
            ]
        ]
        settings.settingsDelegate = self
        
        self.navigationController?.pushViewController(settings, animated: true)
    }
    
    override func viewDidLoad(){
        super.viewDidLoad()
        
        // set up agora instance when view loadedlet config = AgoraRtcEngineConfig()
        let config = AgoraRtcEngineConfig()
        config.appId = KeyCenter.AppId
        config.areaCode = GlobalSettings.shared.area.rawValue
        agoraKit = AgoraRtcEngineKit.sharedEngine(with: config, delegate: self)
        
        guard let channelName = configs["channelName"] as? String else {return}
        
        self.title = channelName
        
        // disable video module
        agoraKit.disableVideo()
        
        // Set audio route to speaker
        agoraKit.setDefaultAudioRouteToSpeakerphone(true)
        
        // start joining channel
        // 1. Users can only see each other after they join the
        // same channel successfully using the same app id.
        // 2. If app certificate is turned on at dashboard, token is needed
        // when joining channel. The channel name and uid used to calculate
        // the token has to match the ones used for channel join
        let result = agoraKit.joinChannel(byToken: nil, channelId: channelName, info: nil, uid: 0) {[unowned self] (channel, uid, elapsed) -> Void in
            self.isJoined = true
            LogUtils.log(message: "Join \(channel) with uid \(uid) elapsed \(elapsed)ms", level: .info)
            
            //set up local audio view, this view will not show video but just a placeholder
            let view = Bundle.loadView(fromNib: "VideoView", withType: VideoView.self)
            self.audioViews[uid] = view
            view.setPlaceholder(text: self.getAudioLabel(uid: uid, isLocal: true))
            self.container.layoutStream3x3(views: Array(self.audioViews.values))
        }
        if result != 0 {
            // Usually happens with invalid parameters
            // Error code description can be found at:
            // en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            // cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
            self.showAlert(title: "Error", message: "joinChannel call failed: \(result), please check your params")
        }
    }
    
    override func willMove(toParent parent: UIViewController?) {
        if parent == nil {
            // leave channel when exiting the view
            if isJoined {
                agoraKit.leaveChannel { (stats) -> Void in
                    LogUtils.log(message: "left channel, duration: \(stats.duration)", level: .info)
                }
            }
        }
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
        LogUtils.log(message: "warning: \(warningCode.description)", level: .warning)
    }
    
    /// callback when error occured for agora sdk, you are recommended to display the error descriptions on demand
    /// to let user know something wrong is happening
    /// Error code description can be found at:
    /// en: https://docs.agora.io/en/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// cn: https://docs.agora.io/cn/Voice/API%20Reference/oc/Constants/AgoraErrorCode.html
    /// @param errorCode error code of the problem
    func rtcEngine(_ engine: AgoraRtcEngineKit, didOccurError errorCode: AgoraErrorCode) {
        LogUtils.log(message: "error: \(errorCode)", level: .error)
        self.showAlert(title: "Error", message: "Error \(errorCode.description) occur")
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
        self.container.layoutStream3x3(views: Array(self.audioViews.values))
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
        self.container.layoutStream3x3(views: Array(self.audioViews.values))
        self.container.reload(level: 0, animated: true)
    }
}

/// custom voice changer events
extension VoiceChanger:SettingsViewControllerDelegate
{
    func didChangeValue(key: String, value: AnyObject) {
        LogUtils.log(message: "set \(key): \(value)", level: .info)
        if key == "pitch" {
            agoraKit.setLocalVoicePitch(value.doubleValue)
        }
    }
}
