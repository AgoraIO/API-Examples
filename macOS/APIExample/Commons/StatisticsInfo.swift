//
//  MediaInfo.swift
//  OpenVideoCall
//
//  Created by GongYuhua on 4/11/16.
//  Copyright © 2016 Agora. All rights reserved.
//

import Foundation
import AgoraRtcKit

struct StatisticsInfo {
    struct LocalInfo {
        var channelStats : AgoraChannelStats?
        var videoStats : AgoraRtcLocalVideoStats?
        var audioStats : AgoraRtcLocalAudioStats?
        var audioVolume : UInt?
    }
    
    struct RemoteInfo {
        var videoStats : AgoraRtcRemoteVideoStats?
        var audioStats : AgoraRtcRemoteAudioStats?
        var audioVolume : UInt?
    }
    
    enum StatisticsType {
        case local(LocalInfo), remote(RemoteInfo)
        
        var isLocal: Bool {
            switch self {
            case .local:  return true
            case .remote: return false
            }
        }
    }
    
    var type: StatisticsType
    
    init(type: StatisticsType) {
        self.type = type
    }
    
    mutating func updateChannelStats(_ stats: AgoraChannelStats) {
        guard self.type.isLocal else {
            return
        }
        switch type {
        case .local(let info):
            var new = info
            new.channelStats = stats
            self.type = .local(new)
        default:
            break
        }
    }
    
    mutating func updateLocalVideoStats(_ stats: AgoraRtcLocalVideoStats) {
        guard self.type.isLocal else {
            return
        }
        switch type {
        case .local(let info):
            var new = info
            new.videoStats = stats
            self.type = .local(new)
        default:
            break
        }
    }
    
    mutating func updateLocalAudioStats(_ stats: AgoraRtcLocalAudioStats) {
        guard self.type.isLocal else {
            return
        }
        switch type {
        case .local(let info):
            var new = info
            new.audioStats = stats
            self.type = .local(new)
        default:
            break
        }
    }
    
    mutating func updateVideoStats(_ stats: AgoraRtcRemoteVideoStats) {
        switch type {
        case .remote(let info):
            var new = info
            new.videoStats = stats
//            dimension = CGSize(width: Int(stats.width), height: Int(stats.height))
//            fps = stats.rendererOutputFrameRate
            self.type = .remote(new)
        default:
            break
        }
    }
    
    mutating func updateAudioStats(_ stats: AgoraRtcRemoteAudioStats) {
        switch type {
        case .remote(let info):
            var new = info
            new.audioStats = stats
            self.type = .remote(new)
        default:
            break
        }
    }
    
    mutating func updateVolume(_ volume: UInt) {
        switch type {
        case .local(let info):
            var new = info
            new.audioVolume = volume
            self.type = .local(new)
        case .remote(let info):
            var new = info
            new.audioVolume = volume
            self.type = .remote(new)
        }
    }
    
    func description(audioOnly:Bool) -> String {
        var full: String
        switch type {
        case .local(let info):  full = localDescription(info: info, audioOnly: audioOnly)
        case .remote(let info): full = remoteDescription(info: info, audioOnly: audioOnly)
        }
        return full
    }
    
    func localDescription(info: LocalInfo, audioOnly: Bool) -> String {
        var results:[String] = []
        
        if(!audioOnly) {
            if let volume = info.audioVolume {
                results.append("Volume: \(volume)")
            }
            
            if let videoStats = info.videoStats, let channelStats = info.channelStats, let audioStats = info.audioStats {
                results.append("\(Int(videoStats.encodedFrameWidth))×\(Int(videoStats.encodedFrameHeight)),\(videoStats.sentFrameRate)fps")
                results.append("LM Delay: \(channelStats.lastmileDelay)ms")
                results.append("VSend: \(videoStats.sentBitrate)kbps")
                results.append("ASend: \(audioStats.sentBitrate)kbps")
                results.append("CPU: \(channelStats.cpuAppUsage)%/\(channelStats.cpuTotalUsage)%")
                results.append("Send Loss: \(channelStats.txPacketLossRate)%")
            }
        } else {
            if let volume = info.audioVolume {
                results.append("Volume: \(volume)")
            }
            
            if let channelStats = info.channelStats, let audioStats = info.audioStats {
                results.append("LM Delay: \(channelStats.lastmileDelay)ms")
                results.append("ASend: \(audioStats.sentBitrate)kbps")
                results.append("CPU: \(channelStats.cpuAppUsage)%/\(channelStats.cpuTotalUsage)%")
                results.append("Send Loss: \(channelStats.txPacketLossRate)%")
            }
        }
        
        return results.joined(separator: "\n")
    }
    
    func remoteDescription(info: RemoteInfo, audioOnly: Bool) -> String {
        var results:[String] = []
        
        
        if(!audioOnly) {
            if let volume = info.audioVolume {
                results.append("Volume: \(volume)")
            }
            
            if let videoStats = info.videoStats, let audioStats = info.audioStats {
                let audioQuality:AgoraNetworkQuality = AgoraNetworkQuality(rawValue: audioStats.quality) ?? .unknown
                results.append("\(Int(videoStats.width))×\(Int(videoStats.height)),\(videoStats.decoderOutputFrameRate)fps")
                results.append("VRecv: \(videoStats.receivedBitrate)kbps")
                results.append("ARecv: \(audioStats.receivedBitrate)kbps")
                results.append("VLoss: \(videoStats.packetLossRate)%")
                results.append("ALoss: \(audioStats.audioLossRate)%")
                results.append("AQuality: \(audioQuality.description())")
            }
        } else {
            if let volume = info.audioVolume {
                results.append("Volume: \(volume)")
            }
            
            if let audioStats = info.audioStats {
                let audioQuality:AgoraNetworkQuality = AgoraNetworkQuality(rawValue: audioStats.quality) ?? .unknown
                results.append("ARecv: \(audioStats.receivedBitrate)kbps")
                results.append("ALoss: \(audioStats.audioLossRate)%")
                results.append("AQuality: \(audioQuality.description())")
            }
        }
        
        return results.joined(separator: "\n")
    }
}
