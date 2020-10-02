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
        case .excellent:   return "excellent"
        case .good:        return "good"
        case .poor:        return "poor"
        case .bad:         return "bad"
        case .vBad:        return "very bad"
        case .down:        return "down"
        case .unknown:     return "unknown"
        case .unsupported: return "unsupported"
        case .detecting:   return "detecting"
        default:           return "unknown"
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
