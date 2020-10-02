//
//  AgoraCode.swift
//  OpenLive
//
//  Created by CavanSu on 2019/9/16.
//  Copyright © 2019 Agora. All rights reserved.
//

import AgoraRtcKit

extension AgoraErrorCode {
    var description: String {
        var text: String
        switch self {
        case .joinChannelRejected:  text = "join channel rejected"
        case .leaveChannelRejected: text = "leave channel rejected"
        case .invalidAppId:         text = "invalid app id"
        case .invalidToken:         text = "invalid token"
        case .invalidChannelId:     text = "invalid channel id"
        default:                    text = "\(self.rawValue)"
        }
        return text
    }
}

extension AgoraWarningCode {
    var description: String {
        var text: String
        switch self {
        case .invalidView: text = "invalid view"
        default:           text = "\(self.rawValue)"
        }
        return text
    }
}

extension AgoraNetworkQuality {
    func description() -> String {
        switch self {
        case .excellent:   return "excel"
        case .good:        return "good"
        case .poor:        return "poor"
        case .bad:         return "bad"
        case .vBad:        return "vBad"
        case .down:        return "down"
        case .unknown:     return "NA"
        case .unsupported: return "unsupported"
        case .detecting:   return "detecting"
        default:           return "NA"
        }
    }
}

extension AgoraVideoOutputOrientationMode {
    func description() -> String {
        switch self {
        case .fixedPortrait: return "fixed portrait"
        case .fixedLandscape: return "fixed landscape"
        case .adaptative: return "adaptive"
        default: return "unknown"
        }
    }
}

extension AgoraAudioProfile {
    func description() -> String {
        switch self {
        case .default: return "default"
        case .musicStandard: return "Music Standard"
        case .musicStandardStereo: return "Music Standard Stereo"
        case .musicHighQuality: return "Music High Quality"
        case .musicHighQualityStereo: return "Music High Quality Stereo"
        case .speechStandard: return "Speech Standard"
        default:
            return "\(self.rawValue)"
        }
    }
    static func allValues() -> [AgoraAudioProfile] {
        return [.default, .speechStandard, .musicStandard, .musicStandardStereo, .musicHighQuality, .musicHighQualityStereo]
    }
}

extension AgoraAudioScenario {
    func description() -> String {
        switch self {
        case .default: return "default"
        case .chatRoomGaming: return "Chat Room Gaming"
        case .education: return "Education"
        case .gameStreaming: return "Game Streaming"
        case .chatRoomEntertainment: return "Chat Room Entertainment"
        case .showRoom: return "Show Room"
        default:
            return "\(self.rawValue)"
        }
    }
    
    static func allValues() -> [AgoraAudioScenario] {
        return [.default, .chatRoomGaming, .education, .gameStreaming, .chatRoomEntertainment, .showRoom]
    }
}

extension AgoraEncryptionMode {
    func description() -> String {
        switch self {
        case .AES128XTS: return "AES128XTS"
        case .AES256XTS: return "AES256XTS"
        case .AES128ECB: return "AES128ECB"
        case .SM4128ECB: return "SM4128ECB"
        default:
            return "\(self.rawValue)"
        }
    }
    
    static func allValues() -> [AgoraEncryptionMode] {
        return [.AES128XTS, .AES256XTS, .AES128ECB, .SM4128ECB]
    }
}

extension AgoraAudioVoiceChanger {
    func description() -> String {
        switch self {
        case .voiceChangerOff:return "Off"
        case .generalBeautyVoiceFemaleFresh:return "FemaleFresh"
        case .generalBeautyVoiceFemaleVitality:return "FemaleVitality"
        case .generalBeautyVoiceMaleMagnetic:return "MaleMagnetic"
        case .voiceBeautyVigorous:return "Vigorous"
        case .voiceBeautyDeep:return "Deep"
        case .voiceBeautyMellow:return "Mellow"
        case .voiceBeautyFalsetto:return "Falsetto"
        case .voiceBeautyFull:return "Full"
        case .voiceBeautyClear:return "Clear"
        case .voiceBeautyResounding:return "Resounding"
        case .voiceBeautyRinging:return "Ringing"
        case .voiceBeautySpacial:return "Spacial"
        case .voiceChangerEthereal:return "Ethereal"
        case .voiceChangerOldMan:return "Old Man"
        case .voiceChangerBabyBoy:return "Baby Boy"
        case .voiceChangerBabyGirl:return "Baby Girl"
        case .voiceChangerZhuBaJie:return "ZhuBaJie"
        case .voiceChangerHulk:return "Hulk"
        default:
            return "\(self.rawValue)"
        }
    }
}

extension AgoraAudioReverbPreset {
    func description() -> String {
        switch self {
        case .off:return "Off"
        case .fxUncle:return "FxUncle"
        case .fxSister:return "FxSister"
        case .fxPopular:return "Pop"
        case .popular:return "Pop(Old Version)"
        case .fxRNB:return "R&B"
        case .rnB:return "R&B(Old Version)"
        case .rock:return "Rock"
        case .hipHop:return "HipHop"
        case .fxVocalConcert:return "Vocal Concert"
        case .vocalConcert:return "Vocal Concert(Old Version)"
        case .fxKTV:return "KTV"
        case .KTV:return "KTV(Old Version)"
        case .fxStudio:return "Studio"
        case .studio:return "Studio(Old Version)"
        case .fxPhonograph:return "Phonograph"
        case .virtualStereo:return "Virtual Stereo"
        default:
            return "\(self.rawValue)"
        }
    }
}

extension AgoraAudioEqualizationBandFrequency {
    func description() -> String {
        switch self {
        case .band31:     return "31Hz"
        case .band62:     return "62Hz"
        case .band125:     return "125Hz"
        case .band250:     return "250Hz"
        case .band500:     return "500Hz"
        case .band1K:     return "1kHz"
        case .band2K:     return "2kHz"
        case .band4K:     return "4kHz"
        case .band8K:     return "8kHz"
        case .band16K:     return "16kHz"
        @unknown default:
            return "\(self.rawValue)"
        }
    }
}

extension AgoraAudioReverbType {
    func description() -> String {
        switch self {
        case .dryLevel:     return "Dry Level"
        case .wetLevel:     return "Wet Level"
        case .roomSize:     return "Room Size"
        case .wetDelay:     return "Wet Delay"
        case .strength:     return "Strength"
        @unknown default:
            return "\(self.rawValue)"
        }
    }
}

extension UIAlertController {
    func addCancelAction() {
        self.addAction(UIAlertAction(title: "Cancel", style: .cancel, handler: nil))
    }
}
