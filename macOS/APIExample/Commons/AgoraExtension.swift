//
//  AgoraCode.swift
//  OpenLive
//
//  Created by CavanSu on 2019/9/16.
//  Copyright © 2019 Agora. All rights reserved.
//

import AgoraRtcKit

extension String {
    var localized: String { NSLocalizedString(self, comment: "") }
}

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
        case .excellent:   return "excellent"
        case .good:        return "good"
        case .poor:        return "poor"
        case .bad:         return "bad"
        case .vBad:        return "very bad"
        case .down:        return "down"
        case .unknown:     return "unknown"
        // TODO
//        case .unsupported: return "unsupported"
//        case .detecting:   return "detecting"
        default:           return "unknown"
        }
    }
}

extension AgoraAudioProfile {
    func description() -> String {
        switch self {
        case .default: return "Default".localized
        case .musicStandard: return "Music Standard".localized
        case .musicStandardStereo: return "Music Standard Stereo".localized
        case .musicHighQuality: return "Music High Quality".localized
        case .musicHighQualityStereo: return "Music High Quality Stereo".localized
        case .speechStandard: return "Speech Standard".localized
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
        case .default: return "Default".localized
        case .chatRoomGaming: return "Chat Room Gaming".localized
        case .education: return "Education".localized
        case .gameStreaming: return "Game Streaming".localized
        case .chatRoomEntertainment: return "Chat Room Entertainment".localized
        case .showRoom: return "Show Room".localized
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
        case .voiceChangerOff:return "Off".localized
        case .generalBeautyVoiceFemaleFresh:return "FemaleFresh".localized
        case .generalBeautyVoiceFemaleVitality:return "FemaleVitality".localized
        case .generalBeautyVoiceMaleMagnetic:return "MaleMagnetic".localized
        case .voiceBeautyVigorous:return "Vigorous".localized
        case .voiceBeautyDeep:return "Deep".localized
        case .voiceBeautyMellow:return "Mellow".localized
        case .voiceBeautyFalsetto:return "Falsetto".localized
        case .voiceBeautyFull:return "Full".localized
        case .voiceBeautyClear:return "Clear".localized
        case .voiceBeautyResounding:return "Resounding".localized
        case .voiceBeautyRinging:return "Ringing".localized
        case .voiceBeautySpacial:return "Spacial".localized
        case .voiceChangerEthereal:return "Ethereal".localized
        case .voiceChangerOldMan:return "Old Man".localized
        case .voiceChangerBabyBoy:return "Baby Boy".localized
        case .voiceChangerBabyGirl:return "Baby Girl".localized
        case .voiceChangerZhuBaJie:return "ZhuBaJie".localized
        case .voiceChangerHulk:return "Hulk".localized
        default:
            return "\(self.rawValue)"
        }
    }
}

extension AgoraAudioReverbPreset {
    func description() -> String {
        switch self {
        case .off:return "Off".localized
        case .fxUncle:return "FxUncle".localized
        case .fxSister:return "FxSister".localized
        case .fxPopular:return "Pop".localized
        case .popular:return "Pop(Old Version)".localized
        case .fxRNB:return "R&B".localized
        case .rnB:return "R&B(Old Version)".localized
        case .rock:return "Rock".localized
        case .hipHop:return "HipHop".localized
        case .fxVocalConcert:return "Vocal Concert".localized
        case .vocalConcert:return "Vocal Concert(Old Version)".localized
        case .fxKTV:return "KTV".localized
        case .KTV:return "KTV(Old Version)".localized
        case .fxStudio:return "Studio".localized
        case .studio:return "Studio(Old Version)".localized
        case .fxPhonograph:return "Phonograph".localized
        // TODO
//        case .virtualStereo:return "Virtual Stereo".localized
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
        case .dryLevel:     return "Dry Level".localized
        case .wetLevel:     return "Wet Level".localized
        case .roomSize:     return "Room Size".localized
        case .wetDelay:     return "Wet Delay".localized
        case .strength:     return "Strength".localized
        @unknown default:
            return "\(self.rawValue)"
        }
    }
}
