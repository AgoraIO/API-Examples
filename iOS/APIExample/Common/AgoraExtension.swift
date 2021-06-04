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
        case .fixedPortrait: return "fixed portrait".localized
        case .fixedLandscape: return "fixed landscape".localized
        case .adaptative: return "adaptive".localized
        default: return "\(self.rawValue)"
        }
    }
}

extension AgoraClientRole {
    func description() -> String {
        switch self {
        case .broadcaster: return "Broadcaster".localized
        case .audience: return "Audience".localized
        default:
            return "\(self.rawValue)"
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
        case .AES128GCM2: return "AES128GCM2"
        case .AES256GCM2: return "AES256GCM2"
        default:
            return "\(self.rawValue)"
        }
    }
    
    static func allValues() -> [AgoraEncryptionMode] {
        return [.AES128GCM2, .AES256GCM2]
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

extension AgoraVoiceBeautifierPreset{
    func description() -> String {
        switch self {
        case .voiceBeautifierOff:return "Off".localized
        case .chatBeautifierFresh:return "FemaleFresh".localized
        case .chatBeautifierMagnetic:return "MaleMagnetic".localized
        case .chatBeautifierVitality:return "FemaleVitality".localized
        case .timbreTransformationVigorous:return "Vigorous".localized
        case .timbreTransformationDeep:return "Deep".localized
        case .timbreTransformationMellow:return "Mellow".localized
        case .timbreTransformationFalsetto:return "Falsetto".localized
        case .timbreTransformationFull:return "Full".localized
        case .timbreTransformationClear:return "Clear".localized
        case .timbreTransformationResounding:return "Resounding".localized
        case .timbreTransformationRinging:return "Ringing".localized
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
        case .virtualStereo:return "Virtual Stereo".localized
        default:
            return "\(self.rawValue)"
        }
    }
}

extension AgoraAudioEffectPreset {
    func description() -> String {
        switch self {
        case .audioEffectOff:return "Off".localized
        case .voiceChangerEffectUncle:return "FxUncle".localized
        case .voiceChangerEffectOldMan:return "Old Man".localized
        case .voiceChangerEffectBoy:return "Baby Boy".localized
        case .voiceChangerEffectSister:return "FxSister".localized
        case .voiceChangerEffectGirl:return "Baby Girl".localized
        case .voiceChangerEffectPigKing:return "ZhuBaJie".localized
        case .voiceChangerEffectHulk:return "Hulk".localized
        case .styleTransformationRnB:return "R&B".localized
        case .styleTransformationPopular:return "Pop".localized
        case .roomAcousticsKTV:return "KTV".localized
        case .roomAcousticsVocalConcert:return "Vocal Concert".localized
        case .roomAcousticsStudio:return "Studio".localized
        case .roomAcousticsPhonograph:return "Phonograph".localized
        case .roomAcousticsVirtualStereo:return "Virtual Stereo".localized
        case .roomAcousticsSpacial:return "Spacial".localized
        case .roomAcousticsEthereal:return "Ethereal".localized
        case .roomAcoustics3DVoice:return "3D Voice".localized
        case .pitchCorrection:return "Pitch Correction".localized
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

extension AgoraVoiceConversionPreset {
    func description() -> String {
        switch self {
        case .conversionOff:
            return "Off".localized
        case .changerNeutral:
            return "Neutral".localized
        case .changerSweet:
            return "Sweet".localized
        case .changerSolid:
            return "Solid".localized
        case .changerBass:
            return "Bass".localized
        @unknown default:
            return "\(self.rawValue)"
        }
    }
}

extension UIAlertController {
    func addCancelAction() {
        self.addAction(UIAlertAction(title: "Cancel".localized, style: .cancel, handler: nil))
    }
}

extension UIApplication {
    /// The top most view controller
    static var topMostViewController: UIViewController? {
        return UIApplication.shared.keyWindow?.rootViewController?.visibleViewController
    }
}

extension UIViewController {
    /// The visible view controller from a given view controller
    var visibleViewController: UIViewController? {
        if let navigationController = self as? UINavigationController {
            return navigationController.topViewController?.visibleViewController
        } else if let tabBarController = self as? UITabBarController {
            return tabBarController.selectedViewController?.visibleViewController
        } else if let presentedViewController = presentedViewController {
            return presentedViewController.visibleViewController
        } else {
            return self
        }
    }
}

extension OutputStream {

    /// Write `String` to `OutputStream`
    ///
    /// - parameter string:                The `String` to write.
    /// - parameter encoding:              The `String.Encoding` to use when writing the string. This will default to `.utf8`.
    /// - parameter allowLossyConversion:  Whether to permit lossy conversion when writing the string. Defaults to `false`.
    ///
    /// - returns:                         Return total number of bytes written upon success. Return `-1` upon failure.

    func write(_ string: String, encoding: String.Encoding = .utf8, allowLossyConversion: Bool = false) -> Int {

        if let data = string.data(using: encoding, allowLossyConversion: allowLossyConversion) {
            let ret = data.withUnsafeBytes {
                write($0, maxLength: data.count)
            }
            if(ret < 0) {
                print("write fail: \(streamError.debugDescription)")
            }
        }

        return -1
    }

}

extension Date {
   func getFormattedDate(format: String) -> String {
        let dateformat = DateFormatter()
        dateformat.dateFormat = format
        return dateformat.string(from: self)
    }
}
