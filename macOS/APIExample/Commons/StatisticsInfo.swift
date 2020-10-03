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
    }
    
    struct RemoteInfo {
        var videoStats : AgoraRtcRemoteVideoStats?
        var audioStats : AgoraRtcRemoteAudioStats?
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
    
    var dimension = CGSize.zero
    var fps:UInt = 0
    
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
        dimension = CGSize(width: Int(stats.encodedFrameWidth), height: Int(stats.encodedFrameHeight))
        fps = stats.sentFrameRate
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
            dimension = CGSize(width: Int(stats.width), height: Int(stats.height))
            fps = stats.rendererOutputFrameRate
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
    
    func description(audioOnly:Bool) -> String {
        var full: String
        switch type {
        case .local(let info):  full = localDescription(info: info, audioOnly: audioOnly)
        case .remote(let info): full = remoteDescription(info: info, audioOnly: audioOnly)
        }
        return full
    }
    
    func localDescription(info: LocalInfo, audioOnly: Bool) -> String {
        guard let channelStats = info.channelStats, let audioStats = info.audioStats else {return ""}
        
        let dimensionFps = "\(Int(dimension.width))×\(Int(dimension.height)),\(fps)fps"
        
        let lastmile = "LM Delay: \(channelStats.lastmileDelay)ms"
        let audioSend = "ASend: \(audioStats.sentBitrate)kbps"
        let cpu = "CPU: \(channelStats.cpuAppUsage)%/\(channelStats.cpuTotalUsage)%"
        let aSendLoss = "ASend Loss: \(audioStats.txPacketLossRate)%"
        
        if(!audioOnly) {
            guard let videoStats = info.videoStats else {return ""}
            let videoSend = "VSend: \(videoStats.sentBitrate)kbps"
            let vSendLoss = "VSend Loss: \(videoStats.txPacketLossRate)%"
            return [dimensionFps,lastmile,videoSend,audioSend,cpu,vSendLoss,aSendLoss].joined(separator: "\n")
        }
        return [lastmile,audioSend,cpu,aSendLoss].joined(separator: "\n")
    }
    
    func remoteDescription(info: RemoteInfo, audioOnly: Bool) -> String {
        guard let audioStats = info.audioStats else {return ""}
        let dimensionFpsBit = "\(Int(dimension.width))×\(Int(dimension.height)), \(fps)fps"
        
        var audioQuality: AgoraNetworkQuality
        if let quality = AgoraNetworkQuality(rawValue: audioStats.quality) {
            audioQuality = quality
        } else {
            audioQuality = AgoraNetworkQuality.unknown
        }
        
        let audioRecv = "ARecv: \(audioStats.receivedBitrate)kbps"
        let audioLoss = "ALoss: \(audioStats.audioLossRate)%"
        let aquality = "AQuality: \(audioQuality.description())"
        
        if(!audioOnly) {
            guard let videoStats = info.videoStats else {return ""}
            let videoRecv = "VRecv: \(videoStats.receivedBitrate)kbps"
            let videoLoss = "VLoss: \(videoStats.packetLossRate)%"
            return [dimensionFpsBit,videoRecv,audioRecv,videoLoss,audioLoss,aquality].joined(separator: "\n")
        }
        return [audioRecv,audioLoss,aquality].joined(separator: "\n")
    }
}
